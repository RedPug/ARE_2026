#include <WiFiS3.h>
#include <WebSocketsServer.h>
#include "webManager.hpp"
#include "index.hpp"
#include "settings.hpp"
#include <rideManager.hpp>


void onWebSocketEvent(
    uint8_t clientNumber,
    WStype_t type,
    uint8_t* payload,
    size_t length
);

namespace WebServer {
char ssid[] = "Arduino-Control";
char pass[] = "12345678";

IPAddress ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);
WebSocketsServer webSocketServer(81);

bool sendWebSocketMessage(const char* message, uint8_t clientNumber) {
    if (clientNumber == BROADCAST_CLIENT) {
        return webSocketServer.broadcastTXT(message);
    }

    return webSocketServer.sendTXT(clientNumber, message);
}
}

void WebServer::setup(){
    // Configure access point IP
    WiFi.config(ip);

    // Start WiFi access point
    int status = WiFi.beginAP(ssid, pass);

    if (status != WL_AP_LISTENING) {
        Serial.println("AP startup failed!");
        while (true);
    }

    delay(2000);

    // Display network information
    Serial.println("Access Point Started!");

    Serial.print("Network: ");
    Serial.println(ssid);

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Start the HTTP server for the dashboard and the WebSocket server for control.
    server.begin();
    webSocketServer.begin();
    webSocketServer.onEvent(onWebSocketEvent);
}


void sendResponse(
    WiFiClient& client,
    const char* contentType,
    const char* body,
    int statusCode = 200,
    const char* statusText = "OK"
) {
    size_t len = strlen(body);

    client.print("HTTP/1.1 ");
    client.print(statusCode);
    client.print(" ");
    client.println(statusText);

    client.print("Content-Type: ");
    client.println(contentType);

    client.print("Content-Length: ");
    client.println(len);

    client.println("Connection: close");
    client.println();

    client.write((const uint8_t*)body, len);
}


bool ledState = false;

void WebServer::send_variable_state(const char* variable_name, const char* state, uint8_t clientNumber) {
    char json[128];
    snprintf_P(
        json,
        sizeof(json),
        PSTR("{\"type\":\"status\",\"variable\":\"%s\",\"state\":\"%s\"}"),
        variable_name,
        state
    );

    sendWebSocketMessage(json, clientNumber);
}

void WebServer::send_variable_state(const char* variable_name, bool state, uint8_t clientNumber) {
    char json[64];
    snprintf_P(
        json,
        sizeof(json),
        PSTR("{\"type\":\"status\",\"variable\":\"%s\",\"state\":%s}"),
        variable_name,
        state ? "true" : "false"
    );
    sendWebSocketMessage(json, clientNumber);
}

void WebServer::send_variable_state(const char* variable_name, int state, uint8_t clientNumber) {
    char json[64];
    snprintf_P(
        json,
        sizeof(json),
        PSTR("{\"type\":\"status\",\"variable\":\"%s\",\"state\":%d}"),
        variable_name,
        state
    );
    sendWebSocketMessage(json, clientNumber);
}

void WebServer::send_ride_mode(RideMode mode, uint8_t clientNumber) {
    char json[64];
    snprintf_P(
        json,
        sizeof(json),
        PSTR("{\"type\":\"ride_mode\",\"mode\":%d}"),
        static_cast<int>(mode)
    );
    sendWebSocketMessage(json, clientNumber);
}

void WebServer::send_fault(const char* message, uint8_t clientNumber) {
    char json[128];
    snprintf_P(
        json,
        sizeof(json),
        PSTR("{\"type\":\"fault\",\"message\":\"%s\"}"),
        message
    );
    sendWebSocketMessage(json, clientNumber);
}

void WebServer::sendRideStates(uint8_t clientNumber) {
    send_ride_mode(RideManager::rideMode, clientNumber);
    send_variable_state(
        to_string(WebVariable::LIFT_STATE),
        toString(RideManager::liftState),
        clientNumber
    );
    send_variable_state(
        to_string(WebVariable::STATION_STATE),
        toString(RideManager::stationState),
        clientNumber
    );
    send_variable_state(
        to_string(WebVariable::TRACK_STATE),
        toString(RideManager::trackState),
        clientNumber
    );
}

void onWebSocketEvent(
    uint8_t clientNumber,
    WStype_t type,
    uint8_t* payload,
    size_t length
) {
    if (type == WStype_CONNECTED) {
        WebServer::send_variable_state(to_string(WebVariable::LED_STATUS), ledState, clientNumber);
        WebServer::sendRideStates(clientNumber);
        return;
    }

    if (type != WStype_TEXT) {
        return;
    }

    String message;
    message.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        message += static_cast<char>(payload[i]);
    }

    if (message.indexOf("\"action\":\"led\"") >= 0) {
        int stateIndex = message.indexOf("\"state\":");
        if (stateIndex >= 0) {
            ledState = message.substring(stateIndex + 8).startsWith("true");
            digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
            WebServer::send_variable_state(to_string(WebVariable::LED_STATUS), ledState, clientNumber);
            return;
        }
    }

    if (message.indexOf("\"action\":\"request_start\"") >= 0) {
        RideManager::requestStart();
        return;
    }

    WebServer::sendWebSocketMessage(
        "{\"type\":\"error\",\"message\":\"Invalid command\"}",
        clientNumber
    );
}

bool handle_http_request(WiFiClient client, String& method, String& path){
    if (path == "/" && method == "GET") {
        // send the webpage
        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-Type: text/html; charset=UTF-8"));
        client.println(F("Connection: close"));
        client.println();

        client.write((const uint8_t*)DASHBOARD_HTML, strlen(DASHBOARD_HTML));

        return true;
    }
    
    return false;
}

void WebServer::loop(){
    // websockets handles messages through callbacks. It only needs to loop
    webSocketServer.loop();

    // Handle regular HTTP requests
    WiFiClient client = server.available();

    if(!client){
        return;
    }

    String method = "";
    String path = "";

    // Read request line
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();

    Serial.print("Request: ");
    Serial.println(requestLine);

    // Parse method and path
    int firstSpace = requestLine.indexOf(' ');
    int secondSpace = requestLine.indexOf(' ', firstSpace + 1);

    if (firstSpace > 0 && secondSpace > firstSpace) {
        method = requestLine.substring(0, firstSpace);
        path = requestLine.substring(firstSpace + 1, secondSpace);
    }

    Serial.print("Method: ");
    Serial.println(method);

    Serial.print("Path: ");
    Serial.println(path);

    // Read and discard remaining HTTP headers
    while (client.connected()) {
        String header = client.readStringUntil('\n');

        if (header == "\r" || header.length() == 0) {
            break;
        }
    }

    // ======================================
    // Route handling
    // ======================================

    if(!handle_http_request(client, method, path)){

        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();

        client.println("Request received, but not implemented.");
    }

    client.flush();
    client.stop();
}

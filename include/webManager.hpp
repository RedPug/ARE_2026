#pragma once

#include <WiFiS3.h>
#include <rideManager.hpp>

namespace WebServer{
constexpr uint8_t BROADCAST_CLIENT = 0xFF;

// Access Point credentials
extern char ssid[];
extern char pass[];

// Arduino access point IP
extern IPAddress ip;
extern IPAddress gateway;
extern IPAddress subnet;

extern WiFiServer server;

void setup();
void loop();

void send_variable_state(const char* variable_name, bool state, uint8_t clientNumber=BROADCAST_CLIENT);
void send_variable_state(const char* variable_name, int state, uint8_t clientNumber=BROADCAST_CLIENT);
void send_variable_state(const char* variable_name, const char* state, uint8_t clientNumber=BROADCAST_CLIENT);
void send_ride_mode(RideMode mode, uint8_t clientNumber=BROADCAST_CLIENT);
void send_fault(const char* message, uint8_t clientNumber=BROADCAST_CLIENT);
void sendRideStates(uint8_t clientNumber=BROADCAST_CLIENT);

// End of namespace
}

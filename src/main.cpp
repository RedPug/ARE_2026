#include <Arduino.h>
#include "webManager.hpp"
#include "rideManager.hpp"
#include "faultManager.hpp"
#include "eventManager.hpp"

void setup() {
    Serial.begin(9600);

    FaultManager::setup();
    WebServer::setup();
    RideManager::setup();
    EventManager::setup();
}

bool station_event_flag = false;
uint64_t station_event_timer = 0;
StationState station_event_next_state;

bool lift_event_flag = false;
uint64_t lift_event_timer = 0;
LiftState lift_event_next_state;

bool track_event_flag = false;
uint64_t track_event_timer = 0;
TrackState track_event_next_state;

void simulateEvents(){
    if(RideManager::rideMode == RideMode::RUNNING) {
        if(RideManager::liftState == LiftState::RAISING) {
            if(!lift_event_flag) {
                lift_event_flag = true;
                lift_event_timer = millis() + 8000;
                lift_event_next_state = LiftState::RAISED;
            }
        }else if(RideManager::liftState == LiftState::LOADING) {
            if(!lift_event_flag) {
                lift_event_flag = true;
                lift_event_timer = millis() + 5000;
                lift_event_next_state = LiftState::LOADED;
            }
        }else if(RideManager::liftState == LiftState::UNLOADING) {
            if(!lift_event_flag) {
                lift_event_flag = true;
                lift_event_timer = millis() + 5000;
                lift_event_next_state = LiftState::UNLOADED;
            }
        }else if(RideManager::liftState == LiftState::LOWERING) {
            if(!lift_event_flag) {
                lift_event_flag = true;
                lift_event_timer = millis() + 8000;
                lift_event_next_state = LiftState::LOWERED;
            }
        }

        if(RideManager::stationState == StationState::ARRIVING) {
            if(!station_event_flag) {
                station_event_flag = true;
                station_event_timer = millis() + 5000;
                station_event_next_state = StationState::HOLDING;
            }
        }else if(RideManager::stationState == StationState::DEPARTING) {
            if(!station_event_flag) {
                station_event_flag = true;
                station_event_timer = millis() + 5000;
                station_event_next_state = StationState::EMPTY;
            }
        }

        if(RideManager::trackState == TrackState::ARRIVING) {
            if(!track_event_flag) {
                track_event_flag = true;
                track_event_timer = millis() + 5000;
                track_event_next_state = TrackState::ACTIVE;
            }
        }else if(RideManager::trackState == TrackState::ACTIVE) {
            if(!track_event_flag) {
                track_event_flag = true;
                track_event_timer = millis() + 10000;
                track_event_next_state = TrackState::HOLDING;
            }
        }else if(RideManager::trackState == TrackState::DEPARTING) {
            if(!track_event_flag) {
                track_event_flag = true;
                track_event_timer = millis() + 5000;
                track_event_next_state = TrackState::EMPTY;
            }
        }


        if(millis() > station_event_timer && station_event_flag) {
            station_event_flag = false;
            RideManager::stationState = station_event_next_state;
        }

        if(millis() > lift_event_timer && lift_event_flag) {
            lift_event_flag = false;
            RideManager::liftState = lift_event_next_state;
        }

        if(millis() > track_event_timer && track_event_flag) {
            track_event_flag = false;
            RideManager::trackState = track_event_next_state;
        }
    }
}

void loop() {
    simulateEvents();

    FaultManager::loop();

    WebServer::loop();
    RideManager::loop();
    EventManager::loop();

    delay(100);
}

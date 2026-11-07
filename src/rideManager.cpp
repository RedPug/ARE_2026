#include "rideManager.hpp"
#include "webManager.hpp"
#include "settings.hpp"

using namespace RideManager;

RideMode RideManager::rideMode = RideMode::IDLE;
RideMode lastRideMode = RideManager::rideMode;

LiftState RideManager::liftState = LiftState::STANDBY;
LiftState lastLiftState = RideManager::liftState;

StationState RideManager::stationState = StationState::HOLDING;
StationState lastStationState = RideManager::stationState;

TrackState RideManager::trackState = TrackState::HOLDING;
TrackState lastTrackState = RideManager::trackState;

void RideManager::setup() {
    rideMode = RideMode::IDLE;
}

void handleIdle() {
    // Handle idle mode
}

void handleRunning() {
    // Handle running mode
    if(stationState == StationState::HOLDING && liftState == LiftState::STANDBY) {
        stationState = StationState::DEPARTING;
        liftState = LiftState::LOADING;
    }

    if(liftState == LiftState::LOADED){
        liftState = LiftState::RAISING;
    }

    if(liftState == LiftState::RAISED && trackState == TrackState::EMPTY){
        liftState = LiftState::UNLOADING;
        trackState = TrackState::ARRIVING;
    }

    if(liftState == LiftState::UNLOADED){
        liftState = LiftState::LOWERING;
    }
    
    if(liftState == LiftState::LOWERED){
        liftState = LiftState::STANDBY;
    }

    if(stationState == StationState::EMPTY && trackState == TrackState::HOLDING) {
        stationState = StationState::ARRIVING;
        trackState = TrackState::DEPARTING;
    }
}

void handleFault() {
    // Handle fault mode
}

void RideManager::loop() {
    if(rideMode != lastRideMode) {
        lastRideMode = rideMode;
        WebServer::send_ride_mode(rideMode);
        Serial.println("Ride mode changed to: " + String(static_cast<int>(rideMode)));
    }

    if(liftState != lastLiftState) {
        lastLiftState = liftState;
        WebServer::send_variable_state(to_string(WebVariable::LIFT_STATE), to_string(liftState));
        Serial.println("Lift state changed to: " + String(to_string(liftState)));
    }

    if(stationState != lastStationState) {
        lastStationState = stationState;
        WebServer::send_variable_state(to_string(WebVariable::STATION_STATE), to_string(stationState));
        Serial.println("Station state changed to: " + String(to_string(stationState)));
    }

    if(trackState != lastTrackState) {
        lastTrackState = trackState;
        WebServer::send_variable_state(to_string(WebVariable::TRACK_STATE), to_string(trackState));
        Serial.println("Track state changed to: " + String(to_string(trackState)));
    }

    switch (rideMode) {
        case RideMode::IDLE:
            handleIdle();
            break;
        case RideMode::RUNNING:
            handleRunning();
            break;
        case RideMode::FAULT:
            handleFault();
            break;
    }
}

void RideManager::beginRide() {
    rideMode = RideMode::RUNNING;
    Serial.println("Ride started");
}

void RideManager::emergencyStopRide() {
    rideMode = RideMode::FAULT;
    Serial.println("Ride emergency stopped");
}

void RideManager::requestStart() {
    if (rideMode == RideMode::IDLE) {
        beginRide();
        Serial.println("Ride requested to start");
    }
}

void RideManager::requestStop() {
    if (rideMode == RideMode::RUNNING) {
        rideMode = RideMode::IDLE;
        Serial.println("Ride stopped");
    }
}

#include "rideManager.hpp"
#include "webManager.hpp"
#include "settings.hpp"
#include "eventManager.hpp"
#include "faultManager.hpp"

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

void handleLift() {
    if(liftState == LiftState::LOADED){
        liftState = LiftState::RAISING;
    }

    if(liftState == LiftState::UNLOADED){
        liftState = LiftState::LOWERING;
    }
    
    if(liftState == LiftState::LOWERED){
        liftState = LiftState::STANDBY;
    }

}

void handleBlockTransitions(bool isLastCycle) {
    if(!isLastCycle) {
        if(stationState == StationState::HOLDING && liftState == LiftState::STANDBY) {
            stationState = StationState::DEPARTING;
            liftState = LiftState::LOADING;
        }
    }

    if(stationState == StationState::EMPTY && trackState == TrackState::HOLDING) {
        stationState = StationState::ARRIVING;
        trackState = TrackState::DEPARTING;
    }

    if(liftState == LiftState::RAISED && trackState == TrackState::EMPTY){
        liftState = LiftState::UNLOADING;
        trackState = TrackState::ARRIVING;
    }
}

void handleFault() {
    FaultBehavior severity = FaultManager::getFaultBehavior();
    if(!(severity & FaultBehavior::DISABLE_LIFT_OPERATION)){
        handleLift();
    }

    if(!(severity & FaultBehavior::DISABLE_BLOCK_ENTRY)){
        handleBlockTransitions(true);
    }

}

void RideManager::loop() {
    if(rideMode != lastRideMode) {
        lastRideMode = rideMode;
        WebServer::send_ride_mode(rideMode);
    }

    if(liftState != lastLiftState) {
        lastLiftState = liftState;
        WebServer::send_variable_state(to_string(WebVariable::LIFT_STATE), toString(liftState));
    }

    if(stationState != lastStationState) {
        lastStationState = stationState;
        WebServer::send_variable_state(to_string(WebVariable::STATION_STATE), toString(stationState));
    }

    if(trackState != lastTrackState) {
        lastTrackState = trackState;
        WebServer::send_variable_state(to_string(WebVariable::TRACK_STATE), toString(trackState));
    }

    switch (rideMode) {
        case RideMode::IDLE:
            handleIdle();
            break;
        case RideMode::RUNNING:
            handleLift();
            handleBlockTransitions(false);
            break;
        case RideMode::LAST_CYCLE:
            handleLift();
            handleBlockTransitions(true);
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
        rideMode = RideMode::LAST_CYCLE;
        Serial.println("Ride requested to stop");
    }
}

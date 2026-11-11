#include "eventManager.hpp"
#include "faultManager.hpp"
#include "rideManager.hpp"

EventTimer<StationState> EventManager::station_timer;
EventTimer<LiftState> EventManager::lift_timer;
EventTimer<TrackState> EventManager::track_timer;

void EventManager::setup() {
    station_timer.reset();
    lift_timer.reset();
    track_timer.reset();
}

void EventManager::loop() {
    if(RideManager::stationState == StationState::DEPARTING) {
        setStationTimer(millis() + 5000, StationState::EMPTY);
    }else if(RideManager::stationState == StationState::ARRIVING) {
        setStationTimer(millis() + 5000, StationState::HOLDING);
    }

    if(RideManager::liftState == LiftState::RAISING) {
        setLiftTimer(millis() + 8000, LiftState::RAISED);
    }else if(RideManager::liftState == LiftState::UNLOADING) {
        setLiftTimer(millis() + 5000, LiftState::UNLOADED);
    }else if(RideManager::liftState == LiftState::LOWERING) {
        setLiftTimer(millis() + 8000, LiftState::LOWERED);
    }

    if(RideManager::trackState == TrackState::ARRIVING) {
        setTrackTimer(millis() + 5000, TrackState::HOLDING);
    }else if(RideManager::trackState == TrackState::DEPARTING) {
        setTrackTimer(millis() + 5000, TrackState::EMPTY);
    }


    if (station_timer.isActive() && station_timer.getExpectedState() == RideManager::stationState){
        station_timer.reset();
    }
    if (lift_timer.isActive() && lift_timer.getExpectedState() == RideManager::liftState){
        lift_timer.reset();
    }
    if (track_timer.isActive() && track_timer.getExpectedState() == RideManager::trackState){
        track_timer.reset();
    }


    if (station_timer.isExpired()) {
        // Handle station timer expiration
        FaultManager::handleStationTimeout();
        station_timer.reset();
    }

    if (lift_timer.isExpired()) {
        // Handle lift timer expiration
        FaultManager::handleLiftTimeout();
        lift_timer.reset();
    }

    if (track_timer.isExpired()) {
        // Handle track timer expiration
        FaultManager::handleTrackTimeout();
        track_timer.reset();
    }
}
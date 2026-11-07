#pragma once

enum class RideMode {
    IDLE,
    RUNNING,
    FAULT
};

enum class StationState {
    EMPTY,
    ARRIVING,
    HOLDING,
    DEPARTING
};

enum class LiftState {
    STANDBY,
    LOADING,
    LOADED,
    RAISING,
    RAISED,
    UNLOADING,
    UNLOADED,
    LOWERING,
    LOWERED
};

enum class TrackState {
    EMPTY,
    ARRIVING,
    ACTIVE,
    HOLDING,
    DEPARTING,
};

namespace RideManager {
    extern RideMode rideMode;

    extern LiftState liftState;
    extern StationState stationState;
    extern TrackState trackState;

void setup();

void loop();

void beginRide();

void emergencyStopRide();

void requestStart();

void requestStop();

}

constexpr const char* to_string(RideMode mode){
    switch(mode){
        case RideMode::IDLE:
            return "idle";
        case RideMode::RUNNING:
            return "running";
        case RideMode::FAULT:
            return "fault";
    }
}

constexpr const char* to_string(LiftState state){
    switch(state){
        case LiftState::STANDBY:
            return "standby";
        case LiftState::LOADING:
            return "loading";
        case LiftState::LOADED:
            return "loaded";
        case LiftState::RAISING:
            return "raising";
        case LiftState::RAISED:
            return "raised";
        case LiftState::UNLOADING:
            return "unloading";
        case LiftState::UNLOADED:
            return "unloaded";
        case LiftState::LOWERING:
            return "lowering";
        case LiftState::LOWERED:
            return "lowered";
    }
}

constexpr const char* to_string(StationState state){
    switch(state){
        case StationState::EMPTY:
            return "empty";
        case StationState::ARRIVING:
            return "arriving";
        case StationState::HOLDING:
            return "holding";
        case StationState::DEPARTING:
            return "departing";
    }
}

constexpr const char* to_string(TrackState state){
    switch(state){
        case TrackState::EMPTY:
            return "empty";
        case TrackState::ARRIVING:
            return "arriving";
        case TrackState::ACTIVE:
            return "active";
        case TrackState::HOLDING:
            return "holding";
        case TrackState::DEPARTING:
            return "departing";
    }
}
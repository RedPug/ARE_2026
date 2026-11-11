#pragma once

enum class WebVariable {
    LED_STATUS,
    LIFT_STATE,
    STATION_STATE,
    TRACK_STATE,
};

enum class FaultCode {
    ESTOP,
};

const unsigned int MAX_FAULT_HISTORY = 16;

constexpr const char* to_string(FaultCode fault){
    switch(fault){
        case FaultCode::ESTOP:
            return "e-stop";
        default:
            return "unknown";
    }
}

constexpr const char* to_string(WebVariable variable){
    switch(variable){
        case WebVariable::LED_STATUS:
            return "led-status";
        case WebVariable::LIFT_STATE:
            return "lift-state";
        case WebVariable::STATION_STATE:
            return "station-state";
        case WebVariable::TRACK_STATE:
            return "track-state";
    }
}
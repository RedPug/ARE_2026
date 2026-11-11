#pragma once

#include "settings.hpp"
#include <Arduino.h>

enum class FaultBehavior : uint8_t {
    NONE = 0,
    DISABLE_BLOCK_ENTRY =       1, // indicates an unknown state, do not move between blocks
    DISABLE_LIFT_OPERATION = 1<<1, // disables the lift from operating
    SLOW_STOP =    1<<2, // gradually stop (specifically applies to motor)
    FAST_STOP =  1<<3, // immediately stop everything
    REQUIRE_FULL_RESET = 1<<4 // doesn't continue automatically once reset. a counter example is a visual check.
};

constexpr FaultBehavior operator|(FaultBehavior a, FaultBehavior b) {
    return static_cast<FaultBehavior>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
constexpr FaultBehavior operator&(FaultBehavior a, FaultBehavior b) {
    return static_cast<FaultBehavior>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}
constexpr bool operator!(FaultBehavior a) {
    return static_cast<uint8_t>(a) == static_cast<uint8_t>(FaultBehavior::NONE);
}

enum class FaultReason {
    NONE,
    ESTOP,
    MOTOR,
    SENSOR,
    SENSOR_TIMEOUT
};

constexpr const FaultBehavior getFaultBehavior(FaultReason reason){
    switch(reason){
        case FaultReason::NONE:
            return  FaultBehavior::DISABLE_LIFT_OPERATION |
                    FaultBehavior::DISABLE_BLOCK_ENTRY;
        case FaultReason::ESTOP:
            return  FaultBehavior::FAST_STOP |
                    FaultBehavior::DISABLE_LIFT_OPERATION |
                    FaultBehavior::REQUIRE_FULL_RESET |
                    FaultBehavior::DISABLE_BLOCK_ENTRY;
        case FaultReason::MOTOR:
            return  FaultBehavior::SLOW_STOP |
                    FaultBehavior::REQUIRE_FULL_RESET |
                    FaultBehavior::DISABLE_LIFT_OPERATION |
                    FaultBehavior::DISABLE_BLOCK_ENTRY;
        case FaultReason::SENSOR_TIMEOUT:
            return FaultBehavior::DISABLE_BLOCK_ENTRY;
    }
}

constexpr const char* toString(FaultReason reason){
    switch(reason){
        case FaultReason::NONE:
            return "none";
        case FaultReason::ESTOP:
            return "estop";
        case FaultReason::MOTOR:
            return "motor";
        case FaultReason::SENSOR:
            return "sensor";
        case FaultReason::SENSOR_TIMEOUT:
            return "timeout";
    }
}

class Fault {
private:
    FaultReason reason;
    int timestamp;

public:
    static const Fault& noneFault;


    Fault() : reason(FaultReason::NONE), timestamp(millis()) {}
    Fault(FaultReason t) : reason(t), timestamp(millis()) {}
    Fault(FaultReason t, int ts) : reason(t), timestamp(ts) {}

    Fault(const Fault& other) : reason(other.reason), timestamp(other.timestamp) {}

    FaultBehavior getBehavior() const {
        return getFaultBehavior(reason);
    }

    FaultReason getReason() const {
        return reason;
    }
    
    int getTimestamp() const {
        return timestamp;
    }
};

class FaultManager {
private:
    static Fault faultHistory[MAX_FAULT_HISTORY];
    static int faultCount;

public:
    static void setup();
    static void loop();
    static void addFault(Fault fault);
    static void acknowledgeFault();
    static void clearFaultHistory();
    static const Fault& getCurrentFault();
    static const Fault& getFaultHistory(int index);
    static int getFaultCount(){return faultCount;}
    static bool hasFault(){return faultCount > 0;}

    static FaultBehavior getFaultBehavior();

    static void handleStationTimeout();
    static void handleLiftTimeout();
    static void handleTrackTimeout();
};
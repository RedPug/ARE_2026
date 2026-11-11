#pragma once
#include "rideManager.hpp"
#include <cstdint>
#include "Arduino.h"

template<typename T>
class EventTimer {
private:
    bool flag = false;
    uint64_t expire_time = 0;
    T next_state = T();
public:
    EventTimer() = default;
    EventTimer(uint64_t expire_time)
        : flag(true), expire_time(expire_time) {}

    bool isExpired() const {
        return flag && (millis() > expire_time);
    }

    void reset() {
        flag = false;
        expire_time = 0;
    }

    void set(uint64_t expire_time, T next_state) {
        this->expire_time = expire_time;
        this->next_state = next_state;
        flag = true;
    }
    
    bool isActive() const {
        return flag;
    }

    T getExpectedState() const {
        return next_state;
    }
};

class EventManager {
private:
    static EventTimer<StationState> station_timer;

    static EventTimer<LiftState> lift_timer;

    static EventTimer<TrackState> track_timer;
    
public:
    static void setup();
    static void loop();

    static void setStationTimer(uint64_t expire_time, StationState next_state){
        station_timer.set(expire_time, next_state);
    }
    static void setLiftTimer(uint64_t expire_time, LiftState next_state){
        lift_timer.set(expire_time, next_state);
    }
    static void setTrackTimer(uint64_t expire_time, TrackState next_state){
        track_timer.set(expire_time, next_state);
    }
};
#include "faultManager.hpp"
#include "rideManager.hpp"
#include "settings.hpp"

Fault FaultManager::faultHistory[MAX_FAULT_HISTORY];
int FaultManager::faultCount = 0;

static const Fault actualNoneFault(FaultReason::NONE, 0);
const Fault& Fault::noneFault = actualNoneFault;

void FaultManager::setup() {
    faultCount = 0;
}

void FaultManager::loop() {
    if (faultCount > 0) {
        RideManager::rideMode = RideMode::FAULT;
    }
}

void FaultManager::addFault(Fault fault) {
    if (faultCount < MAX_FAULT_HISTORY) {
        faultHistory[faultCount] = fault;
        faultCount++;
    } else {
        for (int i = 0; i < MAX_FAULT_HISTORY - 1; i++) {
            faultHistory[i] = faultHistory[i + 1];
        }
        faultHistory[MAX_FAULT_HISTORY - 1] = fault;
    }
}

void FaultManager::acknowledgeFault() {
    if (faultCount > 0) {
        faultCount--;
    }
}

void FaultManager::clearFaultHistory() {
    faultCount = 0;
}

const Fault& FaultManager::getCurrentFault() {
    if (faultCount > 0) {
        return faultHistory[faultCount - 1];
    }

    return Fault::noneFault;
}

const Fault& FaultManager::getFaultHistory(int index) {
    if (index >= 0 && index < faultCount) {
        return faultHistory[index];
    }
    return Fault::noneFault;
}

void FaultManager::handleStationTimeout() {
    addFault(Fault(FaultReason::SENSOR_TIMEOUT));
}

void FaultManager::handleLiftTimeout() {
    addFault(Fault(FaultReason::SENSOR_TIMEOUT));
}

void FaultManager::handleTrackTimeout() {
    addFault(Fault(FaultReason::SENSOR_TIMEOUT));
}

FaultBehavior FaultManager::getFaultBehavior() {
    FaultBehavior maxSeverity = FaultBehavior::NONE;
    for (int i = 0; i < faultCount; i++) {
        FaultBehavior severity = faultHistory[i].getBehavior();
        maxSeverity = maxSeverity | severity;
    }
    return maxSeverity;
}
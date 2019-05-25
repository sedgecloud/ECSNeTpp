//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "CPUPowerConsumer.h"

namespace ecsnet {

simsignal_t CPUPowerConsumer::cpuPowerConsumptionChangedSignal = registerSignal("cpuPowerConsumptionChanged");

Define_Module(CPUPowerConsumer);

void CPUPowerConsumer::initialize(int stage) {
    cSimpleModule::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        cpuBusyUtilisation = par("cpuBusyUtilisation").doubleValue();
        cpuIdleUtilisation = par("cpuIdleUtilisation").doubleValue();
        cpuPowerConsumptionScalar =
                par("cpuPowerConsumptionScalar").doubleValue();
        idlePowerConsumption = W(par("idlePowerConsumption"));

        prevState = States::CPU_IDLE;
        cModule *host = findContainingNode(this);
        host->subscribe(ISTask::cpuStateChangedSignal, this);
        powerConsumption = W(0);
        WATCH(powerConsumption);
    }
}

W CPUPowerConsumer::computeCPUPowerConsumption(double value) const {
    W _powerConsumption = W(0);
    if (value == States::CPU_BUSY) {
        if (prevState == States::CPU_BUSY) {
            _powerConsumption = W(
                    cpuPowerConsumptionScalar * cpuBusyUtilisation);
        } else if (prevState == States::CPU_IDLE) {
            _powerConsumption = W(
                    cpuPowerConsumptionScalar * cpuIdleUtilisation);
        }
        prevState = States::CPU_BUSY;
    } else if (value == States::CPU_IDLE) {
        if (prevState == States::CPU_BUSY) {
            _powerConsumption = W(
                    cpuPowerConsumptionScalar * cpuBusyUtilisation);
        } else if (prevState == States::CPU_IDLE) {
            _powerConsumption = W(
                    cpuPowerConsumptionScalar * cpuIdleUtilisation);
        }
        prevState = States::CPU_IDLE;
    } else {
        _powerConsumption = W(0);
    }
//    EV << "CPU Power consumption=" << _powerConsumption << endl;
    return _powerConsumption;
}

void CPUPowerConsumer::receiveSignal(cComponent *source, simsignal_t signal,
        long value, cObject *details) {

    if (simTime() >= getSimulation()->getWarmupPeriod()) {
        if (signal == ISTask::cpuStateChangedSignal) {
            powerConsumption = computeCPUPowerConsumption(value);
            emit(cpuPowerConsumptionChangedSignal, powerConsumption.get());
        } else
            throw cRuntimeError("Unknown signal");
    }
}

} /* namespace ecsnet */

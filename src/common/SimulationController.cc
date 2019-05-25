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

#include "SimulationController.h"

namespace ecsnetpp {

simsignal_t SimulationController::totalSimTimeChangedSignal = registerSignal("totalSimTimeChanged");

Define_Module(SimulationController);

void SimulationController::initialize() {
    packetCountLimit = par("packetCountLimit").longValue();
    enableLimitFromSource = par("enableLimitFromSource").boolValue();
    packetCount = 0;
    sourcePacketCount = 0;
    stopEventGeneration = false;
    stopEdgeIdleEnergyRecording = false;
    cModule *parent = getParentModule();
//    if (enableLimitFromSource) {
        parent->subscribe(ISTask::packetGeneratedSignal, this);
//    } else {
        parent->subscribe(ISTask::receivedStreamingMsgsSignal, this);
//    }
    startTime = simTime();
}

void SimulationController::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage())
        throw cRuntimeError("This module does not process messages.");

    delete msg;
}

void SimulationController::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details){
    if (signalID == ISTask::receivedStreamingMsgsSignal){
        if (simTime() >= getSimulation()->getWarmupPeriod()) {
            packetCount++;
            if (packetCountLimit >= 0) { // if the packetCountLimit = -1 then run simulation until manually stopped.
                if (packetCount >= packetCountLimit) {
                    std::cout << "Packet count limit of " << packetCountLimit
                              << " reached. Ending simulation..." << endl;

                    const omnetpp::simtime_t _totalSimTime = simTime() - startTime;
                    emit(totalSimTimeChangedSignal, _totalSimTime.dbl());
                    endSimulation();
                }
            }
            if (packetCount % 1000 == 0) {
                std::cout << "Sink PKT COUNT=" << packetCount << endl;
            }
        }
    }
}

void SimulationController::receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details){
    if (signalID == ISTask::packetGeneratedSignal){
        if (simTime() >= getSimulation()->getWarmupPeriod()) {
            sourcePacketCount++;
            if (packetCountLimit >= 0) { // if the packetCountLimit = -1 then run simulation until manually stopped.
                if (sourcePacketCount >= packetCountLimit) {
//                    EV << "Packet count limit of " << packetCountLimit
//                              << " reached. Ending simulation..." << endl;
//                    endSimulation();
                    if (enableLimitFromSource) {
                        stopEventGeneration = true;
                    }
                    stopEdgeIdleEnergyRecording = true;
                    std::cout << "Stopping event generation now. Source PKT COUNT=" << sourcePacketCount << endl;
                }
            }
            if (sourcePacketCount % 1000 == 0) {
                std::cout << "Source PKT COUNT=" << sourcePacketCount << endl;
            }
        }
    }
}

bool SimulationController::getStopEventGeneration(){
    return stopEventGeneration;
}

bool SimulationController::getStopEdgeIdleEnergyRecording(){
    return stopEdgeIdleEnergyRecording;
}
//SimulationController::SimulationController() {
//    // TODO Auto-generated constructor stub
//
//}
//
//SimulationController::~SimulationController() {
//    // TODO Auto-generated destructor stub
//}

} /* namespace ecsnetpp */

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

#include "CPUCore.h"
#include "../stask/ISTask.h"
#include "../power/States.h"

namespace ecsnet {

Define_Module(CPUCore);

void CPUCore::finish() {
//    cancelAndDelete(timerMsg);
    std::map<long, cQueue>::iterator it;
    for (it = processingQueueMap.begin(); it != processingQueueMap.end();
            ++it) {
        cQueue _q = it->second;
        if (!_q.isEmpty()) {
            _q.clear();
        }
    }
}

void CPUCore::initialize() {
    ackersEnabled = getAncestorPar("ackersEnabled").boolValue();
    perCoreFreq = getAncestorPar("perCoreFreq").doubleValue();
    parallelisationFactor = par("parallelisationFactor").doubleValue();
    threadsPerCore = getAncestorPar("threadsPerCore").longValue();
    totalCores = getAncestorPar("cores").longValue();
    isOnEdgeDevice = getAncestorPar("isEdgeDevice").boolValue();
    std::cout<< "Cores: " << getParentModule()->getFullPath() << " : " << totalCores << endl;
//    timerMsg = new cMessage();
//    timerMsg->setKind(2);
}

omnetpp::SimTime CPUCore::calculateDelay(bool isProcessingDelayInCpuCycles, double processingDelay, long threads) {
    // * normal(1, 0.02)
    if (!isProcessingDelayInCpuCycles) {
        return omnetpp::SimTime(processingDelay * parallelisationFactor / (totalCores * 1000000000));    // convert processing delay to seconds from nanoseconds
    }
    delay = 0;
    double cyclesPerEvent = processingDelay;
    if (perCoreFreq != 0) {
        delay = cyclesPerEvent / perCoreFreq;
    }
    if (delay < 0) {
//        std::cout << "LT 0 CPE=" << cyclesPerEvent << " PCF=" << perCoreFreq
//                << " D=" << delay << endl;
        delay = 0;
    }
    double _delay = threads > 0 ? (delay * threads / threadsPerCore) : delay;
//    std::cout << "DELAY= " << _delay << " CPE=" << cyclesPerEvent << " PCF="
//            << perCoreFreq << " T=" << threads << endl;
    return omnetpp::SimTime(_delay);
}

void CPUCore::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage()) {
        emit(ISTask::cpuStateChangedSignal, States::CPU_BUSY);
        StreamingMessage *msgToProcess = check_and_cast<StreamingMessage *>(msg);
        msgToProcess->setOperatorIngressTime(simTime());
        bool isProcessingDelayInCpuCycles = msgToProcess->getIsProcessingDelayInCyclesPerEvent();
        double processingDelay = msgToProcess->getProcessingDelayPerEvent();
//        double cyclesPerEvent = msgToProcess->getCyclesPerEvent();
        int senderModuleId = msgToProcess->getSenderModuleId();
        const omnetpp::SimTime delay = calculateDelay(isProcessingDelayInCpuCycles, processingDelay, processingQueueMap.size());
//        msgToProcess->setProcessingDelay(msgToProcess->getProcessingDelay() + delay.dbl());
        processingQueueMap[senderModuleId].insert(msgToProcess);
        cMessage *timerMsg = new cMessage();
        timerMsg->setKind(senderModuleId);
        scheduleAt(simTime() + delay, timerMsg);
    } else {
        StreamingMessage *msgToProcess = check_and_cast<StreamingMessage *>(
                processingQueueMap[msg->getKind()].pop());

        const omnetpp::SimTime _processingTime = simTime() - msgToProcess->getOperatorIngressTime();
        double _procdelay = msgToProcess->getProcessingDelay() + _processingTime.dbl();
//        std::cout << " CPU proc delay: " << msgToProcess->getSender() << _procdelay << endl;
        msgToProcess->setProcessingDelay(_procdelay);
        if (isOnEdgeDevice) {
            msgToProcess->setEdgeProcessingDelay(msgToProcess->getEdgeProcessingDelay() + _processingTime.dbl());
        }
//        std::cout << "Queue length: " << msg->getKind() << " : " << processingQueueMap[msg->getKind()].length() << endl;
        if (processingQueueMap[msg->getKind()].isEmpty()) {
            processingQueueMap.erase(msg->getKind());
        }
        cModule *senderModule = msgToProcess->getSenderModule();
        double selectivityRatio = msgToProcess->getSelectivityRatio();
        if (selectivityRatio > 1) {
            int _sratio = std::lround(selectivityRatio); // get the rounded value of the selectivity
            for (int j = 0; j < _sratio; j++) { // send a msg for each incoming msg
                sendDirect(msgToProcess->dup(), senderModule, "fromCPU");
            }
        } else {
            sendDirect(msgToProcess, senderModule, "fromCPU");
        }
        emit(ISTask::cpuStateChangedSignal, States::CPU_IDLE);
    }
}

} /* namespace ecsnet */

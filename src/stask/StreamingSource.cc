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

#include "StreamingSource.h"

namespace ecsnet {

Define_Module(StreamingSource);

simsignal_t ISTask::packetGeneratedSignal = registerSignal("packetGenerated");

StreamingSource::~StreamingSource() {
    cancelAndDelete(timerMsg);
}

void StreamingSource::initialize() {
    perCoreFreq = getAncestorPar("perCoreFreq").doubleValue();
    omnetpp::cModule* submodule = getParentModule()->getSubmodule("cpuCoreScheduler");
    myCpuCoreScheduler = check_and_cast<ICpuCoreScheduler *>(submodule);
//    if (isProcessingDelayInCpuCycles) {
//        cyclesPerEvent = par("cyclesPerEvent").doubleValue();
//    } else {
        processingDelayPerEvent = par("processingDelayPerEvent").doubleValue();
//    }
//    calculateDelay();
    lastCPUIndex = 0;
    cpuCores = getAncestorPar("cores").longValue();
    if (cpuCores < 1) {
        throw new cRuntimeError("Number of CPU Cores is not set.");
    }
    mySTaskCategory = par("mySTaskCategory").stringValue();

    isSourceMsgSizeDistributed = par("isSourceMsgSizeDistributed").boolValue();
    if (!isSourceMsgSizeDistributed) {
        msgSize = par("msgSize").doubleValue();
    } else {
        mySourceMsgSizeDistributionModuleName = par("mySourceMsgSizeDistributionModuleName").stringValue();
        omnetpp::cModule* submodule = getParentModule()->getSubmodule(mySourceMsgSizeDistributionModuleName);

        mySourceMsgSizeDistributionModule = check_and_cast<IMessageSizeDistribution *>(submodule);
    }

    isSourceEvRateDistributed = par("isSourceEvRateDistributed").boolValue();
    if (!isSourceEvRateDistributed) {
        eventRate = par("eventRate").doubleValue(); //evs per second
        if (eventRate != 0) {
            msgDelay = 1 / eventRate;
        } else {
            throw cRuntimeError("Event rate is not set for the source : %s",
                    getFullPath().c_str());
        }
    } else {
        mySourceEvRateDistributionModuleName = par("mySourceEvRateDistributionModuleName").stringValue();
        omnetpp::cModule* submodule = getParentModule()->getSubmodule(mySourceEvRateDistributionModuleName);

        mySourceEvRateDistributionModule = check_and_cast<ISourceEventRateDistribution *>(submodule);
    }

    timerMsg = new cMessage("BEGIN");
    commonSimController = check_and_cast<SimulationController *>(
            getParentModule()->getParentModule()->getModuleByPath(
                    ".simController"));
    scheduleAt(simTime() + getMessageDelay(), timerMsg);
}

void StreamingSource::handleMessage(cMessage *msg) {
//    publishCpuStateChanged(States::CPU_BUSY);

    if (msg->isSelfMessage()) {
        if (!commonSimController->getStopEventGeneration()) {
            StreamingMessage *sourceMsg = new StreamingMessage();
            sourceMsg->setBitLength(getMessageSize());

            sourceMsg->setIsProcessingDelayInCyclesPerEvent(isProcessingDelayInCpuCycles);
            sourceMsg->setProcessingDelayPerEvent(processingDelayPerEvent);

//            if (isProcessingDelayInCpuCycles){
//                sourceMsg->setIsProcessingDelayInCyclesPerEvent(true);
//                sourceMsg->setProcessingDelayPerEvent(cyclesPerEvent);
//            } else {
//                sourceMsg->setIsProcessingDelayInCyclesPerEvent(false);
//                sourceMsg->setProcessingDelayPerEvent(processingDelayPerEvent);
//            }
            sourceMsg->setStartTime(simTime());
            sourceMsg->setSender(mySTaskCategory);
            sourceMsg->setSelectivityRatio(1);
            sourceMsg->setNetworkDelay(0);
            sourceMsg->setProcessingDelay(0);
            sourceMsg->setEdgeProcessingDelay(0);
            long nextInLine = getNextProcessorCoreIndex();
            cModule *cpuCore = getParentModule()->getSubmodule("cpuCore",
                    nextInLine);
//        send(sourceMsg,"toCPU");
            sendDirect(sourceMsg, cpuCore, "incomingBus");
            cancelEvent(timerMsg);
            scheduleAt(simTime() + getMessageDelay(), timerMsg);
        }
    } else if (msg->arrivedOn("fromCPU")) {
        StreamingMessage *sourceMsg = check_and_cast<StreamingMessage *>(msg);
        emit(packetGeneratedSignal, 1, getParentModule());
        for (int i = 0; i < gateSize("outgoingStream"); i++) {
            send(sourceMsg->dup(), "outgoingStream", i);
        }
    } else {
        delete msg;
    }

//    publishCpuStateChanged(States::CPU_IDLE);
}

double StreamingSource::getMessageDelay() {
    if (isSourceEvRateDistributed) {
        return mySourceEvRateDistributionModule->getMessageDelay();
    }
    return msgDelay;
}

double StreamingSource::getMessageSize() {
    if (isSourceMsgSizeDistributed) {
        return mySourceMsgSizeDistributionModule->getMsgSize();
    }
    return msgSize;
}
} /* namespace ecsnet */

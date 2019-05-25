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

#include "StreamingOperator.h"

namespace ecsnet {

Define_Module(StreamingOperator);

void StreamingOperator::finish() {
    cancelAndDelete(timerMsg);
    if (!outgoingQueue.isEmpty()) {
        outgoingQueue.clear();
    }
}

void StreamingOperator::initialize() {
    omnetpp::cModule* submodule = getParentModule()->getSubmodule("cpuCoreScheduler");
    myCpuCoreScheduler = check_and_cast<ICpuCoreScheduler *>(submodule);
    ackersEnabled = getAncestorPar("ackersEnabled").boolValue();
    perCoreFreq = getAncestorPar("perCoreFreq").doubleValue();
    isProcessingDelayInCpuCycles = par("isProcessingDelayInCpuCycles").boolValue();
//    if (isProcessingDelayInCpuCycles) {
//        cyclesPerEvent = par("cyclesPerEvent").doubleValue();
//    } else {
        processingDelayPerEvent = par("processingDelayPerEvent").doubleValue();
//    }

    cpuCores = getAncestorPar("cores").longValue();
    if (cpuCores < 1) {
        throw new cRuntimeError("Number of CPU Cores is not set.");
    }

    isOperatorSelectivityDistributed = par("isOperatorSelectivityDistributed").boolValue();
    if(!isOperatorSelectivityDistributed){
        selectivityRatio = par("selectivityRatio").doubleValue();
        selectivityWindowLength = 0;
        if (selectivityRatio > 0) {
            selectivityWindowLength = std::lround(1 / selectivityRatio);
        }
    } else {
        myOperatorSelectivityDistributionModuleName = par("myOperatorSelectivityDistributionModuleName").stringValue();
        cModule* submodule = getParentModule()->getSubmodule(myOperatorSelectivityDistributionModuleName);

        myOperatorSelectivityDistributionModule = check_and_cast<IOperatorSelectivityDistribution *>(submodule);
    }

    isOperatorProductivityDistributed = par("isOperatorProductivityDistributed").boolValue();
    if (!isOperatorProductivityDistributed) {
        productivityRatio = par("productivityRatio").doubleValue();
    } else {
        myOperatorProductivityDistributionModuleName = par("myOperatorProductivityDistributionModuleName").stringValue();
        cModule* submodule = getParentModule()->getSubmodule(myOperatorProductivityDistributionModuleName);

        myOperatorProductivityDistributionModule = check_and_cast<IOperatorProductivityDistribution *>(submodule);
    }

    mySenders = par("mySenders").stringValue();
    mySTaskCategory = par("mySTaskCategory").stringValue();
    lastCPUIndex = 0;
    selectivityWindowCount = 0;

    timerMsg = new cMessage();
    timerMsg->setKind(2);
//    calculateDelay();
}

void StreamingOperator::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("fromCPU")) {
        nextToSend = check_and_cast<StreamingMessage *>(msg);
        for (int i = 0; i < gateSize("outgoingStream"); i++) {
            send(nextToSend->dup(), "outgoingStream", i);
        }
//        sendAck(nextToSend);
        delete nextToSend;
    } else if (msg->arrivedOn("incomingStream")) {
        StreamingMessage *msgToSend = check_and_cast<StreamingMessage *>(msg);
        double outgoingMsgSize = msgToSend->getBitLength() * getProductivityRatio();
        msgToSend->setBitLength(outgoingMsgSize);
        msgToSend->setSender(mySTaskCategory);

        msgToSend->setIsProcessingDelayInCyclesPerEvent(isProcessingDelayInCpuCycles);
        msgToSend->setProcessingDelayPerEvent(processingDelayPerEvent);

        double _selectivityRatio = getSelectivityRatio();
        msgToSend->setSelectivityRatio(_selectivityRatio);
        double _selectivityWindowLength = getSelectivityWindowLength();
        long nextInLine = getNextProcessorCoreIndex();
        cModule *cpuCore = getParentModule()->getSubmodule("cpuCore", nextInLine);
        if(_selectivityRatio < 1) {
            selectivityWindow.insert(msgToSend);
            if (selectivityWindow.getLength() >= _selectivityWindowLength) {
                nextToSend = check_and_cast<StreamingMessage *>(
                        selectivityWindow.pop());
                sendDirect(nextToSend, cpuCore, "incomingBus");
                if (!selectivityWindow.isEmpty()) {
                    if (selectivityWindow.getLength() < _selectivityWindowLength) {
                        selectivityWindow.clear();
                    } else {
                        for (int i = 0; i < _selectivityWindowLength - 1; i++) {
                            selectivityWindow.pop();
                        }
                    }
                }
            }
        } else{
            sendDirect(msgToSend, cpuCore, "incomingBus");
        }
    }
//    if (msg->isSelfMessage()) {
//        if (msg->getKind() == 2) {
////            std::cout << "sending new msg" << endl;
//            nextToSend = check_and_cast<StreamingMessage *>(
//                    outgoingQueue.pop());
//            if (selectivityRatio >= 1) {
//                int _sratio = std::lround(selectivityRatio); // get the rounded value of the selectivity
//                for (int j = 0; j < _sratio; j++) { // send a msg for each incoming msg
//                    for (int i = 0; i < gateSize("outgoingStream"); i++) {
//                        send(nextToSend->dup(), "outgoingStream", i);
//                    }
//                    sendAck(nextToSend);
//                }
//                delete nextToSend;
//            } else {
//                // If selectivity < 1, then hold the message in a queue
//                // until the queue length = 1/selectivity. Then pop the first
//                // msg in the queue and send it to the downstream.
//                // Clear the queue afterwards and wait for the next event.
//                selectivityWindow.insert(nextToSend);
//                if(selectivityWindow.getLength() >= selectivityWindowLength){
//                    nextToSend = check_and_cast<StreamingMessage *>(selectivityWindow.pop());
//                    for (int i = 0; i < gateSize("outgoingStream"); i++) {
//                        send(nextToSend->dup(), "outgoingStream", i);
//                    }
//                    if (!selectivityWindow.isEmpty()) {
//                        selectivityWindow.clear();
//                    }
//                    sendAck(nextToSend);
//                    delete nextToSend;
//                }
//            }
//        } else {
//            // unknown msg
//            delete msg;
//        }
//        publishCpuStateChanged(States::CPU_IDLE);
//    } else {    // not self message
//        StreamingMessage *msgToSend = check_and_cast<StreamingMessage *>(msg);
//        const char* sender = msgToSend->getSender();
//        if (strstr(mySenders, sender) == NULL) { // check if the intended receiver is the current node
//            delete msg;
//        } else {
//            publishCpuStateChanged(States::CPU_BUSY);
//            double outgoingMsgSize = msgToSend->getSize() * sizeIncreaseRatio;
//            msgToSend->setSize(outgoingMsgSize);
//            msgToSend->setSender(mySTaskCategory);
//            msgToSend->setCyclesPerEvent(cyclesPerEvent);
//            outgoingQueue.insert(msgToSend);
//
//            if (!timerMsg->isScheduled()) {
//                cancelAndDelete(timerMsg);
//            }
//
//            timerMsg = new cMessage();
//            timerMsg->setKind(2);
//            scheduleAt(simTime() + delay, timerMsg);
//        }
//    }
}

double StreamingOperator::getSelectivityRatio(){
    if (isOperatorSelectivityDistributed) {
        return myOperatorSelectivityDistributionModule->getSelectivityRatio();
    }
    return selectivityRatio;
}

double StreamingOperator::getSelectivityWindowLength(){
    if (isOperatorSelectivityDistributed) {
        return myOperatorSelectivityDistributionModule->getSelectivityWindowLength();
    }
    return selectivityWindowLength;
}

double StreamingOperator::getProductivityRatio(){
    if (isOperatorProductivityDistributed) {
        return myOperatorProductivityDistributionModule->getProductivityRatio();
    }
    return productivityRatio;
}

} /* namespace ecsnet */

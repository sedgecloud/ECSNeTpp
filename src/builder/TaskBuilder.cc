/*
 * taskbuilder.cc
 *
 *  Created on: Sep 25, 2017
 *      Author: gayashan
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <omnetpp.h>
#include "TaskBuilder.h"

using namespace omnetpp;
/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
namespace ecsnet {

Define_Module(TaskBuilder);

struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return std::strcmp(a, b) < 0;
    }
};

void TaskBuilder::initialize() {
    hasGlobalSupervisor = getAncestorPar("hasGlobalSupervisor").boolValue();
    if (hasGlobalSupervisor) {
        globalSupervisor = check_and_cast<GlobalStreamingSupervisor *>(
                getParentModule()->getModuleByPath(
                        ".globalSupervisorNode.globalSupervisor"));
    }
    ackersEnabled = getAncestorPar("ackersEnabled").boolValue();
    // build the network in event 1, because it is undefined whether the simkernel
    // will implicitly initialize modules created *during* initialization, or this needs
    // to be done manually.
    scheduleAt(simTime() + 1, new cMessage());
}

void TaskBuilder::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage())
        throw cRuntimeError("This module does not process messages.");

    delete msg;
    executeAllocationPlan(getParentModule());
}

void TaskBuilder::connect(cGate *src, cGate *dest, double delay, double ber,
        double datarate) {
    cDatarateChannel *channel = nullptr;
    if (delay > 0 || ber > 0 || datarate > 0) {
        channel = cDatarateChannel::create("channel");
        if (delay > 0)
            channel->setDelay(delay);
        if (ber > 0)
            channel->setBitErrorRate(ber);
        if (datarate > 0)
            channel->setDatarate(datarate);
    }
    src->connectTo(dest, channel);
}

void TaskBuilder::setStaskBoolPar(cModule* stask, const char* name, bool value) {
    cBoolParImpl* isDistributionEnabledBoolPar = new cBoolParImpl();
    isDistributionEnabledBoolPar->setName(name);
    isDistributionEnabledBoolPar->setBoolValue(value);
    stask->addPar(isDistributionEnabledBoolPar);
}

void TaskBuilder::executeAllocationPlan(cModule *parent) {
    std::map<std::string, std::vector<cModule *>> allocationMap;
    std::map<std::string, int> incomingConnectionsCountMap;
    std::map<std::string, int> outgoingConnectionsCountMap;
    std::map<std::string, std::map<std::string,int>> incomingConnectionsUsageMap;
    std::map<std::string, std::map<std::string,int>> outgoingConnectionsUsageMap;
    std::map<std::string, std::string> staskNameToCategoryMap;
    std::map<std::string, std::vector<std::string>> staskDownstreamCategoryToSenderMap;
    std::map<std::string, std::vector<std::string>> staskSenderToDownstreamStaskCategoryMap;
    std::map<std::string, std::vector<std::string>> staskCategoryToParentMap;
    std::map<std::string, std::string> staskNameToSendersStringMap;
    std::string line;


    const char* allocationFileName = par("allocationPlanFile").stringValue();
    std::fstream nodesFile(allocationFileName, std::ios::in);
    while (getline(nodesFile, line, '\n')) {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens =
                cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 7)
            throw cRuntimeError(
                    "Incomplete allocation plan parameters: 7 parameters required, line: \"%s\"",
                    line.c_str());

        // get fields from tokens
        std::string parentName = tokens[0];
        std::string staskName = tokens[1];
        std::string staskType = tokens[2];
        std::string staskCategory = tokens[3];
        double staskCyclesPerEvent = atof(tokens[4].c_str());
        double var1 = atof(tokens[5].c_str());
        double var2 = atof(tokens[6].c_str());

        // create module
        cModuleType *modtype = cModuleType::find(staskType.c_str());
        cModule *parent = getParentModule()->getModuleByPath(
                parentName.c_str());

        if (nullptr == parent) {
            std::cout << "Node " << parentName
                    << " is not present in the network." << endl;
            continue;
        }

        if (!modtype)
            throw cRuntimeError("module type `%s' for node `%s' not found",
                    staskType.c_str(), staskName.c_str());
        cModule *stask = modtype->create(staskName.c_str(), parent);

        cDoubleParImpl *cyclesPerEvent = new cDoubleParImpl();
        cyclesPerEvent->setName("cyclesPerEvent");
        cyclesPerEvent->setUnit("MHz");
        cyclesPerEvent->setDoubleValue(staskCyclesPerEvent);
        stask->addPar(cyclesPerEvent);

        // Add parameters to the tasks
        if (staskType == "ecsnet.stask.StreamingSource") {

            cDoubleParImpl *msgSize = new cDoubleParImpl();
            msgSize->setName("msgSize");
            msgSize->setDoubleValue(var1);
            stask->addPar(msgSize);
            setStaskBoolPar(stask, "isSourceMsgSizeDistributed", false);

            cDoubleParImpl *eventRate = new cDoubleParImpl();
            eventRate->setName("eventRate");
            eventRate->setDoubleValue(var2);
            stask->addPar(eventRate);
            setStaskBoolPar(stask, "isSourceEvRateDistributed", false);

        } else if (staskType == "ecsnet.stask.StreamingOperator") {
            cDoubleParImpl *selectivityRatio = new cDoubleParImpl();
            selectivityRatio->setName("selectivityRatio");
            selectivityRatio->setDoubleValue(var1);
            stask->addPar(selectivityRatio);
            setStaskBoolPar(stask, "isOperatorSelectivityDistributed", false);

            cDoubleParImpl *sizeIncreaseRatio = new cDoubleParImpl();
            sizeIncreaseRatio->setName("productivityRatio");
            sizeIncreaseRatio->setDoubleValue(var2);
            stask->addPar(sizeIncreaseRatio);
            setStaskBoolPar(stask, "isOperatorProductivityDistributed", false);
        }

        cStringParImpl *mySTaskCategory = new cStringParImpl();
        mySTaskCategory->setName("mySTaskCategory");
        mySTaskCategory->setStringValue(staskCategory.c_str());
        stask->addPar(mySTaskCategory);

        allocationMap[parentName].push_back(stask);
        incomingConnectionsCountMap[staskCategory] = 0;
        outgoingConnectionsCountMap[staskCategory] = 0;
        incomingConnectionsUsageMap[parentName][staskName] = 0;
        outgoingConnectionsUsageMap[parentName][staskName] = 0;
        staskNameToCategoryMap[staskName] = staskCategory;
        std::cout << "sname->scat map" << staskName << ":" << staskCategory << endl;
//        if(parentName.find("pi") == std::string::npos){
            staskCategoryToParentMap[staskCategory].push_back(parentName);
            std::cout << "scat->parent map" << staskCategory << ":" << parentName << endl;
//        }
        // read params from the ini file, etc
//        stask->finalizeParameters();
    }

    std::map<std::string, std::map<std::string, bool>> connectedSTasks;

    // read and create connections
    std::fstream connectionsFile(par("dspTopologyFile").stringValue(),
            std::ios::in);
    while (getline(connectionsFile, line, '\n')) {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens =
                cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError(
                    "wrong line in parameters file: 3 items required, line: \"%s\"",
                    line.c_str());

        // get fields from tokens
        std::string srcSTaskCategory = tokens[0];
        std::string destSTaskCategory = tokens[1];
        bool connected = strncmp(tokens[2].c_str(), "1", 1) == 0 ? true : false;
        connectedSTasks[srcSTaskCategory][destSTaskCategory] = connected;
        if (connected) {
            // count maps were initialized earlier
            incomingConnectionsCountMap[destSTaskCategory] += 1;
            outgoingConnectionsCountMap[srcSTaskCategory] += 1;
            staskDownstreamCategoryToSenderMap[destSTaskCategory].push_back(srcSTaskCategory);
            staskSenderToDownstreamStaskCategoryMap[srcSTaskCategory].push_back(destSTaskCategory);
        }
    }

    std::map<std::string, std::vector<cModule *>>::iterator it;
    // iterate per each parent
    for (it = allocationMap.begin(); it != allocationMap.end(); ++it) {
        std::string _parentName = it->first;
        std::vector<cModule *>::iterator it2;
        std::vector<cModule *> tmp = it->second;

        // iterate per each stask in the vector
        for (it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
            std::vector<cModule *>::iterator it3;
            cModule *_src = check_and_cast<cModule *>(*it2);
            std::string _srcSTaskCategory =
                    staskNameToCategoryMap[_src->getFullName()];


            std::stringstream ss;
            for (size_t i = 0; i < staskDownstreamCategoryToSenderMap[_srcSTaskCategory].size();
                    ++i) {
                if (i != 0) {
                    ss << ",";
                }
                ss << staskDownstreamCategoryToSenderMap[_srcSTaskCategory][i];
            }

            cStringParImpl *mySenders = new cStringParImpl();
            mySenders->setName("mySenders");
            mySenders->setStringValue(ss.str().c_str());
            _src->addPar(mySenders);
            // read params from the ini file, etc
            _src->finalizeParameters();

            // iterate per each stask in the vector
            for (it3 = tmp.begin(); it3 != tmp.end(); ++it3) {
                cModule *_dest = check_and_cast<cModule *>(*it3);

                std::string _destSTaskCategory =
                        staskNameToCategoryMap[_dest->getFullName()];

                if (_src->getFullName() != _dest->getFullName()) {

                    if (connectedSTasks[_srcSTaskCategory][_destSTaskCategory]) {
                        cGate *srcOut, *destIn;

                        srcOut = _src->getOrCreateFirstUnconnectedGate(
                                "outgoingStream", 0, false, true);
                        destIn = _dest->getOrCreateFirstUnconnectedGate(
                                "incomingStream", 0, false, true);

                        connect(srcOut, destIn, -1, -1, -1);
                        incomingConnectionsUsageMap[_parentName][_dest->getFullName()] += 1;
                        outgoingConnectionsUsageMap[_parentName][_src->getFullName()] += 1;
                    }
                }
            }
        }
    }
    for (it = allocationMap.begin(); it != allocationMap.end(); ++it) {
        std::string _parentName = it->first;
        cModule *_parent = getParentModule()->getModuleByPath(
                _parentName.c_str());
        std::vector<cModule *>::iterator it2;
        std::vector<cModule *> tmp = it->second;

        // iterate per each stask in the vector
        for (it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
            std::vector<cModule *>::iterator it3;
            cModule *_src = check_and_cast<cModule *>(*it2);
            cGate *taskIn, *taskOut, *supervisorIn, *supervisorOut;
            cModule *supervisor = _parent->getSubmodule("supervisor");

            while (incomingConnectionsUsageMap[_parentName][_src->getFullName()]
                    < incomingConnectionsCountMap[staskNameToCategoryMap[_src->getFullName()]]) {
                taskIn = _src->getOrCreateFirstUnconnectedGate("incomingStream",
                        0, false, true);
                supervisorOut = supervisor->getOrCreateFirstUnconnectedGate(
                        "streamingPortOut", 0, false, true);
                connect(supervisorOut, taskIn, -1, -1, -1);
                incomingConnectionsUsageMap[_parentName][_src->getFullName()] += 1;
            }

            while (outgoingConnectionsUsageMap[_parentName][_src->getFullName()]
                    < outgoingConnectionsCountMap[staskNameToCategoryMap[_src->getFullName()]]) {
                taskOut = _src->getOrCreateFirstUnconnectedGate(
                        "outgoingStream", 0, false, true);
                supervisorIn = supervisor->getOrCreateFirstUnconnectedGate(
                        "streamingPortIn", 0, false, true);
                connect(taskOut, supervisorIn, -1, -1, -1);
                outgoingConnectionsUsageMap[_parentName][_src->getFullName()] += 1;
            }

//            cGate *toCPU, *fromCPU, *incomingBus, *outgoingBus;
//            toCPU = _src->gate("toCPU");
//            incomingBus = cpuCore->getOrCreateFirstUnconnectedGate(
//                    "incomingBus", 0, false, true);
//            connect(toCPU, incomingBus, -1, -1, -1);
//            fromCPU = _src->gate("fromCPU");
//            outgoingBus = cpuCore->getOrCreateFirstUnconnectedGate(
//                    "outgoingBus", 0, false, true);
//            connect(outgoingBus, fromCPU, -1, -1, -1);

            if (ackersEnabled) {
                cGate *srcToAcker, *supToAcker;
                srcToAcker = _src->gate("ackerOut");
                supToAcker = supervisor->getOrCreateFirstUnconnectedGate(
                        "sendToAcker", 0, false, true);
                connect(srcToAcker, supToAcker, -1, -1, -1);
            }

        }   // end of per each stask per parent->vector

        StreamingSupervisor *_supervisor = check_and_cast<StreamingSupervisor *>(
                _parent->getSubmodule("supervisor"));

        // add the sender->parent_node_fullname mapping
        std::map<std::string, std::vector<std::string>>::iterator _senderIt;
        for (_senderIt = staskSenderToDownstreamStaskCategoryMap.begin();
                _senderIt != staskSenderToDownstreamStaskCategoryMap.end();
                ++_senderIt){
            std::string sender = _senderIt->first;
            std::vector<std::string> _staskCategories = _senderIt->second;

            for (size_t i = 0; i < _staskCategories.size(); i++) {
                _supervisor->addSTaskCategoryToDownstreamNodeMapping(sender,
                        staskCategoryToParentMap[_staskCategories[i]]);
            }
        }
        // resolve the parent names to IP addresses
        _supervisor->resolveDownstreamNodeIPs();
    }

    if (hasGlobalSupervisor) {
        // add the sender->parent_node_fullname mapping
        std::map<std::string, std::vector<std::string>>::iterator _senderIt;
        for (_senderIt = staskSenderToDownstreamStaskCategoryMap.begin();
                _senderIt != staskSenderToDownstreamStaskCategoryMap.end();
                ++_senderIt) {
            std::string sender = _senderIt->first;
            std::vector<std::string> _staskCategories = _senderIt->second;

            for (size_t i = 0; i < _staskCategories.size(); i++) {
                globalSupervisor->addSTaskCategoryToDownstreamNodeMapping(
                        sender, staskCategoryToParentMap[_staskCategories[i]]);
            }
        }
        // resolve the parent names to IP addresses
        globalSupervisor->resolveDownstreamNodeIPs();
    }

    for (it = allocationMap.begin(); it != allocationMap.end(); ++it) {
        std::vector<cModule *>::iterator it2;
        std::vector<cModule *> tmp = it->second;

        for (it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
            cModule *_module = check_and_cast<cModule *>(*it2);
            _module->buildInside();
        }
    }

    // multi-stage init
    bool more = true;
    for (int stage = 0; more; stage++) {
        more = false;
        for (it = allocationMap.begin(); it != allocationMap.end(); ++it) {
            std::vector<cModule *>::iterator it2;
            std::vector<cModule *> tmp = it->second;

            for (it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
                cModule *_module = check_and_cast<cModule *>(*it2);
                if (_module->callInitialize(stage)) {
                    more = true;
                }
            }
        }
    }
}
}

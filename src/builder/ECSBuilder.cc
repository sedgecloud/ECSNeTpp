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

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "ECSBuilder.h"

using namespace tinyxml2;
using namespace omnetpp;

namespace ecsnetpp {

Define_Module(ECSBuilder);

struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return std::strcmp(a, b) < 0;
    }
};

void ECSBuilder::initialize() {
    hasGlobalSupervisor = getAncestorPar("hasGlobalSupervisor").boolValue();
    if (hasGlobalSupervisor) {
        globalSupervisor = check_and_cast<GlobalStreamingSupervisor *>(getParentModule()->getModuleByPath(".globalSupervisorNode.globalSupervisor"));
    }
    ackersEnabled = getAncestorPar("ackersEnabled").boolValue();
    // build the network in event 1, because it is undefined whether the simkernel
    // will implicitly initialize modules created *during* initialization, or this needs
    // to be done manually.
    scheduleAt(simTime() + 1, new cMessage());
}

void ECSBuilder::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage())
        throw cRuntimeError("This module does not process messages.");

    delete msg;
    executeAllocationPlan(getParentModule());
}

void ECSBuilder::connect(cGate *src, cGate *dest, double delay, double ber, double datarate) {
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

void ECSBuilder::executeAllocationPlan(cModule *parent) {
    const char* allocationFileName = par("allocationPlanFile").stringValue();
    const char* STREAMING_SOURCE_NAME = "ecsnetpp.stask.StreamingSource";
    const char* STREAMING_OPERATOR_NAME = "ecsnetpp.stask.StreamingOperator";
    std::string line;

    // maps needed to create connections between tasks
    // all in memory
    std::map<std::string, std::vector<cModule *>> allocationMap;
    std::map<std::string, int> incomingConnectionsCountMap;
    std::map<std::string, int> outgoingConnectionsCountMap;
    std::map<std::string, std::map<std::string, int>> incomingConnectionsUsageMap;
    std::map<std::string, std::map<std::string, int>> outgoingConnectionsUsageMap;
    std::map<std::string, std::string> staskNameToCategoryMap;
    std::map<std::string, std::vector<std::string>> staskDownstreamCategoryToSenderMap;
    std::map<std::string, std::vector<std::string>> staskSenderToDownstreamStaskCategoryMap;
    std::map<std::string, std::vector<std::string>> staskCategoryToParentMap;
    std::map<std::string, std::string> staskNameToSendersStringMap;

    XMLDocument doc;
    XMLError error = doc.LoadFile(allocationFileName);
    if (error != tinyxml2::XML_SUCCESS) {
        throw cRuntimeError("Unable to read the xml file : %s", allocationFileName);
    }

    XMLElement* root = doc.FirstChildElement("devices");
    if (root == nullptr) {
        throw cRuntimeError("Malformed XML! Root is null.");
    }
    XMLElement* device = root->FirstChildElement("device");

    while (device != nullptr) {
        const char* parentName = device->FirstChildElement("name")->GetText();
        const char* indexRange = device->FirstChildElement("index-range")->GetText();

        std::vector<int> deviceIndices = cStringTokenizer(indexRange, "..").asIntVector();

        // iterate per each task
        XMLElement* tasks = device->FirstChildElement("tasks");
        XMLElement* task = tasks->FirstChildElement("task");
        while (task != nullptr) {
            const char* staskName = task->FirstChildElement("name")->GetText();
            const char* staskCategory = task->FirstChildElement("category")->GetText();
            const char* staskType = task->FirstChildElement("type")->GetText();
            bool delayInCpuCycles = false;
            bool delayInTime = false;
            const char* delay;

            cBoolParImpl* isProcessingDelayInCpuCyclesPar = new cBoolParImpl();
            isProcessingDelayInCpuCyclesPar->setName("isProcessingDelayInCpuCycles");
            isProcessingDelayInCpuCyclesPar->setBoolValue(false);

            // configure processing delay for each operator
            // TODO: Setup per-processor-architecture delay configurations
            XMLElement* processingDelayXmlElement = task->FirstChildElement("processingdelay");
            if (processingDelayXmlElement != nullptr) {
                XMLElement* delayElement = processingDelayXmlElement->FirstChildElement("cpucycles");
                if (delayElement != nullptr) {
                    delayInCpuCycles = true;
                    isProcessingDelayInCpuCyclesPar->setBoolValue(true);
                    delay = delayElement->GetText();
                } else {
                    delayElement = processingDelayXmlElement->FirstChildElement("measuredtime");
                    if (delayElement != nullptr) {
                        delayInCpuCycles = false;
                        delay = delayElement->GetText();
                    }
                }
            }

            if (deviceIndices.size() == 1) {
                deviceIndices[1] = deviceIndices[0];
            }

            for (int i = deviceIndices[0]; i <= deviceIndices[1]; i++) {
                //TODO: Take this out from the loop - reads the xml per each iteration
                cModule* _parent = getParentModule()->getSubmodule(parentName, i);
                if (nullptr == _parent) {
                    std::cout << "Node " << parentName << "[" << i << "] is not present in the network." << endl;
                    continue;
                }

                std::stringstream ss;
                ss << staskName << i;
                const char* newStaskName = ss.str().c_str();

                cModuleType *modtype = cModuleType::find(staskType);
                if (!modtype) {
                    throw cRuntimeError("Module type `%s' for node `%s' not found", staskType, newStaskName);
                }
                cModule *stask = modtype->create(newStaskName, _parent);

//                if (delayInCpuCycles) {
//                    std::cout << "$$$$$$$$$$$DELAY=" << delayInCpuCycles << endl;
//                    cDoubleParImpl *cyclesPerEvent = new cDoubleParImpl();
//                    cyclesPerEvent->setName("cyclesPerEvent");
//                    cyclesPerEvent->setUnit("MHz");
//                    cyclesPerEvent->setDoubleValue(atof(delay));
//                    stask->addPar(cyclesPerEvent);
//                } else {
//                    std::cout << "##########DELAY=" << delayInCpuCycles << endl;
//                    cDoubleParImpl *processingTimePerEvent = new cDoubleParImpl();
//                    processingTimePerEvent->setName("processingDelayPerEvent");
//                    processingTimePerEvent->setUnit("ns");
//                    processingTimePerEvent->setDoubleValue(atof(delay));
//                    stask->addPar(processingTimePerEvent);
//                }

                cDoubleParImpl *processingDelayPerEvent = new cDoubleParImpl();
                processingDelayPerEvent->setName("processingDelayPerEvent");
                processingDelayPerEvent->setDoubleValue(atof(delay));
                stask->addPar(processingDelayPerEvent);

                // add previously created boolean par
                stask->addPar(isProcessingDelayInCpuCyclesPar);

                cStringParImpl *mySTaskCategory = new cStringParImpl();
                mySTaskCategory->setName("mySTaskCategory");
                mySTaskCategory->setStringValue(staskCategory);
                stask->addPar(mySTaskCategory);

                if (strncmp(STREAMING_SOURCE_NAME, staskType, strlen(STREAMING_SOURCE_NAME)) == 0) {
                    // enable distributions for source message size
                    setupDistribution(task, "msgsizedistribution", "isSourceMsgSizeDistributed", "mySourceMsgSizeDistributionModuleName", stask,
                            _parent, "msgsize", "msgSize");
                    // enable distributions for source event rate
                    setupDistribution(task, "sourceevdistribution", "isSourceEvRateDistributed", "mySourceEvRateDistributionModuleName", stask,
                            _parent, "eventrate", "eventRate");

                } else if (strncmp(STREAMING_OPERATOR_NAME, staskType, strlen(STREAMING_OPERATOR_NAME)) == 0) {
                    // enable distributions for operator selectivity
                    setupDistribution(task, "selectivitydistribution", "isOperatorSelectivityDistributed",
                            "myOperatorSelectivityDistributionModuleName", stask, _parent, "selectivity", "selectivityRatio");
                    // enable distributions for operator productivity
                    setupDistribution(task, "productivitydistribution", "isOperatorProductivityDistributed",
                            "myOperatorProductivityDistributionModuleName", stask, _parent, "productivity", "productivityRatio");
                }

                std::string _parentName = _parent->getFullPath();
                allocationMap[_parentName].push_back(stask);
                incomingConnectionsCountMap[staskCategory] = 0;
                outgoingConnectionsCountMap[staskCategory] = 0;
                incomingConnectionsUsageMap[_parentName][newStaskName] = 0;
                outgoingConnectionsUsageMap[_parentName][newStaskName] = 0;
                staskNameToCategoryMap[newStaskName] = staskCategory;
//                std::cout << "sname->scat map" << newStaskName << ":" << staskCategory << endl;
                staskCategoryToParentMap[staskCategory].push_back(_parentName);
//                std::cout << "scat->parent map" << staskCategory << ":" << _parentName << endl;
            }

            task = task->NextSiblingElement("task");
        }
        device = device->NextSiblingElement("device");
    }

    std::map<std::string, std::map<std::string, bool>> connectedSTasks;

    // read and create connections
    std::fstream connectionsFile(par("dspTopologyFile").stringValue(), std::ios::in);
    while (getline(connectionsFile, line, '\n')) {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError("wrong line in parameters file: 3 items required, line: \"%s\"", line.c_str());

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
            std::string _srcSTaskCategory = staskNameToCategoryMap[_src->getFullName()];

            std::stringstream ss;
            for (size_t i = 0; i < staskDownstreamCategoryToSenderMap[_srcSTaskCategory].size(); ++i) {
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

                std::string _destSTaskCategory = staskNameToCategoryMap[_dest->getFullName()];

                if (_src->getFullName() != _dest->getFullName()) {

                    if (connectedSTasks[_srcSTaskCategory][_destSTaskCategory]) {
                        cGate *srcOut, *destIn;

                        srcOut = _src->getOrCreateFirstUnconnectedGate("outgoingStream", 0, false, true);
                        destIn = _dest->getOrCreateFirstUnconnectedGate("incomingStream", 0, false, true);

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
        cModule *_parent = getParentModule()->getModuleByPath(_parentName.c_str());
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
                taskIn = _src->getOrCreateFirstUnconnectedGate("incomingStream", 0, false, true);
                supervisorOut = supervisor->getOrCreateFirstUnconnectedGate("streamingPortOut", 0, false, true);
                connect(supervisorOut, taskIn, -1, -1, -1);
                incomingConnectionsUsageMap[_parentName][_src->getFullName()] += 1;
            }

            while (outgoingConnectionsUsageMap[_parentName][_src->getFullName()]
                    < outgoingConnectionsCountMap[staskNameToCategoryMap[_src->getFullName()]]) {
                taskOut = _src->getOrCreateFirstUnconnectedGate("outgoingStream", 0, false, true);
                supervisorIn = supervisor->getOrCreateFirstUnconnectedGate("streamingPortIn", 0, false, true);
                connect(taskOut, supervisorIn, -1, -1, -1);
                outgoingConnectionsUsageMap[_parentName][_src->getFullName()] += 1;
            }

            if (ackersEnabled) {
                cGate *srcToAcker, *supToAcker;
                srcToAcker = _src->gate("ackerOut");
                supToAcker = supervisor->getOrCreateFirstUnconnectedGate("sendToAcker", 0, false, true);
                connect(srcToAcker, supToAcker, -1, -1, -1);
            }

        }   // end of per each stask per parent->vector

        StreamingSupervisor *_supervisor = check_and_cast<StreamingSupervisor *>(_parent->getSubmodule("supervisor"));

        // add the sender->parent_node_fullname mapping
        std::map<std::string, std::vector<std::string>>::iterator _senderIt;
        for (_senderIt = staskSenderToDownstreamStaskCategoryMap.begin(); _senderIt != staskSenderToDownstreamStaskCategoryMap.end(); ++_senderIt) {
            std::string sender = _senderIt->first;
            std::vector<std::string> _staskCategories = _senderIt->second;

            for (size_t i = 0; i < _staskCategories.size(); i++) {
                _supervisor->addSTaskCategoryToDownstreamNodeMapping(sender, staskCategoryToParentMap[_staskCategories[i]]);
            }
        }
        // resolve the parent names to IP addresses
        _supervisor->resolveDownstreamNodeIPs();
    }

    if (hasGlobalSupervisor) {
        // add the sender->parent_node_fullname mapping
        std::map<std::string, std::vector<std::string>>::iterator _senderIt;
        for (_senderIt = staskSenderToDownstreamStaskCategoryMap.begin(); _senderIt != staskSenderToDownstreamStaskCategoryMap.end(); ++_senderIt) {
            std::string sender = _senderIt->first;
            std::vector<std::string> _staskCategories = _senderIt->second;

            for (size_t i = 0; i < _staskCategories.size(); i++) {
                globalSupervisor->addSTaskCategoryToDownstreamNodeMapping(sender, staskCategoryToParentMap[_staskCategories[i]]);
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

void ECSBuilder::setupDistribution(XMLElement* task, const char* taskDistributionXmlElementName, const char* isDistributionEnabledBoolVarName,
        const char* distributionModuleName, cModule* stask, cModule* _parent, const char* nonDistributedValueXmlElementName,
        const char* nonDistributedValueVarName) {

    XMLElement* taskDistributionXmlElement = task->FirstChildElement(taskDistributionXmlElementName);

    cBoolParImpl *isDistributionEnabledBoolPar = new cBoolParImpl();
    isDistributionEnabledBoolPar->setName(isDistributionEnabledBoolVarName);
    isDistributionEnabledBoolPar->setBoolValue(false);

    if (taskDistributionXmlElement != nullptr) {
        const char* distName = taskDistributionXmlElement->FirstChildElement("name")->GetText();
        const char* distType = taskDistributionXmlElement->FirstChildElement("type")->GetText();

        isDistributionEnabledBoolPar->setBoolValue(true);

        cStringParImpl *distributionModule = new cStringParImpl();
        distributionModule->setName(distributionModuleName);
        distributionModule->setStringValue(distName);
        stask->addPar(distributionModule);

        cModuleType *distModType = cModuleType::find(distType);
        if (!distModType) {
            throw cRuntimeError("Module type `%s' not found", distType);
        }
        cModule *dist = distModType->create(distName, _parent);

        XMLElement* distValues = taskDistributionXmlElement->FirstChildElement("values");
        if (distValues != nullptr) {
            XMLElement* distValueElement = distValues->FirstChildElement();

            while (distValueElement != nullptr) {
                const char* distValue = distValueElement->GetText();
                const char* distValueName = distValueElement->Name();

                cDoubleParImpl *distValuePar = new cDoubleParImpl();
                distValuePar->setName(distValueName);
                double __distValue = atof(distValue);
                distValuePar->setDoubleValue(__distValue);
                dist->addPar(distValuePar);

                distValueElement = distValueElement->NextSiblingElement();
            }
        }

        dist->buildInside();
        dist->callInitialize();
    } else {
        // if a distribution is not specified, read the fixed value for the parameter
        // TODO handle the scenario if this value is not present
        const char* nonDistributedValue = task->FirstChildElement(nonDistributedValueXmlElementName)->GetText();
        cDoubleParImpl *nonDistributedValuePar = new cDoubleParImpl();
        nonDistributedValuePar->setName(nonDistributedValueVarName);
        nonDistributedValuePar->setDoubleValue(atof(nonDistributedValue));
        stask->addPar(nonDistributedValuePar);
    }
    stask->addPar(isDistributionEnabledBoolPar);
}

} /* namespace ecsnetpp */

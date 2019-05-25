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

#ifndef BUILDER_ECSBUILDER_H_
#define BUILDER_ECSBUILDER_H_

#include <omnetpp.h>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include "../global/GlobalStreamingSupervisor.h"
#include "../stask/StreamingSupervisor.h"

using namespace tinyxml2;

namespace ecsnet {

class ECSBuilder : public cSimpleModule {
//public:
//    ECSBuilder();
//    virtual ~ECSBuilder();
protected:
    GlobalStreamingSupervisor *globalSupervisor;
    bool ackersEnabled;
    bool hasGlobalSupervisor;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void connect(cGate *src, cGate *dest, double delay, double ber, double datarate);
    void executeAllocationPlan(cModule *parent);
    void setupDistribution(XMLElement* task, const char* taskDistributionXmlElementName, const char* isDistributionEnabledBoolVarName,
            const char* distributionModuleName, cModule* stask, cModule* _parent, const char* nonDistributedValueXmlElementName,
            const char* nonDistributedValueVarName);
};

} /* namespace ecsnet */

#endif /* BUILDER_ECSBUILDER_H_ */

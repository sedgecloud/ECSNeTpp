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

#ifndef STASK_STREAMINGOPERATOR_H_
#define STASK_STREAMINGOPERATOR_H_

#include "omnetpp.h"
#include "../msg/StreamingMessage_m.h"
#include "ISTask.h"
#include "../model/operator/selectivity/IOperatorSelectivityDistribution.h"
#include "../model/operator/productivity/IOperatorProductivityDistribution.h"

using namespace omnetpp;

namespace ecsnetpp {

class StreamingOperator : public ISTask{
private:
    int selectivityWindowLength;
    int selectivityWindowCount;
protected:
    cQueue outgoingQueue;
    cQueue selectivityWindow;
    cMessage *timerMsg = nullptr;
    StreamingMessage* nextToSend;
    double selectivityRatio = 0;   // number of outgoing messages to be generated per each incoming message
    double productivityRatio = 0;  // sizeof(outgoing msg)/sizeof(incoming msg)
    const char* myOperatorSelectivityDistributionModuleName;
    const char* myOperatorProductivityDistributionModuleName;
    bool isOperatorSelectivityDistributed = false;
    bool isOperatorProductivityDistributed = false;
    IOperatorSelectivityDistribution* myOperatorSelectivityDistributionModule;
    IOperatorProductivityDistribution* myOperatorProductivityDistributionModule;
public:
////    StreamingTask();
//    virtual ~StreamingOperator();
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual double getSelectivityRatio();
    virtual double getSelectivityWindowLength();
    virtual double getProductivityRatio();
};

} /* namespace ecsnetpp */

#endif /* STASK_STREAMINGOPERATOR_H_ */

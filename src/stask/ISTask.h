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

#ifndef STASK_ISTASK_H_
#define STASK_ISTASK_H_

#include "omnetpp.h"
#include "../msg/StreamingMessage_m.h"
#include "../msg/Ack_m.h"
#include "../power/States.h"
#include "../cpu/scheduling/ICpuCoreScheduler.h"

using namespace omnetpp;

namespace ecsnet {

class ISTask: public cSimpleModule {
public:
    static simsignal_t receivedStreamingMsgsSignal;
    static simsignal_t cpuUtilisationSignal;
    static simsignal_t cpuStateChangedSignal;
    static simsignal_t packetGeneratedSignal;
    static simsignal_t latencySignal;
    static simsignal_t transmissionTimeSignal;
    static simsignal_t processingTimeSignal;
    static simsignal_t edgeProcessingTimeSignal;

protected:
    const char* mySenders;
    const char* mySTaskCategory;
    std::vector<std::string> mySendersList;
    long cpuCores;
    long lastCPUIndex = 0;
    double perCoreFreq;
    double processingDelayPerEvent;
    double delay;
    bool ackersEnabled;
    bool isProcessingDelayInCpuCycles;
    ICpuCoreScheduler* myCpuCoreScheduler;
protected:
    virtual void initialize() {
    }
    ;
    virtual void handleMessage(cMessage *msg) {
    }
    ;
    virtual long getNextProcessorCoreIndex();
    virtual void sendAck(cMessage* msg);
    virtual void publishCpuStateChanged(States::CPUState state);
};

} /* namespace ecsnet */

#endif /* STASK_ISTASK_H_ */

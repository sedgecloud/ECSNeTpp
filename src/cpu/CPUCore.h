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

#ifndef CPU_CPUCORE_H_
#define CPU_CPUCORE_H_

#include "omnetpp.h"
#include "../msg/StreamingMessage_m.h"

using namespace omnetpp;

namespace ecsnet {

class CPUCore : public cSimpleModule{
protected:
    std::map<long, cQueue> processingQueueMap;
//    cMessage *timerMsg = nullptr;
    cMessage *index;
    double perCoreFreq;
    double parallelisationFactor;
    long threadsPerCore;
    long totalCores;
    double delay;
    bool ackersEnabled;
    bool isOnEdgeDevice;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual omnetpp::SimTime calculateDelay(bool isProcessingDelayInCpuCycles, double processingDelay, long threads);
};

} /* namespace ecsnet */

#endif /* CPU_CPUCORE_H_ */

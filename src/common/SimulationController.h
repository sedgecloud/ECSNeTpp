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

#ifndef COMMON_SIMULATIONCONTROLLER_H_
#define COMMON_SIMULATIONCONTROLLER_H_

#include "omnetpp.h"
#include "../stask/ISTask.h"

using namespace omnetpp;

namespace ecsnet{

class SimulationController  :  public cSimpleModule, public cListener{
protected:
    long packetCountLimit;
    long packetCount = 0;
    long sourcePacketCount = 0;
    bool enableLimitFromSource;
    bool stopEventGeneration;
    bool stopEdgeIdleEnergyRecording;
    simtime_t startTime = -1;
public:
    static simsignal_t totalSimTimeChangedSignal;
public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool getStopEventGeneration();
    virtual bool getStopEdgeIdleEnergyRecording();
//    SimulationController();
//    virtual ~SimulationController();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details) override;
};

} /* namespace ecsnet */

#endif /* COMMON_SIMULATIONCONTROLLER_H_ */

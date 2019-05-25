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

#include "ISTask.h"

namespace ecsnet {

simsignal_t ISTask::receivedStreamingMsgsSignal = registerSignal("receivedStreamingMsgs");
simsignal_t ISTask::cpuUtilisationSignal = registerSignal("cpuUtilisationSignal");
simsignal_t ISTask::cpuStateChangedSignal = registerSignal("cpuStateChanged");
simsignal_t ISTask::latencySignal = registerSignal("latency");
simsignal_t ISTask::transmissionTimeSignal = registerSignal("transmissionTime");
simsignal_t ISTask::processingTimeSignal = registerSignal("processingTime");
simsignal_t ISTask::edgeProcessingTimeSignal = registerSignal("edgeProcessingTime");

void ISTask::sendAck(cMessage* msg) {
//     send ack
    if(ackersEnabled){
        Ack* ack = new Ack();
        ack->setAckedMessageId(msg->getId());
        ack->setSender(getFullPath().c_str());
        send(ack, "ackerOut");
    }
}

void ISTask::publishCpuStateChanged(States::CPUState state){
    emit(cpuStateChangedSignal, state);
}

long ISTask::getNextProcessorCoreIndex() {
    return myCpuCoreScheduler->getNextCPUCoreIndex();
}

} /* namespace ecsnet */

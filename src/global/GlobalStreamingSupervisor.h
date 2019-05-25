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

#ifndef GLOBAL_GLOBALSTREAMINGSUPERVISOR_H_
#define GLOBAL_GLOBALSTREAMINGSUPERVISOR_H_

#include "omnetpp.h"
#include "../msg/StreamingMessage_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

//using namespace inet;
using namespace omnetpp;

namespace ecsnetpp {

class GlobalStreamingSupervisor: public cSimpleModule {
private:
    inet::UDPSocket socket;
    cMessage *bindMsg = nullptr;
    std::map<std::string, std::vector<std::string>> senderStaskCategoryToDownstreamNodeMap;
    std::map<std::string, std::vector<inet::L3Address>> senderStaskCategoryToDownstreamNodeIPMap;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    virtual void addSTaskCategoryToDownstreamNodeMapping(
            std::string senderSTaskCategory,
            std::string downstreamNodeFullPath);
    virtual void addSTaskCategoryToDownstreamNodeMapping(
            std::string senderSTaskCategory,
            std::vector<std::string> downstreamNodeFullPaths);
    virtual void resolveDownstreamNodeIPs();
//    GlobalStreamingSupervisor();
//    virtual ~GlobalStreamingSupervisor();
};

} /* namespace ecsnetpp */

#endif /* GLOBAL_GLOBALSTREAMINGSUPERVISOR_H_ */

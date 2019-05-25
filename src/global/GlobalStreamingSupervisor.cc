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

#include "GlobalStreamingSupervisor.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace ecsnetpp {

Define_Module(GlobalStreamingSupervisor);

void GlobalStreamingSupervisor::initialize() {
    socket.setOutputGate(gate("udpOut"));
    bindMsg = new cMessage("UDP_C_BIND", inet::UDP_C_BIND);
    inet::UDPBindCommand *ctrl2 = new inet::UDPBindCommand();
    int socketId = inet::UDPSocket::generateSocketId();
    ctrl2->setSockId(socketId);
    ctrl2->setLocalPort(1000);
    bindMsg->setControlInfo(ctrl2);
    send(bindMsg, "udpOut");
}

void GlobalStreamingSupervisor::handleMessage(cMessage *msg) {
    if (!msg->isSelfMessage()) {
        StreamingMessage *msgToSend = check_and_cast<StreamingMessage *>(msg);
        std::string sender = msgToSend->getSender();
        std::vector<inet::L3Address> _downstreamNodes =
                senderStaskCategoryToDownstreamNodeIPMap[sender];
        for (size_t i = 0; i < _downstreamNodes.size(); i++) {
            socket.connect(_downstreamNodes[i], 1000);

            socket.send(msgToSend->dup());
            socket.close();
        }
        delete msgToSend;
    }
}

void GlobalStreamingSupervisor::addSTaskCategoryToDownstreamNodeMapping(
        std::string senderSTaskCategory, std::string downstreamNodeFullPath) {
    senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].push_back(
            downstreamNodeFullPath);
}

void GlobalStreamingSupervisor::addSTaskCategoryToDownstreamNodeMapping(
        std::string senderSTaskCategory,
        std::vector<std::string> downstreamNodeFullPaths) {
    senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].insert(
            senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].end(),
            downstreamNodeFullPaths.begin(), downstreamNodeFullPaths.end());
}

void GlobalStreamingSupervisor::resolveDownstreamNodeIPs() {
    std::map<std::string, std::vector<std::string>>::iterator it;
    for (it = senderStaskCategoryToDownstreamNodeMap.begin();
            it != senderStaskCategoryToDownstreamNodeMap.end(); ++it) {
        std::vector<std::string> _downstreamNodes = it->second;
        std::string _staskCategory = it->first;
        senderStaskCategoryToDownstreamNodeIPMap[_staskCategory] =
                inet::L3AddressResolver().resolve(_downstreamNodes);
    }
}

} /* namespace ecsnetpp */

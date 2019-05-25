/*
 * ConsoleSink.cc
 *
 *  Created on: Sep 26, 2017
 *      Author: gayashan
 */

#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/transportlayer/contract/tcp/TCPSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "inet/transportlayer/contract/tcp/TCPCommand_m.h"
#include "../msg/StreamingMessage_m.h"
#include "../msg/Ack_m.h"
#include "StreamingSupervisor.h"

namespace ecsnetpp {

simsignal_t StreamingSupervisor::sentPkSize = registerSignal("sentPkSize");
simsignal_t StreamingSupervisor::completedMIPS = registerSignal(
        "completedMIPS");

Define_Module(StreamingSupervisor);

StreamingSupervisor::~StreamingSupervisor() {
    cancelAndDelete(selfMsg);
}

void StreamingSupervisor::initialize() {
    cloudAddress = getAncestorPar("cloudAddress").stringValue();
    joinMulticastGroup = par("joinMulticastGroup").boolValue();
    ackersEnabled = getAncestorPar("ackersEnabled").boolValue();
    checkpointsEnabled = false; //TODO: Checkpointing
    hasUdp = getParentModule()->par("hasUdp").boolValue();
    hasTcp = getParentModule()->par("hasTcp").boolValue();
    if (hasUdp) {
        udpSocket.setOutputGate(gate("udpOut"));
    } else if (hasTcp) {
        serverSocket.setOutputGate(gate("tcpOut"));
        serverSocket.readDataTransferModePar(*this);
        serverSocket.bind(1000);
        serverSocket.listen();
    }
    startTime = simTime();
    count = 0;
    selfMsg = new cMessage();
    scheduleAt(simTime(), selfMsg);
}

void StreamingSupervisor::processSelfMessage() {
    if (hasUdp) {
        bindMsg = new cMessage("UDP_C_BIND", inet::UDP_C_BIND);
        inet::UDPBindCommand* ctrl2 = new inet::UDPBindCommand();
        int socketId = inet::UDPSocket::generateSocketId();
        ctrl2->setSockId(socketId);
        //        ctrl2->setLocalAddr(inet::L3AddressResolver().resolve("225.0.0.1"));
        ctrl2->setLocalPort(1000);
        bindMsg->setControlInfo(ctrl2);
        send(bindMsg, "udpOut");
        // Join the multicast group only if the node connects to a sink
        if (joinMulticastGroup) {
            joinMCastMsg = new cMessage("UDP_C_SETOPTION", inet::UDP_C_SETOPTION);
            inet::UDPJoinMulticastGroupsCommand* ctrl = new inet::UDPJoinMulticastGroupsCommand();
            ctrl->setSockId(socketId);
            ctrl->setMulticastAddrArraySize(1);
            ctrl->setMulticastAddr(0, inet::L3AddressResolver().resolve(cloudAddress));
            joinMCastMsg->setControlInfo(ctrl);
            send(joinMCastMsg, "udpOut");
        }
    } else if (hasTcp) {
        //            tcpSocket.bind(inet::L3Address(), 1000);
        //            tcpSocket.listen();
    }
}

void StreamingSupervisor::processUDPMessage(cMessage* msg) {
    if (!msg->arrivedOn("udpIn")) {
        if (!msg->arrivedOn("sendToAcker")) {
            StreamingMessage* msgToSend = check_and_cast<StreamingMessage*>(msg);
            std::string sender = msgToSend->getSender();
            std::vector<inet::L3Address> _downstreamNodes = senderStaskCategoryToDownstreamNodeIPMap[sender];
            for (size_t i = 0; i < _downstreamNodes.size(); i++) {
                udpSocket.connect(_downstreamNodes[i], 1000);
                udpSocket.send(msgToSend->dup());
                udpSocket.close();
            }
            delete msgToSend;
        } else {
            const char* ackerAddress = getAncestorPar("ackerAddress").stringValue();
            udpSocket.connect(inet::L3AddressResolver().resolve(ackerAddress), 1000);
            Ack* ack = check_and_cast<Ack*>(msg);
            udpSocket.send(ack);
            udpSocket.close();
        }
    } else {
        // if the acker is disabled
        //            StreamingMessage *_msg = check_and_cast<StreamingMessage *>(msg);
        //            EV << "RCVD from=" << _msg->getSender() << _msg->getSenderModule()->getFullPath() << endl;
        //            std::cout << "RCVD from=" << _msg->getSender() << _msg->getSenderModule()->getFullPath() << endl;
        if (!ackersEnabled) {
            for (int i = 0; i < gateSize("streamingPortOut"); i++) {
                send(msg->dup(), "streamingPortOut", i);
            }
        } else {
            Ack* ack = dynamic_cast<Ack*>(msg);
            if (nullptr == ack) {
                for (int i = 0; i < gateSize("streamingPortOut"); i++) {
                    send(msg->dup(), "streamingPortOut", i);
                }
            } else {
                for (int i = 0; i < gateSize("ackerOut"); i++) {
                    send(ack->dup(), "ackerOut", i);
                }
            }
        }
        delete msg;
    }
}

void StreamingSupervisor::processTCPMessage(cMessage* msg) {
    if (!msg->arrivedOn("tcpIn")) {
        if (!msg->arrivedOn("sendToAcker")) {
            StreamingMessage* msgToSend = check_and_cast<StreamingMessage*>(msg);
            msgToSend->setChannelIngressTime(simTime());
//            std::cout << getParentModule()->getFullPath() << " Proc delay: " << msgToSend->getProcessingDelay() << endl;
            std::string sender = msgToSend->getSender();
            std::vector<inet::L3Address> _downstreamNodes = senderStaskCategoryToDownstreamNodeIPMap[sender];
//            std::cout << ">>>>>>>>>>>>>TEST" << msg->getKind() << " Sender=" << sender << " recpts="<< _downstreamNodes.size() << endl;
            for (size_t i = 0; i < _downstreamNodes.size(); i++) {
                inet::TCPSocket* sock = destinationSocketMap[_downstreamNodes[i]];
                if (!sock) {
//                std::cout << "RR=" << _downstreamNodes[i] << endl;
                    sock = new inet::TCPSocket();
                    sock->setOutputGate(gate("tcpOut"));
                    sock->readDataTransferModePar(*this);
                    sock->setCallbackObject(this, nullptr);
                    tcpSocketMap.addSocket(sock);
                    destinationSocketMap[_downstreamNodes[i]] = sock;
                    sock->connect(_downstreamNodes[i], 1000);
                }
//                std::cout << "Sending: " << msgToSend->getByteLength() << endl;
                sock->send(msgToSend->dup());
            }
            delete msgToSend;
        }
    } else {
        inet::TCPSocket* socket = tcpSocketMap.findSocketFor(msg);
        if (!socket) {
            socket = new inet::TCPSocket(msg);
            socket->setOutputGate(gate("tcpOut"));
            socket->readDataTransferModePar(*this);
            socket->setCallbackObject(this, nullptr);
            tcpSocketMap.addSocket(socket);
        }
        socket->processMessage(msg);
    }
}

void StreamingSupervisor::handleMessage(cMessage *msg) {
//    std::cout << "Node=" << getFullPath() <<" KIND= " << msg->getKind() << endl;
    if (msg->isSelfMessage()) {
        processSelfMessage();
    } else if (msg->getKind() == inet::UDP_I_DATA  && hasUdp) {
        processUDPMessage(msg);
    } else if (msg->getKind() == inet::UDP_I_ERROR && hasUdp) {
        processUDPError(msg);
    } else if (hasTcp) {
        processTCPMessage(msg);
    } else {
        delete msg;
    }
}

void StreamingSupervisor::socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent) {
//    StreamingMessage *pk = check_and_cast<StreamingMessage *>(msg);
//    std::cout << "Received TCP data, " << msg->getByteLength() << " Length:" << pk->getByteLength() << " bytes" << endl;
//    std::cout << getParentModule()->getFullPath() << "Proc delay: " << pk->getProcessingDelay() << endl;
    if (!ackersEnabled && !checkpointsEnabled) {
        StreamingMessage* msgToSend = check_and_cast<StreamingMessage*>(msg);
        const omnetpp::SimTime _networkDelay = simTime() - msgToSend->getChannelIngressTime();
        msgToSend->setNetworkDelay(_networkDelay.dbl());
        for (int i = 0; i < gateSize("streamingPortOut"); i++) {
            send(msgToSend->dup(), "streamingPortOut", i);
        }
    } else {
        Ack *ack = dynamic_cast<Ack *>(msg);
        if (nullptr == ack) {
            for (int i = 0; i < gateSize("streamingPortOut"); i++) {
                send(msg->dup(), "streamingPortOut", i);
            }
        } else {
            for (int i = 0; i < gateSize("ackerOut"); i++) {
                send(ack->dup(), "ackerOut", i);
            }
        }
    }
    delete msg;
}

void StreamingSupervisor::socketFailure(int connId, void *yourPtr, int code) {
    if (code == inet::TCP_I_CONNECTION_RESET)
        EV << "Connection reset!\\n";
    else if (code == inet::TCP_I_CONNECTION_REFUSED)
        EV << "Connection refused!\\n";
    else if (code == inet::TCP_I_TIMED_OUT)
        EV << "Connection timed out!\\n";
}

void StreamingSupervisor::processUDPPacket(cMessage *msg) {
    for (int i = 0; i < gateSize("streamingPortOut"); i++) {
        send(msg->dup(), "streamingPortOut", i);
    }
}

void StreamingSupervisor::processUDPError(cMessage *error) {
    EV << "ERROR " << error->getName() << endl;
}

void StreamingSupervisor::addSTaskCategoryToDownstreamNodeMapping(
        std::string senderSTaskCategory, std::string downstreamNodeFullPath) {
    senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].push_back(
            downstreamNodeFullPath);
}

void StreamingSupervisor::addSTaskCategoryToDownstreamNodeMapping(
        std::string senderSTaskCategory,
        std::vector<std::string> downstreamNodeFullPaths) {
    senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].insert(
            senderStaskCategoryToDownstreamNodeMap[senderSTaskCategory].end(),
            downstreamNodeFullPaths.begin(), downstreamNodeFullPaths.end());
}

void StreamingSupervisor::resolveDownstreamNodeIPs() {
    std::map<std::string, std::vector<std::string>>::iterator it;
    for (it = senderStaskCategoryToDownstreamNodeMap.begin();
            it != senderStaskCategoryToDownstreamNodeMap.end(); ++it) {
        std::vector<std::string> _downstreamNodes = it->second;
        std::string _staskCategory = it->first;
        senderStaskCategoryToDownstreamNodeIPMap[_staskCategory] =
                inet::L3AddressResolver().resolve(_downstreamNodes);
    }
}

}

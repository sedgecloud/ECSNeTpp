/*
 * ConsoleSink.cc
 *
 *  Created on: Sep 26, 2017
 *      Author: gayashan
 */

#include "omnetpp.h"
#include "../msg/Ack_m.h"

using namespace omnetpp;

namespace ecsnetpp {

class Acker: public cSimpleModule {

protected:
    static simsignal_t rcvdAckSignal;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acker);

simsignal_t Acker::rcvdAckSignal = registerSignal("rcvdAck");

void Acker::initialize() {
//    cMessage *msg = new cMessage("Random Number");
//    scheduleAt(simTime() + 1, msg);
}

void Acker::handleMessage(cMessage *msg) {
    Ack *pk = check_and_cast<Ack *>(msg);
    EV << "ACK RCVD from=" << pk->getSender() << " for MSG=" << pk->getAckedMessageId()
              << endl;
    emit(rcvdAckSignal, pk);
    delete msg;
}

}

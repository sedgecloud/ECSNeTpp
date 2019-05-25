/*
 * TaskBuilder.h
 *
 *  Created on: Oct 24, 2017
 *      Author: gayashan
 */

#ifndef BUILDER_TASKBUILDER_H_
#define BUILDER_TASKBUILDER_H_

#include <omnetpp.h>
#include "../global/GlobalStreamingSupervisor.h"
#include "../stask/StreamingSupervisor.h"

using namespace omnetpp;
/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
namespace ecsnet {

class TaskBuilder: public cSimpleModule {
protected:
    GlobalStreamingSupervisor *globalSupervisor;
    bool ackersEnabled;
    bool hasGlobalSupervisor;
protected:
    void connect(cGate *src, cGate *dest, double delay, double ber,
            double datarate);
    void executeAllocationPlan(cModule *parent);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

private:
    void setStaskBoolPar(omnetpp::cModule* stask, const char* name, bool value);
};

}
#endif /* BUILDER_TASKBUILDER_H_ */

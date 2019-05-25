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

#ifndef __EDGECLOUDSTREAMINGSIMULATOR_FIXEDSOURCEEVENTRATEDISTRIBUTION_H_
#define __EDGECLOUDSTREAMINGSIMULATOR_FIXEDSOURCEEVENTRATEDISTRIBUTION_H_

#include <omnetpp.h>

#include "ISourceEventRateDistribution.h"

using namespace omnetpp;

namespace ecsnetpp {

/**
 * TODO - Generated class
 */
class FixedSourceEventRateDistribution : public ISourceEventRateDistribution
{
protected:
    double rate;
    double messageDelay;
  protected:
    virtual void initialize() override;
  public:
    virtual double getMessageDelay() override;
};

} //namespace

#endif

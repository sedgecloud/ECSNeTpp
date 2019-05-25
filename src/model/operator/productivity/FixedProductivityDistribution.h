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

#ifndef __EDGECLOUDSTREAMINGSIMULATOR_FIXEDPRODUCTIVITYDISTRIBUTION_H_
#define __EDGECLOUDSTREAMINGSIMULATOR_FIXEDPRODUCTIVITYDISTRIBUTION_H_

#include <omnetpp.h>
#include "IOperatorProductivityDistribution.h"

using namespace omnetpp;

namespace ecsnet {

/**
 * TODO - Generated class
 */
class FixedProductivityDistribution : public IOperatorProductivityDistribution
{
protected:
    double productivityRatio;
protected:
  virtual void initialize() override;
public:
  virtual double getProductivityRatio() override;
};

} //namespace

#endif

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

#ifndef CPU_SCHEDULING_ROUNDROBINCPUCORESCHEDULER_H_
#define CPU_SCHEDULING_ROUNDROBINCPUCORESCHEDULER_H_

#include <omnetpp.h>
#include "ICpuCoreScheduler.h"

using namespace omnetpp;

namespace ecsnetpp {

class RoundRobinCpuCoreScheduler : public ICpuCoreScheduler{
private:
    long cpuCores;
    long lastCPUIndex = 0;
protected:
  virtual void initialize() override;
public:
    virtual long getNextCPUCoreIndex() override;
};

} /* namespace ecsnetpp */

#endif /* CPU_SCHEDULING_ROUNDROBINCPUCORESCHEDULER_H_ */

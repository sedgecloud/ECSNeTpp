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

#include "UniformSourceEventRateDistribution.h"

namespace ecsnetpp {

Define_Module(UniformSourceEventRateDistribution);

void UniformSourceEventRateDistribution::initialize()
{
    begin = par("begin").doubleValue();
    end = par("end").doubleValue();
}

double UniformSourceEventRateDistribution::getMessageDelay(){
    return uniform(begin, end);
}

} //namespace

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

#include "FixedSelectivityDistribution.h"

namespace ecsnetpp {

Define_Module(FixedSelectivityDistribution);

void FixedSelectivityDistribution::initialize()
{
    selectivityRatio = par("selectivityratio").doubleValue();
    selectivityWindowLength = std::lround(1 / selectivityRatio);
}

double FixedSelectivityDistribution::getSelectivityRatio()
{
    return selectivityRatio;
}

double FixedSelectivityDistribution::getSelectivityWindowLength()
{
    return selectivityWindowLength;
}

} //namespace

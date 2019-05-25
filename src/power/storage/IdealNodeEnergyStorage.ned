//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package ecsnet.power.storage;

import inet.power.base.EpEnergyStorageBase;

//
// This energy storage model stores an infinite amount of energy. It can provide
// energy for any number of consumers, and it can absorb energy from any number
// of generators. The ideal energy storage never gets completely charged or
// depleted. This module is primarily useful for testing energy consumer and
// energy generator models. See the base module for signals and statistics.
//
// @author Levente Meszaros
//
simple IdealNodeEnergyStorage
{
    parameters:
        double idlePowerConsumption @unit(W) = default(0W);
        double nominalCapacity @unit(J); // the maximum amount of energy stored
        double initialCapacity @unit(J) = default(nominalCapacity); // the initially stored amount of energy
        @signal[energyConsumptionChanged](type=double);
        @signal[powerConsumptionChanged](type=double);
        @signal[idleEnergyConsumptionChanged](type=double);
//        @signal[idlePowerConsumptionChanged](type=double);
        @signal[cpuEnergyConsumptionChanged](type=double);
        @signal[networkEnergyConsumptionChanged](type=double);
        @signal[residualCapacityChanged](type=double);
        @signal[totalEnergySimTimeChanged](type=double);
		@statistic[energyConsumption](title="Total energy consumption"; unit=J; source=energyConsumptionChanged; record=last; interpolationmode=sample-hold);
		@statistic[powerConsumption](title="Total power consumption"; unit=W; source=powerConsumptionChanged; record=last; interpolationmode=sample-hold);
		@statistic[idleEnergyConsumption](title="Idle energy consumption"; unit=J; source=idleEnergyConsumptionChanged; record=last; interpolationmode=sample-hold);
//		@statistic[idlePowerConsumption](title="Idle power consumption"; unit=W; source=idlePowerConsumptionChanged; record=last; interpolationmode=sample-hold);
		@statistic[cpuEnergyConsumption](title="CPU energy consumption"; source=cpuEnergyConsumptionChanged; record=vector,last; interpolationmode=sample-hold);
		@statistic[networkEnergyConsumption](title="Network energy consumption"; source=networkEnergyConsumptionChanged; record=vector,last; interpolationmode=sample-hold);
        @statistic[residualCapacity](title="Residual Capacity"; source=residualCapacityChanged; record=vector; interpolationmode=sample-hold);
        @statistic[totalEnergySimTime](title="Total Energy sim time"; source=totalEnergySimTimeChanged; record=last; interpolationmode=sample-hold);
        
        @class(IdealNodeEnergyStorage);
}

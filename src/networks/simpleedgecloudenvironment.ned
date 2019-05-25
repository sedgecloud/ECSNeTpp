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

package ecsnet.networks;

import ecsnet.host.RaspberryPiModel2B;
import ecsnet.host.RaspberryPiModel3B;
import ecsnet.host.CloudNodeA;
import ecsnet.builder.TaskBuilder;
import ecsnet.builder.ECSBuilder;
import ecsnet.common.SimulationController;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.common.lifecycle.LifecycleController;
import inet.common.figures.DelegateSignalConfigurator;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EtherLink;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.internetcloud.InternetCloud;
import inet.node.wireless.AccessPoint;
import ned.DatarateChannel;

network SimpleEdgeCloudEnvironment
{
    parameters:
        double eToCDelayMean @unit(ms) = default(0ms);
        double eToCDelayStdev @unit(ms) = default(0ms);
        double eToCDatarate @unit(bps) = default(1Gbps);
        bool ackersEnabled = default(false);
        bool hasGlobalSupervisor = default(true);
        int numPiModel2Bs = default(4);
        int numPiModel3Bs = default(4);
        string mediumType = default("IdealRadioMedium");
        string cloudAddress = default("");
        @display("bgb=884.304,707.72797");
    types:
        channel E2C_low extends DatarateChannel
        {
            delay = 0;
            datarate = eToCDatarate;
        }

        channel E2C_high extends DatarateChannel
        {
            delay = eToCDelayMean;
            datarate = eToCDatarate;
        }
    submodules:
        pi2Bs[numPiModel2Bs]: RaspberryPiModel2B;
        pi3Bs[numPiModel3Bs]: RaspberryPiModel3B;
        stargazer3: CloudNodeA {
            @display("p=246.35199,131.008,r");
        }
        albacore5: Router {
            @display("p=331.792,263.44");
        }
        //        switch1: EtherSwitch {
        //            @display("p=371.664,226.416");
        //        }
        area1AP1: AccessPoint {
            @display("p=357.42398,380.208");
        }
        taskbuilder: ECSBuilder {
            @display("p=492.70398,39.871998");
        }
        simController: SimulationController {
            @display("p=783.2,39.871998");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=344.608,38.447998");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=193.664,38.447998");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=629.408,38.447998");
        }
        lifecycleController: LifecycleController {
            @display("p=24.208,41.295998");
        }
    connections:
        stargazer3.ethg++ <--> E2C_high <--> albacore5.ethg++;
        albacore5.ethg++ <--> E2C_low <--> area1AP1.ethg++;

}

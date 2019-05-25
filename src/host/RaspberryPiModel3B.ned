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

package ecsnet.host;
import ecsnet.stask.ISTask;
import ecsnet.stask.StreamingSupervisor;
import ecsnet.stask.Acker;
import ecsnet.cpu.CPUCore;
import inet.node.inet.StandardHost;
import inet.node.inet.AdhocHost;
import inet.node.inet.WirelessHost;
import inet.node.inet.NodeBase;
import inet.power.contract.IEnergyConsumer;
import ecsnet.power.consumer.CPUPowerConsumer;
import ecsnet.power.consumer.NetworkPowerConsumer;
import ecsnet.power.storage.IdealNodeEnergyStorage;
import ecsnet.cpu.scheduling.ICpuCoreScheduler;
import ecsnet.cpu.scheduling.RoundRobinCpuCoreScheduler;
import inet.power.contract.IEpEnergyConsumer;

module RaspberryPiModel3B extends WirelessHost
{
    parameters:
        int cores = default(4);
        double perCoreFreq @unit(Hz) = 1200000000Hz;
        int threadsPerCore = 1;
        double totalMemory @unit(Mb) = 1024Mb;
        double fixedSourceEventRate = default(0);
        bool hasAcker = default(false);
        bool isEdgeDevice = true;
        string ackerAddress = default("");
        string cpuCoreSchedulerType = default("RoundRobinCpuCoreScheduler");
		string energyConsumerType = default(""); // NED type of the energy consumer model
        @display("bgb=728.91003,656.82");
        @statistic[residualEnergyConsumption](title="Residual energy consumption"; source=residualEnergyCapacityChanged; record=last);
    gates:
        inout hostport[];
    submodules:
        cpuCore[cores]: CPUCore {
            @display("p=674.52,39.6,c");
        }
        supervisor: StreamingSupervisor;
        cpuCoreScheduler : <cpuCoreSchedulerType> like ICpuCoreScheduler if cpuCoreSchedulerType != "";
        acker: Acker if hasAcker;
        cpuPowerConsumer: CPUPowerConsumer;
        networkPowerConsumer: NetworkPowerConsumer;
        energyStore: IdealNodeEnergyStorage;
    connections allowunconnected:
        supervisor.udpIn <-- udp.appOut++ if hasUdp;
        supervisor.udpOut --> udp.appIn++ if hasUdp;
        supervisor.tcpIn <-- tcp.appOut++ if hasTcp;
        supervisor.tcpOut --> tcp.appIn++ if hasTcp;
        // connect supervisor to acker 
        // only if the acker is placed in this node
        supervisor.ackerOut++ --> acker.ackerIn if hasAcker;
}

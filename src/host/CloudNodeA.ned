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

package ecsnetpp.host;
import ecsnetpp.stask.ISTask;
import ecsnetpp.stask.StreamingSupervisor;
import ecsnetpp.stask.Acker;
import ecsnetpp.cpu.CPUCore;
import ecsnetpp.cpu.scheduling.ICpuCoreScheduler;
import ecsnetpp.cpu.scheduling.RoundRobinCpuCoreScheduler;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;
import inet.node.inet.NodeBase;

module CloudNodeA extends StandardHost
{
    parameters:
        // Intel Xeon Gold 6140, 2.30GHz
        int cores = default(1);
        double perCoreFreq @unit(Hz) = default(700000000Hz);
        int threadsPerCore = default(1);
        double totalMemory @unit(Mb) = default(16384Mb);
        double fixedSourceEventRate = default(0);
        bool hasAcker = default(false);
        string ackerAddress = default("");
        bool isEdgeDevice = false;
        string cpuCoreSchedulerType = default("RoundRobinCpuCoreScheduler");
        @display("bgb=728.91003,656.82");
        @statistic[processingLatency](title="processingLatency"; source=processingTime; record=vector);
        @statistic[transmissionLatency](title="Transmission Latency"; source=transmissionTime; record=vector);
        @statistic[totalLatency](title="Total Latency"; source=latency; record=vector);
    gates:
        inout hostport[];
    submodules:
        cpuCore[cores]: CPUCore {
            @display("p=673.2,39.6");
        }
        supervisor: StreamingSupervisor;
        cpuCoreScheduler : <cpuCoreSchedulerType> like ICpuCoreScheduler if cpuCoreSchedulerType != "";
        acker: Acker if hasAcker;
    connections allowunconnected:
        supervisor.udpIn <-- udp.appOut++ if hasUdp;
        supervisor.udpOut --> udp.appIn++ if hasUdp;
        supervisor.tcpIn <-- tcp.appOut++ if hasTcp;
        supervisor.tcpOut --> tcp.appIn++ if hasTcp;
        // connect supervisor to acker 
        // only if the acker is placed in this node
        supervisor.ackerOut++ --> acker.ackerIn if hasAcker;
}

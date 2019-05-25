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

package ecsnet.stask;

simple StreamingSink like ISTask
{
    parameters:
        string name = default("StreamingSink");
        @signal[cpuStateChanged](type=long);
        @signal[receivedStreamingMsgs](type=cPacket);
        @signal[latency](type=double);
        @signal[transmissionTime](type=double);
        @signal[processingTime](type=double);
        @signal[edgeProcessingTime](type=double);
        @statistic[rcvdStreamingMsgsStat](title="packets received"; source=receivedStreamingMsgs; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[throughput](title="throughput"; unit=bps; source="throughput(receivedStreamingMsgs)"; record=vector);
        @statistic[rcvdStreamingMsgsCount](title="Streaming Msg Count"; source=receivedStreamingMsgs; record=count);
        @statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(receivedStreamingMsgs)"; unit=s; record=min,max,mean,stats,histogram,vector; interpolationmode=sample-hold);
        @statistic[receivedStreamingMsgsSeqNo](title="received packet sequence number"; source="appPkSeqNo(receivedStreamingMsgs)"; record=vector; interpolationmode=none);

    gates:
        inout port[];
        input incomingStream[];
        output outgoingStream[];
        input fromCPU;
        output toCPU;
        output ackerOut;
}

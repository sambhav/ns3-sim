/*
MIT License

Copyright (c) 2017 Sambhav Kothari

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Network topology
//
//       n0 ----------- n1
//            1 Mbps
//             10 ms
//
// - Flow from n0 to n1 using BulkSendApplication.



// This code heavily borrows from ns3 itself which are copyright of their
// respective authors and redistributable under the same conditions.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpComparision");

AsciiTraceHelper ascii;
Ptr<PacketSink> cbrSinks[5], tcpSink;

int totalVal;
int total_drops=0;
bool first_drop=true;

// Function to record packet drops
static void
RxDrop (Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p)
{
    if(first_drop)
    {
        first_drop=false;
        *stream->GetStream ()<<0<<" "<<0<<std::endl;
    }
    *stream->GetStream ()<<Simulator::Now ().GetSeconds ()<<" "<<++total_drops<<std::endl;
}


// Function to find the total cumulative recieved bytes
static void
TotalRx(Ptr<OutputStreamWrapper> stream)
{
    totalVal = tcpSink->GetTotalRx();

    for(int i=0; i<5; i++)
    {
        totalVal += cbrSinks[i]->GetTotalRx();
    }

    *stream->GetStream()<<Simulator::Now ().GetSeconds ()<<" " <<totalVal<<std::endl;

    Simulator::Schedule(Seconds(0.0001),&TotalRx, stream);
}

// Function to record Congestion Window values
static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << std::endl;
}

//Trace Congestion window length
static void
TraceCwnd (Ptr<OutputStreamWrapper> stream)
{
    //Trace changes to the congestion window
    Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeBoundCallback (&CwndChange,stream));
}

int
main (int argc, char *argv[])
{

    bool tracing = false;
    uint32_t maxBytes = 0;
    std::string prot = "TcpWestwood";
    double error = 0.000001;

    // Allow the user to override any of the defaults at
    // run-time, via command-line arguments
    CommandLine cmd;
    cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
    cmd.AddValue ("maxBytes",
                  "Total number of bytes for application to send", maxBytes);
    cmd.AddValue ("prot", "Transport protocol to use: TcpNewReno, "
                  "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                  "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus ", prot);
    cmd.AddValue ("error", "Packet error rate", error);

    cmd.Parse (argc, argv);

    if (prot.compare ("TcpNewReno") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
    }
    else if (prot.compare ("TcpHybla") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHybla::GetTypeId ()));
    }
    else if (prot.compare ("TcpHighSpeed") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHighSpeed::GetTypeId ()));
    }
    else if (prot.compare ("TcpVegas") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
    }
    else if (prot.compare ("TcpScalable") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpScalable::GetTypeId ()));
    }
    else if (prot.compare ("TcpHtcp") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHtcp::GetTypeId ()));
    }
    else if (prot.compare ("TcpVeno") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVeno::GetTypeId ()));
    }
    else if (prot.compare ("TcpBic") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpBic::GetTypeId ()));
    }
    else if (prot.compare ("TcpYeah") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpYeah::GetTypeId ()));
    }
    else if (prot.compare ("TcpIllinois") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpIllinois::GetTypeId ()));
    }
    else if (prot.compare ("TcpWestwood") == 0)
    {
        // the default protocol type in ns3::TcpWestwood is WESTWOOD
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
        Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
    else if (prot.compare ("TcpWestwoodPlus") == 0)
    {
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
        Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
        Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
    else
    {
        NS_LOG_DEBUG ("Invalid TCP version");
        exit (1);
    }
    std::string a_s = "bytes_"+prot+".dat";
    std::string b_s = "drop_"+prot+".dat";
    std::string c_s = "cw_"+prot+".dat";

    // Create file streams for data storage
    Ptr<OutputStreamWrapper> total_bytes_data = ascii.CreateFileStream (a_s);
    Ptr<OutputStreamWrapper> dropped_packets_data = ascii.CreateFileStream (b_s);
    Ptr<OutputStreamWrapper> cw_data = ascii.CreateFileStream (c_s);

    // Explicitly create the nodes required by the topology (shown above).
    NS_LOG_INFO ("Create nodes.");
    NodeContainer nodes;
    nodes.Create (2);

    NS_LOG_INFO ("Create channels.");

    // Explicitly create the point-to-point link required by the topology (shown above).
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));
    pointToPoint.SetQueue ("ns3::DropTailQueue");

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    // Create error model
    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (error));
    devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

    // Install the internet stack on the nodes
    InternetStackHelper internet;
    internet.Install (nodes);

    // We've got the "hardware" in place.  Now we need to add IP addresses.
    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ipv4Container = ipv4.Assign (devices);

    NS_LOG_INFO ("Create Applications.");

    // Create a BulkSendApplication and install it on node 0
    uint16_t port = 12344;
    BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (ipv4Container.GetAddress (1), port));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps = source.Install (nodes.Get (0));
    sourceApps.Start (Seconds (0.0));
    sourceApps.Stop (Seconds (1.80));

    // Create a PacketSinkApplication and install it on node 1
    PacketSinkHelper sink ("ns3::TcpSocketFactory",
                           InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (nodes.Get (1));


    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (1.80));

    tcpSink = DynamicCast<PacketSink> (sinkApps.Get (0));


    uint16_t cbrPort = 12345;

    double startTimes[5] = {0.2, 0.4, 0.6, 0.8, 1.0};
    double endTimes[5]   = {1.8, 1.8, 1.2, 1.4, 1.6};

    for(int i=0; i<5; i++)
    {
        // Install applications: five CBR streams each saturating the channel
        ApplicationContainer cbrApps;
        ApplicationContainer cbrSinkApps;
        OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (ipv4Container.GetAddress (1), cbrPort+i));
        onOffHelper.SetAttribute ("PacketSize", UintegerValue (1024));
        onOffHelper.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

        onOffHelper.SetAttribute ("DataRate", StringValue ("300Kbps"));
        onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (startTimes[i])));
        onOffHelper.SetAttribute ("StopTime", TimeValue (Seconds (endTimes[i])));
        cbrApps.Add (onOffHelper.Install (nodes.Get (0)));
        // Packet sinks for each CBR agent

        PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), cbrPort+i));
        cbrSinkApps = sink.Install (nodes.Get (1));
        cbrSinkApps.Start (Seconds (0.0));
        cbrSinkApps.Stop (Seconds (1.8));
        cbrSinks[i] = DynamicCast<PacketSink> (cbrSinkApps.Get (0));
    }

    devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, dropped_packets_data));

    // Enable tracing
    if (tracing)
    {
        AsciiTraceHelper ascii;
        pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tcp-comparision.tr"));
        pointToPoint.EnablePcapAll ("tcp-comparision", true);
    }

    NS_LOG_INFO ("Run Simulation.");

    Simulator::Schedule(Seconds(0.00001),&TotalRx, total_bytes_data);
    Simulator::Schedule(Seconds(0.00001),&TraceCwnd, cw_data);

    // Flow monitor
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Simulator::Stop (Seconds (1.80));
    Simulator::Run ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
    std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;
    std::cout << "  Tx Packets:   " << stats[1].txPackets << std::endl;
    std::cout << "  Tx Bytes:   " << stats[1].txBytes << std::endl;
    std::cout << "  Offered Load: " << stats[1].txBytes * 8.0 / (stats[1].timeLastTxPacket.GetSeconds () - stats[1].timeFirstTxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
    std::cout << "  Rx Packets:   " << stats[1].rxPackets << std::endl;
    std::cout << "  Rx Bytes:   " << stats[1].rxBytes<< std::endl;
    std::cout << "  Throughput: " << stats[1].rxBytes * 8.0 / (stats[1].timeLastRxPacket.GetSeconds () - stats[1].timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
    std::cout << "  Mean delay:   " << stats[1].delaySum.GetSeconds () / stats[1].rxPackets << std::endl;
    std::cout << "  Mean jitter:   " << stats[1].jitterSum.GetSeconds () / (stats[1].rxPackets - 1) << std::endl;
    flowMonitor->SerializeToXmlFile("data.flowmon", true, true);
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");

}

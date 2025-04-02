/*
 * lab2-xszucm00.cc
 *
 * NS-3 lab – IPv4 vs IPv6 fragmentation
 * Topology: 2 LANs connected via point-to-point link
 * UDP echo server on n7, client on n0 (IPv4 and IPv6)
 * PCAP capture on n0 (LAN1) for analysis (Úkol 1)
 *
 * Author: Martin Szuc
 */

#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/gnuplot.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Lab");

// Globals for RTT graph (used later)
Gnuplot2dDataset datasetIPv4;
Gnuplot2dDataset datasetIPv6;
double rtt;
double simulationTime;
int ipv4PacketCount = 0;
int ipv6PacketCount = 0;

// RTT callbacks
void ipv4_node0_tx (string, Ptr<const Packet> p, Ptr<Ipv4>, uint32_t) {
  if (p->GetSize() >= 100) {
    ipv4PacketCount++;
    simulationTime = Simulator::Now().GetSeconds();
    rtt = simulationTime;
  }
}
void ipv4_node0_rx (string, Ptr<const Packet> p, Ptr<Ipv4>, uint32_t) {
  if (p->GetSize() > 100 && ipv4PacketCount > 1) {
    rtt = (Simulator::Now().GetSeconds() - rtt) * 1000;
    datasetIPv4.Add(simulationTime, rtt);
  }
}
void ipv6_node0_tx (string, Ptr<const Packet> p, Ptr<Ipv6>, uint32_t) {
  if (p->GetSize() >= 100) {
    ipv6PacketCount++;
    simulationTime = Simulator::Now().GetSeconds();
    rtt = simulationTime;
  }
}
void ipv6_node0_rx (string, Ptr<const Packet> p, Ptr<Ipv6>, uint32_t) {
  if (p->GetSize() > 100 && ipv6PacketCount > 1) {
    rtt = (Simulator::Now().GetSeconds() - rtt) * 1000;
    datasetIPv6.Add(simulationTime, rtt);
  }
}

int main (int argc, char *argv[])
{
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Hook up RTT tracing
  Config::Connect("/NodeList/0/$ns3::Ipv4L3Protocol/Tx", MakeCallback(&ipv4_node0_tx));
  Config::Connect("/NodeList/0/$ns3::Ipv4L3Protocol/Rx", MakeCallback(&ipv4_node0_rx));
  Config::Connect("/NodeList/0/$ns3::Ipv6L3Protocol/Tx", MakeCallback(&ipv6_node0_tx));
  Config::Connect("/NodeList/0/$ns3::Ipv6L3Protocol/Rx", MakeCallback(&ipv6_node0_rx));

  // Node setup
  NodeContainer csmaNodesLAN1;
  csmaNodesLAN1.Create(3); // n0, n1, n2
  Ptr<Node> router1 = CreateObject<Node>();
  Ptr<Node> router2 = CreateObject<Node>();
  NodeContainer csmaNodesLAN2;
  csmaNodesLAN2.Create(3); // n5, n6, n7

  // Topology layout
  NodeContainer LAN1(csmaNodesLAN1, router1);
  NodeContainer p2p(router1, router2);
  NodeContainer LAN2(router2, csmaNodesLAN2);
  NodeContainer netAll(csmaNodesLAN1, router1, router2, csmaNodesLAN2);

  // Install IP stack
  InternetStackHelper stack;
  stack.Install(netAll);

  // Channel config
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
  pointToPoint.SetDeviceAttribute("Mtu", UintegerValue(1500));

  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

  // Devices
  NetDeviceContainer netDevicesLAN1 = csma.Install(LAN1);
  NetDeviceContainer netDevicesP2P  = pointToPoint.Install(p2p);
  NetDeviceContainer netDevicesLAN2 = csma.Install(LAN2);

  // IPv4 addresses
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ipv4InterfacesLAN1 = ipv4.Assign(netDevicesLAN1);
  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer ipv4InterfacesP2P = ipv4.Assign(netDevicesP2P);
  ipv4.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer ipv4InterfacesLAN2 = ipv4.Assign(netDevicesLAN2);

  // IPv6 addresses
  Ipv6AddressHelper ipv6;
  ipv6.SetBase("2001:1:0:1::", 64);
  Ipv6InterfaceContainer ipv6InterfacesLAN1 = ipv6.Assign(netDevicesLAN1);
  ipv6InterfacesLAN1.SetForwarding(3, true);
  ipv6InterfacesLAN1.SetDefaultRouteInAllNodes(3);

  ipv6.SetBase("2001:1:0:2::", 64);
  Ipv6InterfaceContainer ipv6InterfacesP2P = ipv6.Assign(netDevicesP2P);
  ipv6InterfacesP2P.SetForwarding(0, true);
  ipv6InterfacesP2P.SetForwarding(1, true);

  ipv6.SetBase("2001:1:0:3::", 64);
  Ipv6InterfaceContainer ipv6InterfacesLAN2 = ipv6.Assign(netDevicesLAN2);
  ipv6InterfacesLAN2.SetForwarding(0, true);
  ipv6InterfacesLAN2.SetDefaultRouteInAllNodes(0);

  // IPv4 routes
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // IPv6 static routes
  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6StaticRouting> routing1 = routingHelper.GetStaticRouting(router1->GetObject<Ipv6>());
  routing1->AddNetworkRouteTo("2001:1:0:3::", 64, ipv6InterfacesP2P.GetInterfaceIndex(0));
  Ptr<Ipv6StaticRouting> routing2 = routingHelper.GetStaticRouting(router2->GetObject<Ipv6>());
  routing2->AddNetworkRouteTo("2001:1:0:1::", 64, ipv6InterfacesP2P.GetInterfaceIndex(1));

  // UDP echo server on n7 (LAN2[3])
  UdpEchoServerHelper echoServer(7);
  ApplicationContainer serverApps = echoServer.Install(LAN2.Get(3));
  serverApps.Start(Seconds(2.0));
  serverApps.Stop(Seconds(16.0));

  // UDP echo client (IPv4) on n0 → n7
  UdpEchoClientHelper echoClientIPv4(ipv4InterfacesLAN2.GetAddress(3), 7);
  echoClientIPv4.SetAttribute("MaxPackets", UintegerValue(4));
  echoClientIPv4.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClientIPv4.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientAppsIPv4 = echoClientIPv4.Install(csmaNodesLAN1.Get(0));
  clientAppsIPv4.Start(Seconds(2.0));
  clientAppsIPv4.Stop(Seconds(8.0));

  // UDP echo client (IPv6) on n0 → n7
  UdpEchoClientHelper echoClientIPv6(ipv6InterfacesLAN2.GetAddress(3, 1), 7);
  echoClientIPv6.SetAttribute("MaxPackets", UintegerValue(4));
  echoClientIPv6.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClientIPv6.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientAppsIPv6 = echoClientIPv6.Install(csmaNodesLAN1.Get(0));
  clientAppsIPv6.Start(Seconds(8.0));
  clientAppsIPv6.Stop(Seconds(16.0));

  // PCAP on n0
  csma.EnablePcap("ipv4-6-xszucm00", netDevicesLAN1.Get(0));

  // Dump IPv6 routing tables (debug)
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
  routingHelper.PrintRoutingTableAt(Seconds(16.0), router1, routingStream);
  routingHelper.PrintRoutingTableAt(Seconds(16.0), router2, routingStream);
  routingHelper.PrintRoutingTableAt(Seconds(16.0), csmaNodesLAN1.Get(0), routingStream);


  Simulator::Run();
  Simulator::Destroy();

  // Gnuplot output
  string base = "RTT";
  datasetIPv4.SetTitle("IPv4");
  datasetIPv4.SetStyle(Gnuplot2dDataset::LINES_POINTS);
  datasetIPv6.SetTitle("IPv6");
  datasetIPv6.SetStyle(Gnuplot2dDataset::LINES_POINTS);

  Gnuplot plot(base + ".png");
  plot.SetTitle("RTT");
  plot.SetTerminal("png");
  plot.SetLegend("Time [s]", "RTT [ms]");
  plot.AppendExtra("set yrange [0:+10]");
  plot.AddDataset(datasetIPv4);
  plot.AddDataset(datasetIPv6);

  ofstream plotFile((base + ".plt").c_str());
  plot.GenerateOutput(plotFile);
  plotFile.close();

  return 0;
}

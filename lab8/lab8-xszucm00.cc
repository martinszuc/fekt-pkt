#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Icmpv6aLOG");

int main (int argc, char **argv)
{
  // Create nodes
  Ptr<Node> SOU = CreateObject<Node> (); // Creating nodes
  Ptr<Node> R1 = CreateObject<Node> ();
  Ptr<Node> R2 = CreateObject<Node> ();
  Ptr<Node> host1 = CreateObject<Node> ();
  Ptr<Node> host2 = CreateObject<Node> ();

  // Create node containers
  NodeContainer all (SOU, R1, R2, host1, host2); // Putting nodes in a container
  NodeContainer ncm (SOU, R1, R2);
  NodeContainer nc1 (R1, host1);
  NodeContainer nc2 (R2, host2);

  // Create CSMA links
  CsmaHelper csma; // Creating CSMA links
  csma.SetChannelAttribute ("DataRate", DataRateValue (10e6));
  csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (1)));
  csma.SetDeviceAttribute ("Mtu", UintegerValue (1500));

  // Install devices on nodes
  NetDeviceContainer devm = csma.Install (ncm); // Installing devices on nodes from the given container (ncm)
  NetDeviceContainer dev1 = csma.Install (nc1);
  NetDeviceContainer dev2 = csma.Install (nc2);

  // Install protocol stack on nodes
  InternetStackHelper stackIPv6;
  stackIPv6.Install (all); // Installing protocol stack on nodes from the container (all)

  // Assign IPv6 addresses
  Ipv6AddressHelper address_space_1;
  Ipv6AddressHelper address_space_2;
  Ipv6AddressHelper address_space_3;

  address_space_1.SetBase (Ipv6Address ("2001:1:1:3::"), Ipv6Prefix (64)); // Address range for bottom network
  Ipv6InterfaceContainer intcm = address_space_1.Assign (devm);
  intcm.SetForwarding (1, true); // R1
  intcm.SetForwarding (2, true); // R2
  intcm.SetDefaultRouteInAllNodes (1); // Default route to R1

  address_space_2.SetBase (Ipv6Address ("2001:1:1:4::"), Ipv6Prefix (64)); // Address range for network with host1
  Ipv6InterfaceContainer intc1 = address_space_2.Assign (dev1);
  intc1.SetForwarding (0, true); // R1
  intc1.SetDefaultRouteInAllNodes (0); // Default route to R1

  address_space_3.SetBase (Ipv6Address ("2001:1:1:5::"), Ipv6Prefix (64)); // Address range for network with host2
  Ipv6InterfaceContainer intc2 = address_space_3.Assign (dev2);
  intc2.SetForwarding (0, true); // R2
  intc2.SetDefaultRouteInAllNodes (0); // Default route to R2

  // Create static route on R1 to reach host2 via R2
  Ptr<Ipv6> ipv6R1 = R1->GetObject<Ipv6> ();
  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6StaticRouting> routingR1 = routingHelper.GetStaticRouting (ipv6R1);

  // Manually adding static route to router R1's table, leading to the network with host2 via R2
  routingR1->AddNetworkRouteTo (Ipv6Address ("2001:1:1:5::"), 64, Ipv6Address ("fe80::200:ff:fe00:3"), 1);
  // Alternative method:
  // routingR1->AddNetworkRouteTo (intc2.GetAddress (1, 1), 64, intcm.GetAddress (2, 0), intcm.GetInterfaceIndex (1));

  // Create ping6 application
  Ping6Helper ping6_host2;
  ping6_host2.SetLocal (Ipv6Address ("2001:1:1:3:200:ff:fe00:1")); // Local (source) address (node SOU)
  // ping6_host2.SetLocal (intcm.GetAddress (0, 1)); // Alternative method
  ping6_host2.SetRemote (Ipv6Address ("2001:1:1:5:200:ff:fe00:7")); // Remote (destination) address (node host2)
  // ping6_host2.SetRemote (intc2.GetAddress (1, 1)); // Alternative method
  ping6_host2.SetAttribute ("MaxPackets", UintegerValue (10));
  ping6_host2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  ping6_host2.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer appPing_host2 = ping6_host2.Install (SOU);
  appPing_host2.Start (Seconds (3.0));
  appPing_host2.Stop (Seconds (15.0));

  // Print routing tables to a file
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("icmpv6a-routing", std::ios::out);
  routingHelper.PrintRoutingTableAt (Seconds (3.0), R1, routingStream);
  routingHelper.PrintRoutingTableAt (Seconds (3.0), R2, routingStream);
  routingHelper.PrintRoutingTableAt (Seconds (3.0), SOU, routingStream);
  routingHelper.PrintRoutingTableAt (Seconds (4.0), SOU, routingStream);

  // Create trace files
  csma.EnablePcapAll ("icmpv6a"); // Create trace files

  // Enable checksum calculation
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true)); // Checksum calculation enabled

  // Run the simulation
  Simulator::Run ();
  Simulator::Destroy ();
}
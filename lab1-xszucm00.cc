#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab1Assignment");

int main(int argc, char *argv[])
{
    // Enable logging for UDP Echo applications
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Define number of CSMA nodes (n2, n3, n4)
    uint32_t nCsma = 3;

    // Create nodes
    NodeContainer p2pNodes;
    p2pNodes.Create(2);  // nodes n0 and n1

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));  // Add n1 to CSMA network
    csmaNodes.Create(nCsma);  // Create n2, n3, n4

    // Create point-to-point link
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // Connect p2p devices
    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    // Create CSMA network
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // Connect CSMA devices
    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));  // Install on n0 only
    stack.Install(csmaNodes);  // Install on n1-n4

    // Assign IP addresses
    Ipv4AddressHelper address;

    // For point-to-point network
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    // For CSMA network
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    // Create UDP Echo Server on n4
    UdpEchoServerHelper echoServer(7);  // Port 7
    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(nCsma));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Create UDP Echo Client on n0
    UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(nCsma), 7);  // Server IP and port
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Enable packet capturing
    pointToPoint.EnablePcapAll("lab1");
    csma.EnablePcap("lab1_node", csmaDevices.Get(1), false);
    csma.EnablePcap("lab1_prom", csmaDevices.Get(1), true);

    // Run simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"

using namespace ns3;
using namespace std;

AnimationInterface * anim = 0;

Gnuplot2dDataset txAllDataset;
Gnuplot2dDataset txTransferredDataset;
Gnuplot2dDataset txDroppedDataset;

int txAllDataSize = 0;
int txDroppedDataSize = 0;
int txTransferedDataSize = 0;
int dropCount = 0;

double startTime = 2.0;
double endTime = 10.0;
double section = 0.25;

NS_LOG_COMPONENT_DEFINE("Unicast");

/*
 * Funkce pro mereni prenosove rychlosti
 */

int main(int argc, char *argv[]) {

	LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
	LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

	string appBitRate = "0.2Mbps";
	string queueLength = "10";
	string linkBitRate = "1Mbps";

	int nodeCount = 14;
	NodeContainer nodes;
	nodes.Create(nodeCount);

/*
 * Definice pohybového modelu
 */

	InternetStackHelper internetStack;
	internetStack.Install(nodes);

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue(linkBitRate));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	pointToPoint.SetDeviceAttribute("Mtu", UintegerValue(1500));
	pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxPackets", StringValue (queueLength));

	NetDeviceContainer netDevices0_5 = pointToPoint.Install(nodes.Get(0), nodes.Get(5));
	NetDeviceContainer netDevices5_8 = pointToPoint.Install(nodes.Get(5), nodes.Get(8));
	NetDeviceContainer netDevices5_7 = pointToPoint.Install(nodes.Get(5), nodes.Get(7));
	NetDeviceContainer netDevices8_11 = pointToPoint.Install(nodes.Get(8), nodes.Get(11));
	NetDeviceContainer netDevices11_13 = pointToPoint.Install(nodes.Get(11), nodes.Get(13));
	NetDeviceContainer netDevices13_4 = pointToPoint.Install(nodes.Get(13), nodes.Get(4));
	NetDeviceContainer netDevices13_12 = pointToPoint.Install(nodes.Get(13), nodes.Get(12));
	NetDeviceContainer netDevices12_3 = pointToPoint.Install(nodes.Get(12), nodes.Get(3));
	NetDeviceContainer netDevices12_10 = pointToPoint.Install(nodes.Get(12), nodes.Get(10));
	NetDeviceContainer netDevices11_10 = pointToPoint.Install(nodes.Get(11), nodes.Get(10));
	NetDeviceContainer netDevices10_7 = pointToPoint.Install(nodes.Get(10), nodes.Get(7));
	NetDeviceContainer netDevices7_6 = pointToPoint.Install(nodes.Get(7), nodes.Get(6));
	NetDeviceContainer netDevices6_1 = pointToPoint.Install(nodes.Get(6), nodes.Get(1));
	NetDeviceContainer netDevices6_9 = pointToPoint.Install(nodes.Get(6), nodes.Get(9));
	NetDeviceContainer netDevices10_9 = pointToPoint.Install(nodes.Get(9), nodes.Get(10));
	NetDeviceContainer netDevices9_2 = pointToPoint.Install(nodes.Get(9), nodes.Get(2));

	Ipv4AddressHelper ipv4;

	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces0_5 = ipv4.Assign(netDevices0_5);
	ipv4.SetBase("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces5_8 = ipv4.Assign(netDevices5_8);
	ipv4.SetBase("10.1.3.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces8_11 = ipv4.Assign(netDevices8_11);
	ipv4.SetBase("10.1.4.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces11_13 = ipv4.Assign(netDevices11_13);
	ipv4.SetBase("10.1.5.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces13_4 = ipv4.Assign(netDevices13_4);
	ipv4.SetBase("10.1.6.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces13_12 = ipv4.Assign(netDevices13_12);
	ipv4.SetBase("10.1.7.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces12_3 = ipv4.Assign(netDevices12_3);
	ipv4.SetBase("10.1.8.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces12_10 = ipv4.Assign(netDevices12_10);
	ipv4.SetBase("10.1.9.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces10_7 = ipv4.Assign(netDevices10_7);
	ipv4.SetBase("10.1.10.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces7_6 = ipv4.Assign(netDevices7_6);
	ipv4.SetBase("10.1.11.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces6_1 = ipv4.Assign(netDevices6_1);
	ipv4.SetBase("10.1.12.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces6_9 = ipv4.Assign(netDevices6_9);
	ipv4.SetBase("10.1.13.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces10_9 = ipv4.Assign(netDevices10_9);
	ipv4.SetBase("10.1.14.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces9_2 = ipv4.Assign(netDevices9_2);
	ipv4.SetBase("10.1.15.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces5_7 = ipv4.Assign(netDevices5_7);

	/*
	 * Definice unicastoveho globalniho smerovani
	 */

	/*
	 * Definice multicastoveho statickeho smerovani
	 */

	/*
	 * Definice OnOff unicastove a multicastove aplikace
	 */

   anim = new AnimationInterface("unicast");
	/*
	 * Definice vystupu pro program NetAnim
	 */

	anim->SetConstantPosition (nodes.Get(0), 30, 0);
	anim->SetConstantPosition (nodes.Get(1), 0, 20);
	anim->SetConstantPosition (nodes.Get(2), 10, 40);
	anim->SetConstantPosition (nodes.Get(3), 20, 40);
	anim->SetConstantPosition (nodes.Get(4), 50, 40);
	anim->SetConstantPosition (nodes.Get(5), 30, 10);
	anim->SetConstantPosition (nodes.Get(6), 10, 20);
	anim->SetConstantPosition (nodes.Get(7), 20, 20);
	anim->SetConstantPosition (nodes.Get(8), 40, 20);
	anim->SetConstantPosition (nodes.Get(9), 10, 30);
	anim->SetConstantPosition (nodes.Get(10), 30, 30);
	anim->SetConstantPosition (nodes.Get(11), 40, 30);
	anim->SetConstantPosition (nodes.Get(12), 30, 40);
	anim->SetConstantPosition (nodes.Get(13), 40, 40);

	/*
	 * Pripojeni na trasovaci system
	 */

	/*
	 * Volani funkce saveData v pravidelnych intervalech
	 */
	 
  NS_LOG_INFO("Run Simulation.");
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");
	
	/*
	 * Definice vystupu pro program Gnuplot
	 */

	return 0;
}

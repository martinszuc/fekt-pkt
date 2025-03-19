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

void ipv4_node0_tx(string path, Ptr<Packet const> packet, Ptr<Ipv4> ipv4, unsigned int parameter) {
	if (packet->GetSize() >= 100) {
		txAllDataSize = txAllDataSize + packet->GetSize();
	}}

void ipv4_node0_phyTx (string path, Ptr<Packet const> packet) {
	if (packet->GetSize() >= 100) {
		txTransferedDataSize = txTransferedDataSize + packet->GetSize();
	}}

void ipv4_node0_macTxDrop (string path, Ptr<Packet const> packet) {
	if (packet->GetSize() >= 100) {
		txDroppedDataSize = txDroppedDataSize + packet->GetSize();
            dropCount++;
		NS_LOG_UNCOND("Packet Drop Event. Total number of dropped packets in simulation (up to now): "); NS_LOG_UNCOND(dropCount);
	}}

void saveData(double time) {
	txAllDataset.Add(time, (double)txAllDataSize*8/section/(1024*1024));
	txTransferredDataset.Add(time,(double)txTransferedDataSize*8/section/(1024*1024));
	txDroppedDataset.Add(time,(double)txDroppedDataSize*8/section/(1024*1024));

	txAllDataSize = 0;
	txTransferedDataSize = 0;
	txDroppedDataSize = 0;
}



int main(int argc, char *argv[]) {

	LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
	LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

	string appBitRate = "0.9Mbps";
	string queueLength = "10";
	string linkBitRate = "1Mbps";

	int nodeCount = 14;
	NodeContainer nodes;
	nodes.Create(nodeCount);

/*
 * Definice pohybového modelu
 */
	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.InstallAll();


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
	ipv4.SetBase("10.1.16.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces11_10 = ipv4.Assign(netDevices11_10);

	/*
	 * Definice unicastoveho globalniho smerovani
	 */
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	/*
	 * Definice multicastoveho statickeho smerovani
	 */
	Ipv4Address multicastSource("10.1.1.1"); // IP adresa uzlu n0
	Ipv4Address multicastGroup("239.1.2.4");

	Ipv4StaticRoutingHelper multicastRouting;
	multicastRouting.SetDefaultMulticastRoute(nodes.Get(0),netDevices0_5.Get(0));

//-----------------------------------Multicast RP-----------------------------------------------//

	NetDeviceContainer outputDevices_n7;
	outputDevices_n7.Add(netDevices7_6.Get(0));

	NetDeviceContainer outputDevices_n10;
	outputDevices_n10.Add(netDevices12_10.Get(1));
	outputDevices_n10.Add(netDevices11_10.Get(1));
	outputDevices_n10.Add(netDevices10_9.Get(1));
	outputDevices_n10.Add(netDevices10_7.Get(0));


	//n5
	multicastRouting.AddMulticastRoute(nodes.Get(5), multicastSource,
	multicastGroup, netDevices0_5.Get(1), netDevices5_7.Get(0));

	//n6
	multicastRouting.AddMulticastRoute(nodes.Get(6), multicastSource,
	multicastGroup, netDevices7_6.Get(1), netDevices6_1.Get(0));

	//n7
	multicastRouting.AddMulticastRoute(nodes.Get(7), multicastSource,
	multicastGroup, netDevices5_7.Get(1), netDevices10_7.Get(1));

	//n7
	multicastRouting.AddMulticastRoute(nodes.Get(7), multicastSource,
	multicastGroup, netDevices10_7.Get(1), netDevices7_6.Get(0));

	//n9
	multicastRouting.AddMulticastRoute(nodes.Get(9), multicastSource,
	multicastGroup, netDevices10_9.Get(0), netDevices9_2.Get(0));

	//n10
	multicastRouting.AddMulticastRoute(nodes.Get(10), multicastSource,
	multicastGroup, netDevices10_7.Get(0), outputDevices_n10);

	//n11
	multicastRouting.AddMulticastRoute(nodes.Get(11), multicastSource,
	multicastGroup, netDevices11_10.Get(0), netDevices11_13.Get(0));

	//n12
	multicastRouting.AddMulticastRoute(nodes.Get(12), multicastSource,
	multicastGroup, netDevices12_10.Get(0), netDevices12_3.Get(0));


	//n13
	multicastRouting.AddMulticastRoute(nodes.Get(13), multicastSource,
	multicastGroup, netDevices11_13.Get(1), netDevices13_4.Get(0));


//-----------------------------------Multicast RP-----------------------------------------------//


	/*
	 * Definice OnOff unicastove a multicastove aplikace
	 */

	PacketSinkHelper sink("ns3::UdpSocketFactory",
	InetSocketAddress(Ipv4Address::GetAny (), 9));

	/*

	OnOffHelper onOffUnicast1("ns3::UdpSocketFactory",
	InetSocketAddress(interfaces6_1.GetAddress(1), 9));
	onOffUnicast1.SetAttribute("PacketSize", UintegerValue(1024));
	onOffUnicast1.SetAttribute("DataRate", StringValue (appBitRate));
	onOffUnicast1.SetAttribute("OffTime", StringValue
		("ns3::ConstantRandomVariable[Constant=0]"));

	ApplicationContainer unicastAppCont1 = onOffUnicast1.Install(nodes.Get(0));
	unicastAppCont1.Start(Seconds(startTime));
	unicastAppCont1.Stop(Seconds(endTime));
	unicastAppCont1 = sink.Install(nodes.Get(1));

	OnOffHelper onOffUnicast2("ns3::UdpSocketFactory",
	InetSocketAddress(interfaces9_2.GetAddress(1), 9));
	onOffUnicast2.SetAttribute("PacketSize", UintegerValue(1024));
	onOffUnicast2.SetAttribute("DataRate", StringValue(appBitRate));
	onOffUnicast2.SetAttribute("OffTime", StringValue
		("ns3::ConstantRandomVariable[Constant=0]"));

	ApplicationContainer unicastAppCont2 = onOffUnicast2.Install(nodes.Get(0));
	unicastAppCont2.Start(Seconds(startTime));
	unicastAppCont2.Stop(Seconds(endTime));
	unicastAppCont2 = sink.Install(nodes.Get(2));

	OnOffHelper onOffUnicast3("ns3::UdpSocketFactory",
	InetSocketAddress(interfaces12_3.GetAddress(1), 9));
	onOffUnicast3.SetAttribute("PacketSize", UintegerValue(1024));
	onOffUnicast3.SetAttribute("DataRate", StringValue(appBitRate));
	onOffUnicast3.SetAttribute("OffTime", StringValue
		("ns3::ConstantRandomVariable[Constant=0]"));

	ApplicationContainer unicastAppCont3 = onOffUnicast3.Install(nodes.Get(0));
	unicastAppCont3.Start(Seconds(startTime));
	unicastAppCont3.Stop(Seconds(endTime));
	unicastAppCont3 = sink.Install(nodes.Get(3));

			OnOffHelper onOffUnicast4("ns3::UdpSocketFactory",
				InetSocketAddress(interfaces13_4.GetAddress(1), 9));
				onOffUnicast4.SetAttribute("PacketSize", UintegerValue(1024));
				onOffUnicast4.SetAttribute("DataRate", StringValue(appBitRate));
				onOffUnicast4.SetAttribute("OffTime", StringValue
					("ns3::ConstantRandomVariable[Constant=0]"));

				ApplicationContainer unicastAppCont4 = onOffUnicast4.Install(nodes.Get(0));
				unicastAppCont4.Start(Seconds(startTime));
				unicastAppCont4.Stop(Seconds(endTime));
				unicastAppCont4 = sink.Install(nodes.Get(4));
	*/

				/* multicasti On/OFF bude se komentovat !!!
				 *
				 *
				 **/

				OnOffHelper onOffMulticast("ns3::UdpSocketFactory",
				Address(InetSocketAddress(multicastGroup, 9)));
				onOffMulticast.SetAttribute("PacketSize", UintegerValue(1024));
				onOffMulticast.SetAttribute("DataRate", StringValue (appBitRate));
				onOffMulticast.SetAttribute("OffTime", StringValue
					("ns3::ConstantRandomVariable[Constant=0]"));
				ApplicationContainer multicastAppCont = onOffMulticast.Install(nodes.Get(0));
				multicastAppCont.Start(Seconds(startTime));
				multicastAppCont.Stop(Seconds(endTime));
				multicastAppCont = sink.Install(nodes.Get(1));
				multicastAppCont = sink.Install(nodes.Get(2));
				multicastAppCont = sink.Install(nodes.Get(3));
				multicastAppCont = sink.Install(nodes.Get(4));




   anim = new AnimationInterface("multicast");
	/*
	 * Definice vystupu pro program NetAnim
	 *
	 */

   	   anim->EnablePacketMetadata (true);

   	   // anim->SetConstantPosition (nodes.Get(0), 30, 0);

   	   anim->UpdateNodeColor(nodes.Get(0), 0, 255, 0);
   	   anim->UpdateNodeDescription(nodes.Get(0), "SERVER");
   	   anim->UpdateNodeSize(0, 2, 2);
   	   anim->UpdateNodeColor(nodes.Get(1), 255, 0, 0);
   	   anim->UpdateNodeDescription(nodes.Get(1), "Klient 1");
   	   anim->UpdateNodeSize(0, 2, 2);
   	   anim->UpdateNodeColor(nodes.Get(2), 255, 0, 0);
   	   anim->UpdateNodeDescription(nodes.Get(2), "Klient 2");
   	   anim->UpdateNodeSize(0, 2, 2);
   	   anim->UpdateNodeColor(nodes.Get(3), 255, 0, 0);
   	   anim->UpdateNodeDescription(nodes.Get(3), "Klient 3");
   	   anim->UpdateNodeSize(0, 2, 2);
   	   anim->UpdateNodeColor(nodes.Get(4), 255, 0, 0);
   	   anim->UpdateNodeDescription(nodes.Get(4), "Klient 4");
   	   anim->UpdateNodeSize(0, 2, 2);


				   anim->UpdateNodeColor(nodes.Get(5), 0, 0, 0);
				   anim->UpdateNodeSize(0, 2, 2);
				   anim->UpdateNodeColor(nodes.Get(6), 0, 0, 0);
				   anim->UpdateNodeSize(0, 2, 2);
				   anim->UpdateNodeColor(nodes.Get(7), 0, 0, 0);
				   anim->UpdateNodeSize(0, 2, 2);
				   anim->UpdateNodeColor(nodes.Get(8), 0, 0, 0);
				   anim->UpdateNodeSize(0, 2, 2);
   	   	   	   	   anim->UpdateNodeColor(nodes.Get(9), 0, 0, 0);
   	   	   	   	   anim->UpdateNodeSize(0, 2, 2);
   	   	   	   	   anim->UpdateNodeColor(nodes.Get(10), 0, 255, 255);
   	   	   	   	   anim->UpdateNodeSize(0, 2, 2);
   	   	   	   	   anim->UpdateNodeDescription(nodes.Get(10), "RP");
   	   	   	   	   anim->UpdateNodeColor(nodes.Get(11), 0, 0, 0);
   	   	   	   	   anim->UpdateNodeSize(0, 2, 2);
   	   	   	   	   anim->UpdateNodeColor(nodes.Get(12), 0, 0, 0);
   	   	   	   	   anim->UpdateNodeSize(0, 2, 2);
   	   	   	   	   anim->UpdateNodeColor(nodes.Get(13), 0, 0, 0);
   	   	   	   	   anim->UpdateNodeSize(0, 2, 2);


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
	Config::Connect("/NodeList/0/$ns3::Ipv4L3Protocol/Tx",MakeCallback(&ipv4_node0_tx));
	Config::Connect("/NodeList/0/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd",MakeCallback(&ipv4_node0_phyTx));
	Config::Connect("/NodeList/0/DeviceList/*/$ns3::PointToPointNetDevice/MacTxDrop",	MakeCallback(&ipv4_node0_macTxDrop));


	/*
	 * Volani funkce saveData v pravidelnych intervalech
	 */

	for(double time=0; time<endTime; time=time+section)
	Simulator::Schedule (Seconds(time), &saveData, time);


  NS_LOG_INFO("Run Simulation.");
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");

	/*
	 * Definice vystupu pro program Gnuplot
	 */

	string fileNameWithNoExtension = "multicast";
	string graphicsFileName = fileNameWithNoExtension + ".png";
	string plotFileName = fileNameWithNoExtension + ".plt";
	string plotTitle = "Bitrate-multicast";

	txAllDataset.SetTitle("Sent");
	txAllDataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	txTransferredDataset.SetTitle("Transferred");
	txTransferredDataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	txDroppedDataset.SetTitle("Dropped");
	txDroppedDataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);

	Gnuplot plot(graphicsFileName);

	plot.SetTitle(plotTitle);
	plot.SetTerminal("png");
	plot.SetLegend("Time [s]", "Bitrate [Mb/s]");
	//plot.AppendExtra ("set yrange [0:+1.2]");
	plot.AddDataset(txAllDataset);
	plot.AddDataset(txTransferredDataset);
	plot.AddDataset(txDroppedDataset);
	ofstream plotFile(plotFileName.c_str());
	plot.GenerateOutput(plotFile);
	plotFile.close();


	return 0;
}




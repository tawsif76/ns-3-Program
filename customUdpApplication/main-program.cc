#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"
#include "simple-udp-application.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Task1");

int  main (int argc, char *argv[]) {
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nWifi = 3;
    bool tracing = false;
    ns3::PacketMetadata::Enable ();

    CommandLine cmd (__FILE__);
    cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

    cmd.Parse (argc,argv);
    if (verbose) {
        LogComponentEnable("SimpleUdpApplication", LOG_LEVEL_INFO);
        LogComponentEnable("SimpleUdpApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    }
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    

    NodeContainer LStaNodes;
    LStaNodes.Create(nWifi);
    NodeContainer wifiApNodeL = p2pNodes.Get(0);

    YansWifiChannelHelper channelL = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phyL;
    
    phyL.SetChannel(channelL.Create());

    WifiHelper wifiL;
    wifiL.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper macL;

    Ssid ssidL = Ssid ("ns-3-ssid");
    macL.SetType ("ns3::StaWifiMac", "Ssid", 
                SsidValue (ssidL), 
                "ActiveProbing", BooleanValue (false));

    NetDeviceContainer staDevicesL;
    staDevicesL = wifiL.Install(phyL, macL, LStaNodes);

    macL.SetType ("ns3::ApWifiMac", 
                "Ssid", SsidValue (ssidL));

    NetDeviceContainer apDevicesL;
    apDevicesL = wifiL.Install(phyL, macL, wifiApNodeL);

    MobilityHelper mobilityL;

    mobilityL.SetPositionAllocator ("ns3::GridPositionAllocator",
        "MinX", DoubleValue (0.0),
        "MinY", DoubleValue (0.0),
        "DeltaX", DoubleValue (5.0),
        "DeltaY", DoubleValue (10.0),
        "GridWidth", UintegerValue (3),
        "LayoutType", StringValue ("RowFirst"));

    mobilityL.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", 
    "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

    mobilityL.Install(LStaNodes);

    mobilityL.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobilityL.Install (wifiApNodeL);


    NodeContainer RStaNodes;
    RStaNodes.Create(nWifi);
    NodeContainer wifiApNodeR = p2pNodes.Get(1);

    YansWifiChannelHelper channelR = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phyR;
    
    phyR.SetChannel(channelR.Create());

    WifiHelper wifiR;
    wifiR.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper macR;

    Ssid ssidR = Ssid ("ns-3-ssid");
    macR.SetType ("ns3::StaWifiMac", "Ssid", 
                SsidValue (ssidR), 
                "ActiveProbing", BooleanValue (false));

    NetDeviceContainer staDevicesR;
    staDevicesR = wifiR.Install(phyR, macR, RStaNodes);

    macR.SetType ("ns3::ApWifiMac", 
                "Ssid", SsidValue (ssidR));

    NetDeviceContainer apDevicesR;
    apDevicesR = wifiR.Install(phyR, macR, wifiApNodeR);


    MobilityHelper mobilityR;

    mobilityR.SetPositionAllocator ("ns3::GridPositionAllocator",
        "MinX", DoubleValue (0.0),
        "MinY", DoubleValue (0.0),
        "DeltaX", DoubleValue (5.0),
        "DeltaY", DoubleValue (10.0),
        "GridWidth", UintegerValue (3),
        "LayoutType", StringValue ("RowFirst"));

    mobilityR.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", 
    "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

    mobilityR.Install(RStaNodes);

    mobilityR.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobilityR.Install (wifiApNodeR);

    InternetStackHelper stack;
    stack.Install(wifiApNodeL);
    stack.Install(LStaNodes);
    stack.Install(wifiApNodeR);
    stack.Install(RStaNodes);

    Ipv4AddressHelper address;

    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign (p2pDevices);

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer InterfacesL;
    InterfacesL = address.Assign (staDevicesL);
    address.Assign(apDevicesL);

    address.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer InterfacesR;
    InterfacesR = address.Assign (staDevicesR);
    address.Assign(apDevicesR);


    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(LStaNodes.Get(nWifi - 1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    
    Ptr<Node> n_i = RStaNodes.Get(nWifi - 1);
    Ptr<SimpleUdpApplication> udp_i = CreateObject<SimpleUdpApplication> ();

    udp_i->SetStartTime(Seconds(2.00));
    udp_i->SetStopTime(Seconds(10.0));
    n_i->AddApplication(udp_i);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    AnimationInterface anim ("animation2.xml");

    std::ostringstream msg; msg << "0101010" << '\0';
    uint16_t packetSize = msg.str().length()+1;
    Ptr<Packet> packet = Create<Packet>((uint8_t*) msg.str().c_str(), packetSize);

    Simulator::Schedule(Seconds(3.00), &SimpleUdpApplication::SendPacket, udp_i, packet, 
    InterfacesL.GetAddress(nWifi - 1), 9);

    Simulator::Stop (Seconds (10.0));

    Simulator::Run();
    Simulator::Destroy();
    
}
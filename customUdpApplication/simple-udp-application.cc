#include "ns3/log.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/csma-net-device.h"
#include "ns3/ethernet-header.h"
#include "ns3/arp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/packet.h"
#include "ns3/tag.h"
#include "simple-udp-application.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SimpleUdpApplication");
NS_OBJECT_ENSURE_REGISTERED(SimpleUdpApplication);

TypeId SimpleUdpApplication::GetTypeId() {
    static TypeId tid = TypeId("ns3::SimpleUdpApplication")
    .AddConstructor<SimpleUdpApplication> ()
    .SetParent<Application> ();

    return tid;

}

TypeId SimpleUdpApplication::GetInstanceTypeId() const{
    return SimpleUdpApplication::GetTypeId();
}

SimpleUdpApplication::SimpleUdpApplication(){

}
SimpleUdpApplication::~SimpleUdpApplication(){

}

void SimpleUdpApplication::StartApplication() {
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket(GetNode(), tid);
    m_socket->SetRecvCallback(MakeCallback(&SimpleUdpApplication::HandleRead, this));
}


void SimpleUdpApplication::HandleRead(Ptr<Socket> socket) {
    NS_LOG_FUNCTION(this << socket);
    

    Ptr<Packet> packet;
    Address from;
    Address localAddress;
    while ((packet = socket->RecvFrom(from))) {
        NS_LOG_INFO("Received a Packet of size: " << packet->GetSize() << " at time " << Now().GetSeconds()
            << " from " << InetSocketAddress::ConvertFrom(from).GetIpv4());
        uint8_t *buffer = new uint8_t[packet->GetSize ()];
        packet->CopyData(buffer, packet->GetSize ());
        std::string s = std::string(buffer, buffer+packet->GetSize());
        std::cout << "Received:" << s << std::endl;
        NS_LOG_INFO(packet->ToString());
        //std::cerr << packet->ToString() << std::endl;
    }
}

void SimpleUdpApplication::SendPacket(Ptr<Packet> packet, Ipv4Address destination, uint16_t port) {
    NS_LOG_FUNCTION (packet << destination << port);
    m_socket->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(destination), port));
    m_socket->Send(packet);
}


}



    // Ptr<Packet> packet = Create<Packet>(100);
    // std::string packetString = "Hello, NS-3!";
    // Ptr<StringTag> tag = Create<StringTag>();
    // tag->SetStringValue(packetString);
    // packet->AddPacketTag(*tag);
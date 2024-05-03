#ifndef NS3_SIMPLE_UDP_APPLICATION_H
#define NS3_SIMPLE_UDP_APPLICATION_H

#include "ns3/socket.h"
#include "ns3/application.h"

using namespace ns3;

namespace ns3 {
    class SimpleUdpApplication : public Application{
        public:
            SimpleUdpApplication();
            virtual ~SimpleUdpApplication();
            static TypeId GetTypeId();
            virtual TypeId GetInstanceTypeId() const;
            void HandleRead(Ptr<Socket> socket);
            void SendPacket(Ptr<Packet> packet, Ipv4Address destination, uint16_t port);
        private:
            virtual void StartApplication();
            Ptr<Socket> m_socket;
    };
};

#endif



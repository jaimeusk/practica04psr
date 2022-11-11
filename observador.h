#include "ns3/ptr.h"
#include "ns3/udp-server.h"

using namespace ns3;

class Observador
{
public:
           Observador      (Ptr<UdpServer>    servidor);
  void     PaqueteRecibido (Ptr<const Packet> paquete);
  uint32_t TotalPaquetes   ();

private:
  uint32_t m_cuenta;
};

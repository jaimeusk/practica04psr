#include "ns3/object-base.h"
#include "ns3/log.h"
#include "observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador (Ptr<UdpServer> servidor)
{
  m_cuenta = 0;
  servidor->TraceConnectWithoutContext ("Rx",
                                        MakeCallback(&Observador::PaqueteRecibido,
                                                     this));  
}


void
Observador::PaqueteRecibido (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  m_cuenta++;
}


uint32_t
Observador::TotalPaquetes ()
{
  return m_cuenta;
}

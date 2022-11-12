#include "ns3/object-base.h"
#include "ns3/log.h"
#include "retardo.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Retardo");


Retardo::Retardo (Ptr<PointToPointNetDevice> disp_server, Ptr<PointToPointNetDevice> disp_client)
{

  disp_server->TraceConnectWithoutContext ("PhyRxEnd",
                                        MakeCallback(&Retardo::FinalRx, 
                                                     this));  //Nos suscribimos a la traza de final de recepción

  disp_client->TraceConnectWithoutContext ("PhyTxBegin",MakeCallback(&Retardo::ComienzoTx,this)); //Nos suscribimos a la traza de comienzo de transmisión
}


void
Retardo::ComienzoTx (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  m_comienzo = Simulator::Now();  //Almacenamos el instante de simulación actual(instante de comienzo de tx) en una variable privada
}

void Retardo::FinalRx (Ptr<const Packet> paquete){
    
    NS_LOG_FUNCTION (paquete);

    //Calculamos el retardo como la diferencia entre el instante de simulación actual(instante en el que terminó de rx el pkt) menos el valor de la variable privada(instante en el que se emitió)
    m_retardo_medio.Update((Simulator::Now()-m_comienzo).GetSeconds());  //Añadimos el retardo calculado a la variable privada de tipo Average
}


Time
Retardo::Get_RetardoMedio ()
{
  return Seconds(m_retardo_medio.Avg());  //Devolvemos el retardo medio como un Time
}

#include "ns3/ptr.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/nstime.h"
#include "ns3/average.h"

using namespace ns3;

class Retardo
{
public:
           Retardo      (Ptr<PointToPointNetDevice> disp_server, Ptr<PointToPointNetDevice> disp_client);
  void     ComienzoTx (Ptr<const Packet> paquete);   //Método que atiende la traza de comienzo de transmisión
  void     FinalRx (Ptr<const Packet> paquete);    //Método que atiende la traza de final de recepción
  Time Get_RetardoMedio   ();  //Método que nos devolverá el retardo medio a partir de los retardos que hemos ido almacenando en la variable privada

private:
  Time m_comienzo; //Instante de la simulación en el que almacenaremos el comienzo de transmisión de un paquete
  Average<double> m_retardo_medio; //Variable donde iremos almacenando los distintos retardos del enlace que calculemos
  

};

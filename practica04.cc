#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-server.h"
#include "ns3/udp-client.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include <iostream>
#include "ns3/data-rate.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-channel.h"




using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica04");

int main(int argc,char *argv[]){

  UintegerValue puerto_server;
  DataRateValue reg_bin;
  TimeValue retardo_canal;
  UintegerValue max_pkts_c;
  TimeValue intervalo_pkts;
  UintegerValue tamano_pkt;

  //Creamos el contenedor de nodos y le añadimos dos nodos
  NodeContainer c_nodos;
  c_nodos.Create(2);
  NS_LOG_INFO("Identificador del nodo servidor: " << c_nodos.Get(0)->GetId());
  NS_LOG_INFO("Identificador del nodo cliente: " << c_nodos.Get(1)->GetId());

  //Añadimos la pila de protocolos a cada nodo
  InternetStackHelper h_pila;
  h_pila.SetIpv6StackInstall (false);
  h_pila.Install (c_nodos);

  //Añadimos un dispositivo punto a punto en cada nodo
  PointToPointHelper h_ptp;
  NetDeviceContainer c_dispositivos = h_ptp.Install (c_nodos);
  
  NS_LOG_INFO("Dirección MAC del servidor: " << c_nodos.Get(0)->GetDevice(0)->GetAddress());
  NS_LOG_INFO("Dirección MAC del cliente: " << c_nodos.Get(1)->GetDevice(1)->GetAddress());
  
  Ptr<PointToPointNetDevice> d_server = c_dispositivos.Get(0)->GetObject<PointToPointNetDevice> ();
  d_server->GetAttribute("DataRate",reg_bin);
  
  NS_LOG_INFO("Velocidad de transmisión del enlace: " << reg_bin.Get());

  Ptr<PointToPointChannel> canal_ptp = d_server->GetChannel()->GetObject<PointToPointChannel> ();
  //canal_ptp->SetAttribute("Delay",TimeValue(Time("10ns")));
  canal_ptp->GetAttribute("Delay",retardo_canal);
  
  NS_LOG_INFO("Retardo de propagación del canal: " << retardo_canal.Get());

  //Asignamos direcciones IP a cada dispositivo de red, devolviendonos un tipo interfaz que almacenamos
  Ipv4AddressHelper h_direcciones ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer c_interfaces = h_direcciones.Assign(c_dispositivos);

  //Creamos Servidor UDP en el primer nodo sin usar el helper
  Ptr<UdpServer> server_udp = CreateObject<UdpServer> ();
  c_nodos.Get(0)->AddApplication(server_udp);  //El método Get devuelve un puntero al nodo 0, por eso después usamos ->

  server_udp->GetAttribute ("Port",puerto_server); //Obtenemos el puerto del servidor

  NS_LOG_INFO("Puerto de escucha del servidor: " << puerto_server.Get());

  //Creamos cliente UDP y se lo asociamos al nodo 1. Indicamos la IP y puerto del servidor
  Ptr<UdpClient> client_udp = CreateObject<UdpClient> ();
  c_nodos.Get(1)->AddApplication(client_udp);
  server_udp->GetAttribute ("Port",puerto_server); //Obtenemos el puerto donde escucha el servidor como un IntegerValue. Hay que pasarlo a int con Get()
  client_udp->SetRemote(c_interfaces.GetAddress(0),puerto_server.Get());

  client_udp->GetAttribute("MaxPackets",max_pkts_c);
  NS_LOG_INFO("Número máximo de paquetes que envía la aplicación: " << max_pkts_c.Get());
  client_udp->GetAttribute("Interval",intervalo_pkts);
  NS_LOG_INFO("Intervalo de envío de paquetes: " << intervalo_pkts.Get());
  client_udp->GetAttribute("PacketSize",tamano_pkt);
  NS_LOG_INFO("Tamaño de los paquetes que envía el cliente: " << tamano_pkt.Get());


  Simulator::Stop(Time("60s"));
  NS_LOG_INFO ("Arranca la simulación");
  Simulator::Run();
  NS_LOG_INFO("Termina la simulación");
  Simulator::Destroy();


}


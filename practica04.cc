#include <iostream>
#include "observador.h"
#include "ns3/command-line.h"
#include "ns3/data-rate.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/mac48-address.h"
#include "ns3/net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/simulator.h"
#include "ns3/udp-client.h"
#include "ns3/udp-server.h"
#include "ns3/uinteger.h"
#include "retardo.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica04");

int main(int argc,char *argv[]){
  
  // Parametros para NS_LOG
  UintegerValue puerto_server;
  DataRateValue reg_bin;
  TimeValue retardo_canal;
  UintegerValue max_pkts_c;
  TimeValue intervalo_pkts;
  UintegerValue tamano_pkt;
  Mac48AddressValue mac_server;
  Mac48AddressValue mac_client;


  // Parametros para configuracion del escenario
  // Se dan valores por defecto por si no se concretan en la linea de comandos.
  // Cliente UDP
  uint conf_max_pkts_c_cmmdline = 100; // Aux para cmmdline
  uint conf_tamano_pkt_cmmdline = 1024; // Aux para cmmdline
  Time  conf_intervalo_pkts_cmmdline("1s"); // Auxiliar para poder usar cmmdline
  
  // Escenario
  Time conf_retardo_canal_cmmdline("0s"); // Auxiliar para cmmdline
  DataRate conf_reg_bin_cmmdline("32768bps"); // Auxiliar para cmmdline



  /***********************************************************/
  /**************   CREACION NODOS Y CONTENIDO   *************/
  /***********************************************************/

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

  //Asignamos direcciones IP a cada dispositivo de red, devolviendonos un tipo interfaz que almacenamos
  Ipv4AddressHelper h_direcciones ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer c_interfaces = h_direcciones.Assign(c_dispositivos);
  
  //Creamos Servidor UDP en el primer nodo sin usar el helper
  Ptr<UdpServer> server_udp = CreateObject<UdpServer> ();
  c_nodos.Get(0)->AddApplication(server_udp);  //El método Get devuelve un puntero al nodo 0, por eso después usamos ->

  //Obtenemos el puerto del servidor
  server_udp->GetAttribute ("Port",puerto_server); 
  NS_LOG_INFO("Puerto de escucha del servidor: " << puerto_server.Get());
  
  //Creamos cliente UDP y se lo asociamos al nodo 1. Indicamos la IP y puerto del servidor
  Ptr<UdpClient> client_udp = CreateObject<UdpClient> ();
  c_nodos.Get(1)->AddApplication(client_udp);
  server_udp->GetAttribute ("Port",puerto_server); //Obtenemos el puerto donde escucha el servidor como un IntegerValue. Hay que pasarlo a int con Get()
  client_udp->SetRemote(c_interfaces.GetAddress(0),puerto_server.Get());
  


  /***********************************************************/
  /*******    Obtenemos objetos utiles para el codigo   ******/
  /***********************************************************/

  // Obtenemos el dispositivo_servidor
  Ptr<PointToPointNetDevice> d_server = c_dispositivos.Get(0)->GetObject<PointToPointNetDevice> ();

  // Obtenemos el dispositivo_cliente
  Ptr<PointToPointNetDevice> d_client = c_dispositivos.Get(1)->GetObject<PointToPointNetDevice> ();

  // Obtenemos el canal a traves del dispositivo servidor
  Ptr<PointToPointChannel> canal_ptp = d_server->GetChannel()->GetObject<PointToPointChannel> ();



  /***********************************************************/
  /*****    Modificacion parametros desde commandline   ******/
  /***********************************************************/

  // A traves de la linea de comandos elegimos los siguientes parametros
  CommandLine cmd;

  cmd.AddValue("intervaloPkts","Intervalo entre paquetes", conf_intervalo_pkts_cmmdline);
  cmd.AddValue("tamPkts","Tamaño de paquetes, en octetos", conf_tamano_pkt_cmmdline);
  cmd.AddValue("maxPkts","Numero máximo de paquetes a enviar", conf_max_pkts_c_cmmdline);
  cmd.AddValue("regBinCanal","Regimen binario del canal",conf_reg_bin_cmmdline);
  cmd.AddValue("delayCanal","Retardo de propagación del canal",conf_retardo_canal_cmmdline);
  cmd.Parse(argc,argv);

  // Cambiamos los atributos del cliente.
  client_udp->SetAttribute("MaxPackets",UintegerValue(conf_max_pkts_c_cmmdline));
  client_udp->SetAttribute("PacketSize",UintegerValue(conf_tamano_pkt_cmmdline));
  client_udp->SetAttribute("Interval",TimeValue(conf_intervalo_pkts_cmmdline));

  // Cambiamos los atributos del canal y de los dispositivos.
  canal_ptp->SetAttribute("Delay",TimeValue(conf_retardo_canal_cmmdline));
  d_client->SetAttribute("DataRate",DataRateValue(conf_reg_bin_cmmdline));  // ¿Debe ajustarse en ambos para que sea igual? Si sólo
  d_server->SetAttribute("DataRate",DataRateValue(conf_reg_bin_cmmdline));  // se ajusta en cliente, servidor tiene otra y viceversa
  

  /***********************************************************/
  /*******    Obtencion de valores de los atributos   ********/
  /*******  Y uso de trazas NS_LOG asociadas a ellos  ********/
  /***********************************************************/

  // Obtenemos la dirección mac del servidor y del cliente
  d_server->GetAttribute("Address",mac_server);
  NS_LOG_INFO("Dirección MAC del servidor: " << mac_server.Get());
  d_client->GetAttribute("Address",mac_client);
  NS_LOG_INFO("Direccioń MAC del cliente: " << mac_client.Get());
  
  // Obtenemos el regimen binario  del enlace PtP (Lo obtenemos del dispositivo servidor)
  d_server->GetAttribute("DataRate",reg_bin);  
  NS_LOG_INFO("Velocidad de transmisión del enlace: " << reg_bin.Get());

  // Obtenemos el retardo del canal (PtP Channel).
  canal_ptp->GetAttribute("Delay",retardo_canal);
  NS_LOG_INFO("Retardo de propagación del canal: " << retardo_canal.Get());

  // Obtenemos el numero maximo de paquetes que envia la aplicacion
  client_udp->GetAttribute("MaxPackets",max_pkts_c);
  NS_LOG_INFO("Número máximo de paquetes que envía la aplicación: " << max_pkts_c.Get());
  // Obtenemos el intervalo
  client_udp->GetAttribute("Interval",intervalo_pkts);
  NS_LOG_INFO("Intervalo de envío de paquetes: " << intervalo_pkts.Get());
  // Obtenemos el tamaño de los paquetes que envia el cliente
  client_udp->GetAttribute("PacketSize",tamano_pkt);
  NS_LOG_INFO("Tamaño de los paquetes que envía el cliente: " << tamano_pkt.Get());


  Observador obs(server_udp);
  Retardo retardo(d_server,d_client);

  Simulator::Stop();  //Podría ponerse un tiempo de fin de simulación pero el enunciado no dice nada
  NS_LOG_INFO ("Arranca la simulación");
  Simulator::Run();
  NS_LOG_INFO("Termina la simulación");
  Simulator::Destroy();

  NS_LOG_INFO("Observador: Num. paquetes recibidos: " << obs.TotalPaquetes());
  NS_LOG_INFO("UdpServer:  Num. paquetes recibidos: " << server_udp->GetReceived());
  NS_LOG_INFO("Retardo medio del enlace punto a punto: " << retardo.Get_RetardoMedio());


}
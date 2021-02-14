/*
 * Ethernet.h
 *
 * Created: 08/02/2021 14:03:28
 *  Author: Livio
 */ 


#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "Definitions.h"
#include <WIRE.h>
#include <SPI.h>
#include <EthernetLarge.h>
#include <SSLClient/SSLClient.h>
#include "trust_anchors.h"

	
void EthernetInit(void);
bool EthernetStartClient(void);
bool EthernetLoop(void);


void ETH_test(void);




#endif /* ETHERNET_H_ */
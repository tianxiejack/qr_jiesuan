/*******************************************************************************
 *                                                                           
 * Copyright (c) 2012 ChamRun Tech Incorporated - http://www.casevision.com/ 
 *                        ALL RIGHTS RESERVED                                
 *                                                                           
 ******************************************************************************/


/**
 *  \file udp_port.c
 *
 *  \brief
 *
 *
 */

#include <udp_port.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
typedef int Socket;

typedef struct _udp_port_handle
{
	port_handle Hdl;
	udp_open_params openParams;
	udp_trans_context transContext;
} udp_port_handle;

static Int32 udp_port_open(void *Hdl, void *openParams);
static Int32 udp_port_close(void *Hdl);
static Int32 udp_port_send(void *Hdl, UInt8 *data, UInt32 len);
static Int32 udp_port_recv(void *Hdl, UInt8 *buffer, UInt32 size);

Int32 udp_port_create(port_handle **ppHdl)
{
	udp_port_handle *udpPort;

	udpPort = (udp_port_handle*)malloc(sizeof(udp_port_handle));

	if(udpPort == NULL)
		return OSA_EFAIL;

	memset(udpPort, 0, sizeof(*udpPort));

	udpPort->Hdl.portType = PORT_NET_UDP;

	udpPort->Hdl.fd = -1;
	udpPort->Hdl.openParams = &udpPort->openParams;
	udpPort->Hdl.transContext = &udpPort->transContext;

	udpPort->Hdl.open = udp_port_open;
	udpPort->Hdl.close = udp_port_close;
	udpPort->Hdl.send = udp_port_send;
	udpPort->Hdl.recv = udp_port_recv;

	if(ppHdl != NULL)
		*ppHdl = (port_handle*)udpPort;

	return OSA_SOK;
}

Int32 udp_port_destroy(port_handle *pHdl)
{
	udp_port_handle *udpPort = (udp_port_handle *)pHdl;

	if(pHdl == NULL)
		return  OSA_EFAIL;

	if(pHdl->fd != -1)
		udp_port_close(pHdl);

	free(udpPort);

	return OSA_SOK;
}

/* \brief Function to open udp port. */
static Int32 udp_port_open(void *Hdl, void *openParams)
{
	udp_open_params *udpOpenParams = (udp_open_params*)openParams;
	udp_port_handle *port = (udp_port_handle*)Hdl;
	struct sockaddr_in addr;
	Socket fd;

	memcpy(&port->openParams, udpOpenParams, sizeof(*udpOpenParams));

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ( fd == INVALID_SOCKET )
	{
		OSA_ERROR("stun_udp: Could not create a UDP socket");
		return INVALID_SOCKET;
	}

	memset((char*) &(addr),0, sizeof((addr)));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(udpOpenParams->port);

	if ( (udpOpenParams->interfaceIp != 0) && ( udpOpenParams->interfaceIp != 0x100007f ))
	{
		addr.sin_addr.s_addr = htonl(udpOpenParams->interfaceIp);
		OSA_printf("Binding to interface 0x%lx\n",(unsigned long) htonl(udpOpenParams->interfaceIp));
	}

	if ( bind( fd,(struct sockaddr*)&addr, sizeof(addr)) != 0 )
	{
		OSA_ERROR("Could not bind socket\n");
		close(fd);
		return INVALID_SOCKET;
	}

	OSA_printf("%s: opened port %i with fd %i\n", __func__, port->openParams.port, fd);

	port->Hdl.fd = fd;

	return OSA_SOK;
}
/* \brief Function to close udp port. */
static Int32 udp_port_close(void *Hdl)
{
	udp_port_handle *port = (udp_port_handle*)Hdl;

	if(Hdl == NULL || port->Hdl.fd < 0)
		return OSA_EFAIL;
	
	close(port->Hdl.fd);

	OSA_printf("%s: closed port %i with fd %i\n", __func__, port->openParams.port, port->Hdl.fd);

	port->Hdl.fd = -1;

	return OSA_SOK;
}

/* \brief Function to send data by udp port. */
static Int32 udp_port_send(void *Hdl, UInt8 *data, UInt32 len)
{
	UInt32 writeLen = 0;
	int curLen;
	udp_port_handle *port = (udp_port_handle*)Hdl;
	struct sockaddr_in to;
	int toLen = sizeof(to);

	/*
	OSA_printf("%s port  %p remotePort %x,remoteIp %x data %x len %d\r\n",__func__,
			port,port->transContext.remotePort,port->transContext.remoteIp,data,len);
	*/
	if(port == NULL || port->Hdl.fd <0 || data == NULL || len == 0 || port->transContext.remoteIp == 0)
		return OSA_EFAIL;

	memset(&to,0,toLen);

	to.sin_family = AF_INET;
	to.sin_port = htons(port->transContext.remotePort);
	to.sin_addr.s_addr = htonl(port->transContext.remoteIp);

	while(len != writeLen){
		curLen = sendto(port->Hdl.fd, data+writeLen, len - writeLen, 0, (struct sockaddr*)&to, toLen);
		if(curLen <= 0)
			return OSA_EFAIL;
		writeLen += curLen;
		port->Hdl.cntSnd++;
		port->Hdl.cntSndbytes += curLen;
	}

	return  writeLen;
}

/* \brief Function to receive data form udp port. */
static Int32 udp_port_recv(void *Hdl, UInt8 *buffer, UInt32 size)
{
	Int32 nread;
	udp_port_handle *port = (udp_port_handle*)Hdl;
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	if(port == NULL || port->Hdl.fd <0 || buffer == NULL || size == 0)
		return OSA_EFAIL;

	nread = recvfrom(port->Hdl.fd,
                        buffer,
                        size,
                        0,
                        (struct sockaddr *)&from,
                        (socklen_t*)&fromLen);

	if(nread > 0){
		port->transContext.fromIp 	= ntohl(from.sin_addr.s_addr);
		port->transContext.fromPort 	= ntohs(from.sin_port);
		port->transContext.remoteIp 	= ntohl(from.sin_addr.s_addr);
		port->transContext.remotePort	= ntohs(from.sin_port);
		port->Hdl.cntRecv++;
		port->Hdl.cntRecvBytes += nread;
	}
	return  nread;
}


/*******************************************************************************
 *                                                                           
 * Copyright (c) 2012 ChamRun Tech Incorporated - http://www.casevision.com/ 
 *                        ALL RIGHTS RESERVED                                
 *                                                                           
 ******************************************************************************/


/**
 *  \file uart_port.c
 *
 *  \brief
 *
 *
 */

#include <uart_port.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>

typedef struct _uart_port_handle
{
	port_handle Hdl;
	uart_open_params openParams;

} uart_port_handle;

static Int32 uart_port_open(void *Hdl, void *openParams);
static Int32 uart_port_close(void *Hdl);
static Int32 uart_port_set(uart_port_handle *port);
static Int32 uart_port_send(void *Hdl, UInt8 *data, UInt32 len);
static Int32 uart_port_recv(void *Hdl, UInt8 *buffer, UInt32 size);

Int32 uart_port_create(port_handle **ppHdl)
{
	uart_port_handle *uartPort;

	uartPort = (uart_port_handle*)malloc(sizeof(uart_port_handle));

	if(uartPort == NULL)
		return OSA_EFAIL;

	memset(uartPort, 0, sizeof(*uartPort));

	uartPort->Hdl.portType = PORT_UART;

	uartPort->Hdl.fd = -1;
	uartPort->Hdl.openParams = &uartPort->openParams;
	uartPort->openParams.baudrate = 115200;
	uartPort->openParams.databits = 8;
	uartPort->openParams.parity = 'e';
	uartPort->openParams.stopbits = 1;

	uartPort->Hdl.open = uart_port_open;
	uartPort->Hdl.close = uart_port_close;
	uartPort->Hdl.send = uart_port_send;
	uartPort->Hdl.recv = uart_port_recv;

	if(ppHdl != NULL)
		*ppHdl = (port_handle*)uartPort;

	return OSA_SOK;
}

Int32 uart_port_destroy(port_handle *pHdl)
{
	uart_port_handle *uartPort = (uart_port_handle *)pHdl;

	if(pHdl == NULL)
		return  OSA_EFAIL;

	if(pHdl->fd != -1)
		uart_port_close(pHdl);

	free(uartPort);

	return OSA_SOK;
}

/* \brief Function to open uart port. */
static Int32 uart_port_open(void *Hdl, void *openParams)
{
	uart_open_params *uartOpenParams = (uart_open_params*)openParams;
	uart_port_handle *port = (uart_port_handle*)Hdl;

	memcpy(&port->openParams, uartOpenParams, sizeof(*uartOpenParams));

	port->Hdl.fd = open( uartOpenParams->device, O_RDWR| O_NOCTTY | O_NDELAY );
	
	if (-1 == port->Hdl.fd)
	{ 
		perror(" ERROR:: Can't Open Serial Port");
		return OSA_EFAIL;
	}

	return uart_port_set(port);
}
/* \brief Function to close uart port. */
static Int32 uart_port_close(void *Hdl)
{
	uart_port_handle *port = (uart_port_handle*)Hdl;
	OSA_assert(Hdl != NULL);
	if(Hdl == NULL || port->Hdl.fd < 0)
		return OSA_EFAIL;
	
	close(port->Hdl.fd);

	OSA_printf("%s:: dev %s  fd %d !!!\n", __func__, port->openParams.device, port->Hdl.fd);

	port->Hdl.fd = -1;

	return OSA_SOK;
}

/* \brief Function to send data by uart port. */
static Int32 uart_port_send(void *Hdl, UInt8 *data, UInt32 len)
{
	UInt32 writeLen = 0;
	int curLen;
	uart_port_handle *port = (uart_port_handle*)Hdl;

	if(port == NULL || port->Hdl.fd <0 || data == NULL || len == 0)
		return OSA_EFAIL;

	while(len != writeLen){
		curLen = write(port->Hdl.fd, data+writeLen, len - writeLen);
		if(curLen <= 0)
			return OSA_EFAIL;
		writeLen += curLen;
		port->Hdl.cntSnd++;
		port->Hdl.cntSndbytes += curLen;
	}

	return  writeLen;
}

/* \brief Function to receive data form uart port. */
static Int32 uart_port_recv(void *Hdl, UInt8 *buffer, UInt32 size)
{
	Int32 nread;
	uart_port_handle *port = (uart_port_handle*)Hdl;

	if(port == NULL || port->Hdl.fd <0 || buffer == NULL || size == 0)
		return OSA_EFAIL;

	nread =  read(port->Hdl.fd, buffer, size);

	if(nread > 0){
		port->Hdl.cntRecv++;
		port->Hdl.cntRecvBytes += nread;
	}
	return  nread;
}

static int baudrate_map[] = {
	B300, B600,  B1200, B2400,  B4800, B9600, B19200, B38400
	};

/* \brief Function to set uart port params. */
static Int32 uart_port_set(uart_port_handle *port)
{
	struct termios new_cfg,old_cfg;
	int speed = B115200;
	int index = 0;
	int databits = port->openParams.databits;
	char parity = port->openParams.parity;
	int stopbits = port->openParams.stopbits;

	OSA_printf("-----------------------the pari=%c databit=%d stop=%d\n",parity,databits,stopbits);

	speed = port->openParams.baudrate;
	if(speed <= 0 || speed == 115200)
		speed = B115200;
	else if(speed == 57600)
	{
		speed = B57600;
	}
	else
	{
		for(index = 0; index < 8 ; index++ ){
			if(speed == (1<<index)*300){
				speed = baudrate_map[index];
				break;
			}
		}
		if(index == 8){
			speed = B115200;
			OSA_printf("%s: WARN:: baud rate set default (115200) !!!\n", __FUNCTION__);
		}
	}

	if(tcgetattr(port->Hdl.fd, &old_cfg)!=0)
	{
		perror("tcgetattr");
		return(-1);
	}

	new_cfg=old_cfg;
	cfmakeraw(&new_cfg);
	//new_cfg.c_lflag |=(ICANON|ECHO|IEXTEN);
	//new_cfg.c_lflag &=~(ECHOPRT|ECHOCTL);
	new_cfg.c_cflag &= ~CSIZE;
	
	cfsetispeed(&new_cfg,	speed);
	cfsetospeed(&new_cfg,	speed);

	/* set data bit */
	switch(databits)
	{
		case 7:
			new_cfg.c_cflag |= CS7;
		break;
		case 8:
		default:		
			new_cfg.c_cflag |= CS8;
		break;
	}

	/* set parity bit */ 
	switch(parity)
	{
		
		case 'n':
		case 'N':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK;
		}
		break;
		
		case 'o':
		case 'O':
		{
		new_cfg.c_cflag |= (PARODD|PARENB);
		new_cfg.c_iflag |= INPCK;
		}
		break;
		
		case 'e':
		case 'E':
		{
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@fun=%s\n",__func__);
			new_cfg.c_cflag |= PARENB;
			new_cfg.c_cflag &= ~PARODD;
			new_cfg.c_cflag |= INPCK;
		}
		break;
		
		case 's':
		case 'S':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_cflag &= ~CSTOPB;
		}

		default:
		break;

	}

	/* set stop bit */
	switch(stopbits)
	{
		
		case 1:
		{
			new_cfg.c_cflag &= ~CSTOPB;
		}
		break;
		case 2:
		{
			new_cfg.c_cflag |= CSTOPB;
		}
		default:
		break;
	}

	/* set wait time */
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN]  = 1;

	/* set unrecevie data*/
	tcflush(port->Hdl.fd,TCIFLUSH);

	if((tcsetattr(port->Hdl.fd,TCSANOW,&new_cfg))!=0)
	{
		perror("tcsetattr");
		return OSA_EFAIL;
	}
	
	return OSA_SOK;
}





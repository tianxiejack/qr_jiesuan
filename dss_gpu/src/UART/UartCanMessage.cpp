/*******************************************************************************
 *                                                                                                                                      *
 * Copyright (c) 2012 ChamRun Tech Incorporated - http://www.casevision.com/                   *
 *                        ALL RIGHTS RESERVED                                                                             *
 *                                                                                                                                       *
 ******************************************************************************/


/**
 *  \file uartApi.c
 *
 *  \brief api interface
 *  This file implements functionality for the switch sem.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>



#include "UartCanMessage.h"

static int fd_can;


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
int speed_arr[] = {B115200,B38400, B19200, B9600, B4800, B2400, B1200, B300};

int name_arr[]  = {115200,38400, 19200,  9600, 4800,  2400, 1200,  300};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* \brief Function to open Uart device. */
int Uart_dev_open(const char *Dev)
{
	int	fd = open( Dev, O_RDWR| O_NOCTTY | O_NDELAY );
	if (-1 == fd)
	{
		perror(" [error Uart_dev_open] Can't Open Serial Port");
		return -1;
	}
	/*
	if(fcntl(fd,F_SETFL,0)<0)
	{
		perror(" [error Uart_dev_open] fcntl F_SETFL \n");
	}
	*/
	return fd;
}

/* \brief Function to close Uart device. */
void Uart_dev_close(int fd)
{
	close(fd);
}

/* \brief Function to set Uart param. */
int Uart_set_com_config(int fd,int baud_rate,int data_bits,char parity,int stop_bits)
{
	struct termios new_cfg,old_cfg;
	int speed;

	if(tcgetattr(fd,&old_cfg)!=0)
	{
		perror("tcgetattr");
		return(-1);
	}

	new_cfg=old_cfg;
	cfmakeraw(&new_cfg);
	//new_cfg.c_lflag |=(ICANON|ECHO|IEXTEN);
	//new_cfg.c_lflag &=~(ECHOPRT|ECHOCTL);
	new_cfg.c_cflag &= ~CSIZE;

	/*set baud rate */
	switch (baud_rate)
	{
		case 2400:
		{
			speed = B2400;
		}
		break;
		case 4800:
		{
			speed = B4800;
		}
		break;
		case 9600:
		{
			speed = B9600;
		}
		break;
		case 19200:
		{
			speed = B19200;
		}
		break;
		case 38400:
		{
			speed = B38400;
		}
		break;

		default:
		case 115200:
		{
			speed = B115200;
		}
		break;
	}

	cfsetispeed(&new_cfg,	speed);
	cfsetospeed(&new_cfg,	speed);

	/* set data bit */
	switch(data_bits)
	{
		case 7:
		{
			new_cfg.c_cflag |= CS7;
		}
		break;

		case 8:
		{
			new_cfg.c_cflag |= CS8;
		}
		break;
	}

	/* set parity bit */
	switch(parity)
	{
		default:
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
			new_cfg.c_cflag |= PARENB;
			new_cfg.c_cflag &= ~PARODD;
			new_cfg.c_iflag |= INPCK;
		}
		break;

		case 's':
		case 'S':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_cflag &= ~CSTOPB;
		}
		break;

	}

	/* set stop bit */
	switch(stop_bits)
	{
		default:
		case 1:
		{
			new_cfg.c_cflag &= ~CSTOPB;
		}
		break;
		case 2:
		{
			new_cfg.c_cflag |= CSTOPB;
		}
		break;
	}

	/* set wait time */
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN]  = 1;

	/* set unrecevie data*/
	tcflush(fd,TCIFLUSH);

	if((tcsetattr(fd,TCSANOW,&new_cfg))!=0)
	{
		perror("tcsetattr");
		return(-1);
	}
	return 0;
}


#if 0

void *Uart_Test(void *context)
{
	int i = 0;
	unsigned char buf[128]={0};
	int nread = 0, nwrite = 0;
	int LoopExit  = 0;

	int fd = Uart_dev_open("/dev/ttyTHS1");

	if(Uart_set_com_config(fd,115200,8,'N',1)<0)
	{
		printf("Uart Set Error\n");
		exit(1);
	}

	printf("[Uart_Test ] Loop start  FD %d!!!\r\n",fd);

	while(!LoopExit)
	{
		if((nread = read(fd, buf, 128))>0)
		{
			nwrite = write(fd, buf, nread);

			printf(" [xavier %s] uart recevie nLen=%d\r nwrite=%d\n",__func__, nread, nwrite);

			for(i=0; i<nread; i++)
			{
				if(buf[i] == 'Q' || buf[i]=='q')
				{
					LoopExit = 0;
					break;
				}

				printf("0x%02x,",buf[i]);

				if(i%32==0 && i>0)
				{
					printf("\n");
				}
			}
			printf("\n");
		}
		else
		{
			usleep(10*1000);
		}
	}

	printf(" [xavier %s ] Loop over and exit!!!\r\n",__func__);
	close(fd);

	exit(0);
}

int main(int argc, char **argv)
{
//	Uart_TestMain( argc, argv);

	Uart_Test(NULL);
	return 0;
}

#endif


int OpenCANDevice()
{
	fd_can = Uart_dev_open("/dev/ttyTHS2");
	if(Uart_set_com_config(fd_can,115200,8,'N',1)<0)
	{
		printf("Uart Set Error\n");
	}
	return fd_can;
}

int GetCanfd()
{
	return fd_can;
}

int CloseCANDevice()
{
	close(fd_can);
	return 0;
}

int SetCANConfig(int baud_rate,int data_bits,char parity,int stop_bits)
{
	Uart_set_com_config(fd_can,  baud_rate,  data_bits,  parity,  stop_bits);
	return 0;
}

int ReadCANBuf(char *buf, int length)
{
	int nread = 0 ;
	nread = read(fd_can, buf, length);

	return nread;
}

int SendCANBuf(char *buf, int length)
{
	int nread = 0 ;
	nread = write(fd_can, buf, length);

	return nread;
}












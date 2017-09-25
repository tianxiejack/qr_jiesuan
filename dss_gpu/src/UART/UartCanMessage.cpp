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
#include "WeaponCtrl.h"
#include "record_log.h"


bool test_flag_uart = 0;
static int fd_can;

pthread_mutex_t can_mutex;

//\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u839e\u951f\u65a4\u62f7\u6b27\u951f\u65a4\u62f7\u6a21\u6851\u66ae\u62e7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u89d2?\u951f\u65a4\u62f7	\u951f\u65a4\u62f7\u53f0
#define CODE_SERVO_MACHGUN   	(0x2C)
#define CODE_SERVO_GRENADE	(0x37)
#define CODE_SERVO_TURRET     	(0x42)


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
		return -1;
	}
	
	pthread_mutex_init(&can_mutex,NULL);
	
	Servo_start_init();

	return fd_can;
}

int GetCanfd()
{
	return fd_can;
}

int CloseCANDevice()
{
	close(fd_can);
	pthread_mutex_destroy(&can_mutex);
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

	pthread_mutex_lock(&can_mutex);
	nread = read(fd_can, buf, length);
	pthread_mutex_unlock(&can_mutex);
	
	return nread;
}

int SendCANBuf(char *buf, int length)
{
	#if 0
		int nwrite= 0 ;

		pthread_mutex_lock(&can_mutex);
		
		nwrite = write(fd_can, buf, length);

		pthread_mutex_unlock(&can_mutex);
		
		return nwrite;	
	#endif
}


int TestSendCANBuf(char *buf, int length)
{
		int nwrite= 0 ;

		pthread_mutex_lock(&can_mutex);
		
		nwrite = write(fd_can, buf, length);
		record_log_send_data(record_log_can_send, length, (unsigned char*)buf);
	

		pthread_mutex_unlock(&can_mutex);
		
		return nwrite;	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ServoAbsPosRequest( char code)
{
	 char servoPos[6]={ 0x03, 0x00, 0x50, 0x58, 0x00, 0x00  };
	servoPos[1] = code;
	SendCANBuf(servoPos,  sizeof(servoPos));
}

//\u951f\u65a4\u62f7\u59cb\u951f\u65a4\u62f7\u951f\u65a4\u62f7
void ServoStart( char code)
{
	 char start[6] = {0x03, 0x00, 0x42, 0x47, 0x00, 0x00 };
	start[1] = code;
	SendCANBuf(start, sizeof(start));
}

//\u951f\u65a4\u62f7\u951f\u65a4\u62f7startServoServer()\u951f\u65a4\u62f7\u951f\u65a4\u62f7
//\u951f\u65a4\u62f7\u59cb\u951f\u65a4\u62f7\u951f\u811a\u51e4\u62f7\u951f\u501f\u5907
int InitServo(  char code )
{
	 char buf[4] = {0x00, 0x00, 0x01, 0x00};
	 char MOTOR0[10] = { 0x03, 0x00, 0x4d, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	 char MOTOR1[10] = { 0x03, 0x00, 0x4d, 0x4f, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
	 char MODE[10] 	 =  { 0x03, 0x00, 0x55, 0x4d, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00 };

	SendCANBuf(buf,  sizeof(buf));  //\u951f\u65a4\u62f7\u951f\u65a4\u62f7can\u951f\u65a4\u62f7\u951f\u65a4\u62f7

	MOTOR0[1] = code;
	SendCANBuf(MOTOR0,  sizeof(MOTOR0));	//\u951f\u65a4\u62f7\u951f\u7ede\u7678\u62f7\u951f\u8f7f\ue04e\u62f7\u951f?
	MODE[1] = code;
	SendCANBuf(MODE, sizeof(MODE)); 	//\u951f\u52ab\u8bb9\u62f7\u9009\u951f\u65a4\u62f7\u6a21\u5f0f\u4e3a\u951f\u65a4\u62f7
	MOTOR1[1] = code;
	SendCANBuf(MOTOR1, sizeof(MOTOR1)); //\u951f\u65a4\u62f7\u951f\u7ede\u7678\u62f7\u951f\u8f7f\ue04e\u62f7\u951f?
	ServoAbsPosRequest(code); //\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u4f4d\u951f\u65a4\u62f7
	ServoStart(code); //\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u7855\u951f?

	return 0;
}

///////////////////////////////////////////////////

//\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u89d2\u7678\u62f7\u4fe3\u951f?
void Mach_servo_move_speed(float xSpeed, float ySpeed)
{



}

//\u951f\u4fa5\u618b\u62f7\u951f\u89d2\u7678\u62f7\u951f\u8f7f\u4f19\u62f7\u951f?
void Mach_servo_move_offset(float xOffset, float yOffset)
{



}

//\u951f\u65a4\u62f7\u67aa\u505c\u6b62\u951f\u527f\u8bb9\u62f7
void Mach_servo_stop()
{

	char mach_stop_buff[10] 	 =  { 0x03, CODE_SERVO_MACHGUN, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	char turret_stop_buff[10] 	 =  { 0x03, CODE_SERVO_TURRET, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	SendCANBuf(mach_stop_buff,  sizeof(mach_stop_buff));
	usleep(10*1000);
	SendCANBuf(turret_stop_buff,  sizeof(turret_stop_buff));
	usleep(10*1000);

}

///////////////////////////////////////////////////

//\u951f\u65a4\u62f7\u951f\u811a\u51e4\u62f7\u951f\u52ab\u8bb9\u62f7
void Grenade_servo_move_speed(float xSpeed, float ySpeed)
{



}

//\u951f\u65a4\u62f7\u4f4d\u951f\u65a4\u62f7\u504f\u951f\u65a4\u62f7
void Grenade_servo_move_offset(float xOffset, float yOffset)
{



}

//\u951f\u65a4\u62f7\u951f\u811a\u51e4\u62f7\u505c\u6b62\u951f\u527f\u8bb9\u62f7
void Grenade_servo_stop()
{
		char grenade_stop_buff[10] 	 =  { 0x03, CODE_SERVO_GRENADE, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		char turret_stop_buff[10] 			 =  { 0x03, CODE_SERVO_TURRET, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		SendCANBuf(grenade_stop_buff,  sizeof(grenade_stop_buff));
		usleep(10*1000);
		SendCANBuf(turret_stop_buff,  sizeof(turret_stop_buff));
		usleep(10*1000);

}

///////////////////////////////////////////////////////////////////////////////////////////////

//\u951f\u65a4\u62f7\u53f0\u951f\u811a\u51e4\u62f7\u951f\u52ab\u8bb9\u62f7
void Turret_servo_move_speed(float xSpeed, float ySpeed)
{



}

//\u951f\u65a4\u62f7\u53f0\u951f\u811a\u51e4\u62f7\u4f4d\u951f\u65a4\u62f7\u504f\u951f\u65a4\u62f7
void Turret_servo_move_offset(float xOffset, float yOffset)
{


}

//\u951f\u65a4\u62f7\u53f0\u951f\u811a\u51e4\u62f7\u505c\u6b62\u951f\u527f\u8bb9\u62f7
void Turret_servo_stop()
{
	char turret_stop_buff[10] 			 =  { 0x03, CODE_SERVO_TURRET, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	SendCANBuf(turret_stop_buff,  sizeof(turret_stop_buff));
	usleep(10*1000);

}


void Servo_start_init()
{
	startServoServer( CODE_SERVO_MACHGUN );
	usleep(10*1000);
	startServoServer( CODE_SERVO_GRENADE );
	usleep(10*1000);
	startServoServer( CODE_SERVO_TURRET );
	usleep(10*1000);
}







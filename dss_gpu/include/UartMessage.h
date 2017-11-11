#ifndef _UARTMESSAGE_H_
#define _UARTMESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>


#define MSG_HEAD_SFCC	(0xCC)
#define MSG_HEAD_SYSID	(0x06)
#define MSG_HEAD_SIZE	(0x03)
#define MSG_CHSUM_SIZE	(0x01)

#define CAN_ID_PANEL 		(0x0002)
#define CAN_ID_MACHGUN 	(0x02AC)	// all Confirm
#define CAN_ID_GRENADE		(0x02B7)
#define CAN_ID_TURRET 		(0x02C2)

#define CODE_SERVO_MACHGUN    (0x2C)
#define CODE_SERVO_GRENADE	(0x37)
#define CODE_SERVO_TURRET     	(0x42)

#define CODE_MACHGUN 		(0x2c)  //jiqiang --confirm
#define CODE_GRENADE 		(0x37)
#define CODE_TURRET   		(0x42)



int uartMessage(void) ;
void MSGAPI_AckSnd(int CmdType);
void* recv_pth(unsigned char *RS422,int len);
int  send_pth(unsigned char * RS422);
void inituartmessage();

void* fdTest_open(void* prm);

void processMessage(char* pData);
char valueToHexCh(const int value);
int strToHex(char* ch,char* hex);





void* tcpTestFunc(void* prm);
void chartohex(char* buf,char *recvhex,int recvlen);
void compundHex(unsigned char* recv,unsigned char* get,int len);

int NetPort_ParseByte(unsigned char* buf);
void * SPI_CAN_process(void * prm);
int CanPort_parseByte(unsigned char* buf );
void *self_memcpy(void *__dest, __const void *__src, int __n);
void * SPI_CAN_Send40(void * prm);


#ifdef __cplusplus
}
#endif

#endif

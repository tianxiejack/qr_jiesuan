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



int uartMessage(void) ;
void MSGAPI_AckSnd(int CmdType);
void* recv_pth(unsigned char *RS422,int len);
int  send_pth(unsigned char * RS422);
void inituartmessage();

void* fdTest_open(void* prm);

void processMessage(char* pData);
char valueToHexCh(const int value);
int strToHex(char* ch,char* hex);



#ifdef __cplusplus
}
#endif

#endif

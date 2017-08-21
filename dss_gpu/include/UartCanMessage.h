#ifndef  _UART_CAN_MESSAGE_H
#define _UART_CAN_MESSAGE_H

int OpenCANDevice();

int GetCanfd();

int CloseCANDevice();

int SetCANConfig(int baud_rate,int data_bits,char parity,int stop_bits);

int SendCANBuf(char *buf, int length);

int ReadCANBuf(char *buf, int length);

#endif
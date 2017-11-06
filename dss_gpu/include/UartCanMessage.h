#ifndef  _UART_CAN_MESSAGE_H
#define _UART_CAN_MESSAGE_H


int OpenCANDevice();

int GetCanfd();

int CloseCANDevice();

int SetCANConfig(int baud_rate,int data_bits,char parity,int stop_bits);

int SendCANBuf(char *buf, int length);

int ReadCANBuf(char *buf, int length);

void Servo_start_init();

int TestSendCANBuf(char *buf, int length);
void servoLookupMainPos();
void servoLookupGetPos();
extern bool test_flag_uart;

#endif

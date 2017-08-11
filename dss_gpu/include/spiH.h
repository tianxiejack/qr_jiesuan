#ifndef __SPIH_H
#define __SPIH_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))  
#define SPI_IOC_RD_INTERUPT_NUM  _IOR(SPI_IOC_MAGIC, 6, __u32)
#define SPI_IOC_WR_OPEN_INTERUPT  _IOW(SPI_IOC_MAGIC, 6, __u32)

#define com0 (0x000)
#define com1 (0x100)
#define com2 (0x200)
#define com3 (0x000)
#define com4 (0x100)
#define com5 (0x200)
#define com6 (0x000)
#define com7 (0x100)

#define combase0 (0x000)
#define combase1 (0x100)
#define combase2 (0x200)


#define RS422_ROTER 0    //电旋
#define RS422_DECODE 1   //倾斜角传感器
#define RS422_BAK2 2     //备份接口2
#define RS422_MIRROR 3    //瞄准镜
#define RS422_VCODE 4    //俯仰角传感器
#define RS422_BAK1 5    //备份接口1
#define RS422_TEST 6   //测试（被网络口占用）
#define RS422_HCODE 7   //方位传感器


struct RS422_data
{
	uint8_t receiveData[2048];
	int length;
	pthread_mutex_t mutex;
};


static void pabort(const char *s);

int open_device();  //打开三个spi

int transfer_init(uint8_t comNum, uint8_t interuptThreshold,int Baudrate);  //单个串口初始化

int transfer_init_all(uint8_t interuptThreshold,int Baudrate);   //全部串口初始化

int transfer_ban(int fd,int comAddr);    //单个串口关闭接收使能

int transfer_open(int fd,int comAddr);     //单个串口开启接收使能

int transfer_readDataCount(int fd,int comAddr);   //读取单个串口字节数

int transfer_readOneData(int comNum);  //读取单个串口一个字节数

int transfer_readData(int comNum,int length,struct RS422_data* RS422_data_buff); //读取串口length长度的字节数到receiveData中


/****中断处理..e.g:interuptHandleSpi1->interuptHandleDataSpi1****/
void interuptHandleSpi1(struct RS422_data* RS422_ROTER_buff,struct RS422_data* RS422_DECODE_buff,struct RS422_data* RS422_BAK2_buff);
void interuptHandleSpi2(struct RS422_data* RS422_MIRROR_buff,struct RS422_data* RS422_VCODE_buff,struct RS422_data* RS422_BAK1_buff);
void interuptHandleSpi3(struct RS422_data* RS422_TEST_buff,struct RS422_data* RS422_HCODE_buff);
void interuptHandleDataSpi1(int interuptNum,struct RS422_data* RS422_ROTER_buff,struct RS422_data* RS422_DECODE_buff,struct RS422_data* RS422_BAK2_buff);
void interuptHandleDataSpi2(int interuptNum,struct RS422_data* RS422_MIRROR_buff,struct RS422_data* RS422_VCODE_buff,struct RS422_data* RS422_BAK1_buff);
void interuptHandleDataSpi3(int interuptNum,struct RS422_data* RS422_TEST_buff,struct RS422_data* RS422_HCODE_buff);



/****发数据****/
int sendDataToSpi(int comNum,uint8_t* buff,int length);//把整包数据发到spi上
int writeOneData(int comNum,uint8_t data);  //写单个字节





#if 0
/****测试收数据用****/
int interuptHandleTest(); //中断处理测试
int interuptHandleDataTest(int interuptNum);  //接收到中断后数据打印
void printf_read_data(uint8_t* receiveData,int readCountTmp);  
#endif


#endif






















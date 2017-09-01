#include "spiH.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/select.h>
#include <string.h>
#include <pthread.h>

#include "UartMessage.h"
#include "TurretPosPort.h"
#include "MachGunPort.h"
#include "msgDriv.h"
#include "PositionPort.h"
#include "statCtrl.h"
#include "GrenadePort.h"
#include "osdPort.h"
#include "LaserPort.h"


static int fd0;
static int fd1;
static int fd2;

static uint8_t mode;
static uint8_t bits = 32;
static uint32_t speed = 20000000;
static uint16_t delay = 0;

//long msg_tmp;

bool uart_open_close_flag = 0;

#define   SPI_DEBUG   0

/****************************************************************************/
//decode 倾斜角头的解析
int SPI_decode_recvFlg(char * buf, int iLen, int * index)
{
		char * pCur = buf;
		int length = iLen;
		int i = 0;

		//小于1个字节
		if(length<1)
			return -1;

		while(1){

				if( 0xAA == pCur[0] )	{	//判断开头标志
						break;
				}else{
						memcpy(pCur, pCur+1, length-1);
						length--;
						(*index) +=1 ;

						if(length<1)
							return -1;
				}
		}

	return 0;
}

int process_decode(struct RS422_data * pRS422_data)
{
		int ret = 0;
		int index = 0;
		int have_data = 1;
		int parse_length = 0;

		short PlatformThetaX = 0.00, PlatformThetaY = 0.00, Temperature = 0.00;
		
		short sum_xor=0;
		int i=0;

		int length = pRS422_data->length;
		char * buf = (char*)pRS422_data->receiveData;

		if(length <=0)
			return -1;

		while(have_data){
			index=0;
			ret = SPI_decode_recvFlg(buf, length, &index);
			length -= index;

			if(ret != 0){
				have_data=0;
				continue;
			}

			parse_length = 8;

			if(length<parse_length){

				printf(" length<dataLength ...\n");
				memset(buf+length, 0, sizeof(buf)-length);
				have_data=0;

			}else{

				//解析数据
				sum_xor=0;
				for(i=0; i<parse_length-1; i++){
					sum_xor ^= buf[i];
				}

				if( sum_xor ==  buf[parse_length-1] ){
					PlatformThetaX = buf[1]<<8|buf[2];
					PlatformThetaY =  buf[3]<<8|buf[4];
					Temperature =  buf[5]<<8|buf[6];

#if SPI_DEBUG
				printf(" PlatformThetaX=%d PlatformThetaY=%d weather=%d \n", PlatformThetaX, PlatformThetaY, Temperature);
#endif

				if((abs(PlatformThetaX) > 30000)||(abs(PlatformThetaY) >30000))
				{
					// do nothing
				}
				else
				{
					hPositionX = PlatformThetaX*0.001;		
					hPositionY = PlatformThetaY*0.001;
					Tempre = Temperature*0.01; 
				}
				
					memcpy(buf, buf+parse_length, length-parse_length);
					memset(buf+length-parse_length, 0, sizeof(buf)-(length-parse_length)  );
					length -= parse_length;
				}else{
					printf("[%s] sum of xor=%02x   buf[%d]=%02x is error.\n", __func__, sum_xor,  parse_length-1, buf[parse_length-1]  );
					memcpy(buf, buf+1, length-1);
					length--;
				}
			}

		}
		pRS422_data->length = length ;
		return 0;
}

/****************************************************************************/

//mirror　瞄准镜　头的解析
int SPI_mirror_recvFlg(char * buf, int iLen, int * index)
{
		char * pCur = buf;
		int length = iLen;
		int i = 0;

		//小于1个字节
		if(length<1)
			return -1;

		while(1){

				if( 0xCC == pCur[0] )	{	//判断开头标志
						break;
				}else{
						memcpy(pCur, pCur+1, length-1);
						length--;
						(*index) +=1 ;

						if(length<1)
							return -1;
				}
		}

	return 0;
}

//解析激光数据。
int SPI_mirror_parse(char * pRecv)
{
		int value;

		value = pRecv[2]<<8 | pRecv[3];
		if(9999 == value){
			//LaserDistance = -1;
			//SendMessage(CMD_LASER_FAIL,LASERERR_NOECHO);
		}else if(0 == value){
			//LaserDistance = -1;
			//SendMessage(CMD_LASER_FAIL,LASERERR_NOSAMPLE);//无取样
		}else{
			//LaserDistance = value;
			//SendMessage(CMD_LASER_OK,value);//保存测距值
		}
		return 0;
}

int Process_mirror(struct RS422_data * pRS422_data)
{
		int ret = 0;
		int index = 0;
		int have_data = 1;
		int parse_length = 0;

		int16_t  laser_dis=0;
		int16_t sum_xor=0;
		int i=0;

		static int16_t last_laser_dis = 0;

		int length = pRS422_data->length;
		char * buf = (char*)pRS422_data->receiveData;

		if(length <=0 )
			return -1;

		while(have_data){
			index=0;
			ret = SPI_mirror_recvFlg(buf, length, &index);
			length -= index;

			if(ret != 0){
				have_data=0;
				continue;
			}

			parse_length = 5;

			if(length<parse_length){
				memset(buf+length, 0, sizeof(buf)-length);
				have_data=0;
			}
			else
			{
				//解析数据
				sum_xor=0;
				for(i=0; i<parse_length-1; i++){
					sum_xor ^= buf[i];
				}

				//if( sum_xor ==  buf[parse_length-1] )
				if(1)
				{
					laser_dis = buf[2]<<8|buf[3];
#if SPI_DEBUG
					printf(" count=%d laser_dis=%d  \n", buf[1], laser_dis);
#endif 
					if(valid_measure == 1)
					{
						if(9999 == laser_dis)
						{
							LaserDistance = -1;
							//msg_tmp = LASERERR_NOECHO;
							MSGDRIV_send(CMD_LASER_FAIL,(void*)LASERERR_NOECHO);
						}
						else if(0 == laser_dis)
						{
							LaserDistance = -1;	
							MSGDRIV_send(CMD_LASER_FAIL,(void*)LASERERR_NOSAMPLE);
						}
						else
						{
							LaserDistance = laser_dis;
							last_laser_dis = LaserDistance;
							//SendMessage(CMD_LASER_OK,value);
							MSGDRIV_send(CMD_LASER_OK,0);
						}
					}
					
					LaserDistance = last_laser_dis;

					valid_measure = 0;
					//LaserPORT_Ack();   send out the distance
					
					memcpy(buf, buf+parse_length, length-parse_length);
					memset(buf+length-parse_length, 0, sizeof(buf)-(length-parse_length)  );
					length -= parse_length;

					SPI_mirror_send_requst() ;
					SPI_mirror_send_ack();
				}
				else
				{
					printf("[%s] sum of xor=%02x   buf[%d]=%02x is error.\n", __func__, sum_xor,  parse_length-1, buf[parse_length-1]  );
					memcpy(buf, buf+1, length-1);
					length--;
				}
			}

		}
		pRS422_data->length = length ;
		return 0;
}

int SPI_mirror_send_requst()  //发送测距请求
{
	unsigned char buf[4] = { 0xcc, 0x01, 0x01  };

	buf[3] = ( buf[0]^buf[1]^buf[2] );

	sendDataToSpi( RS422_MIRROR, buf, sizeof(buf));

	 return 0;
}

int SPI_mirror_send_ack()  //激光数据确认
{

	unsigned char buf[4] = { 0xcc, 0x01, 0x02  };

	buf[3] = ( buf[0]^buf[1]^buf[2] );

	sendDataToSpi( RS422_MIRROR, buf, sizeof(buf));

	return 0;
}

/****************************************************************************/
//vcode　机枪耳轴　头的解析
int SPI_vcode_recvFlg(char * buf, int iLen, int * index)
{
		char * pCur = buf;
		int length = iLen;
		int i = 0;

		//小于1个字节
		if(length<1)
			return -1;

		while(1){

				if( 0x5FF5 == ((pCur[0]<<8)|(pCur[1])) )	{	//判断开头标志
						break;
				}else{
						memcpy(pCur, pCur+1, length-1);
						length--;
						(*index) +=1 ;

						if(length<1)
							return -1;
				}
		}

	return 0;
}

int Process_vcode(struct RS422_data * pRS422_data)
{
		int ret = 0;
		int index = 0;
		int have_data = 1;
		int parse_length = 0;

		int  angle=0;
		int positive=1;
		int i=0;
		double tmp;

		int length = pRS422_data->length;
		char * buf = (char*)pRS422_data->receiveData;

		if(length <=0 )
			return -1;

		while(have_data)
		{
			index=0;
			ret = SPI_vcode_recvFlg(buf, length, &index);
			length -= index;

			if(ret != 0){
				have_data=0;
				continue;
			}
			parse_length = 5;

			if(length<parse_length)
			{
				//printf(" length<dataLength ...\n");
				memset(buf+length, 0, sizeof(buf)-length);
				have_data=0;
			}
			else
			{
				//解析数据
				  positive=1;

#if SPI_DEBUG
				 printf( "buf[2]=%02x buf[3]=%02x \n" , buf[2], buf[3]);
#endif
				 angle = buf[2]<<8|buf[3];  //对超出范围的数据如何处理
				 positive = buf[4] ==0 ? 1: -1;

					tmp = (positive)*angle*0.01;
					if(tmp>= -5 && tmp<=75)
					{
#if SPI_DEBUG
						printf("tmp = %f\n",tmp);
#endif
						MachGunAngle.theta = tmp;
					}
				
					memcpy(buf, buf+parse_length, length-parse_length);
					memset(buf+length-parse_length, 0, sizeof(buf)-(length-parse_length)  );
					length -= parse_length;

				}
		}
		pRS422_data->length = length ;
		return 0;
}

/****************************************************************************/
//Grenade　榴弹耳轴　头的解析
int SPI_grenade_recvFlg(char * buf, int iLen, int * index)
{
		char * pCur = buf;
		int length = iLen;
		int i = 0;

		//小于1个字节
		if(length<1)
			return -1;

		while(1){

				if( 0x5FF5 == ((pCur[0]<<8)|(pCur[1])) )	{	//判断开头标志
						break;
				}else{
						memcpy(pCur, pCur+1, length-1);
						length--;
						(*index) +=1 ;

						if(length<1)
							return -1;
				}
		}

	return 0;
}

int Process_grenade(struct RS422_data * pRS422_data)
{
		int ret = 0;
		int index = 0;
		int have_data = 1;
		int parse_length = 0;

		int  angle=0;
		int positive=1;
		int sum_xor =0;
		int i=0;

		double tmp;
		int length = pRS422_data->length;
		char * buf = (char*)pRS422_data->receiveData;

		if(length <=0 )
			return -1;

		while(have_data)
		{
			index=0;
			ret = SPI_grenade_recvFlg(buf, length, &index);
			length -= index;

			if(ret != 0){
				have_data=0;
				continue;
			}

			parse_length = 5;

			if(length<parse_length){

				printf(" length<dataLength ...\n");
				memset(buf+length, 0, sizeof(buf)-length);
				have_data=0;

			}else{

				//解析数据

				positive=1;

				angle = buf[2]<<8|buf[3];  //不应该把所有的数据都删除掉。，对超出范围的数据如何处理
				positive = buf[4] ==0 ? 1: -1;
#if SPI_DEBUG
				printf(" about the GrenadeAngle  !!!!!! positive=%d angle=%d  \n", positive, angle);
#endif
				tmp = angle*0.01;
				if(!bGrenadeSensorOK())
				{
					MSGDRIV_send(CMD_GENERADE_SENSOR_OK,0);
				}

				if(tmp >= -5 && tmp <= 75)
					GrenadeAngle = positive*tmp;

				memcpy(buf, buf+parse_length, length-parse_length);
				memset(buf+length-parse_length, 0, sizeof(buf)-(length-parse_length)  );
				length -= parse_length;
					
			}
		}
		pRS422_data->length = length ;
		return 0;
}

/****************************************************************************/
//hcode　炮塔方位　头的解析
int SPI_hcode_recvFlg(char * buf, int iLen, int * index)
{
		char * pCur = buf;
		int length = iLen;
		int i = 0;

		//小于1个字节
		if(length<1)
			return -1;

		while(1){

				if( 0x8FF8 == ((pCur[0]<<8)|(pCur[1]))  )	{	//判断开头标志
						break;
				}else{
						memcpy(pCur, pCur+1, length-1);
						length--;
						(*index) +=1 ;

						if(length<1)
							return -1;
				}
		}

	return 0;
}

int Process_hcode(struct RS422_data * pRS422_data)
{
		int ret = 0;
		int index = 0;
		int have_data = 1;
		int parse_length = 0;

		int  angle=0;
		int positive=1;
		int sum_xor =0;
		int i=0;
		double tmp;
		int length = pRS422_data->length;
		char * buf = (char*)pRS422_data->receiveData;

		if(length <=0 )
			return -1;

		while(have_data)
		{
			index=0;
			ret = SPI_hcode_recvFlg(buf, length, &index);
			length -= index;

			if(ret != 0)
			{
				have_data=0;
				continue;
			}

			parse_length = 5;

			if(length<parse_length){

				printf(" length<dataLength ...\n");
				memset(buf+length, 0, sizeof(buf)-length);
				have_data=0;

			}
			else
			{

				//解析数据

				  positive=1;

					angle = buf[2]<<8|buf[3];
					positive = buf[4] ==0 ? 1: -1;	
#if SPI_DEBUG
					printf(" positive=%d angle=%d  \n", positive, angle);
#endif
					
					tmp = (positive)*angle/100.00;
					if(tmp>= -5 && tmp<=75)
						gTurretTheta.theta = tmp;
					
					pFovCtrlObj->theta = (int)tmp;
					
					memcpy(buf, buf+parse_length, length-parse_length);
					memset(buf+length-parse_length, 0, sizeof(buf)-(length-parse_length)  );
					length -= parse_length;
			}

		}
		pRS422_data->length = length ;
		return 0;
}

/****************************************************************************/

static void pabort(const char *s)
{
	perror(s);
	abort();
}


/***open all device*****/
int open_device()
{
    	int ret,returnValue=0;
		const char *device = "/dev/spidev3.0";
		const char *device1 = "/dev/spidev0.0";
		const char *device2 = "/dev/spidev1.0";
		fd0 = open(device, O_RDWR);
		fd1 = open(device1, O_RDWR);
		fd2 = open(device2, O_RDWR);
		if (fd0 < 0)
			returnValue|=0x1;
		if (fd1 < 0)
			returnValue|=0x2;
		if (fd2 < 0)
			returnValue|=0x4;

		/*
		 * spi mode
		 */
		if(fd0>=0)
		{
				ret = ioctl(fd0, SPI_IOC_WR_MODE, &mode);
				if (ret == -1)
					pabort("can't set spi mode");
		}
		if(fd1>=0)
		{
				ret = ioctl(fd1, SPI_IOC_WR_MODE, &mode);
				if (ret == -1)
					pabort("can't set spi mode");
		}
		if(fd2>=0)
		{
				ret = ioctl(fd2, SPI_IOC_WR_MODE, &mode);
				if (ret == -1)
					pabort("can't set spi mode");
		}

		/*
		 * bits per word
		 */
		if(fd0>=0)
		{
				ret = ioctl(fd0, SPI_IOC_WR_BITS_PER_WORD, &bits);
				if (ret == -1)
					pabort("can't set bits per word");
		}
		if(fd1>=0)
		{
				ret = ioctl(fd1, SPI_IOC_WR_BITS_PER_WORD, &bits);
				if (ret == -1)
					pabort("can't set bits per word");
		}
		if(fd2>=0)
		{
				ret = ioctl(fd2, SPI_IOC_WR_BITS_PER_WORD, &bits);
				if (ret == -1)
					pabort("can't set bits per word");
		}


		/*
		 * max speed hz
		 */
		if(fd0>=0)
		{
				ret = ioctl(fd0, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
				if (ret == -1)
					pabort("can't set max speed hz");
		}
		if(fd1>=0)
		{
				ret = ioctl(fd1, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
				if (ret == -1)
					pabort("can't set max speed hz");
		}
		if(fd2>=0)
		{
				ret = ioctl(fd2, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
				if (ret == -1)
					pabort("can't set max speed hz");
		}

		return returnValue;
}

int openCanDevice()
{

	return 0;
}


/****串口初始化*****
comNum  0-7
interuptThreshold  中断阈值
Baud rate    波特率
*****/
int transfer_init( uint8_t comNum, uint8_t interuptThreshold,int Baudrate)
{
	int ret;
	int comNumtemp,fdTmp;
	uint8_t BaudrateLowTemp,BaudrateHighTemp;
	switch(comNum)
	{
		case 0:
			comNumtemp=com0;
			fdTmp=fd0;
			break;
		case 1:
			comNumtemp=com1;
			fdTmp=fd0;
			break;
		case 2:
			comNumtemp=com2;
			fdTmp=fd0;
			break;
		case 3:
			comNumtemp=com3;
			fdTmp=fd1;
			break;
		case 4:
			comNumtemp=com4;
			fdTmp=fd1;
			break;
		case 5:
			comNumtemp=com5;
			fdTmp=fd1;
			break;
		case 6:
			comNumtemp=com6;
			fdTmp=fd2;
			break;
		case 7:
			comNumtemp=com7;
			fdTmp=fd2;
			break;
		default:
			return 1;
	}
	if(interuptThreshold>127||interuptThreshold<1)
		return 2;
	switch(Baudrate)
	{
		case 2400:
			BaudrateLowTemp=(uint8_t)286;
			BaudrateHighTemp=286>>8;
			break;
		case 4800:
			BaudrateLowTemp=142;
			BaudrateHighTemp=0x00;
			break;
		case 9600:
			BaudrateLowTemp=70;
			BaudrateHighTemp=0x00;
			break;
		case 19200:
			BaudrateLowTemp=34;
			BaudrateHighTemp=0x00;
			break;
		case 38400:
			BaudrateLowTemp=16;
			BaudrateHighTemp=0x00;
			break;
		case 57600:
			BaudrateLowTemp=10;
			BaudrateHighTemp=0x00;
			break;
		case 115200:
			BaudrateLowTemp=4;
			BaudrateHighTemp=0x00;
			break;
		default:
			return 3;
	}	
	uint8_t tx[] = {
		(comNumtemp+7)>>3,(((comNumtemp+7)&0x7)<<5)|(1<<4)|(0x01>>4),(0x01&0xf)<<4,0x0,  //清空接收fifo
		(comNumtemp+7)>>3,(((comNumtemp+7)&0x7)<<5)|(1<<4)|(0x00>>4),(0x00&0xf)<<4,0x0,  //拉升接收fifo
		(comNumtemp+1)>>3,(((comNumtemp+1)&0x7)<<5)|(1<<4)|(0x01>>4),(0x01&0xf)<<4,0x0,  //允许产生接收中断，禁止产生发送中断		
		(comNumtemp+3)>>3,(((comNumtemp+3)&0x7)<<5)|(1<<4)|(interuptThreshold>>4),(interuptThreshold&0xf)<<4,0x0,  //设置串口接收中断阈值16        
        (comNumtemp+2)>>3,(((comNumtemp+2)&0x7)<<5)|(1<<4)|(BaudrateLowTemp>>4),(BaudrateLowTemp&0xf)<<4,0x0,  //设置串口波特率低两位
        (comNumtemp+6)>>3,(((comNumtemp+6)&0x7)<<5)|(1<<4)|(BaudrateHighTemp>>4),(BaudrateHighTemp&0xf)<<4,0x0,   //设置串口波特率高两位
        };
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	/*
	 	struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
	 * */

	struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed,bits, };
	ret = ioctl(fdTmp, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");
	
	printf("%d初始化成功\n",comNum);   
	return 0;          
}
/***初始化全部串口****/
int transfer_init_all(uint8_t interuptThreshold,int Baudrate)
{
	int ret;
#if 1
	ret=transfer_init(0,interuptThreshold,Baudrate);
	if(ret)
		return 1;
#endif

	ret=transfer_init(1,interuptThreshold,Baudrate);
	if(ret)
		return 1;

#if 1
	ret=transfer_init(2,interuptThreshold,Baudrate);
	if(ret)
		return 1;
#endif

	ret=transfer_init(3,interuptThreshold,Baudrate);
	if(ret)
		return 1;
	ret=transfer_init(4,interuptThreshold,Baudrate);
	if(ret)
		return 1;

#if 1
	ret=transfer_init(5,interuptThreshold,Baudrate);
	if(ret)
		return 1;
	ret=transfer_init(6,interuptThreshold,Baudrate);
	if(ret)
		return 1;
#endif

	ret=transfer_init(7,interuptThreshold,Baudrate);
	if(ret)
		return 1;

	return 0;
}

int transfer_ban(int fd,int comAddr)
{
	int ret;
	      
	uint8_t tx[] = {
		(comAddr+1)>>3,(((comAddr+1)&0x7)<<5)|(1<<4)|(0x00>>4),(0x00&0xf)<<4,0x0,
          };
        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        /*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
        */
        struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed,bits, };
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
              pabort("can't send spi message");
	return 0;
}

int transfer_open(int fd,int comAddr)
{
	int ret;

	uint8_t tx[] = {
		(comAddr+1)>>3,(((comAddr+1)&0x7)<<5)|(1<<4)|(0x01>>4),(0x01&0xf)<<4,0x0,
         };
        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        /*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
        */
        struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed,bits, };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
              pabort("can't send spi message");
		return 0;
}

int transfer_readDataCount(int fd,int comAddr)
{
	int ret;
	uint8_t tx[] = {
		(comAddr+2)>>3,(((comAddr+2)&0x7)<<5)|(0<<4)|(0x00>>4),(0x00&0xf)<<4,0x0,
		(comAddr+4)>>3,(((comAddr+4)&0x7)<<5)|(0<<4)|(0x00>>4),(0x00&0xf)<<4,0x0,
          };
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	/*
			struct spi_ioc_transfer tr = {
					.tx_buf = (unsigned long)tx,
					.rx_buf = (unsigned long)rx,
					.len = ARRAY_SIZE(tx),
					.delay_usecs = delay,
					.speed_hz = speed,
					.bits_per_word = bits,
			};
      */
        struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed, bits, };
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
            pabort("can't send spi message");
	return rx[2]|((rx[6]&0x3)<<8);
}

int transfer_readOneData(int comNum)
{
	int comNumtemp,fdTmp,ret;
	switch(comNum)
	{
		case 0:
			comNumtemp=com0;
			fdTmp=fd0;
			break;
		case 1:
			comNumtemp=com1;
			fdTmp=fd0;
			break;
		case 2:
			comNumtemp=com2;
			fdTmp=fd0;
			break;
		case 3:
			comNumtemp=com3;
			fdTmp=fd1;
			break;
		case 4:
			comNumtemp=com4;
			fdTmp=fd1;
			break;
		case 5:
			comNumtemp=com5;
			fdTmp=fd1;
			break;
		case 6:
			comNumtemp=com6;
			fdTmp=fd2;
			break;
		case 7:
			comNumtemp=com7;
			fdTmp=fd2;
			break;
		default:
			return -1;
	}
	uint8_t tx[] = {
		comNumtemp>>3,((comNumtemp&0x7)<<5)|(0<<4)|(0x00>>4),(0x00&0xf)<<4,0x0,
          };
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	/*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
       */

    struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed,bits, };
	ret = ioctl(fdTmp, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	return rx[2];
}

int transfer_readData(int comNum,int length,struct RS422_data* RS422_data_buff)
{
	uint8_t* receiveDataTmp;
	int comNumTemp,fdTmp,ret,lengthTmp;
	if(comNum<0||comNum>7)
	{
		return 1;
	}
	switch(comNum)
	{
		case 0:
			comNumTemp=com0;
			fdTmp=fd0;
			break;
		case 1:
			comNumTemp=com1;
			fdTmp=fd0;
			break;
		case 2:
			comNumTemp=com2;
			fdTmp=fd0;
			break;
		case 3:
			comNumTemp=com3;
			fdTmp=fd1;
			break;
		case 4:
			comNumTemp=com4;
			fdTmp=fd1;
			break;
		case 5:
			comNumTemp=com5;
			fdTmp=fd1;
			break;
		case 6:
			comNumTemp=com6;
			fdTmp=fd2;
			break;
		case 7:
			comNumTemp=com7;
			fdTmp=fd2;
			break;
		default:
			return -1;
	}	
	pthread_mutex_lock(&RS422_data_buff->mutex);

	if(RS422_data_buff->length<0){
		lengthTmp=0;
	}else{
		lengthTmp=RS422_data_buff->length;
	}

	RS422_data_buff->length = lengthTmp+length;

	if( RS422_data_buff->length >= sizeof(RS422_data_buff->receiveData)-1 ){
		RS422_data_buff->length = 0 ;
		lengthTmp=RS422_data_buff->length;
	}

	for(int i=0;i<length;i++)
	{
		RS422_data_buff->receiveData[lengthTmp+i]=transfer_readOneData(comNum);
#if SPI_DEBUG
		printf(" %02x  ",RS422_data_buff->receiveData[lengthTmp+i]);
	}
	printf("\n");
#else
    }
#endif

	pthread_mutex_unlock(&RS422_data_buff->mutex);
	return 0;
}

void interuptHandleSpi1(struct RS422_data* RS422_ROTER_buff,struct RS422_data* RS422_DECODE_buff,struct RS422_data* RS422_BAK2_buff)
{
	int ret = 0,interuptNum=0;
	fd_set readfds;	
	while(1)
	{
#if SPI_DEBUG
		 printf("%s\n",__func__);
#endif
        	FD_ZERO(&readfds);
        	FD_SET(fd0,&readfds);
        	select(fd0+1,&readfds,NULL,NULL,NULL);
        	if(FD_ISSET(fd0,&readfds))
        	{
            	   ioctl(fd0, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
            	   interuptHandleDataSpi1(interuptNum,RS422_ROTER_buff,RS422_DECODE_buff,RS422_BAK2_buff);  
        	}
	 }
}

void interuptHandleSpi2(struct RS422_data* RS422_MIRROR_buff,struct RS422_data* RS422_VCODE_buff,struct RS422_data* RS422_BAK1_buff)
{
	int ret = 0,interuptNum=0;
	fd_set readfds;	
	while(1)
	{
#if SPI_DEBUG
		printf("%s\n",__func__);
#endif

        	FD_ZERO(&readfds);
        	FD_SET(fd1,&readfds);
        	select(fd1+1,&readfds,NULL,NULL,NULL);
        	if(FD_ISSET(fd1,&readfds))
        	{
            	   ioctl(fd1, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
            	   interuptHandleDataSpi2(interuptNum,RS422_MIRROR_buff,RS422_VCODE_buff,RS422_BAK1_buff);  
        	}
	 }
}

void interuptHandleSpi3(struct RS422_data* RS422_TEST_buff,struct RS422_data* RS422_HCODE_buff)
{
	int ret = 0,interuptNum=0;
	fd_set readfds;	
	while(1)
	{
#if SPI_DEBUG
		printf("%s\n",__func__);
#endif
        	FD_ZERO(&readfds);
        	FD_SET(fd2,&readfds);
        	select(fd2+1,&readfds,NULL,NULL,NULL);
        	if(FD_ISSET(fd2,&readfds))
        	{
            	   ioctl(fd2, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
            	   interuptHandleDataSpi3(interuptNum,RS422_TEST_buff,RS422_HCODE_buff);  
        	}
	 }
}


void interuptHandleDataSpi3(int interuptNum,struct RS422_data* RS422_TEST_buff,struct RS422_data* RS422_HCODE_buff)
{
	int fdtmp,comtmp,readCount,interuptNumTmp;
	if(interuptNum!=0)
	{	
		switch(interuptNum)
		{
				case 64:
					transfer_ban(fd2,com0);
					readCount=transfer_readDataCount(fd2,com0);
					fdtmp=fd2;
					comtmp=com0;
					transfer_readData(6,readCount,RS422_TEST_buff);
					interuptNumTmp=6;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);		
#if SPI_DEBUG
					printf("from RS422_TEST: ");
#endif
					break;
				case 128:
					transfer_ban(fd2,com1);
					readCount=transfer_readDataCount(fd2,com1);
					fdtmp=fd2;
					comtmp=com1;
					transfer_readData(7,readCount,RS422_HCODE_buff);
					interuptNumTmp=7;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_HCODE: ");
#endif
					Process_hcode(RS422_HCODE_buff);
					break;
				case 192:
					transfer_ban(fd2,com0);
					readCount=transfer_readDataCount(fd2,com0);
					fdtmp=fd2;
					comtmp=com0;
					transfer_readData(6,readCount,RS422_TEST_buff);
					interuptNumTmp=6;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_TEST: ");
#endif

					transfer_ban(fd2,com1);
					readCount=transfer_readDataCount(fd2,com1);
					fdtmp=fd2;
					comtmp=com1;
					transfer_readData(7,readCount,RS422_HCODE_buff);
					interuptNumTmp=7;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_HCODE: ");
#endif
					Process_hcode(RS422_HCODE_buff);
					break;
				default:
					fdtmp=-1;
					break;
		}
	}
}
			
void interuptHandleDataSpi2(int interuptNum,struct RS422_data* RS422_MIRROR_buff, struct RS422_data* RS422_VCODE_buff, struct RS422_data* RS422_BAK1_buff)
{
	int fdtmp,comtmp,readCount,interuptNumTmp;
	if(interuptNum!=0)
	{	
		switch(interuptNum)
		{
				case 8:
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					transfer_readData(3,readCount,RS422_MIRROR_buff);  //3
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_MIRROR: ");
#endif
					Process_mirror(RS422_MIRROR_buff);
					break;
				case 16:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					transfer_readData(4,readCount,RS422_VCODE_buff);
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_VCODE: ");
#endif
					Process_vcode(RS422_VCODE_buff);
					break;
				case 32:
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;	
					transfer_readData(5,readCount,RS422_BAK1_buff);
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_BAK1: ");
#endif
					break;
				case 24:
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					transfer_readData(3,readCount,RS422_MIRROR_buff);  //3
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_MIRROR: ");
#endif
					Process_mirror(RS422_MIRROR_buff);

					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					transfer_readData(4,readCount,RS422_VCODE_buff);
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_VCODE: ");
#endif
					Process_vcode(RS422_VCODE_buff);
					break;
				case 40:
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					transfer_readData(5,readCount,RS422_BAK1_buff);
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_BAK1: ");
#endif


					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					transfer_readData(3,readCount,RS422_MIRROR_buff);  //3
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_MIRROR: ");
#endif
					Process_mirror(RS422_MIRROR_buff);
					break;
				case 48:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					transfer_readData(4,readCount,RS422_VCODE_buff);
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_VCODE: ");
#endif
					Process_vcode(RS422_VCODE_buff);

					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					transfer_readData(5,readCount,RS422_BAK1_buff);
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_BAK1: ");
#endif
					break;
				case 56:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					transfer_readData(4,readCount,RS422_VCODE_buff);
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
		printf("from RS422_VCODE: ");
#endif
				Process_vcode(RS422_VCODE_buff);


					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					transfer_readData(5,readCount,RS422_BAK1_buff);
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_BAK1: ");
#endif

					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					transfer_readData(3,readCount,RS422_MIRROR_buff);  //3
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
					printf("from RS422_MIRROR: ");
#endif
					Process_mirror(RS422_MIRROR_buff);
					break;
				default:
					fdtmp=-1;
					exit(0);
					break;
			}
	}
}

void interuptHandleDataSpi1(int interuptNum,struct RS422_data* RS422_ROTER_buff,struct RS422_data* RS422_DECODE_buff,struct RS422_data* RS422_BAK2_buff)
{
	int i=0;
	int fdtmp,comtmp,readCount,interuptNumTmp;
	if(interuptNum!=0)
	{	
             // printf("interuptNum==%d\n",interuptNum);

			switch(interuptNum)
			{
					case 1:
						transfer_ban(fd0,com0);
						readCount=transfer_readDataCount(fd0,com0);
						fdtmp=fd0;
						comtmp=com0;

						transfer_readData(0,readCount,RS422_ROTER_buff);  //0
						interuptNumTmp=0;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);

						printf("from RS422_ROTER:--%d--RS422_R0TER_buff->length\n",RS422_ROTER_buff->length);
						//CanPort_parseByte(RS422_ROTER_buff->receiveData);
						/*
						for(i=0; i<RS422_ROTER_buff->length; i++)
						{
							printf("%02x ",RS422_ROTER_buff->receiveData[i]);
						}
						printf("\n");
						*/
						RS422_ROTER_buff->length=0;

						//printf_read_data(receiveData,readCount);
						break;
					case 2:
						transfer_ban(fd0,com1);
						readCount=transfer_readDataCount(fd0,com1);
						fdtmp=fd0;
						comtmp=com1;
						transfer_readData(1,readCount,RS422_DECODE_buff);  //1
						interuptNumTmp=1;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_DECODE:--%d--RS422_DECODE_buff->length",RS422_DECODE_buff->length);
#endif
						process_decode(RS422_DECODE_buff);
						break;

					case 3:
						transfer_ban(fd0,com0);
						transfer_ban(fd0,com1);
						readCount=transfer_readDataCount(fd0,com0);
						fdtmp=fd0;
						comtmp=com0;
						transfer_readData(0,readCount,RS422_ROTER_buff);  //0
						interuptNumTmp=0;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_ROTER:--%d--RS422_R0TER_buff->length",RS422_ROTER_buff->length);
#endif



						readCount=transfer_readDataCount(fd0,com1);
						fdtmp=fd0;
						comtmp=com1;
						transfer_readData(1,readCount,RS422_DECODE_buff);  //1
						interuptNumTmp=1;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_DECODE:--%d--RS422_DECODE_buff->length",RS422_DECODE_buff->length);
#endif
						process_decode(RS422_DECODE_buff);

						break;
					case 4:
						transfer_ban(fd0,com2);
						readCount=transfer_readDataCount(fd0,com2);
						fdtmp=fd0;
						comtmp=com2;
						transfer_readData(2,readCount,RS422_BAK2_buff); //2
						interuptNumTmp=2;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_BAK2: --%d--RS422_BAK2_buff->length\n ",RS422_BAK2_buff->length);
#endif
						Process_grenade(RS422_BAK2_buff);
						break;
					case 5:
						transfer_ban(fd0,com2);
						readCount=transfer_readDataCount(fd0,com2);
						fdtmp=fd0;
						comtmp=com2;
						transfer_readData(2,readCount,RS422_BAK2_buff); //2
						interuptNumTmp=2;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_BAK2: --%d--RS422_BAK2_buff->length\n ",RS422_BAK2_buff->length);
#endif
						Process_grenade(RS422_BAK2_buff);



						transfer_ban(fd0,com0);
						readCount=transfer_readDataCount(fd0,com0);
						fdtmp=fd0;
						comtmp=com0;
						transfer_readData(0,readCount,RS422_ROTER_buff);  //0
						interuptNumTmp=0;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_ROTER:--%d--RS422_R0TER_buff->length\n",RS422_ROTER_buff->length);
#endif
						break;

					case 6:
						transfer_ban(fd0,com2);
						readCount=transfer_readDataCount(fd0,com2);
						fdtmp=fd0;
						comtmp=com2;
						transfer_readData(2,readCount,RS422_BAK2_buff); //2
						interuptNumTmp=2;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_BAK2: --%d--RS422_BAK2_buff->length\n ",RS422_BAK2_buff->length);
#endif
						Process_grenade(RS422_BAK2_buff);
						

						transfer_ban(fd0,com1);
						readCount=transfer_readDataCount(fd0,com1);
						fdtmp=fd0;
						comtmp=com1;
						transfer_readData(1,readCount,RS422_DECODE_buff);  //1
						interuptNumTmp=1;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_DECODE:--%d--RS422_DECODE_buff->length\n",RS422_DECODE_buff->length);
#endif
						process_decode(RS422_DECODE_buff);
						break;

					case 7:
						transfer_ban(fd0,com0);
						readCount=transfer_readDataCount(fd0,com0);
						fdtmp=fd0;
						comtmp=com0;
						transfer_readData(0,readCount,RS422_ROTER_buff);  //0
						interuptNumTmp=0;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_ROTER:--%d--RS422_R0TER_buff->length\n",RS422_ROTER_buff->length);
#endif

						transfer_ban(fd0,com1);
						readCount=transfer_readDataCount(fd0,com1);
						fdtmp=fd0;
						comtmp=com1;
						transfer_readData(1,readCount,RS422_DECODE_buff);  //1
						interuptNumTmp=1;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
#if SPI_DEBUG
						printf("from RS422_DECODE:--%d--RS422_DECODE_buff->length\n",RS422_DECODE_buff->length);
#endif
						process_decode(RS422_DECODE_buff);

						transfer_ban(fd0,com2);
						readCount=transfer_readDataCount(fd0,com2);
						fdtmp=fd0;
						comtmp=com2;
						transfer_readData(2,readCount,RS422_BAK2_buff); //2
						interuptNumTmp=2;
						ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
						transfer_open(fdtmp,comtmp);
						Process_grenade(RS422_BAK2_buff);
#if SPI_DEBUG
						printf("from RS422_BAK2: --%d--RS422_BAK2_buff->length\n ",RS422_BAK2_buff->length);
#endif
						break;
					default:
						fdtmp=-1;
						break;
			}
	}
}
	

/*
int interuptHandleTest()
{		
	int ret = 0,interuptNum=0;
	fd_set readfds;	
	while(1)
	{
        	FD_ZERO(&readfds);
        	FD_SET(fd0,&readfds);
        	FD_SET(fd1,&readfds);
        	FD_SET(fd2,&readfds);
        	select(fd2+1,&readfds,NULL,NULL,NULL);
        	if(FD_ISSET(fd0,&readfds))
        	{
            	   ioctl(fd0, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
            	   interuptHandleData(interuptNum);  
        	}
       		if(FD_ISSET(fd1,&readfds))
        	{ 
            	    ioctl(fd1, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
                    interuptHandleData(interuptNum);
		}
	       	if(FD_ISSET(fd2,&readfds))
		{
		    ioctl(fd2, SPI_IOC_RD_INTERUPT_NUM, &interuptNum);
		    interuptHandleData(interuptNum);
		}
	 }
}
*/

/*
int interuptHandleDataTest(int interuptNum)
{
	int fdtmp,comtmp,readCount,readCountTmp,interuptNumTmp;
	uint8_t* receiveData;
	if(interuptNum!=0)
	{	
		printf("interuptNum==%d\n",interuptNum);
		switch(interuptNum)
		{
				case 1:
					transfer_ban(fd0,com0);
					readCount=transfer_readDataCount(fd0,com0);
					readCountTmp=readCount;
					fdtmp=fd0;
					comtmp=com0;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(0,readCountTmp,receiveData);
					}
					interuptNumTmp=0;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_ROTER: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 2:
					transfer_ban(fd0,com1);
					readCount=transfer_readDataCount(fd0,com1);
					readCountTmp=readCount;
					fdtmp=fd0;
					comtmp=com1;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(1,readCountTmp,receiveData);
					}
					interuptNumTmp=1;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_DECODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 3:	
					transfer_ban(fd0,com0);
					transfer_ban(fd0,com1);
					readCount=transfer_readDataCount(fd0,com0);
					readCountTmp=readCount;
					fdtmp=fd0;
					comtmp=com0;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(0,readCountTmp,receiveData);
					}
					interuptNumTmp=0;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_ROTER: ");			
					printf_read_data(receiveData,readCountTmp);
					readCount=transfer_readDataCount(fd0,com1);
					readCountTmp=readCount;
					fdtmp=fd0;
					comtmp=com1;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(1,readCountTmp,receiveData);
					}
					interuptNumTmp=1;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_DECODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;	
				
				case 4:
					transfer_ban(fd0,com2);
					readCount=transfer_readDataCount(fd0,com2);
					fdtmp=fd0;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(2,readCountTmp,receiveData);
					}
					interuptNumTmp=2;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK2: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 5:
					transfer_ban(fd0,com2);
					readCount=transfer_readDataCount(fd0,com2);
					fdtmp=fd0;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(2,readCountTmp,receiveData);
					}
					interuptNumTmp=2;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK2: ");
					printf_read_data(receiveData,readCountTmp);	
					transfer_ban(fd0,com0);
					readCount=transfer_readDataCount(fd0,com0);
					fdtmp=fd0;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(0,readCountTmp,receiveData);
					}
					interuptNumTmp=0;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_ROTER: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 6:
					transfer_ban(fd0,com2);
					readCount=transfer_readDataCount(fd0,com2);
					fdtmp=fd0;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(2,readCountTmp,receiveData);
					}
					interuptNumTmp=2;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK2: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd0,com1);
					readCount=transfer_readDataCount(fd0,com1);
					fdtmp=fd0;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(1,readCountTmp,receiveData);
					}
					interuptNumTmp=1;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_DECODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 7:
					transfer_ban(fd0,com0);
					readCount=transfer_readDataCount(fd0,com0);
					fdtmp=fd0;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(0,readCountTmp,receiveData);
					}
					interuptNumTmp=0;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_ROTER: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd0,com1);
					readCount=transfer_readDataCount(fd0,com1);
					fdtmp=fd0;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(1,readCountTmp,receiveData);
					}
					interuptNumTmp=1;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_DECODE: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd0,com2);
					readCount=transfer_readDataCount(fd0,com2);
					fdtmp=fd0;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(3,readCountTmp,receiveData);
					}
					interuptNumTmp=2;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK2: ");
					printf_read_data(receiveData,readCountTmp);
					break;				
				case 8:
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(3,readCountTmp,receiveData);
					}
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_MIRROR: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 16:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(4,readCountTmp,receiveData);
					}
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_VCODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 32:
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;	
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(5,readCountTmp,receiveData);
					}
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK1: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 24:
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(3,readCountTmp,receiveData);
					}
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_MIRROR: ");
					printf_read_data(receiveData,readCountTmp);
					
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(4,readCountTmp,receiveData);
					}
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_VCODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 40:
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(5,readCountTmp,receiveData);
					}
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK1: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(3,readCountTmp,receiveData);
					}
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_MIRROR: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 48:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(4,readCountTmp,receiveData);
					}
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_VCODE: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(5,readCountTmp,receiveData);
					}
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK1: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 56:
					transfer_ban(fd1,com1);
					readCount=transfer_readDataCount(fd1,com1);
					fdtmp=fd1;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(4,readCountTmp,receiveData);
					}
					interuptNumTmp=4;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_VCODE: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd1,com2);
					readCount=transfer_readDataCount(fd1,com2);
					fdtmp=fd1;
					comtmp=com2;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(5,readCountTmp,receiveData);
					}
					interuptNumTmp=5;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_BAK1: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd1,com0);
					readCount=transfer_readDataCount(fd1,com0);
					fdtmp=fd1;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(3,readCountTmp,receiveData);
					}
					interuptNumTmp=3;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_MIRROR: ");
					printf_read_data(receiveData,readCountTmp);
					break;
					
				case 64:
					transfer_ban(fd2,com0);
					readCount=transfer_readDataCount(fd2,com0);
					fdtmp=fd2;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(6,readCountTmp,receiveData);
					}
					interuptNumTmp=6;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);		
					printf("from RS422_TEST: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 128:
					transfer_ban(fd2,com1);
					readCount=transfer_readDataCount(fd2,com1);
					fdtmp=fd2;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(7,readCountTmp,receiveData);
					}
					interuptNumTmp=7;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_HCODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				case 192:
					transfer_ban(fd2,com0);
					readCount=transfer_readDataCount(fd2,com0);
					fdtmp=fd2;
					comtmp=com0;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(6,readCountTmp,receiveData);
					}
					interuptNumTmp=6;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_TEST: ");
					printf_read_data(receiveData,readCountTmp);
					transfer_ban(fd2,com1);
					readCount=transfer_readDataCount(fd2,com1);
					fdtmp=fd2;
					comtmp=com1;
					readCountTmp=readCount;
					receiveData=(uint8_t*)malloc(readCount);
					memset(receiveData,0,readCount);
					while(readCount--)
					{
						transfer_readData(7,readCountTmp,receiveData);
					}
					interuptNumTmp=7;
					ioctl(fdtmp, SPI_IOC_WR_OPEN_INTERUPT, &interuptNumTmp);
					transfer_open(fdtmp,comtmp);
					printf("from RS422_HCODE: ");
					printf_read_data(receiveData,readCountTmp);
					break;
				default:
					fdtmp=-1;
					break;
		}
		free(receiveData);
		receiveData=NULL;
	}
			
}
*/

int writeOneData(int comNum,uint8_t data)
{
	int comNumtemp,fdTmp,ret;
	switch(comNum)
	{
		case 0:
			comNumtemp=com0;
			fdTmp=fd0;
			break;
		case 1:
			comNumtemp=com1;
			fdTmp=fd0;
			break;
		case 2:
			comNumtemp=com2;
			fdTmp=fd0;
			break;
		case 3:
			comNumtemp=com3;
			fdTmp=fd1;
			break;
		case 4:
			comNumtemp=com4;
			fdTmp=fd1;
			break;
		case 5:
			comNumtemp=com5;
			fdTmp=fd1;
			break;
		case 6:
			comNumtemp=com6;
			fdTmp=fd2;
			break;
		case 7:
			comNumtemp=com7;
			fdTmp=fd2;
			break;
		default:
			return 1;
	}
	uint8_t tx[] = {
		comNumtemp>>3,((comNumtemp&0x7)<<5)|(1<<4)|(data>>4),(data&0xf)<<4,0x0,
          };
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	/*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
      */

      struct spi_ioc_transfer tr = {(unsigned long)tx,  (unsigned long)rx, ARRAY_SIZE(tx), delay, speed,bits, };
      ret = ioctl(fdTmp, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
            pabort("can't send spi message");
	return 0;
}


int sendDataToSpi(int comNum,uint8_t* buff,int length)
{
	int ret=0;
	if(length==0)
		return 3;
	for(int i=0;i<length;i++)
	{
		ret=writeOneData(comNum,buff[i]);
		if(ret==1)
		{
			printf("comNum is error");
			return 1;
		}
	}
	return 0;
}

void printf_read_data(uint8_t* receiveData,int readCountTmp)
{
	for(int i=0;i<readCountTmp;i++)
	{
	    printf("%.2X ", receiveData[i]);
	}
	printf("  receive data over\n");
}




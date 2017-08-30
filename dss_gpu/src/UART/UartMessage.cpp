#include "UartMessage.h"
#include"app_status.h"
#include"osa.h"
#include"app_global.h"
#include "osa_buf.h"
#include"osa_sem.h"
#include "msgDriv.h"
#include"dx_config.h"
#include"dx.h"
#include"app_ctrl.h"
#include "osdProcess.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include  "spiH.h"

#include "UartCanMessage.h"


#define SIZE		1024

#define CAN_ID_PANEL 		(0x0002)
#define CAN_ID_TURRET 		(0x02AC)
#define CAN_ID_MACHGUN 	(0x02B7)
#define CAN_ID_GRENADE		(0x2C2)
#define CODE_MACHGUN 		(0x37)
#define CODE_GRENADE 		(0x42)
#define CODE_TURRET   		(0x2C)



int globalflag = 1;
  OSA_SemHndl semSend;
    OSA_BufCreate tskSendBufCreate;
    OSA_BufHndl tskSendBuf;
    OSA_ThrHndl tskSendHndl;
    OSA_ThrHndl tskrecvHndl;

static int Reply_OsdVal = 0;
static int colour=0;
static unsigned int sysinfo_count = 0;
SysInfo_t SysInfo_Loop[5];
char FlirStat = 0x02;

int fov_postion_x[5] = {150,150,150,150,150}, fov_postion_y[5] = {940,910,880,850,820};
int fov_position_x_fr[5] = {63,63,63,63,63},fov_position_y_fr[5] = {530,510,490,470,450};

char menuid[33] = {0x00,0xC0,0xC3,0x4A,0x60,0x6C,0x20,0x21,0x6A,0x10,0x70,0x74,0x75,
    0x78,0x88,0x38,0x08,0x0A,0x09,0x13,0x0C,0x02,0x1A,0x0E,0x18,0x19,0x0F,0x11,0x03,0x04,
    0x05,0x06,0x07};
unsigned int MtdAlg_stat;


//��spi��Ӧ���ڵ���ݽṹ

static OSA_ThrHndl spi1ThrHandl;
static OSA_ThrHndl spi2ThrHandl;
static OSA_ThrHndl spi3ThrHandl;

struct RS422_data RS422_ROTER_buff;  //0
struct RS422_data RS422_DECODE_buff; //1
struct RS422_data RS422_BAK2_buff;    //2
struct RS422_data RS422_MIRROR_buff;  //3
struct RS422_data RS422_VCODE_buff;   //4
struct RS422_data RS422_BAK1_buff;    //5
struct RS422_data RS422_TEST_buff;    //6
struct RS422_data RS422_HCODE_buff;   //7

//spi���豸���߳�
void* spi1InteruptHandle(void* args)
{
		interuptHandleSpi1(&RS422_ROTER_buff, &RS422_DECODE_buff, &RS422_BAK2_buff);
		return NULL;
}

//spi���豸���߳�
void* spi2InteruptHandle(void* args)
{
		interuptHandleSpi2(&RS422_MIRROR_buff, &RS422_VCODE_buff, &RS422_BAK1_buff);
		return NULL;
}

//spi���豸���߳�
void* spi3InteruptHandle(void* args)
{
		interuptHandleSpi3(&RS422_TEST_buff, &RS422_HCODE_buff);
		return NULL;
}

//��ʼ��spi�豸
int  InitDevice_spi()
{
		int status = -1;
		int ret = -1;

		memset(RS422_ROTER_buff.receiveData,0,2048);
		RS422_ROTER_buff.length=0;
		pthread_mutex_init(&RS422_ROTER_buff.mutex,NULL);

		memset(RS422_DECODE_buff.receiveData,0,2048);
		RS422_DECODE_buff.length=0;
		pthread_mutex_init(&RS422_DECODE_buff.mutex,NULL);

		memset(RS422_BAK2_buff.receiveData,0,2048);
		RS422_BAK2_buff.length=0;
		pthread_mutex_init(&RS422_BAK2_buff.mutex,NULL);

		memset(RS422_MIRROR_buff.receiveData,0,2048);
		RS422_MIRROR_buff.length=0;
		pthread_mutex_init(&RS422_MIRROR_buff.mutex,NULL);

		memset(RS422_VCODE_buff.receiveData,0,2048);
		RS422_VCODE_buff.length=0;
		pthread_mutex_init(&RS422_VCODE_buff.mutex,NULL);

		memset(RS422_BAK1_buff.receiveData,0,2048);
		RS422_BAK1_buff.length=0;
		pthread_mutex_init(&RS422_BAK1_buff.mutex,NULL);

		memset(RS422_TEST_buff.receiveData,0,2048);
		RS422_TEST_buff.length=0;
		pthread_mutex_init(&RS422_TEST_buff.mutex,NULL);

		memset(RS422_HCODE_buff.receiveData,0,2048);
		RS422_HCODE_buff.length=0;
		pthread_mutex_init(&RS422_HCODE_buff.mutex,NULL);


		ret=open_device();   //open three spi


		switch(ret)
		{
			case 0:
					printf("all spi open success\n");
					break;
			case 1:
					printf("spi3.0 open failed\n");
					break;
			case 2:
					printf("spi0.0 open failed\n");
					break;
			case 3:
					printf("spi3.0 and spi 0.0 open failed\n");
					break;
			case 4:
					printf("spi1.0 open failed\n");
					break;
			case 5:
					printf("spi3.0 and spi 1.0 open failed\n");
					break;
			case 6:
					printf("spi0.0 and spi 1.0 open failed\n");
					break;
			case 7:
					printf("all spi open failed\n");
					return 1;
			default:
					printf("error return");
					return 1;
		}

		transfer_init_all(20,115200);//set all uart property
		// transfer_init(RS422_ROTER,20,115200); //set one uart property

		status = OSA_thrCreate(
					&spi1ThrHandl,
					spi1InteruptHandle,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);

		status = OSA_thrCreate(
					&spi2ThrHandl,
					spi2InteruptHandle,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);

		status = OSA_thrCreate(
					&spi3ThrHandl,
					spi3InteruptHandle,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);


		return 0;
}



int  Piexlx2Windows(int x,int channel)
{
	int ret=0;
	ret= round(x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][0])
	 	{
			ret=vdisWH[0][0];
	 	}

	  return ret;
}


int  Piexly2Windows(int y,int channel)
{
	 int ret=0;
	 ret= round(y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][1])
	 	{
			ret=vdisWH[0][1];
	 	}
	return  ret;
}


int  WindowstoPiexlx(int x,int channel)
{
	int ret=0;
	ret= round(x*1.0/vdisWH[0][0]*vcapWH[channel][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][0])
	 	{
			ret=vcapWH[channel][0];
	 	}


	  return ret;
}


int  WindowstoPiexly(int y,int channel)
{
	 int ret=0;
	 ret= round(y*1.0/vdisWH[0][1]*vcapWH[channel][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][1])
	 	{
			ret=vcapWH[channel][1];
	 	}
	return  ret;
}

void TempUserMtdContrl(unsigned char SetCmd)
{
    int i;
    if(SetCmd != 0 && SetCmd != 1)
        return ;
    if(MtdAlg_stat == SetCmd)
    {
        for( i = 0;i < 2;i++)
        {
            //IMGCTRL_mtdSet(i, !(SetCmd & 0x01));
        }
        MtdAlg_stat = !(SetCmd & 0x01);
    //kaiqi mtd
    }
    return ;
}

int Shift_Operation(unsigned char *buf,int len)
{
    OSA_assert(buf != NULL);
    OSA_assert(len > 0);
    int i,TempData = 0;
    for(i = len - (len - 1) % 4;i <= len;i++)
    {
        TempData |= (buf[i] << (((i - 1) % 4)  * 8));
    }

    return(TempData & 0xFFFFFFFF);
}

void MSGAPI_AckSnd(int CmdType)
{


   int bufId = 0;
   unsigned char *quebuf;

   OSA_bufGetEmpty(&(tskSendBuf), &bufId, OSA_TIMEOUT_NONE);
  	
   quebuf=(unsigned char *)tskSendBuf.bufInfo[bufId].virtAddr;
   tskSendBuf.bufInfo[bufId].size = 7;
   quebuf[3]=CmdType & 0xFF;
   OSA_bufPutFull(&(tskSendBuf), bufId);
 
   
  	
  
    return ;
}
//ext_checksum
UInt8 Ext_Checksum(const unsigned char *DataBuf,int Length)
{
    SDK_ASSERT(DataBuf !=NULL && Length >0);
    unsigned char Checksum = 0; 
    while (Length--)
    {     
        Checksum += *DataBuf++;
    }
    return Checksum;   
}



//extctrl_sndstatack
int  EXTCTRL_sndStatAck(char cmdId, char eCode,BYTE *pData)
{
    if(pData==NULL)
        return -1;
    unsigned char *sndbuf=pData;
    RET_STAT_ACK* ack_data = (RET_STAT_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 7;
    ack_data->CmdId  = cmdId;	// 0x01-POWERON 0x02-SH 0x03
    ack_data->Stuts  = 0x01;
    if(eCode)
        ack_data->Stuts  = 0x02;
        ack_data->ECode  = eCode;
        ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
                                    ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;
}
/******************************************************************************************************
  Function:    int EXTCTRL_sndTrkAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndTrkAck(int track_stuts,BYTE *pData,int cmdid)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = cmdid;
    ack_data->Data   = track_stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;
}
/******************************************************************************************************
  Function:    int EXTCTRL_sndSenorAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int EXTCTRL_sndSenorAck(int SenId, BYTE * pData)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x06;

    ack_data->Data   = SenId&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:    int EXTCTRL_sndTrkErr()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndTrkErr(short xErr,short yErr,BYTE *pData)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_TRACE_OFFSET * ack_data = (RET_TRACE_OFFSET*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 9;
    ack_data->CmdId  = 0x09;
    ack_data->offsetX = xErr;
    ack_data->offsetY = yErr;
   // printf("xErr=%d  yErr=%d\n",xErr,yErr);
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;	
}

int  EXTCTRL_sndTrkErrtest(short xErr,short yErr,BYTE *pData,int chid)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_TRACE_OFFSET * ack_data = (RET_TRACE_OFFSET*)sndbuf;
    
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 9;
    ack_data->CmdId  = 0x09;
    if(chid==0)
    	{
		xErr=640;
		yErr=512;
    	}
	else
	{
		xErr=320;
		yErr=256;

	}
    ack_data->offsetX = xErr;
    ack_data->offsetY = yErr;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:    int EXTCTRL_sndMMTAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndMMTAck(int stuts,BYTE *pData)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x0a;
    ack_data->Data	 = stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen ;
	
}
/******************************************************************************************************
  Function:    int EXTCTRL_sndMMTInfo()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndMMTInfo(unsigned char num, unsigned char *mmtInfo,BYTE *pData)
{
    if(pData==NULL)
    	return -1;
    unsigned char *sndbuf=pData;//i=0;
    RET_MMT_INFO* ack_data = (RET_MMT_INFO *)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 26;
    ack_data->CmdId  = 0x0b;
    ack_data->MMTCount = num;	// 0 ~ 3
    memcpy((char *)ack_data->offsetXY, (char *)mmtInfo, 20);
        //printf("o:xl:%02x  xh:%02x yl:%02x yh:%02x\n",ack_data->offsetXY[0],ack_data->offsetXY[1],ack_data->offsetXY[2],ack_data->offsetXY[3]);
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;
}

/******************************************************************************************************
  Function:    int  EXTCTRL_sndZoomAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndZoomAck(int cmd,int stuts,BYTE *pData)
{
    if( pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_DISP_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = cmd&0xFF;
    ack_data->Data	 = stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function: int  EXTCTRL_sndFovCtrlAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndFovCtrlAck(int fovCtrl,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x18;
    ack_data->Data   = fovCtrl&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:   int  EXTCTRL_sndProductID()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndProductID(int ver,int year,int month,int day,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    PROC_ID_ACK* ack_data = (PROC_ID_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 0x0a;
    ack_data->CmdId  = 0x15;
    ack_data->IDInfoH= 0x40|((ver & 0x00000F00)>>8);
    ack_data->IDInfoL= ver & 0x000000FF;
    ack_data->ID_year= year;
    ack_data->ID_month= month;
    ack_data->ID_day = day;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;

}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndAimAreaSizeAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndAimAreaSizeAck(int stuts,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_DISP_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x17;
    ack_data->Data	 = stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndAimAreaPosAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndAimAreaPosAck(int stuts,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_DISP_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x16;
    ack_data->Data	 = stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;		
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndTargetCoverAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndTargetCoverAck(BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf = pData;
    RET_TRI_ACK* ack_data = (RET_TRI_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 5;
    ack_data->CmdId  = 0x0c;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen ;	
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndDispAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndDispAck(char cmdId, char reqCmdId,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf = pData;
    RET_DISP_ACK* ack_data = (RET_DISP_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = cmdId;	// 0x04-ACK 0x05-NAK
    ack_data->Data  = reqCmdId;	// 0x14~0x22
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen ;		
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndTvCollimationAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndCollimationAck(int cmdText,int cmdId,BYTE *pData)
{
    if(pData== NULL)
    return -1;

    unsigned char *sndbuf=pData;
    RET_CTRL_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 0x06;
    ack_data->CmdId  = cmdId&0xFF;
    ack_data->Data   = cmdText&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
                                            ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndFrCollimationAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndTVCollimationErrAck(CMD_EXT pIStuts,BYTE *pData)
{
    if(pData== NULL)
        return -1;

    unsigned char *sndbuf=pData;

    RET_TVCOLL_ACK* ack_data = (RET_TVCOLL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 0x09;
    ack_data->CmdId  = 0x19;
    ack_data->xErrLsb = pIStuts.TempTvCollX&0xFF;
    ack_data->xErrMsb= (pIStuts.TempTvCollX >> 8)&0xFF;
    ack_data->yErrLsb = pIStuts.TempTvCollY &0xFF;
    ack_data->yErrMsb= (pIStuts.TempTvCollY >> 8)&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
                                    ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;
        
}
/******************************************************************************************************
  Function:   int EXTCTRL_sndTrkWinDisAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int EXTCTRL_sndTrkWinDisAck(int stuts,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x0d;
    ack_data->Data	 = stuts&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:   int EXTCTRL_sndSecondTrkAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int EXTCTRL_sndSecondTrkAck(int stuts,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x14;
    ack_data->Data   = stuts &0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);

    return ack_data->PckLen;	
}
/******************************************************************************************************
  Function:    int  EXTCTRL_sndTarPalAck()
  Description:
  Calls:
  Called By:
  Input:
  Output:
  Return:
  Others:
******************************************************************************************************/
int  EXTCTRL_sndTarPalAck(int TargetPalId,BYTE *pData)
{
    if(pData== NULL)
    	return -1;
    unsigned char *sndbuf=pData;
    RET_DISP_ACK* ack_data = (RET_CTRL_ACK*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 6;
    ack_data->CmdId  = 0x13;
    ack_data->Data   = TargetPalId&0xFF;
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
    					ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;	
}
//******************************************************************************************************

int  EXTCTRL_sndEnhanceAck(CMD_EXT pIStuts,BYTE *pData)
{
    if(pData== NULL)
        return -1;

    unsigned char *sndbuf=pData;

    RET_Enhance* ack_data = (RET_Enhance*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 0x06;
    ack_data->CmdId  = 0x0E;
   if (pIStuts.ImgEnhStat[pIStuts.SensorStat]==1)
    ack_data->Data = pIStuts.ImgEnhStat[pIStuts.SensorStat]&0xFF;
   else
   	ack_data->Data=2;
  
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
                                    ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;
        
}

int  EXTCTRL_sndPicpAck(CMD_EXT pIStuts,BYTE *pData)
{
    if(pData== NULL)
        return -1;

    unsigned char *sndbuf=pData;

    RET_Picp* ack_data = (RET_Picp*)sndbuf;
    ack_data->SFCC   = MSG_HEAD_SFCC;
    ack_data->SysId  = MSG_HEAD_SYSID;
    ack_data->PckLen = 0x06;
    ack_data->CmdId  = 0x0f;
    ack_data->Data = pIStuts.ImgPicp[pIStuts.SensorStat]&0xFF;
  
    ack_data->ChSum  = Ext_Checksum(sndbuf+MSG_HEAD_SIZE, \
                                    ack_data->PckLen-MSG_HEAD_SIZE-MSG_CHSUM_SIZE);
    return ack_data->PckLen;
        
}

int EXTCTRL_platformSend(BYTE *pData, UINT uLen, int context/*MPORT_Obj*/)
{

int sendLen;
CMD_EXT pIStuts;
memcpy(&pIStuts,msgextInCtrl,sizeof(CMD_EXT));
//CMD_EXT *pIStuts = msgextInCtrl;
  switch (pData[3])
    {
    case AckPowerOn:
        //OSA_printf("unit:%02x\n",pIStuts.unitFaultStat);
        
        sendLen  =  EXTCTRL_sndStatAck(pData[3],pIStuts.unitFaultStat,pData);
        break;
    case AckCheck:
       sendLen  =  EXTCTRL_sndStatAck(pData[3],pIStuts.unitFaultStat,pData);
        break;

    case AckPeroidChek: // zhou qi  zi jian 
       sendLen  =  EXTCTRL_sndStatAck(pData[3],pIStuts.unitFaultStat,pData);
        break;

    case ACK:
        sendLen  =  EXTCTRL_sndDispAck(pData[3],Reply_OsdVal,pData);
        break;

    case NAK:
        sendLen  =  EXTCTRL_sndDispAck(pData[3],Reply_OsdVal,pData);
        break;

    case AckSensor:
        sendLen  =  EXTCTRL_sndSenorAck((pIStuts.SensorStat? 2 : 1), pData);
	printf("***********************AckSensor\n");
        break;

    case AckTrkState:
        sendLen   =  EXTCTRL_sndTrkAck(pIStuts.TrkCmd,pData,AckTrkState);
        break;

    case AckTrkType:
        if(pIStuts.unitTrkStat ==2)
        {
            sendLen=EXTCTRL_sndTrkAck(2,pData,AckTrkType);
        }
        else if(pIStuts.unitTrkStat ==1)
        {
            sendLen=EXTCTRL_sndTrkAck(1,pData,AckTrkType);
        }
        else if(pIStuts.unitTrkStat ==3)
        {
            sendLen=EXTCTRL_sndTrkAck(3,pData,AckTrkType);
        }
        else
           sendLen = 0;
        break;

    case AckTrkErr:
        //printf("errx:%d*****erry:%d\n",sysinfo.errx,sysinfo.erry);
        
	    sendLen=EXTCTRL_sndTrkErr(pIStuts.trkerrx,pIStuts.trkerry,pData);
//	sendLen=EXTCTRL_sndTrkErrtest(pIStuts.trkerrx,pIStuts.trkerry,pData,pIStuts.SensorStat);

        break;

    case AckMtdStat:
        sendLen=EXTCTRL_sndMMTAck(pIStuts.MMTTempStat,pData);
        break;

    case AckMtdInfo:
        sendLen = EXTCTRL_sndMMTInfo(pIStuts.Mtdtargetnum, (unsigned char *)pIStuts.MtdOffsetXY, pData);
        break;

    case AckGun:
       sendLen=EXTCTRL_sndTargetCoverAck(pData);
        break;

    case AckWaveDoor:
        sendLen=EXTCTRL_sndTrkWinDisAck((pIStuts.TrkBomenCtrl == 1)?1:2, pData);
        break;

    case AckSmallest:
    case AckZoom:
        sendLen = EXTCTRL_sndZoomAck(pData[3],((pData[3] == AckSmallest)?pIStuts.ImgSmall:pIStuts.ImgZoom),pData);
        break;

    case AckFrColl:
        sendLen=EXTCTRL_sndCollimationAck(pIStuts.FrCollimation,pData[3],pData);
        break;

    case AckTvColl:
        //OSA_printf("time_after:%d\n",OSA_getCurTimeInMsec());
        sendLen=EXTCTRL_sndCollimationAck(pIStuts.TvCollimation,pData[3],pData);
        break;

    case AckTarPal:
        sendLen=EXTCTRL_sndTarPalAck(pIStuts.TargetPal, pData);
        break;

    case AckSerTrk:
        sendLen =EXTCTRL_sndSecondTrkAck((pIStuts.SecAcqStat == 1)?1:2,pData);
        break;
        
    case AckProId:
        //sendLen = EXTCTRL_sndProductID(pIStuts.Proc_ver,pIStuts.Proc_year, pIStuts.Proc_month, pIStuts.Proc_day, pData);
        sendLen = EXTCTRL_sndProductID(0x0100,17, 5, 28, pData);
        break;

    case AckWaveDoorPos:
       // if(pIStuts.FovMov != 0)
            sendLen=EXTCTRL_sndAimAreaPosAck(pIStuts.FovMov,pData);
      //  else
            //pPort->sendLen = 0;
        break;

    case AckWaveDoorScal:
       sendLen=EXTCTRL_sndAimAreaSizeAck(pIStuts.AimScaleStat,pData);
        break;

    case AckFov:
        sendLen=EXTCTRL_sndFovCtrlAck(pIStuts.FovCtrl,pData);
        break;

    case AckTvCollErr:
        sendLen=EXTCTRL_sndTVCollimationErrAck(pIStuts,pData);
        break;
   case Ackenhance:
	sendLen=EXTCTRL_sndEnhanceAck(pIStuts,pData);
	break;
  case AckPicp:
	sendLen=EXTCTRL_sndPicpAck(pIStuts,pData);
	break;

    default:
        sendLen = 0;
        break;
    }

return sendLen;

}


int Uart_open(char *Dev)
{
	int fd = open( Dev, O_RDWR| O_NOCTTY | O_NDELAY );
	if (-1 == fd)
	{
		perror(" [error Uart_dev_open] Can't Open Serial Port");
		return -1;
	}
	return fd;
}

void Uart_close(int fd)
{
	close(fd);
}

//
int Uart_set(int fd)
{
	struct termios options;
	tcgetattr(fd,&options);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB;

	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;
	options.c_cflag |= INPCK;
	
	options.c_iflag |= IGNPAR;
	options.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);

	
	options.c_cc[VMIN]=0;
	options.c_oflag = 0;
	options.c_lflag = 0;
	cfsetispeed(&options,B115200);
	cfsetospeed(&options,B115200);
	tcsetattr(fd,TCSANOW,&options);
	return 0;

}

int Uart_init(int fd)
{
	if(-1 == Uart_set(fd))
	{
		return -1;
	}
	return 0;
}

int Uart_recv(int fd,unsigned char *rcv_buf,int data_len)
{
	int len,fs_sel;
	fd_set fs_read;
	struct timeval timeout;
	FD_ZERO(&fs_read);
	FD_SET(fd,&fs_read);
	timeout.tv_sec = 2;
	timeout.tv_usec = 5000;
	fs_sel = select(fd+1,&fs_read,NULL,NULL,&timeout);
	if(fs_sel)
	{
		len = read(fd,rcv_buf,data_len);
		return len;
	}
	else
		return -1;
}

int Uart_send(int fd,unsigned char* send_buf,int data_len)
{
	int ret;
	ret = write(fd,send_buf,data_len);
	if(data_len == ret)
	{
		return ret;
	}
	else
	{
		tcflush(fd,TCOFLUSH);
	}
	return -1;
}

//
void* recv_pth(unsigned char *RS422,int len)
{
	   int ret;
	   
	//data bag
	    int i=0,crc=0;
	    int infolen=0;
	    int sysid=0;
	    int infohead=0;
	    int infotail=0;
	    int cmdid=0;
	    unsigned char *manbuf;
	   int fdj=0;
	   int flag=0;
	   if(RS422==NULL)
	   	{
	   		return NULL ;
	   	}
		infohead        =  RS422[0];
		sysid           =  RS422[1];
		infolen         =  RS422[2];
		cmdid           =  RS422[3];
		manbuf         =  &RS422[4];
		infotail        =  RS422[infolen - 1];

		//printf("infohead=%x  sysid=%x infolen=%x cmdid=%x  infotail=%x  ret=%d\n",infohead,sysid,infolen,cmdid,infotail,ret);
		crc=0;
		//CRC
		for(i = 0;i < (infolen - 4);i++)
		{
			crc += RS422[3 + i];
		}
		crc %= 256;

		if (infotail != crc)
		{
			printf(" [DEBUG:] %s crc err=%x  infotai=%xl\n", __func__,crc,infotail);
			return NULL;
		}

		CMD_EXT inCtrl, *pMsg = NULL;
		pMsg = &inCtrl;
		memset(pMsg,0,sizeof(CMD_EXT));
		app_ctrl_getSysData(pMsg);
		pMsg ->CmdType = cmdid & 0XFF;
		app_ctrl_freezeReset(pMsg);
		switch(cmdid)
		{
			case PowerOn:
				 MSGAPI_AckSnd( AckPowerOn);
				 app_ctrl_setReset(pMsg);
				
				printf("power on\n");
				break;

			case CheckStart:
			
				if(manbuf[0] == 0x01)
				 MSGAPI_AckSnd( AckCheck);
				printf("CheckStart\n");
				break;

			case SensorStat:
				
				if(manbuf[0] == 0x01)
				{
					pMsg->SensorStat = 0;//tv
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_LEFT_X(BLOCK_BASE_OSD_TEXT_WKMODE),87);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_TOP_Y(BLOCK_BASE_OSD_TEXT_WKMODE),200);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_LEFT_X(BLOCK_BASE_OSD_TEXT_WKMODE + 1),20);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_TOP_Y(BLOCK_BASE_OSD_TEXT_WKMODE + 1),170);
#if 0
					CFGID_FIELD_GET(pMsg->unitAxisX ,CFGID_TRACK_TV_AXIX);
					CFGID_FIELD_GET(pMsg->unitAxisY ,CFGID_TRACK_TV_AXIY);
					if(pMsg->unitAxisX<0||pMsg->unitAxisX>1280)
							pMsg->unitAxisX      = 640;
						if(pMsg->unitAxisY<0||pMsg->unitAxisY>1024)
							pMsg->unitAxisY      = 512;
#endif
			
					
				}
				else if(manbuf[0] == 0x02)
				{
					pMsg->SensorStat = 1;//fir
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_LEFT_X(BLOCK_BASE_OSD_TEXT_WKMODE),87);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_TOP_Y(BLOCK_BASE_OSD_TEXT_WKMODE),230);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_LEFT_X(BLOCK_BASE_OSD_TEXT_WKMODE + 1),20);
					CFGID_CONFIG_SET(CFGID_OSD_TEXT_1_TOP_Y(BLOCK_BASE_OSD_TEXT_WKMODE + 1),190);
#if 0
					CFGID_FIELD_GET(pMsg->unitAxisX ,CFGID_TRACK_FR_AXIX);
					CFGID_FIELD_GET(pMsg->unitAxisY ,CFGID_TRACK_FR_AXIY);
					if(pMsg->unitAxisX<0||pMsg->unitAxisX>1280)
							pMsg->unitAxisX      = 640;
						if(pMsg->unitAxisY<0||pMsg->unitAxisY>1024)
							pMsg->unitAxisY      = 512;
#endif
					
					
				}
				else
					{
					MSGAPI_AckSnd( AckSensor);
					}

				app_ctrl_setSensor(pMsg);
				
				break;
				
			case CmdTrack:

				if(pMsg->AvtTrkStat == eTrk_mode_acq)  //detailed design for technological argeement
			            {
			                 if(manbuf[0] == 0x02 || manbuf[0] == 0x03 || manbuf[0] == 0x04)
			                    pMsg->TrkCmd = 0x02;
			                else 
			                     pMsg->TrkCmd = 0x01; 
			            }
			            else
			             {
			                if(manbuf[0] == 0x01)
			                    pMsg->TrkCmd = 0x01;
			                else
			                    pMsg->TrkCmd = 0x02;
			             }
			           	printf("*****************%s  avtrkstat=%d  mmt=%d\n",__func__,pMsg->AvtTrkStat ,pMsg->ImgMtdStat[pMsg->SensorStat]);
			            if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Disable)
			            {
			                if((pMsg->AvtTrkStat == eTrk_mode_acq)
			                            || (pMsg->AvtTrkStat == eTrk_mode_target)||(pMsg->AvtTrkStat == eTrk_mode_sectrk))
			                {
			                    if(manbuf[0] == 0x1)
			                        pMsg->AvtTrkStat=eTrk_mode_acq;
			                    else if(manbuf[0] == 0x2)
			                    	{
			                    	//printf("*****************%s\n",__func__);
			                        pMsg->AvtTrkStat=eTrk_mode_target;
			                    	}
			                }
			                else if(pMsg->AvtTrkStat == eTrk_mode_mtd)
			                {
			                    if(manbuf[0] == 0x01)
			                        pMsg->AvtTrkStat = eTrk_mode_acq;
			                }
			            }
			            else if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Enable)
			            {
			                if((pMsg->AvtTrkStat == eTrk_mode_acq) 
			                                || (pMsg->AvtTrkStat == eTrk_mode_target) || (pMsg->AvtTrkStat == eTrk_mode_mtd))
			                {
			                    if(manbuf[0] == 0x01)
			                    {
			                        pMsg->AvtTrkStat = eTrk_mode_acq;         
			                        TempUserMtdContrl(0x00);
			                    }
			                    else if(manbuf[0] == 0x02)
			                    {
			                       if((pMsg->AvtTrkStat == eTrk_mode_acq) 
			                                || (pMsg->AvtTrkStat == eTrk_mode_target))
			                        {
			                            pMsg->AvtTrkStat = eTrk_mode_target;
			                           TempUserMtdContrl(0x01);
			                        }
			                    }
			                    else if(manbuf[0] == 0x03)
			                    {
			                        if(pMsg->AvtTrkStat == eTrk_mode_acq)
			                                    pMsg->AvtTrkStat = eTrk_mode_mtd;
			                    }
			                }
			            }
			        	
			            if(manbuf[0] == 0x04 && pMsg->SecAcqStat)
			            {
			                pMsg->AvtTrkStat = eTrk_mode_search;
			
			                if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Enable)
			                      TempUserMtdContrl(0x01);
			                pMsg->SecAcqFlag = 0x00; //for secondtrk cross display
			          
					  OSA_printf("pMsg->AvtPixelX=%d pMsg->AvtPixelY=%d\n",pMsg->AvtPixelX,pMsg->AvtPixelY);
			            }
			            
			            if(manbuf[0] == 0x01)  //for secondtrk cross show
			            {
			                if(pMsg->SecAcqStat == 0x01)
			                       pMsg->SecAcqFlag = 0x01;
			            }
			            app_ctrl_setTrkStat(pMsg);  

				// MSGDRIV_send(MSGID_EXT_INPUT_TRACK, 0);
				//MSG_EVENT(pMsg,manbuf[0]);
				break;

			case CmdMmt:
				      if(manbuf[0] ==0x1)
				            {
				              pMsg->MMTTempStat = 0x01;//two channel mtd open all
				                pMsg->ImgMtdStat[pMsg->SensorStat] = eImgAlg_Enable;
				                pMsg->ImgMtdStat[(pMsg->SensorStat + 1) % 2] = eImgAlg_Enable;
				            }
				            else if(manbuf[0] ==0x2)
				            {
				                pMsg->MMTTempStat = 0x02;
				                pMsg->ImgMtdStat[pMsg->SensorStat] = eImgAlg_Disable;
				                pMsg->ImgMtdStat[(pMsg->SensorStat + 1) % 2] = eImgAlg_Disable;
				            }
				            else if(manbuf[0] ==0x3)
				            {
				                pMsg->MMTTempStat= 0x03;
				                if (pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
				                {
				                    pMsg->ImgMtdSelect[pMsg->SensorStat]  =  eMMT_Next;  //mtd num to next
				                    app_ctrl_setMmtSelect(pMsg);
							MSGAPI_AckSnd(  AckMtdStat);
				                    break;
				                }
				                else
				                    pMsg->MMTTempStat = 0x02;
				            }

					     else if(manbuf[0] ==0x4)
				            {
				                pMsg->MMTTempStat= 0x04;
				                if (pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
				                {
				                    pMsg->ImgMtdSelect[pMsg->SensorStat]  =  eMMT_Prev;  //mtd num to next
				                    app_ctrl_setMmtSelect(pMsg);
							MSGAPI_AckSnd(  AckMtdStat);
				                    break;
				                }
				                else
				                    pMsg->MMTTempStat = 0x02;
				            }
					
				            else
				            {
				                if(pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
				                   pMsg->MMTTempStat = 0x01;
				                else
				                   pMsg->MMTTempStat = 0x02;  
				            }

				            if(pMsg->AvtTrkStat == eTrk_mode_acq )
				                MtdAlg_stat = pMsg->ImgMtdStat[pMsg->SensorStat];
				            else
				               MtdAlg_stat = 0;

						
				            app_ctrl_setMMT(pMsg);
				break;

			case CMDehance:
				  if(manbuf[0] ==0x1)
				            {
				                pMsg->ImgEnhStat[pMsg->SensorStat] = 0x01;
				  	     }
				  else if(manbuf[0] ==0x2)
				  	{

						pMsg->ImgEnhStat[pMsg->SensorStat] = 0x00;
						pMsg->ImgEnhStat[pMsg->SensorStat^1] = 0x00;
				  	}
				  
				app_ctrl_setEnhance(pMsg);

				break;

		       case CmdPicp:
				  if(manbuf[0] ==0x01)
				            {
				                pMsg->ImgPicp[pMsg->SensorStat] = 0x01;
				  	     }
				  else if(manbuf[0] ==0x02)
				  	{

						pMsg->ImgPicp[pMsg->SensorStat] = 0x02;
				  	}
				  else if(manbuf[0] ==0x03)
				  	{
						pMsg->ImgPicp[pMsg->SensorStat] = 0x03;

				  	}
				  else if(manbuf[0] ==0x04)
				  	{
						pMsg->ImgPicp[pMsg->SensorStat] = 0x04;
						  dxCfg[0].configId=CFGID_SYS_SAVE_CONFIG_AS;
						  dxCfg[0].value=0;
						CFGID_CONFIG_SETSAVE(CFGID_SENSOR_TV_PICP_POS,   pMsg->PicpPosStat);
						 CFGID_CONFIG_SETSAVE(CFGID_SENSOR_FR_PICP_POS,   pMsg->PicpPosStat);
						
						 Dx_sendCfgMsg(NULL,DX_MSGID_SET_CFG,&dxCfg[0],FALSE);
				  	}
				  
				  
				app_ctrl_setPicp(pMsg);

				break;
			case CmdGun:
				
				MSGAPI_AckSnd(AckGun);
				
            			app_ctrl_setForceCoast(pMsg);
				OSA_printf("the cmd gun\n");
				break;
			
			case TrkWaveDoor:
				
				  pMsg->TrkBomenCtrl = manbuf[0];
          			  app_ctrl_setTrkBomen(pMsg); // bomen show or hide
          			  OSA_printf("TrkWaveDoor\n");
				break; 

			case WaveDoorPosition:
				  if(manbuf[0]    !=  0 && (manbuf[0] == 1 || manbuf[0] == 2))
			                pMsg->FovMov =1 ;

			            if(manbuf[1]    !=  0 && (manbuf[1] == 1 || manbuf[1] == 2))
			                pMsg->FovMov = 2;

			            if((manbuf[0] !=0)&&(manbuf[1] !=0) &&  (manbuf[0] == 1 || manbuf[0] == 2)
			                        && (manbuf[1] == 1 || manbuf[1] == 2))
			                pMsg->FovMov = 3;

			            if((manbuf[0] < 0x00 || manbuf[0] > 0x02) && (manbuf[1] < 0x00 || manbuf[1] > 0x02))
			                pMsg->FovMov = 0;

			            if(manbuf[0]   ==  1)
			                pMsg->AvtMoveX=eTrk_ref_right;
			            else if(manbuf[0]   ==  2)
			                pMsg->AvtMoveX=eTrk_ref_left;//eTrk_ref_left;
			            else
			                pMsg->AvtMoveX=eTrk_ref_no;
			            
			             if(manbuf[1]  ==  1)
			                pMsg->AvtMoveY=eTrk_ref_down;//eTrk_ref_right;//eTrk_ref_left;
			            else if(manbuf[1] == 2)
			                pMsg->AvtMoveY=eTrk_ref_up;//eTrk_ref_up;//eTrk_ref_right;//eTrk_ref_left;
			            else
			                pMsg->AvtMoveY=eTrk_ref_no;

				    OSA_printf("WaveDoorPosition FovMov=%d   AvtMoveX=%d     AvtMoveY=%d  \n",pMsg->FovMov,pMsg->AvtMoveX,pMsg->AvtMoveY);
			            app_ctrl_setAimPos(pMsg);
				break; 


				  case CmdSmallest:  //smallest is 1280x1024------> 720x576    bitrate
				            if(manbuf[0] == 1)
				            {
				                pMsg->ZoomMultiple = 256.0 / 81;// 16 /9 *(16 / 9)
				                pMsg->ImgSmall  = 0x01;
				                pMsg->ImgZoomStat[0] = eZoom_Ctrl_ADD;
				            }
				            else if(manbuf[0] == 2)
				            {
				                pMsg->ImgSmall = 0x02;
				                if(pMsg->ImgZoom == 0x01)
				                    pMsg->ZoomMultiple = 1024.0 / 81;
				                else
				                {
				                    pMsg->ZoomMultiple = 1.0;
				                    pMsg->ImgZoomStat[0] = eZoom_Ctrl_Pause;
				                }
				            }
				            else
				            {
				                MSGAPI_AckSnd( AckSmallest);
				                break;
				            }
				            app_ctrl_setZoom(pMsg);
				            break;
				  case CmdZoom:
				            if(manbuf[0] == 1)
				           {
				                pMsg->ZoomMultiple = 1024.0 / 81; //(32 /9)*(32/9)      //zoom is 720x576------>360x288
				                pMsg->ImgZoom = pMsg->ImgZoomStat[0] = eImgAlg_Enable;

				            }
				            else if(manbuf[0] == 2)
				            {
				                pMsg->ImgZoom = 0x02;
						  pMsg->ImgZoomStat[0] = eImgAlg_Disable;
				              //  if(pMsg->ImgSmall == 0x01)
				               //     pMsg->ZoomMultiple = 256.0 / 81;
				             //   else
				               // {
				               //     pMsg->ZoomMultiple = 1.0;
				               //     pMsg->ImgZoomStat[0] = eImgAlg_Disable;
				              //   }
				             }
				            else
				            {
				                MSGAPI_AckSnd( AckZoom);
				                break;
				            }             
				            app_ctrl_setZoom(pMsg);
				            break;
				case FrColl:
			            if(pMsg->SensorStat == 0x01)
			            {
			                pMsg->FrCollimation = manbuf[0] & 0x03;
			                if(pMsg->FrCollimation != 0x01
			                    && pMsg->FrCollimation != 0x02)
			                {
			                    MSGAPI_AckSnd(AckFrColl);
			                    break;
			                }
			               // pMsg->unitAxisX[pMsg->SensorStat ] = (manbuf[2] << 8 )| manbuf[1];
			              //  pMsg->unitAxisY[pMsg->SensorStat ] = (manbuf[4] << 8 )| manbuf[3];
			                pMsg->CollPosXFir = (manbuf[2] << 8 )| manbuf[1];
			                pMsg->CollPosYFir = (manbuf[4] << 8 )| manbuf[3];
					  printf("the  fir coll AvtPixelX=%d AvtPixelY=%d\n",pMsg->CollPosXFir,pMsg->CollPosYFir);
					  //printf("fcoe coll!!!!!!!the avt AvtPixelX=%d AvtPixelY=%d\n",pMsg->AvtPixelX,pMsg->AvtPixelY);
			                app_ctrl_setFRColl(pMsg);
					   printf("FrColl  \n");
			            }
				 
			            else
			                MSGAPI_AckSnd( AckFrColl);
				     printf("FrColl  \n");
			            break;

			        case TvColl:
			            if(pMsg->SensorStat == 0x00)
			            {
			                //OSA_printf("time_pre:%d\n",OSA_getCurTimeInMsec());
			                pMsg->TvCollimation = manbuf[0];
			                if(pMsg->TvCollimation != 0x01
			                        && pMsg->TvCollimation != 0x02)
			                {
			                    MSGAPI_AckSnd( AckTvColl);
			                    break;
			                }
			                app_ctrl_setTvColl(pMsg);
			            }
			            else
			                MSGAPI_AckSnd( AckTvColl);

			            break;

			        case AimScal:

				            if(manbuf[0] == 0x01 || manbuf[0] == 0x02)
				                        pMsg->AimScaleStat= manbuf[0] & 0x03;
				            else
				                pMsg->AimScaleStat = 0x00;
				                
				            if(manbuf[0] == 1)
				            {
				                if(pMsg->AvtTrkAimSize < 4)
				                    pMsg->AvtTrkAimSize++;
				            }
				            else if(manbuf[0] == 2)
				            {
				                if(pMsg->AvtTrkAimSize > 0)
				                    pMsg->AvtTrkAimSize--;
				            }
					     // OSA_printf("11111111111111111111pre pMsg->AvtTrkAimSise:%d\n",pMsg->AvtTrkAimSize);
				            app_ctrl_setAimSize(pMsg);
				            break;
				   case TargetPal:
				            if(manbuf[0] == 0x01 || manbuf[0] == 0x02)
				                pMsg->TargetPal = manbuf[0] & 0x3;
				            app_ctrl_setTargetPal(pMsg);
				            break;
				   case ReadProcId:
				            //if(manbuf[0] == 0x01)
				             MSGAPI_AckSnd( AckProId);
				            break;
				    case CmdFov:
				            pMsg->FovCtrl=manbuf[0] &0xFF;       
				            if(pMsg->FovCtrl < 0x01 
				                    ||pMsg->FovCtrl  > 0x05)
				            {
				                MSGAPI_AckSnd( AckFov);
				                break;
				            }
				            
				            pMsg->unitFovAngle[pMsg->SensorStat] = (pMsg->FovCtrl - 1) % 5;
				            app_ctrl_setFovCtrl(pMsg);
				            break;

				      case CmdSerTrk:
				            if(manbuf[0] == 0x01)
				            {
				                pMsg->SecAcqFlag = 
				                        pMsg->SecAcqStat = 0x01;
						if(pMsg->SensorStat==0)
							{
							pMsg->ImgPixelX[0]=641;
							pMsg->ImgPixelY[0]=513;
							pMsg->ImgPixelX[1]=321;
							pMsg->ImgPixelY[1]=257;
							}
						else
							{
							pMsg->ImgPixelX[0]=641;
							pMsg->ImgPixelY[0]=513;
							pMsg->ImgPixelX[1]=321;
							pMsg->ImgPixelY[1]=257;

							}
				            }
				            else if(manbuf[0] == 0x02)
				            {
				                pMsg->SecAcqStat = 
				                    pMsg->SecAcqFlag = 0x00;
				            }
					     

					      printf("SecAcqStat:%d\n",pMsg->SecAcqFlag );
				            app_ctrl_setSerTrk(pMsg);
				            break;
				  case CmdSerTrkXY:
				            pMsg->ImgPixelX[pMsg->SensorStat] = manbuf[0] | (manbuf[1] << 8);   //for sectrk
				            pMsg->ImgPixelY[pMsg->SensorStat] = manbuf[2] | (manbuf[3] << 8);
					    // pMsg->ImgPixelX[pMsg->SensorStat^1] = pMsg->ImgPixelX[pMsg->SensorStat];
					   //  pMsg->ImgPixelY[pMsg->SensorStat^1] = pMsg->ImgPixelY[pMsg->SensorStat] ;
					    #ifdef VIDEO1280X1024

					     #else
					      if(pMsg->SensorStat==1)
					      	{
						      pMsg->ImgPixelX[pMsg->SensorStat] = WindowstoPiexlx( pMsg->ImgPixelX[pMsg->SensorStat],pMsg->SensorStat); //for sectrk
					             pMsg->ImgPixelY[pMsg->SensorStat] =  WindowstoPiexly(pMsg->ImgPixelY[pMsg->SensorStat],pMsg->SensorStat);
					      	}


					    #endif
				           
				            printf("pixelx:%d pixely:%d\n",pMsg->ImgPixelX[pMsg->SensorStat],pMsg->ImgPixelY[pMsg->SensorStat]);
				          if(pMsg->SensorStat==0)
				          	{
					            if(pMsg->ImgPixelX[pMsg->SensorStat] > vcapWH[pMsg->SensorStat][0]-50 || pMsg->ImgPixelX[pMsg->SensorStat] < 50
					                                    ||pMsg->ImgPixelY[pMsg->SensorStat] > vcapWH[pMsg->SensorStat][1]-50 || pMsg->ImgPixelY[pMsg->SensorStat] < 50 || pMsg->SecAcqStat == 0)
					            	{
					            		 MSGAPI_AckSnd( AckSerTrk);
					                    break;
					            	}
				          	}
						else if(pMsg->SensorStat==1)
							{
						#ifdef VIDEO1280X1024
							if(pMsg->ImgPixelX[pMsg->SensorStat] > 1240 || pMsg->ImgPixelX[pMsg->SensorStat] < 50
					                                    ||pMsg->ImgPixelY[pMsg->SensorStat] > 974 || pMsg->ImgPixelY[pMsg->SensorStat] < 50 || pMsg->SecAcqStat == 0)
					            	{
					            		 MSGAPI_AckSnd( AckSerTrk);
					                    break;
					            	}

					        #else
							 if(pMsg->ImgPixelX[pMsg->SensorStat] > vcapWH[pMsg->SensorStat][0]-25|| pMsg->ImgPixelX[pMsg->SensorStat] < 25
					                                    ||pMsg->ImgPixelY[pMsg->SensorStat] >  vcapWH[pMsg->SensorStat][1]-25 || pMsg->ImgPixelY[pMsg->SensorStat] < 25 || pMsg->SecAcqStat == 0)
					            	{
					            		 MSGAPI_AckSnd( AckSerTrk);
					                    break;
					            	}
						#endif


							}

				            //pMsg->SecAcqFlag = 0x01;
				            app_ctrl_setSerTrk(pMsg);	
				            break;
				 case SysInit:
				            Reply_OsdVal = SysInit;

				            if(pMsg->SysMode != 0x01)
				            {  
				                MSGAPI_AckSnd( NAK);
				                break;
				            }    
				            if((manbuf[0]  < 0x11 || (manbuf[0] > 0x17 && manbuf[0] < 0x21) \
				                                        ||(manbuf[0] > 0x25 && manbuf[0] < 0x31) ||(manbuf[0] > 0x31 && manbuf[0] < 0x41)))
				            {  
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            
				            if(manbuf[0] > 0x43)
				            {  
				                MSGAPI_AckSnd( NAK);
				                break;
				            } 
				            
				            if((manbuf[1] < 0x01 || manbuf[1] > 0x04) && (manbuf[0] > 0x43 || manbuf[0] < 0x41))
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            
				            if(sysinfo_count  < 5 )
				            {
				                SysInfo_Loop[sysinfo_count].SysSub = manbuf[0] & 0xFF;
				                SysInfo_Loop[sysinfo_count].SysStat = manbuf[1] & 0xFF;
				            }
				            else
				            {
				                for( i = 0; i< 4; i ++)
				                    SysInfo_Loop[i] = SysInfo_Loop[i + 1]; // shift operation

				                SysInfo_Loop[i].SysSub = manbuf[0] & 0xFF;
				                SysInfo_Loop[i].SysStat = manbuf[1] & 0xFF;
				            }
				            sysinfo_count ++;

						
						OSA_printf("SysSub1=%x SysSub2=%x SysSub3=%x SysSub4=%x SysSub5=%x\n",SysInfo_Loop[0].SysSub,
							SysInfo_Loop[1].SysSub, SysInfo_Loop[2].SysSub, SysInfo_Loop[3].SysSub, SysInfo_Loop[4].SysSub);
				            // for  info num is greater than five,first is to delete,next is to first
				          //  {
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS0, SysInfo_Loop[0].SysSub);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS1, SysInfo_Loop[1].SysSub);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS2, SysInfo_Loop[2].SysSub);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS3, SysInfo_Loop[3].SysSub);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS4, SysInfo_Loop[4].SysSub);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT0, SysInfo_Loop[0].SysStat);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT1, SysInfo_Loop[1].SysStat);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT2, SysInfo_Loop[2].SysStat);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT3, SysInfo_Loop[3].SysStat);
				                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT4, SysInfo_Loop[4].SysStat);	
				          //  }
				            printf("********************SysInit\n");
				            MSGAPI_AckSnd( ACK);
				            break;

			   case SoftVer:
				            Reply_OsdVal = SoftVer;
				            if(pMsg->SysMode!= 0x01)
				            {     
				                CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_EN,0x00);
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            memcpy((void*)pMsg->Swdata,manbuf,5);

				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_EN,0x01);
				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_0,pMsg->Swdata[0]);
				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_1,pMsg->Swdata[1]);
				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_YEAR,pMsg->Swdata[2]);
				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_MON,pMsg->Swdata[3]);
				            CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_DAY,pMsg->Swdata[4]);

				            MSGAPI_AckSnd( ACK);
				            break;
			 case SysMode:
				            Reply_OsdVal = SysMode;

				            if(manbuf[0] < 0x01 || manbuf[0] > 0x0a)
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            
				            if(pMsg->SysMode != manbuf[0])
				            {
				                pMsg->SysMode = manbuf[0];            
				                if(pMsg->SysMode != 0x01)   // sys info  display at init workmode 
				                {
				                    memset(SysInfo_Loop,0,sizeof(SysInfo_Loop));
				                    sysinfo_count = 0;
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS0, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS1, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS2, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS3, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SUBSYS4, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT0, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT1, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT2, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT3, 0);
				                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_SYSSTAT4, 0);
				                }
				                app_ctrl_setSysmode(pMsg);
				                CFGID_CONFIG_SET(CFGID_OC25_SOFTVER_EN,0);
				            }
				            CFGID_CONFIG_SET(CFGID_OC25_MODECHOOSE,pMsg->SysMode);
				            MSGAPI_AckSnd( ACK);
				            break;

			  case FovSelect:
				            Reply_OsdVal = FovSelect;    //for fovselect display postion
				            pMsg->FovStat =manbuf[0];
				            if(pMsg->FovStat < 0x01 || pMsg->FovStat > 0x05)
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }

				            CFGID_CONFIG_SET(CFGID_OC25_FOVCHOOSE,pMsg->FovStat);

				            if((pMsg->FovStat >= 1) && (pMsg->FovStat <= 5))
				            {
				             
				                CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), fov_postion_x[pMsg->FovStat -1]); 
				                CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), fov_postion_y[pMsg->FovStat - 1]);
				                CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 + 1), fov_position_x_fr[pMsg->FovStat -1]);
				                CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 + 1), fov_position_y_fr[pMsg->FovStat - 1]);
						  app_ctrl_setFovselect(pMsg);

						//get biax
						#if 0
						  	switch(pMsg->FovStat)
						    			{
						    			case 1:
										CFGID_FIELD_GET(CFGID_IMAGE_AXIX1);
						   		 		CFGID_FIELD_GET(CFGID_IMAGE_AXIY1);
										break;
									case 2:
										CFGID_FIELD_GET(CFGID_IMAGE_AXIX2);
						   		 		CFGID_FIELD_GET(CFGID_IMAGE_AXIY2);
										break;
									case 3:
										CFGID_FIELD_GET(CFGID_IMAGE_AXIX3);
						   		 		CFGID_FIELD_GET(CFGID_IMAGE_AXIY3);
										break;
									case 4:
										CFGID_FIELD_GET(CFGID_IMAGE_AXIX4);
						   		 		CFGID_FIELD_GET(CFGID_IMAGE_AXIY4);
										break;
									case 5:
										CFGID_FIELD_GET(CFGID_IMAGE_AXIX5);
						   		 		CFGID_FIELD_GET(CFGID_IMAGE_AXIY5);
										break;
									default:
										break;
						  		}
						#endif
				        
				            }
				            MSGAPI_AckSnd( ACK);
				            break;
			        case UpdateTvS: //tv work stat
				            Reply_OsdVal = UpdateTvS;

				            if(manbuf[0] < 0x01 || manbuf[0] > 0x04)
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				                
				            if(manbuf[0]==0x01||manbuf[0]==0x02)
				                pMsg->TVSStat = manbuf[0];
				            else if(manbuf[0]==0x03||manbuf[0]==0x04)
				                pMsg->TVSStat = manbuf[0];

				            CFGID_CONFIG_SET(CFGID_OC25_TVSTAT,pMsg->TVSStat);
				            MSGAPI_AckSnd( ACK);
				            break;
				   case UpdateFlir: // fr work stat
				            Reply_OsdVal = UpdateFlir;
				            if(manbuf[0] < 0x01 || manbuf[0] > 0x06)
				            {
				                MSGAPI_AckSnd( NAK);  //unusual deal
				                break;
				            }
				            FlirStat = manbuf[0];
				            if(manbuf[0]==0x01||manbuf[0]==0x02)
				            {
				                pMsg->FLIRPower= manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_IRPOWER,pMsg->FLIRPower);
				            }
				            else if(manbuf[0]==0x03||manbuf[0]==0x04)
				            {
				                pMsg->FLIRStat= manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_IRSTAT,pMsg->FLIRStat);
				            }
				            else if(manbuf[0]==0x05||manbuf[0]==0x06)
				            {
				                pMsg->FLIRPal = manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_IRPAL,pMsg->FLIRPal);
				            }
				            
				            MSGAPI_AckSnd( ACK);
				            break;


			        case UpdateLrf://lrf work stat
				            Reply_OsdVal = UpdateLrf; 
				            if(manbuf[0] < 0x01 || (manbuf[0] > 0x0b && manbuf[0] != 0x10 && manbuf[0] != 0x11))
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            
				            if(manbuf[0]==0x01||manbuf[0]==0x02)
				            {
				                pMsg->LRFStat =manbuf[0];
				                pMsg->LRFPowerOn = pMsg->LRFStat % 2;

				                CFGID_CONFIG_SET(CFGID_OC25_LRFSTAT,pMsg->LRFStat);
				                CFGID_CONFIG_SET(CFGID_OC25_LRFPOWER,pMsg->LRFPowerOn);
				            }
				            else if(manbuf[0]==0x03||manbuf[0]==0x04)
				            {
				                pMsg->LRFNStat = manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_LRFNSTAT,pMsg->LRFNStat);
				            }
				            else if(manbuf[0]==0x05||manbuf[0]==0x06)
				            {
				                pMsg->LRFReady = manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_LRFREADY,pMsg->LRFReady);
				            }
				            else if(manbuf[0]==0x07||manbuf[0]==0x08)
				            {
				                pMsg->LRFBright = 1;
				                pMsg->LRFRun = manbuf[0]%3;
				                CFGID_CONFIG_SET(CFGID_OC25_LRFBRIGHT,pMsg->LRFBright);
				                CFGID_CONFIG_SET(CFGID_OC25_LRFRUN,pMsg->LRFRun);
				            }
				            else if(manbuf[0]==0x09)
				            {
				                pMsg->LRFRun = 0;
				                pMsg->LRFBright = 0;
				                CFGID_CONFIG_SET(CFGID_OC25_LRFBRIGHT,pMsg->LRFBright);
				                CFGID_CONFIG_SET(CFGID_OC25_LRFRUN,pMsg->LRFRun);
				            }
				            else if(manbuf[0]==0x10||manbuf[0]==0x11)
				            {
				                pMsg->LRFAllow = manbuf[0];
				                CFGID_CONFIG_SET(CFGID_OC25_LRFALLOW,pMsg->LRFAllow);
				            }
				    	else if(manbuf[0] == 0x0a ||manbuf[0] == 0x0b)
				    	{
				    		pMsg->LRFEngLev = manbuf[0];
				    		CFGID_CONFIG_SET(CFGID_OC25_LRFENGLEV,pMsg->LRFEngLev);
				    	}
				            MSGAPI_AckSnd( ACK);
				            break;

				   case CmdVcp:
				            Reply_OsdVal = CmdVcp;
				            if(manbuf[0] != 0x01 && manbuf[0] != 0x02)
				            {
				                MSGAPI_AckSnd( NAK);
				                break;
				            }
				            CFGID_CONFIG_SET(CFGID_OC25_GTSSTAT,manbuf[0]);
				            MSGAPI_AckSnd( ACK);
				            break;

			case CmdSaveCross:
					            Reply_OsdVal = CmdSaveCross;
						     dxCfg[0].configId=CFGID_SYS_SAVE_CONFIG_AS;
						     dxCfg[0].value=0;
							 #if 0
						    if(pMsg->SensorStat==0)
						    	{
						    	
						    		switch(pMsg->FovStat)
						    			{
						    			case 1:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX1,pMsg->unitAxisX[pMsg->SensorStat ] );
						   		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY1,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 2:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX2,pMsg->unitAxisX[pMsg->SensorStat ] );
						   		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY2,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 3:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX3,pMsg->unitAxisX[pMsg->SensorStat ] );
						   		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY3,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 4:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX4,pMsg->unitAxisX[pMsg->SensorStat ] );
						   		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY4,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 5:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX5,pMsg->unitAxisX[pMsg->SensorStat ] );
						   		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY5,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									default:
										break;


						    			}
						    		
						    	}
						    else
						    	{

								switch(pMsg->FovStat)
						    			{
						    			case 1:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX6,pMsg->unitAxisX[pMsg->SensorStat ] );
						      		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY6,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 2:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX7,pMsg->unitAxisX[pMsg->SensorStat ] );
						      		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY7,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 3:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX8,pMsg->unitAxisX[pMsg->SensorStat ] );
						      		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY8,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 4:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX9,pMsg->unitAxisX[pMsg->SensorStat ] );
						      		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY9,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									case 5:
										CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIX10,pMsg->unitAxisX[pMsg->SensorStat ] );
						      		 		CFGID_CONFIG_SETSAVE(CFGID_IMAGE_AXIY10,pMsg->unitAxisY[pMsg->SensorStat ] );
										break;
									default:
										break;
									}
						       	

						    	}
							#endif
						     Dx_sendCfgMsg(NULL,DX_MSGID_SET_CFG,&dxCfg[0],FALSE);
						    //gDXD_infoSave
						#if Coll_Save
					            if(manbuf[0] == 0x1)
					            {
					                pMsg->AvtCfgSave = 0x01;
					                app_ctrl_setSaveCfg(pMsg);
					            }
						#else
					            if(manbuf[0] == 0x1)
					            {
					                if((pMsg->TvCollimation != 0x01) &&(pMsg->FrCollimation != 0x01))
					                {
					                    pMsg->AvtCfgSave = 0x01;
					                    app_ctrl_setSaveCfg(pMsg);
					                }
					                else 
					                    app_ctrl_SaveCollXY();
					            }
						#endif   
					            else
					            {
					                MSGAPI_AckSnd( NAK);
					                break;
					            }
					            break;
		       case CmdUpdateMeau:
					            Reply_OsdVal = CmdUpdateMeau;
					            for(i = 0;i < 33;i++)
					            {
					                if(manbuf[0] == menuid[i])
					                    flag = 0x01;
					            }
					            
					            if(flag == 0x00)
					            {
					                MSGAPI_AckSnd( NAK);  //for unusual deal
					                break;
					            }      
					            pMsg->MenuStat   = manbuf[0];
					            CFGID_CONFIG_SET(CFGID_OC25_MENUINFO,manbuf[0]);
					            MSGAPI_AckSnd( ACK);
					            break;

			case CmdShowLv:
					            Reply_OsdVal = CmdShowLv;

					            if(manbuf[0] < 0x01 || manbuf[0] > 0x04)
					            {
					                MSGAPI_AckSnd( NAK);
					                break;
					            }
					            pMsg->DispGrp[0] = manbuf[0]%5;
					            pMsg->DispGrp[1] = pMsg->DispGrp[0];
					            app_ctrl_setDispGrade(pMsg);
					            break;
			   case CmdShowColor:
					            Reply_OsdVal = CmdShowColor;
					            
					            if(manbuf[0] < 0x01 || manbuf[0] > 0x07)
					            {
					                MSGAPI_AckSnd( NAK);
					                break;
					            }
					            pMsg->DispColor[0] = manbuf[0] & 0x7;
					            pMsg->DispColor[1] = pMsg->DispColor[0];
					            app_ctrl_setDispColor(pMsg);
						     if( pMsg->DispColor[0]==0x07)
						     	{

								     dxCfg[0].configId=CFGID_SYS_SAVE_CONFIG_AS;
						     		     dxCfg[0].value=0;
								    CFGID_CONFIG_SETSAVE(CFGID_RTS_TV_SEN_COLOR,   colour);
								    CFGID_CONFIG_SETSAVE(CFGID_RTS_FR_SEN_COLOR,   colour);
							
						     		    Dx_sendCfgMsg(NULL,DX_MSGID_SET_CFG,&dxCfg[0],FALSE);
								    printf("CmdShowColor\n");
								

						     	}
							else
								{

									colour=pMsg->DispColor[0];
								}
					            break;

			 case ChangeCrossPos:
				   		    
					            Reply_OsdVal = ChangeCrossPos;
						    //zhu pos  ci pos
						     if(pMsg->SensorStat==0)
						     	{
						            pMsg->AvtPosXTv = (manbuf[1] << 8) | manbuf[0];
						            pMsg->AvtPosYTv = (manbuf[3] << 8) | manbuf[2];
							     pMsg->AvtPosXFir= (manbuf[5] << 8) | manbuf[4];
						            pMsg->AvtPosYFir= (manbuf[7] << 8) | manbuf[6];  
							    
						     	}
							 else
						 	{

							     pMsg->AvtPosXFir = (manbuf[1] << 8) | manbuf[0];
						            pMsg->AvtPosYFir = (manbuf[3] << 8) | manbuf[2];
							     pMsg->AvtPosXTv= (manbuf[5] << 8) | manbuf[4];
						            pMsg->AvtPosYTv= (manbuf[7] << 8) | manbuf[6];  


						 	}
							 #ifdef VIDEO1280X1024

							 #else
						//OSA_printf("pri pMsg->AvtMoveXsec:%d  pMsg->AvtMoveYsec:%d\n",pMsg->AvtPosXFir,pMsg->AvtPosYFir);
							pMsg->AvtPosXFir=WindowstoPiexlx( pMsg->AvtPosXFir,1);
							pMsg->AvtPosYFir=WindowstoPiexly( pMsg->AvtPosYFir,1);


							 #endif
					            app_ctrl_setAxisPos(pMsg);
						#if 1
					           // OSA_printf("1:%02x 2:%02x 3:%02x 4:%02x \n",manbuf[0],manbuf[1],manbuf[2],manbuf[3]);
					         //   OSA_printf("pMsg->AvtMoveX:%d  pMsg->AvtMoveY:%d\n",pMsg->AvtPosXTv,pMsg->AvtPosYTv);
						  //   OSA_printf("4:%02x 5:%02x 6:%02x 7:%02x \n",manbuf[4],manbuf[5],manbuf[6],manbuf[7]);
						 //   OSA_printf("pMsg->AvtMoveXsec:%d  pMsg->AvtMoveYsec:%d\n",pMsg->AvtPosXFir,pMsg->AvtPosYFir);
						#endif
					            break;

			 case TarCoordinate: //for target xy 
					            Reply_OsdVal = TarCoordinate;

					            pMsg->TarLongitude = manbuf[3]<<24|manbuf[2]<<16|manbuf[1]<<8|manbuf[0];
					            pMsg->TarLatitude  = manbuf[7]<<24|manbuf[6]<<16|manbuf[5]<<8|manbuf[4];
					            pMsg->TarHeight    = manbuf[9]<<8|manbuf[8];
					            CFGID_CONFIG_SET(CFGID_OC25_TAR_LON,pMsg->TarLongitude);
					            CFGID_CONFIG_SET(CFGID_OC25_TAR_LAT,pMsg->TarLatitude);
					            CFGID_CONFIG_SET(CFGID_OC25_TAR_ALT,pMsg->TarHeight);
					            break;

		        case SysInfoDisp:
					            Reply_OsdVal = SysInfoDisp;

					            pMsg->SysMsgId = manbuf[0];
					            pMsg->SysMsgLen = infolen - 6;// skip head+cmdId+msgId(data[0])+checksum
					            CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_ENABLE,0x01);
					            CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STATE,pMsg->SysMsgId);
					            if(pMsg->SysMsgId == 0 || pMsg->SysMsgId == 1 ||pMsg->SysMsgId == 2)
					            {
					                //just for tv fov angle
					                if(pMsg->SysMsgId == 0)
					                {
					                    pMsg->unitFovAngle[pMsg->SensorStat] = ((manbuf[2]<<8)&0xFF00)|(manbuf[1]&0xFF);
					                    app_ctrl_setTvFovCtrl(pMsg);//(manbuf[2]<<8)&o0xFF00 | manbuf[1]&0xFF
					                }
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_DATA_0,manbuf[1]);
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_DATA_1,manbuf[2]);
					            }
					            else if(pMsg->SysMsgId == 3)
					            {
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,0);
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING1,0);
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING2,0);
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING3,0);
					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING4,0);

					                CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRINGLEN,pMsg->SysMsgLen);
					                
					                if((pMsg->SysMsgLen >= 1) && (pMsg->SysMsgLen <= 4))
					                {
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,Shift_Operation(manbuf, pMsg->SysMsgLen));
					                }
					                else if((pMsg->SysMsgLen >= 5) && (pMsg->SysMsgLen <= 8))
					                {
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,Shift_Operation(manbuf, 4));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING1,Shift_Operation(manbuf, pMsg->SysMsgLen));
					                }
					                else if((pMsg->SysMsgLen >= 9) && (pMsg->SysMsgLen <= 12))
					                {
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,Shift_Operation(manbuf, 4));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING1,Shift_Operation(manbuf,8));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING2,Shift_Operation(manbuf,pMsg->SysMsgLen));
					                }
					                else if((pMsg->SysMsgLen >= 13) && (pMsg->SysMsgLen <= 16))
					                {
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,Shift_Operation(manbuf, 4));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING1,Shift_Operation(manbuf,8));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING2,Shift_Operation(manbuf,12));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING3,Shift_Operation(manbuf,pMsg->SysMsgLen));
					                }
					                else if((pMsg->SysMsgLen >= 17) && (pMsg->SysMsgLen <= 20))
					                {
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING,Shift_Operation(manbuf, 4));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING1,Shift_Operation(manbuf,8));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING2,Shift_Operation(manbuf,12));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING3,Shift_Operation(manbuf,16));
					                    CFGID_CONFIG_SET(CFGID_OC25_SYSINFO_STRING4,Shift_Operation(manbuf,pMsg->SysMsgLen));
					                }
					            }	
					            break;
			case AngleStat:
					            pMsg->PaneData = (manbuf[1]<<8)| manbuf[0];
					            pMsg->TitleData = ( manbuf[3] <<8) |manbuf[2];

					            CFGID_CONFIG_SET(CFGID_RTS_TV_SEN_PAN_ANG,pMsg->PaneData);
					            CFGID_CONFIG_SET(CFGID_RTS_TV_SEN_TIL_ANG,pMsg->TitleData);	
					            CFGID_CONFIG_SET(CFGID_RTS_FR_SEN_PAN_ANG,pMsg->PaneData);
					            CFGID_CONFIG_SET(CFGID_RTS_FR_SEN_TIL_ANG,pMsg->TitleData);
					            break;
		       case DistantStat:
					            pMsg->Distant  = (manbuf[1]<<8) | manbuf[0];
					            CFGID_CONFIG_SET(CFGID_OC25_TAR_RNG,pMsg->Distant);
					            break;
			 case PTZalgvideo:
						     pMsg->TrkPanev=(manbuf[1]<<8) | manbuf[0];
						     pMsg->TrkTitlev=( manbuf[3] <<8) |manbuf[2];
						     app_ctrl_setverti(pMsg);
						     break;
			default:
				break;
		}				
	
}

//
int  send_pth(unsigned char * RS422)
{
	
	CMD_EXT pIStuts;
	memset(&pIStuts,0,sizeof(CMD_EXT));
	unsigned char *pData;
	memset(RS422,0,sizeof(char)*48);
	int bufId=0;
	int sendLen=0;
       OSA_bufGetFull(&tskSendBuf, &bufId, OSA_TIMEOUT_FOREVER);
        sendLen = tskSendBuf.bufInfo[bufId].size;
        memcpy(RS422, tskSendBuf.bufInfo[bufId].virtAddr,sendLen);
        OSA_bufPutEmpty(&tskSendBuf, bufId);
	
      sendLen=EXTCTRL_platformSend(RS422,128,0);
	
	return sendLen;
	


}



void inituartmessage()
{
    int status;
    int i=0;
    tskSendBufCreate.numBuf = OSA_BUF_NUM_MAX;
    for (i = 0; i < tskSendBufCreate.numBuf; i++)
    {
        tskSendBufCreate.bufVirtAddr[i] = SDK_MEM_MALLOC(128);
        OSA_assert(tskSendBufCreate.bufVirtAddr[i] != NULL);
        memset(tskSendBufCreate.bufVirtAddr[i], 0, 128);
    }
    OSA_bufCreate(&tskSendBuf, &tskSendBufCreate);
    status = OSA_semCreate(&semSend, 2, 0);
    OSA_assert(status == OSA_SOK);

}
 
int uartMessage(void) 
{

	return 0;
}


void* fdTest_open(void* prm)
{
	int i;
	int fp;
	char messageRecvBuf[SIZE];
	char hex[SIZE];
	char* p_ch = messageRecvBuf;
	char* p_hex = hex;

	
	if(mkfifo("fifo",0666) == -1)
	{
		if(errno != EEXIST)
		{
			perror("mkfifo");
			return NULL;
		}
	}
	
	fp = open("/home/ubuntu/fifo",O_RDONLY);
	
	if(fp == -1)
	{
		perror("open");
		return NULL;
	}
	
	//printf("fp = %d\n",fp);
	while(1)
	{
		memset(messageRecvBuf,0,sizeof(messageRecvBuf));
		read(fp,messageRecvBuf,SIZE);


		strToHex(p_ch,p_hex);
		//printf("p_hex = %s\n",p_hex);
		for(i =0;i<10;i++)
			printf("p_hex[%d]= %x\n",i,p_hex[i]);
		
		processMessage(p_hex);
		//if(strncmp("quit",(char *)messageRecvBuf,4) == 0)
		//	break;
	}
	if(-1 == close(fp))
	{
		//perror("close");
		return NULL;
	}
	return NULL;
}

char valueToHexCh(const int value)
{
	char result = '\0';
	if(value >=0 && value <=9)
	{
		result = (char)(value + 48);
	}
	else if(value >=10 && value <= 15)
	{
		result = (char)(value-10+65);
	}
	else
		;
	return result;
}

int strToHex(char* ch,char* hex)
{
	int high,low;
	int tmp = 0;
	if(ch == NULL || hex == NULL)
		return -1;
	if(strlen(ch) == 0)
		return -2;
printf("strlen(ch) = %d\n",strlen(ch));
	while(*ch)
	{
		tmp = (int)*ch;
		high = (tmp >> 4)&0xf;
		low = tmp &0xf;
		*hex++ = high;//valueToHexCh(high);
		*hex++ = low;//valueToHexCh(low);
		ch++;
	}
	
	*hex = '\0';
	return 0;
}


void processMessage(char* pData)
{
	unsigned short cmd_ext[SIZE] ;
	
	int i = 0;
	
	for(i=0;i<16;i+=4)
	{
	//	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		cmd_ext[i/2] = (pData[i+1]<<8) + pData[i];
		cmd_ext[i/2 +1] = (pData[i+3]<<8)+pData[i+2];
	}

	if(cmd_ext[0] == 0xA1)
	{
		//B4
		{
			//bit 7
			if((cmd_ext[1]>>7) & 0x1 == 0)
				;//enable shoot
			else
				;//disable shoot

			//bit 6-4
			if((cmd_ext[1]>>4) & 0x7 == 0)
				;// mei xuan zhe dan zhong
			else if((cmd_ext[1]>>4) & 0x7 == 1)
				;// 5.8 ji qiang dan
			else if((cmd_ext[1]>>4) & 0x7 == 2)
				;//35mm liu sha shang dan
			else if((cmd_ext[1]>>4) & 0x7 == 3)
				;//35mm liu fang bao dan
			else
				;//no valid

			//bit 3-2
			if((cmd_ext[1]>>2) & 0x3 == 0)
				MSGDRIV_send(CMD_BUTTON_BATTLE,NULL);//gong zuo
			else if((cmd_ext[1]>>2) & 0x3 == 0)
				MSGDRIV_send(CMD_BUTTON_CALIBRATION,NULL);// jiao zhun	
			
			//bit 1-0
			if((cmd_ext[1] & 0x3) == 0)
				;//zi dong zhuang biao gong kuang
			else if((cmd_ext[1] & 0x3) == 1)
				;//jing jie yu jing gong kuang
		}

		//B5
		{
			//BIT 7-6
			if((cmd_ext[2]>>6) & 0x3 == 1)
				;// land
			else if(cmd_ext[2]>>6 & 0x3 == 0)
				;// sky
			else
				;//nothing

			//BIT 5-4
			if((cmd_ext[2]>>4) & 0x3 == 0)
				;// dan fa
			else if((cmd_ext[2]>>4) & 0x3 == 1)
				;//lian fa
			else
				;//lian fa

			//BIT 3-2
			if((cmd_ext[2]>>2) & 0x3 == 0)
				;//le jian guang -- da shi chang
			else if((cmd_ext[2]>>2) & 0x3 == 1)
				;//ke jian guang -- xiao shi chang

			//BIT 1 default		5.8 ji qiang lian fa fang shi
			if(cmd_ext[2] & 0x1== 0)
				;//duan dian she		
			else if(cmd_ext[2] & 0x1 == 1)
				;//lian she		
		}

		//B6
		{
			//bit 7
			if((cmd_ext[3]>>7) & 0x1== 0)
				;//fang da an niu an xia
			else if((cmd_ext[3]>>7) & 0x1== 1)
				;//fang da an niu tai qi

			
			//bit 6
			if((cmd_ext[3]>>6) & 0x1== 0)
				;//tu ceng an niu an xia
			else if((cmd_ext[3]>>6) & 0x1== 1)
				;// tu ceng an niu tai qi

			//bit 5
			if((cmd_ext[3]>>5) & 0x1== 0)
				;//zeng qiang xian shi an niu an xia
			else if((cmd_ext[3]>>5) & 0x1== 1)
				;//zeng qiang xian shi an niu tai qi

			//bit 4
			if((cmd_ext[3]>>4) & 0x1== 0)
				;//ce ju fang shi an niu an xia
			else if((cmd_ext[3]>>4) & 0x1== 1)
				;//ce ju fang shi an niu tai qi

			//bit 1
			if((cmd_ext[3]>>1) & 0x1== 0)
				;//bo dong kai guan dian ping 0
			else if((cmd_ext[3]>>1) & 0x1== 1)
				;//bo dong kai guan dian ping 1


			//bit 0
			if(cmd_ext[3] & 0x1== 0)
				;//ji guang ce ju an niu an xia
			else if(cmd_ext[3] & 0x1== 1)
				;//ji guang ce ju an niu tai qi
			
		}
		
	}
	else if(cmd_ext[0] == 0xA2)
	{
		//B4
		if(cmd_ext[1] & 0xf== 0x0)
			;//wu xiao
		else if(cmd_ext[1] & 0xf== 0x1)
			;//fang wei +
		else if(cmd_ext[1] & 0xf== 0x2)
			;//fang wei -
		else if(cmd_ext[1] & 0xf== 0x3)
			;//fu yang +
		else if(cmd_ext[1] & 0xf== 0x4)
			;//fu yang -
		else if(cmd_ext[1] & 0xf== 0x5)
			;//que ren
		else if(cmd_ext[1] & 0xf== 0x6)
			;//tui chu
		else if(cmd_ext[1] & 0xf== 0x7)
			;//jie suo
		else if(cmd_ext[1] & 0xf== 0x8)
			;//zi jian
		else
			;//wu xiao

		
	}
	

	//printf("cmd_ext = %s\n",cmd_ext);


	//if(!memcmp(&cmd_ext[0],"00",sizeof(cmd_ext[0])))
	{
	}
	
	return ;
}


void chartohex(char* buf,unsigned char *recv,int recvlen)
{
	unsigned char * recvhex = recv;
	char* tmp = NULL;
 	tmp = buf;
	int i=0;
	int len = recvlen;
	int no = 0;
	unsigned char gethex[48];
	memset(gethex,0,sizeof(gethex));
	for(i=0;i<len;i++)
	{
		if(buf[i] >= '0' && buf[i] <= '9')
		{
			recvhex[no] = (buf[i] - 48);
			//if(i != len-2)
			//	recvhex = recvhex<<4;
			no++;
		}
		else if(buf[i] >= 'A' && buf[i] <= 'F')
		{
			recvhex[no] = (buf[i] - 55);
			//if(i != len-2)
			//	recvhex = recvhex<<4;
			no++;
		}	
		else if(buf[i] >= 'a' && buf[i] <= 'f')
		{	
			recvhex[no] = (buf[i] - 87);
			//if(i != len-2)
			//recvhex = recvhex<<4;
			no++;
		}
		else
			continue;
	}		


	compundHex(recvhex,gethex,no);
	memcpy(recv,gethex,no/2+1);
	
	//for(i = 0;i<no/2;i++)
	//	printf("recvhex = %x\n",recv[i]);

	
	return ;
}


void compundHex(unsigned char* recv,unsigned char* get,int len)
{
	unsigned char* tmp = recv;
	unsigned char* out = get;
	int i =0;
	int num =0;
	for(i=0;i<len;i+= 2)
	{
		out[num++] = tmp[i] <<4 | tmp[i+1];
	}

	//for(i = 0;i<len;i++)
		//printf("recvhex = %d\n",out[i]);
	
	return ;
}

void* tcpTestFunc(void* prm)
{
	int sockfd,connfd;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{

	}

	struct sockaddr_in seraddr,cliaddr;

	int portnum = atoi((char *)prm);
	//printf("portnum = %d\n",portnum);

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(portnum);
	seraddr.sin_addr.s_addr = inet_addr("192.168.1.199");

	if(bind(sockfd,(struct sockaddr *)&seraddr,sizeof(seraddr)) == -1)
	{
		perror("bind");
		return NULL;
	}

	if(listen(sockfd,5) == -1)
	{

	}


	printf("first\n");
	//int clilen = sizeof(cliaddr);
	socklen_t clilen = sizeof(cliaddr);
	int count = 0;
	while(1)
	{
		connfd = accept(sockfd,(struct sockaddr *)&cliaddr,&clilen);
		printf("second\n");

		if(connfd == -1)
		{
			perror("accept");
			return NULL;
		}

		int recvlen;
		char buf[128];
		unsigned char recvhex[48];
		
		memset(buf,0,sizeof(buf));
		memset(recvhex,0,sizeof(recvhex));
		while(1)
		{
			
			recvlen = recv(connfd,buf,sizeof(buf),0);

			//printf("recvlen = %d\n",recvlen);
			if(recvlen == -1)
			{
				perror("recv");
				return NULL;
			}

			if(recvlen == 0)
			{
				//printf("let me go !!!\n");
				close(connfd);
				break;
			}

			//printf("%s\n",buf);
			
			chartohex(buf,recvhex,recvlen);
			NetPort_ParseByte(recvhex);
			
		}
		count ++;
		printf("count = %d\n",count);
	}
	return NULL;
}



/**********************************jie xi ******************************************/

int NetPort_ParseByte(unsigned char* buf)
{	
	switch(stoh2(buf))
	{
		case CAN_ID_PANEL:
			WeaponCtrlPORT_ParseBytePanel(buf);
			break;
		case CAN_ID_TURRET:
			//WeaponCtrlPORT_ParseByteTurret(buf);
			break;
		case CAN_ID_MACHGUN:
		//	WeaponCtrlPORT_ParseByteMachGun(buf);
			break;
		case CAN_ID_GRENADE:
			//WeaponCtrlPORT_ParseByteGrenade(buf);
			break;
		default:
			break;
	}
	
	return 0;
}


/**********************************  test spi ******************************************/
//can ͷ�Ľ���
int CanPort_recvFlg(char * buf, int iLen, int * index)
{
	char * pCur = buf;
	int length = iLen;

	//С�ڣ����ֽ�
	if(length<2)
	{
		//printf(" length<2 \n");
		return -1;
	}
	int i=0;
	while(1)
	{
			if( 0x0002 == stoh2(pCur) )		//�жϿ�ͷ��־
					break;
			else{
					//printf("move one data..%d\n",i++);
					memcpy(pCur, pCur+1, length-1);
					length--;
					(*index) +=1 ;

					if(length<2)
					{
						//printf(" length<2 \n");
						return -1;
					}
			}
	}

	return 0;
}


// can��ͨ�Ž���
int CanPort_parseByte(unsigned char* buf )
{
	switch(stoh2(buf))
	{
		case CAN_ID_PANEL:
			WeaponCtrlPORT_ParseBytePanel(buf);
			break;
		case CAN_ID_TURRET:
			//WeaponCtrlPORT_ParseByteTurret(buf);
			break;
		case CAN_ID_MACHGUN:
			//	WeaponCtrlPORT_ParseByteMachGun(buf);
			break;
		case CAN_ID_GRENADE:
			//WeaponCtrlPORT_ParseByteGrenade(buf);
			break;
		default:
			break;
	}
	return 0;
}
void * SPI_CAN_process(void * prm)
{
		//CanPort_parseByte();

		char buf[2048] = {0};
		char tempBuf[2048] = {0};
		int bufLen=2048;

		int length=0;
		int nread=0;
		int ret=0;
		int index=0;
		int dataLength=0;
		int i =0;
		fd_set readfds;
		int canfd = 0;
		int haveData=0;

		 InitDevice_spi();
		
		OpenCANDevice();

		canfd = GetCanfd();

		while(1)
		{
				FD_ZERO(&readfds);
				FD_SET(canfd,&readfds);
				select(canfd+1,&readfds,NULL,NULL,NULL);
				if(FD_ISSET(canfd,&readfds))
				{
							//���û�����ߣ�������
							if(bufLen < length || length >sizeof(buf))
							{
									printf("bufLen < length, length=0");
									length = 0;
							}
							//�����
							nread = 0;
							length= 0;
							nread =  ReadCANBuf(buf+length, bufLen-length);

							//if(uart_open_close_flag)
								//nread = -1;

						   	printf(" before parse --- length=%d  nread=%d...\n", length, nread);

							if(nread > 0)
							{
									//CanPort_parseByte((unsigned char*)buf);
									//continue;

									length += nread;  //��������ݣ�������ݳ���
									haveData = 1;

									while(haveData)  //һ�����ܶ����
									{
											//�ж��Ƿ���can���ͷ��
											printf("have data length=%d ...\n",  length);
											for(i=0; i<length; i++)
											{
												printf("%02x ", buf[i]);
											}
											printf("\n");
											index=0;
											ret = CanPort_recvFlg(buf, length, &index);
											length -= index;//����֮��ʵ����ݳ��ȸ���

											if(ret != 0)
											{
												haveData=0;
												continue;
											}

											//�ж�Ҫ������ݵĳ���
											dataLength =0;

											switch(buf[2])
											{
													case 0xA0 :
																dataLength=9;  break;
													case 0xA1 :
																dataLength=7;  break;
													case 0xA2 :
																dataLength=4;  break;
													case 0xA3 :
																dataLength=6;  break;
													case 0xA4 :
																dataLength=4; break;
													default :
																memcpy(buf, buf+2, length-2);
																memset(buf+length-2, 0, sizeof(buf)-(length-2)  );
																dataLength =0;
																haveData=0;
																break;
											}


											//����֮��,������ݳ���
											if(dataLength > 0)
											{
													//��ӡҪ���������
													for(i=0; i<dataLength; i++)
													{
														printf("%02x ", buf[i]);
													}

													printf("length=%d, dataLength=%d\n", length, dataLength);

													if(length<dataLength)
													{
														printf(" length<dataLength ...\n");
														memset(buf+length, 0, sizeof(buf)-length);
														haveData=0;
													}
													else
													{

														//�������
														CanPort_parseByte((unsigned char*)buf);

														memcpy(buf, buf+dataLength, length-dataLength);
														memset(buf+length-dataLength, 0, sizeof(buf)-(length-dataLength)  );
														length -= dataLength;
													}
											}

											//usleep(10*1000);
									}
							}
				}

		}

		CloseCANDevice();
		return NULL;
}







// bak2��spi3��com3 ����
int BAK2Port_parseByte(unsigned char * buf)
{



		return 0;
}











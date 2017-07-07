
#include <data_port.h>
#include <osa_rng.h>
#include "app_global.h"
#define DATA_PORT_TSK_PRI                             (2)
#define DATA_PORT_TSK_STACK_SIZE            (0) 

#define DATA_PORT_MTU					(4096)

typedef struct _data_port_obj{
	volatile int flag;
	port_handle *port;
	OSA_ThrHndl thrHandle;
	Bool bDestroy;
	fxnDataPortTransNotify notifyFxn;
	void *notifyContext;
	char filename[128];
	Int32 filesize;
	Int32 transLen;
	Uint8 *buffer;
	Int32 buffersize;
	Bool bCreateBuffer;
	UInt32 timeout;
}data_port_obj;

static data_port_obj dataport ;
#if 0
= {
	.flag = 0,
	.bDestroy = FALSE,
};
#endif
static  void * send_data_fxn(void *prm)
{
	data_port_obj *dataport = ( data_port_obj *) prm;
	int retry = 0;
	Int32 retVal = 0;
	fd_set rd_set;	
	struct timeval timeout;
	Int32 rrSize;
	Uint8 *buffer;
	Int32 nalLen;
	Int32 first =1;
		
	buffer = (Uint8*)malloc(DATA_PORT_MTU+8);
	OSA_assert(buffer != NULL);

	OSA_printf("%s entry!!!\r\n",__func__);

	do{
		FD_ZERO(&rd_set);
		FD_SET(dataport->port->fd, &rd_set);		
		timeout.tv_sec   = dataport->timeout/1000;
		timeout.tv_usec = (dataport->timeout%1000)*1000;
		retVal = select(dataport->port->fd+1, &rd_set, NULL, NULL, &timeout);

		if(retVal == -1)
		{
			dataport->flag = -1;
			OSA_printf("%s:: select error !", __func__);
			break;
		}	
		
		if(retVal == 0)
		{
			retry ++;
			if(retry == 3){
				dataport->flag = -2;
				OSA_printf("%s:: timer out !", __func__);
				break;
			}
		}
		else
		{
			OSA_printf("%s:: fd = %d select rnt = %d \n", __func__, dataport->port->fd, retVal);

			if(FD_ISSET(dataport->port->fd, &rd_set))
			{
				retry = 0;
				dataport->port->recv(dataport->port, (Uint8*)&rrSize, sizeof(rrSize));
				OSA_printf("%s:: fd = %d select rrSize = %d \n", __func__, dataport->port->fd, rrSize);
				if(first==1 && rrSize!=0)
				{
					continue;
				}

				first = 0;
				
				if(rrSize < 0 || rrSize > dataport->buffersize)
					rrSize = dataport->buffersize;
				dataport->transLen = rrSize;
				if(dataport->transLen == dataport->buffersize){
					dataport->flag = 0;
					OSA_printf("%s:: finish ! transfer data lenght = %d", __func__, dataport->transLen);
					break;
				}
			}
			else
			{
				OSA_printf("%s:: WARNNING fd = %d select rnt = %d !!!\n", __func__, dataport->port->fd, retVal);
				continue;
			}
		}

		nalLen = (dataport->buffersize - dataport->transLen > DATA_PORT_MTU) ? DATA_PORT_MTU : (dataport->buffersize - dataport->transLen);
		memcpy(buffer, &dataport->buffersize, 4);
		memcpy(&buffer[4], &dataport->transLen, 4);
		memcpy(&buffer[8], &dataport->buffer[dataport->transLen], nalLen);
		retVal = dataport->port->send(dataport->port, buffer, nalLen+8);

		if(retVal < 0){
			dataport->flag = -3;
			OSA_printf("%s:: send error ! lenght = %d ", __func__, nalLen+8);
			break;
		}else{
			OSA_printf("%s:: send data lenght = %d ... ", __func__, nalLen+8);
		}

		if(dataport->notifyFxn != NULL){
			dataport->notifyFxn (dataport->notifyContext, dataport->buffersize, dataport->transLen, 1);
		}
	}while(dataport->transLen != dataport->buffersize);

	//OSA_printf("%s:: exit ! flag = %d retVal = %d ...", __func__, dataport->flag, retVal);

	//OSA_thrDelete(&dataport->thrHandle);
	//OSA_thrExit(&retVal);

	if(dataport->notifyFxn != NULL){
		dataport->notifyFxn (dataport->notifyContext, dataport->buffersize, dataport->transLen, dataport->flag);
	}

	//if(dataport->bCreateBuffer)
	free(dataport->buffer);
	dataport->buffer = NULL;

	free(buffer);
	OSA_printf("%s:: exit ! flag = %d retVal = %d\n", __func__, dataport->flag, retVal);

	//memset(dataport, 0, sizeof(*dataport));

	return NULL;
}


int data_port_send_data(port_handle *port, Uint8 *data, Int32 size, Uint32 timeout, fxnDataPortTransNotify fxnNotify)
{
	if(dataport.flag > 0)
		return OSA_EFAIL;

	if(dataport.bDestroy)
		OSA_thrDelete(&dataport.thrHandle);

	OSA_assert(port != NULL && data != NULL && size > 0);

	memset(&dataport, 0, sizeof(dataport));	
	dataport.flag = 2;

	dataport.port = port;
	dataport.buffer = data;
	dataport.buffersize = size;
	dataport.timeout = timeout;
	if(dataport.timeout<100)
		dataport.timeout = 100;
	dataport.notifyFxn = fxnNotify;
	dataport.notifyContext = data;

	dataport.bDestroy = TRUE;
	
	return OSA_thrCreate(&dataport.thrHandle,
						send_data_fxn,
						DATA_PORT_TSK_PRI,
						DATA_PORT_TSK_STACK_SIZE,
						&dataport);
}

static  void * recv_data_fxn(void *prm)
{
	data_port_obj *dataport = ( data_port_obj *) prm;
	int retry = 0;
	Int32 retVal = 0;
	fd_set rd_set;	
	struct timeval timeout;
	Int32 rrSize = dataport->buffersize;
	Uint8 *buffer;
	Int32 nalLen;
	int flag = 1;

	buffer =  (Uint8*)malloc(DATA_PORT_MTU+8);
	OSA_assert(buffer != NULL);

	OSA_printf("%s entry!!!\r\n",__func__);

	do{
		FD_ZERO(&rd_set);
		FD_SET(dataport->port->fd, &rd_set);		
		timeout.tv_sec   = dataport->timeout/1000;
		timeout.tv_usec = (dataport->timeout%1000)*1000;
		retVal = select(dataport->port->fd+1, &rd_set, NULL, NULL, &timeout);

		if(retVal == -1){
			flag = -1;
			break;
		}	
		
		if(retVal == 0){
			retry ++;
			if(retry == 3){
				flag = -2;
				break;
			}
		}else{
			OSA_printf("%s:: fd = %d select rnt = %d \n", __func__, dataport->port->fd, retVal);

			if(FD_ISSET(dataport->port->fd, &rd_set))
			{
				retry = 0;
				retVal = dataport->port->recv(dataport->port, buffer, DATA_PORT_MTU+8);

				if(retVal <= 0){
					flag = -4;
					break;					
				}

				if(retVal < 8){
					continue;
				}

				rrSize = *(Int32*)(&buffer[0]);
				nalLen = *(Int32*)(&buffer[4]);
				OSA_assert(rrSize > 0 && nalLen >= 0 && nalLen <= rrSize);
				OSA_printf("%s:: rrSize = %d nalLen = %d \n", __func__, rrSize, nalLen);

				if(dataport->buffer == NULL){
					dataport->buffer = (Uint8*)malloc(rrSize);
					if(dataport->buffer == NULL)
					{
						flag = -6;
						break;
					}
					dataport->buffersize = rrSize;
					dataport->bCreateBuffer = TRUE;
				}
				
				if(rrSize > dataport->buffersize){
					flag = -5;
					break;
				}
				memcpy(&dataport->buffer[nalLen], &buffer[8], retVal - 8);
				dataport->transLen = nalLen + retVal -8;
			}
			else
			{
				OSA_printf("%s:: WARNNING fd = %d select rnt = %d !!!\n", __func__, dataport->port->fd, retVal);
				continue;
			}
		}

		memcpy(buffer, &dataport->transLen, 4);
		retVal = dataport->port->send(dataport->port, buffer, 4);

		if(retVal < 0){
			flag = -3;
			break;
		}

		if(rrSize != 0 && dataport->transLen == rrSize){
			flag = 0;
			break;
		}

		if(dataport->notifyFxn != NULL){
			dataport->notifyFxn (dataport->notifyContext, rrSize, dataport->transLen, dataport->flag);
		}
	}while(dataport->transLen != dataport->buffersize);

	//OSA_thrDelete(&dataport->thrHandle);



	if(flag>0)
		flag = 0;

	dataport->flag = flag;

	if(dataport->notifyFxn != NULL){
		dataport->notifyFxn (dataport->notifyContext, rrSize, dataport->transLen, dataport->flag);
	}	

	if(dataport->bCreateBuffer)
		free(dataport->buffer);
	dataport->buffer = NULL;
	
	free(buffer);
	//memset(dataport, 0, sizeof(*dataport));
	OSA_printf("%s:: exit ! flag = %d retVal = %d\n", __func__, dataport->flag, retVal);

	return NULL;
}

int data_port_recv_data(port_handle *port, Uint8 *data, Int32 size, Uint32 timeout, fxnDataPortTransNotify fxnNotify,Bool isInit)
{
	if(dataport.flag > 0)
		return OSA_EFAIL;

	if(dataport.bDestroy)
		OSA_thrDelete(&dataport.thrHandle);
	dataport.bDestroy = FALSE;

	OSA_assert(port != NULL);

	OSA_printf("%s entry!!!\r\n",__func__);

	if(isInit)
	{
		memset(&dataport, 0, sizeof(dataport));
	}
	
	dataport.flag = 1;

	dataport.port = port;
	dataport.buffer = data;
	dataport.buffersize = size;
	dataport.timeout = timeout;
	if(dataport.timeout<100)
		dataport.timeout = 100;
	
	dataport.notifyFxn = fxnNotify;
	dataport.notifyContext = data;

	dataport.bDestroy = TRUE;

	return OSA_thrCreate(&dataport.thrHandle,
						recv_data_fxn,
						DATA_PORT_TSK_PRI,
						DATA_PORT_TSK_STACK_SIZE,
						&dataport);
}

int data_port_send_file(port_handle *port, char *file, Uint32 timeout, fxnDataPortTransNotify fxnNotify)
{
	FILE *stream;
	Uint8 *buffer = NULL;
	Int32 fileSize;
	Int32 posbak;
	int retVal = OSA_SOK;

	OSA_assert(port != NULL);
	OSA_assert(file != NULL);

	if(dataport.flag > 0)
		return OSA_EFAIL;

	stream = fopen(file, "rb");

	if(stream == NULL)
		return OSA_EFAIL;

	posbak = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	fileSize = ftell(stream);
	fseek(stream, posbak, SEEK_SET);

	if(fileSize > 0){
		buffer = (Uint8*)malloc(fileSize);
		if(buffer != NULL){
			fread(buffer, sizeof(Uint8), fileSize, stream);
			retVal = data_port_send_data(port, buffer, fileSize, timeout, fxnNotify);
		}else{
			retVal = OSA_EFAIL;
		}
	}


	fclose(stream);

	return retVal;
}

static fxnDataPortTransNotify usrFxn = NULL; 
static Int32 recvfile_notify(void *p, Int32 size, Int32 transLen, Int32 flag)
{
	OSA_printf("%s:size %d transLen %d %s\r\n",__func__,size,transLen,dataport.filename);
	if(size > 0 && size == transLen)
	{
		FILE *stream;
		Int32 rntLen;

		stream = fopen(dataport.filename, "w+b");

		if(stream == NULL){
			OSA_printf("%s:open file %s failue\r\n",__func__,dataport.filename);
			return OSA_EFAIL;
		}
		rntLen = fwrite(dataport.buffer, sizeof(Uint8), dataport.buffersize, stream);

		OSA_assert(rntLen == dataport.buffersize);

		fclose(stream);
	}

	if(usrFxn != NULL)
		return  usrFxn(p, size, transLen, flag);

	return OSA_SOK;
}

int data_port_recv_file(port_handle *port, char *file, Uint32 timeout, fxnDataPortTransNotify fxnNotify)
{
	int retVal = OSA_SOK;

	OSA_assert(port != NULL);
	OSA_assert(file != NULL);

	OSA_printf("%s entry!!!\r\n",__func__);
	OSA_printf("%s file %s!!!\r\n",__func__,file);

	if(dataport.flag > 0)
		return OSA_EFAIL;

	memset(&dataport, 0, sizeof(dataport));

	strcpy(dataport.filename, file);

	usrFxn = fxnNotify;

	retVal = data_port_recv_data(port, NULL, 0, timeout, recvfile_notify,FALSE);

	OSA_printf("%s exit!!!\r\n",__func__);

	return retVal;
}


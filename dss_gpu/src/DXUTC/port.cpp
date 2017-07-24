
#include <port.h>
#include <ipc_port.h>
#include <uart_port.h>
#include <udp_port.h>
#include <uart_i2c_port.h>
#include <uart_gpmc_port.h>

Int32 port_create(IN Int32 portType, OUT port_handle** ppHdl)
{
	if(portType == PORT_UART)
	{
		return uart_port_create(ppHdl);
	}

	if(portType == PORT_NET_UDP)
	{
		return udp_port_create(ppHdl);
	}

	if(portType == PORT_I2C_UART)
	{
		//return uart_i2c_port_create(ppHdl);
	}
	if(portType == PORT_GPMC_FPGA)
	{
		//return uart_gpmc_port_create(ppHdl);
	}
	if(portType == PORT_IPC)
	{
		return ipc_port_create(ppHdl);
	}

	return  OSA_EFAIL;
}

Int32 port_destory(IN port_handle* pHdl)
{
	if(pHdl != NULL && pHdl->portType == PORT_UART){
		return uart_port_destroy(pHdl);
	}

	if(pHdl != NULL && pHdl->portType == PORT_NET_UDP){
		return udp_port_destroy(pHdl);
	}

	if(pHdl != NULL && pHdl->portType == PORT_I2C_UART){
		//return uart_i2c_port_destroy(pHdl);
	}
	
	if(pHdl != NULL && pHdl->portType == PORT_GPMC_FPGA){
		//return uart_gpmc_port_destroy(pHdl);
	}
	
	if(pHdl != NULL && pHdl->portType == PORT_IPC){
		return ipc_port_destroy(pHdl);
	}
    
	return  OSA_EFAIL;
}

static char *PortName[] = {"UART" , "NET UDP", "NET SERVER", "NET CLIENT", "I2C UART"};
void port_status_print(IN port_handle* pHdl)
{
	uart_open_params* uartOpenParams = (uart_open_params*)pHdl->openParams;
	udp_open_params* udpOpenParams = (udp_open_params*)pHdl->openParams;
	uart_i2c_open_params * i2cOPenParam = (uart_i2c_open_params*)pHdl->openParams;
	
	OSA_printf("\t------------- port status--------------\n");
	OSA_printf("\tPORT TYPE \t|      DEV \t| FD \t| recv cnt \t| snd cnt \t| recv bytes \t| snd bytes \n");
	OSA_printf("\t %9s ", PortName[pHdl->portType]);
	
	if(pHdl->portType == PORT_UART)
		OSA_printf("\t %9s ", uartOpenParams->device);
	else if(pHdl->portType == PORT_NET_UDP)
		OSA_printf("\t %d ", udpOpenParams->port);
	else if(pHdl->portType == PORT_I2C_UART)
		OSA_printf("\t %d ", i2cOPenParam->dev_i2c_addr);
	else
		OSA_printf("\t    null ");

	OSA_printf("\t %d ", pHdl->fd);
	OSA_printf("\t  %9d", pHdl->cntRecv);
	OSA_printf("\t  %9d", pHdl->cntSnd);
	OSA_printf("\t  %9d", pHdl->cntRecvBytes);
	OSA_printf("\t  %9d", pHdl->cntSndbytes);
	
	OSA_printf("\n");
}

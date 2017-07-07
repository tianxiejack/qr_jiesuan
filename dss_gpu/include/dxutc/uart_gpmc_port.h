/*******************************************************************************
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* WorkContext:
* FileName:    uart_gpmc_port.h
* Author:      xavier       Version :          Date:2015年8月27日
* Description: //  gpmc FPGA 通信模块，连接Gyro /dev/gpmc_fpga
* Version:     //  V1.00
* History:     //  历史修改记录
* <author>      <time>          <version >      <description>
* xavier        2015年8月27日       V1.00          build this module
********************************************************************************/

#ifndef _UART_GPMC_PORT_H_
#define _UART_GPMC_PORT_H_

#include <port.h>

typedef struct _io_cmd_arg_
{
    UInt32 io_addr;
    UInt32 io_buffer;
}io_arg;

#define FPGA_IOC_MAGIC                  'z'

/* io uart ctrl cmd id */
#define FPGA_ICO_UART0_BASE             (11)
#define FPGA_IOCS_UART0_EN_IRQ          _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+1,  char)
#define FPGA_IOCS_UART0_BAUD            _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+2,  char)
#define FPGA_IOCS_UART0_RE_THRD         _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+3,  char)
#define FPGA_IOCS_UART0_WR_THRD         _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+4,  char)
#define FPGA_IOCS_UART0_CLR_TM_IRQ      _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+5,  char)
#define FPGA_IOCS_UART0_CLR_WR_IRQ      _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+6,  char)
#define FPGA_IOCS_UART0_CLR_RD_IRQ      _IOW(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+7,  char)

#define FPGA_IOCG_UART0_WR_FIFO_STAT    _IOR(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+8,  char)
#define FPGA_IOCG_UART0_RD_FIFO_STAT    _IOR(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+9,  char)
#define FPGA_IOCG_UART0_IRQ_REQ_STAT    _IOR(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+10, char)

#define FPGA_IOCS_UART0_FIFO_RESET      _IOR(FPGA_IOC_MAGIC,FPGA_ICO_UART0_BASE+11, char)

#define FPGA_ICO_GYRO_BASE             (32)

#define FPGA_IOCS_GYRO_VIDEO_SYNC_CHAN  _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+1, char)
#define FPGA_IOCS_GYRO_TV_INTERVAL_LINE _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+2, char)
#define FPGA_IOCS_GYRO_FL_INTERVAL_LINE _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+3, char)
#define FPGA_IOCS_GYRO_MODE             _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+4, char)
#define FPGA_IOCS_GYRO_RESET            _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+5, char)
#define FPGA_IOCS_GYRO_ENABLE_INTERVAL 	_IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+6, char)
#define FPGA_IOCS_GYRO_ENABLE_EXT_SYNC  _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+7, char)
#define FPGA_IOCS_GYRO_BUAD             _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GYRO_BASE+9, char)


#define FPGA_ICO_GPMC_UART_BASE         (64)

#define FPGA_IOCS_TV_PWM_HALO           _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GPMC_UART_BASE+1, char)
#define FPGA_IOCS_TV_PWM_ZOOM           _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GPMC_UART_BASE+2, char)
#define FPGA_IOCS_TV_PWM_FCOUS          _IOW(FPGA_IOC_MAGIC,FPGA_ICO_GPMC_UART_BASE+3, char)

Int32 uart_gpmc_port_create( port_handle **ppHdl );
Int32 uart_gpmc_port_destroy( port_handle *pHdl );

#endif
/************************************** The End Of File **************************************/

/*=======================================================
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:		uart_i2c_port.h
* Author:			xavier
* Date:			2014-03-17
* Description:		this file define MAX3109 register addr
* Version:		version 1.00
* Function List:
* History:
*     <author>  <time>   <version >   <desc>
*     xavier    14/03/17     1.0     build this moudle
   =======================================================*/

#ifndef _UART_I2C_PORT_H_
#define _UART_I2C_PORT_H_

#include <port.h>
/* MAX310X register definitions */
#define MAX310X_RHR_REG				( 0x00 )        /* RX FIFO */
#define MAX310X_THR_REG				( 0x00 )        /* TX FIFO */
#define MAX310X_IRQEN_REG			( 0x01 )        /* IRQ enable */
#define MAX310X_IRQSTS_REG			( 0x02 )        /* IRQ status */
#define MAX310X_LSR_IRQEN_REG		( 0x03 )        /* LSR IRQ enable */
#define MAX310X_LSR_IRQSTS_REG		( 0x04 )        /* LSR IRQ status */
#define MAX310X_REG_05				( 0x05 )
#define MAX310X_SPCHR_IRQEN_REG		MAX310X_REG_05  /* Special char IRQ en */
#define MAX310X_SPCHR_IRQSTS_REG	( 0x06 )        /* Special char IRQ status */
#define MAX310X_STS_IRQEN_REG		( 0x07 )        /* Status IRQ enable */
#define MAX310X_STS_IRQSTS_REG		( 0x08 )        /* Status IRQ status */
#define MAX310X_MODE1_REG			( 0x09 )        /* MODE1 */
#define MAX310X_MODE2_REG			( 0x0a )        /* MODE2 */
#define MAX310X_LCR_REG				( 0x0b )        /* LCR */
#define MAX310X_RXTO_REG			( 0x0c )        /* RX timeout */
#define MAX310X_HDPIXDELAY_REG		( 0x0d )        /* Auto transceiver delays */
#define MAX310X_IRDA_REG			( 0x0e )        /* IRDA settings */
#define MAX310X_FLOWLVL_REG			( 0x0f )        /* Flow control levels */
#define MAX310X_FIFOTRIGLVL_REG		( 0x10 )        /* FIFO IRQ trigger levels */
#define MAX310X_TXFIFOLVL_REG		( 0x11 )        /* TX FIFO level */
#define MAX310X_RXFIFOLVL_REG		( 0x12 )        /* RX FIFO level */
#define MAX310X_FLOWCTRL_REG		( 0x13 )        /* Flow control */
#define MAX310X_XON1_REG			( 0x14 )        /* XON1 character */
#define MAX310X_XON2_REG			( 0x15 )        /* XON2 character */
#define MAX310X_XOFF1_REG			( 0x16 )        /* XOFF1 character */
#define MAX310X_XOFF2_REG			( 0x17 )        /* XOFF2 character */
#define MAX310X_GPIOCFG_REG			( 0x18 )        /* GPIO config */
#define MAX310X_GPIODATA_REG		( 0x19 )        /* GPIO data */
#define MAX310X_PLLCFG_REG			( 0x1a )        /* PLL config */
#define MAX310X_BRGCFG_REG			( 0x1b )        /* Baud rate generator conf */
#define MAX310X_BRGDIVLSB_REG		( 0x1c )        /* Baud rate divisor LSB */
#define MAX310X_BRGDIVMSB_REG		( 0x1d )        /* Baud rate divisor MSB */
#define MAX310X_CLKSRC_REG			( 0x1e )        /* Clock source */
#define MAX310X_REG_1F				( 0x1f )

#define MAX310X_REVID_REG MAX310X_REG_1F            /* Revision ID */

#define MAX310X_GLOBALIRQ_REG	MAX310X_REG_1F      /* Global IRQ (RO) */
#define MAX310X_GLOBALCMD_REG	MAX310X_REG_1F      /* Global Command (WO) */

/* Extended registers */
#define MAX310X_REVID_EXTREG MAX310X_REG_05         /* Revision ID */

/* IRQ register bits */
#define MAX310X_IRQ_LSR_BIT		( 1 << 0 )          /* LSR interrupt */
#define MAX310X_IRQ_SPCHR_BIT	( 1 << 1 )          /* Special char interrupt */
#define MAX310X_IRQ_STS_BIT		( 1 << 2 )          /* Status interrupt */
#define MAX310X_IRQ_RXFIFO_BIT	( 1 << 3 )          /* RX FIFO interrupt */
#define MAX310X_IRQ_TXFIFO_BIT	( 1 << 4 )          /* TX FIFO interrupt */
#define MAX310X_IRQ_TXEMPTY_BIT ( 1 << 5 )          /* TX FIFO empty interrupt */
#define MAX310X_IRQ_RXEMPTY_BIT ( 1 << 6 )          /* RX FIFO empty interrupt */
#define MAX310X_IRQ_CTS_BIT		( 1 << 7 )          /* CTS interrupt */

/* LSR register bits */
#define MAX310X_LSR_RXTO_BIT	( 1 << 0 )          /* RX timeout */
#define MAX310X_LSR_RXOVR_BIT	( 1 << 1 )          /* RX overrun */
#define MAX310X_LSR_RXPAR_BIT	( 1 << 2 )          /* RX parity error */
#define MAX310X_LSR_FRERR_BIT	( 1 << 3 )          /* Frame error */
#define MAX310X_LSR_RXBRK_BIT	( 1 << 4 )          /* RX break */
#define MAX310X_LSR_RXNOISE_BIT ( 1 << 5 )          /* RX noise */
#define MAX310X_LSR_CTS_BIT		( 1 << 7 )          /* CTS pin state */

/* Special character register bits */
#define MAX310X_SPCHR_XON1_BIT		( 1 << 0 )      /* XON1 character */
#define MAX310X_SPCHR_XON2_BIT		( 1 << 1 )      /* XON2 character */
#define MAX310X_SPCHR_XOFF1_BIT		( 1 << 2 )      /* XOFF1 character */
#define MAX310X_SPCHR_XOFF2_BIT		( 1 << 3 )      /* XOFF2 character */
#define MAX310X_SPCHR_BREAK_BIT		( 1 << 4 )      /* RX break */
#define MAX310X_SPCHR_MULTIDROP_BIT ( 1 << 5 )      /* 9-bit multidrop addr char */

/* Status register bits */
#define MAX310X_STS_GPIO0_BIT		( 1 << 0 )      /* GPIO 0 interrupt */
#define MAX310X_STS_GPIO1_BIT		( 1 << 1 )      /* GPIO 1 interrupt */
#define MAX310X_STS_GPIO2_BIT		( 1 << 2 )      /* GPIO 2 interrupt */
#define MAX310X_STS_GPIO3_BIT		( 1 << 3 )      /* GPIO 3 interrupt */
#define MAX310X_STS_CLKREADY_BIT	( 1 << 5 )      /* Clock ready */
#define MAX310X_STS_SLEEP_BIT		( 1 << 6 )      /* Sleep interrupt */

/* MODE1 register bits */
#define MAX310X_MODE1_RXDIS_BIT			( 1 << 0 )  /* RX disable */
#define MAX310X_MODE1_TXDIS_BIT			( 1 << 1 )  /* TX disable */
#define MAX310X_MODE1_TXHIZ_BIT			( 1 << 2 )  /* TX pin three-state */
#define MAX310X_MODE1_RTSHIZ_BIT		( 1 << 3 )  /* RTS pin three-state */
#define MAX310X_MODE1_TRNSCVCTRL_BIT	( 1 << 4 )  /* Transceiver ctrl enable */
#define MAX310X_MODE1_FORCESLEEP_BIT	( 1 << 5 )  /* Force sleep mode */
#define MAX310X_MODE1_AUTOSLEEP_BIT		( 1 << 6 )  /* Auto sleep enable */
#define MAX310X_MODE1_IRQSEL_BIT		( 1 << 7 )  /* IRQ pin enable */

/* MODE2 register bits */
#define MAX310X_MODE2_RST_BIT		( 1 << 0 )      /* Chip reset */
#define MAX310X_MODE2_FIFORST_BIT	( 1 << 1 )      /* FIFO reset */
#define MAX310X_MODE2_RXTRIGINV_BIT ( 1 << 2 )      /* RX FIFO INT invert */
#define MAX310X_MODE2_RXEMPTINV_BIT ( 1 << 3 )      /* RX FIFO empty INT invert */
#define MAX310X_MODE2_SPCHR_BIT		( 1 << 4 )      /* Special chr detect enable */
#define MAX310X_MODE2_LOOPBACK_BIT	( 1 << 5 )      /* Internal loopback enable */
#define MAX310X_MODE2_MULTIDROP_BIT ( 1 << 6 )      /* 9-bit multidrop enable */
#define MAX310X_MODE2_ECHOSUPR_BIT	( 1 << 7 )      /* ECHO suppression enable */

/* LCR register bits */
#define MAX310X_LCR_LENGTH0_BIT ( 1 << 0 )          /* Word length bit 0 */
#define MAX310X_LCR_LENGTH1_BIT ( 1 << 1 )          /* Word length bit 1
	                                                 *
	                                                 * Word length bits table:
	                                                 * 00 -> 5 bit words
	                                                 * 01 -> 6 bit words
	                                                 * 10 -> 7 bit words
	                                                 * 11 -> 8 bit words
	                                                 */
#define MAX310X_LCR_STOPLEN_BIT ( 1 << 2 )          /* STOP length bit
	                                                 *
	                                                 * STOP length bit table:
	                                                 * 0 -> 1 stop bit
	                                                 * 1 -> 1-1.5 stop bits if
	                                                 *      word length is 5,
	                                                 *      2 stop bits otherwise
	                                                 */
/* LCR register bits */
#define MAX310X_LCR_LENGTH0_BIT		( 1 << 0 )      /* Word length bit 0 */
#define MAX310X_LCR_LENGTH1_BIT		( 1 << 1 )      /* Word length bit 1
	                                                 *
	                                                 * Word length bits table:
	                                                 * 00 -> 5 bit words
	                                                 * 01 -> 6 bit words
	                                                 * 10 -> 7 bit words
	                                                 * 11 -> 8 bit words
	                                                 */
#define MAX310X_LCR_STOPLEN_BIT		( 1 << 2 )      /* STOP length bit
	                                                 *
	                                                 * STOP length bit table:
	                                                 * 0 -> 1 stop bit
	                                                 * 1 -> 1-1.5 stop bits if
	                                                 *      word length is 5,
	                                                 *      2 stop bits otherwise
	                                                 */
#define MAX310X_LCR_PARITY_BIT		( 1 << 3 )      /* Parity bit enable */
#define MAX310X_LCR_EVENPARITY_BIT	( 1 << 4 )      /* Even parity bit enable */
#define MAX310X_LCR_FORCEPARITY_BIT ( 1 << 5 )      /* 9-bit multidrop parity */
#define MAX310X_LCR_TXBREAK_BIT		( 1 << 6 )      /* TX break enable */
#define MAX310X_LCR_RTS_BIT			( 1 << 7 )      /* RTS pin control */
#define MAX310X_LCR_WORD_LEN_5		( 0x00 )
#define MAX310X_LCR_WORD_LEN_6		( 0x01 )
#define MAX310X_LCR_WORD_LEN_7		( 0x02 )
#define MAX310X_LCR_WORD_LEN_8		( 0x03 )

/* Baud rate generator configuration register bits */
#define MAX310X_BRGCFG_2XMODE_BIT	( 1 << 4 )      /* Double baud rate */
#define MAX310X_BRGCFG_4XMODE_BIT	( 1 << 5 )      /* Quadruple baud rate */

Int32 uart_i2c_port_create( port_handle **ppHdl );
Int32 uart_i2c_port_destroy( port_handle *pHdl );


#endif
/************************************** The End Of File **************************************/

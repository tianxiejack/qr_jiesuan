/******************************************************************************
   Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
   File name:   app_main.c
   Author:      xavier
   Description:
   Version:         1.0.0
   Function List:
      1.    int main()
      2.    char multich_getChar()
      3.    int multich_printInfo(int usercase)
      4.    int multich_printBuffersInfo()
      5.    int multich_startStop(int usercase, Bool IsActiveCase)
      6.    int multich_run(int usercase)
      7.    Int32 multich_eventHandler(UInt32 eventId, Ptr pPrm, Ptr appData)
   History:
      <author>      <time>          <version >          <desc>
      xavier        2014-02-14  1.0.0
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#include "osa.h"
#include "dx.h"
#include "dx_config.h"
#include "app_user.h"
#include "dx_main.h"
//#include "Ti_vdis.h"


char gAPP_mainMenu[] =
    {
        "\r\n ========="
        "\r\n [APP MAIN] Main Menu"
        "\r\n ========="
        "\r\n 1: MCFW VCAP +VDIS "
        "\r\n 2: MCFW VCAP +VENC+ VDIS+RTSP  - SD/HD"
        "\r\n 3: MCFW IMAGE PROCESS BOARD VIDEO START"
        "\r\n"
        "\r\n e: Exit"
        "\r\n"
        "\r\n Enter Choice: "
    };
char gAPP_runMenu[] =
    {
        "\r\n ============="
        "\r\n [APP MAIN] Run-Time Menu"
        "\r\n ============="
        "\r\n"
        "\r\n i: Print detailed system information"
        "\r\n s: Core Status: Active/In-active"
        "\r\n c: Configure setup"
        "\r\n b: Print link buffers statistics"
        "\r\n e: Stop App"
        "\r\n"
        "\r\n Enter Choice: "
    };

/**
 * @Fun:        APP_usercase_run
 * @brief:      // fun discription
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 */
static Int32 usercase_run( int type )
{
    int status;
    Bool    done = FALSE;
    char    ch;
    //unsigned char brigthness;
   

    status = Dx_postMsg( DX_MSGID_START, &type, sizeof( type ) );
    if ( status < 0 )
    {
        printf( " WARNING: app usercase start failed !!!\n" );
        return status;
    }

   // app_createObj();

    while ( !done )
    {
       printf( gAPP_runMenu );

       ch = APP_getChar( );
     

        switch ( ch )
        {
        case 'C':
        case 'c':
            Dx_configMenu( gDXD_info.scripts );
            break;
        case 'I':
        case 'i':
            Dx_sendMsg( NULL, DX_MSGID_CTRL, (void*)( DX_CTL_PRINT_INFO ), sizeof( Int32 ), FALSE );
            break;
        case 'S':
        case 's':
            Dx_sendMsg( NULL, DX_MSGID_CTRL, (void*)( DX_CTL_PRINT_CORE ), sizeof( Int32 ), FALSE );
            break;
        case 'B':
        case 'b':
            Dx_sendMsg( NULL, DX_MSGID_CTRL, (void*)( DX_CTL_PRINT_BUFFER ), sizeof( Int32 ), FALSE );
            break;
        case 'E':
        case 'e':
            done = TRUE;
            break;
/*			
        case '4':
		printf("input the value");
		 brigthness = APP_getint( );
            cltrl_dis_user(brigthness,0);
            break;	
	 case '5':
		printf("input the value");
		 brigthness = APP_getint( );
            cltrl_dis_user(brigthness,1);
            break;
 	case '6':
		printf("input the value");
		 brigthness = APP_getint( );
            cltrl_dis_user(brigthness,2);
            break;
*/
			
           
           
        }
    }

    status = Dx_postMsg( DX_MSGID_STOP, NULL, 0 );

    return 0;
}

/**
 * @brief:      main:
 * @param:
 * @param:
 * @return:
 * @Others:
 */
static void* dxmain(void *pPrm)
{
    Bool ui_loop = TRUE;
    int onece=0;
    char ch = '3';
    OSA_printf("%s:  enter %d   \n", __func__, __LINE__);
    Dx_run();
    Dx_initparam();//init dx param
    OSA_semSignal((OSA_SemHndl *)pPrm);
    OSA_printf("***%s:%d\n", __func__, __LINE__);

    while (ui_loop)
    {
        printf(gAPP_mainMenu);

        switch (ch)
        {
        case '1':
            usercase_run(MCFW_VCAP_VDIS_CASE);
            break;
        case '2':
            usercase_run(MCFW_VCAP_VENC_CASE);
            break;
        case '3':
            usercase_run(MCFW_IMAGE_PROCESS_BOARD_CASE);
            break;
        }

        if (ch == 'e')
        {
            ui_loop = FALSE;
            break;
        }

        ch = APP_getChar();
    }
    Dx_stop();
    //return;
}


  OSA_MutexHndl muxLock;
  OSA_SemHndl tskGraphicSemmain;
  OSA_ThrHndl tskGraphicHndlmain;

int App_dxmain( )
{
	int status=0;
	initgdxd_info();
	
       status = OSA_semCreate(&tskGraphicSemmain, 1, 0);
	
       OSA_assert(status == OSA_SOK);

       status = OSA_thrCreate(
                 &tskGraphicHndlmain,
                 dxmain,
                 OSA_THR_PRI_DEFAULT,
                 0,
             (void *)&tskGraphicSemmain
             );
  OSA_assert(status == OSA_SOK);

  OSA_semWait(&tskGraphicSemmain,OSA_TIMEOUT_FOREVER);

}


/**************************************/

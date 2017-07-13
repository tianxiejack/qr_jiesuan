/*****************************************************************************
 * Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
 * FileName:        APP_project_xgs012.c
 * Author:          xavier
 * Date:            2016-05-20
 * Description:     // description
 * Version:         // version
 * Function List:   // funs and description
 *     1. -------
 * History:         // histroy modfiy record
 *     <author>  <time>   <version >   <desc>
 *     xavier    16/05/20     1.0     build this moudle
*****************************************************************************/
#include "app_global.h"

//#ifdef PROJECT_XGS026

#include <stdio.h>
#include "dx.h"
#include "dx_config.h"
#include "app_osdgrp.h"
#include"app_status.h"
//#include "app_osd_priv.h"
#include "app_proj_xgs026.h"

#define UNUSED 0

typedef struct Osd_Menu 
{
	int id;
	char Menu[5][6];
}Osd_Menu_t;

typedef struct Osd_SysInit
{
	int id;
      char SysInit[18];
}Osd_SysInit_t;

Osd_Menu_t gMenu_Info[33] =
{
		{0x00,{
		{'F','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'L','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'I','N','F','L',' ','\0'},
			  }
		},
		{0xC0,{
		{'P','O','W','R',' ','\0'},{' ','N','U','C',' ','\0'},{'A','T',92,'M','A','\0'},{'M','O','R','E',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0xC3,{
		{' ','G','N','+',' ','\0'},{' ','G','N','-',' ','\0',},{' ','B','R','+' ,' ','\0',},{' ','B','R','-',' ','\0',},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x4A,{
		{'P','O','W','R',' ','\0'},{'E','N','A','B',' ','\0'},{'S','I','M','U',' ','\0'},{' ','L',92,'H',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x60,{
		{'F','S','E','T',' ','\0'},{'A','S','E','T',' ','\0'},{'L','S','E','T',' ','\0'},{'M','O','R','E',' ','\0'},{'I','N','F','L',' ','\0'},
			}
		},
		{0x6C,{
		{'I','M','E','N','1','\0'},{'I','M','E','N','2','\0'},{'C','O','L','R',' ','\0'},{'P','I','N','P',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x20,{
		{' ','V','E','R',' ','\0'},{'V','C','P','T',' ','\0'},{'J','S','-','S',' ','\0'},{'D','R','F','T',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x21,{
		{' ','V','E','R',' ','\0'},{'V','C','P','T',' ','\0'},{'J','S','-','N',' ','\0'},{'D','R','F','T',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x6A,{
		{' ',' ',35,' ',' ','\0',},{' ',' ',33,' ',' ','\0'},{' ',' ',38,' ',' ','\0'},{' ',' ',39,' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},//que shao jian tou
		{0x10,{
		{' ',' ',' ',' ',' ','\0'},{' ',' ',' ',' ',' ','\0'},{' ',' ',' ',' ',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'P','A','U','S','E','\0'},
			}
		},
		{0x70,{
		{'F','S','E','T',' ','\0'},{'S','S','E','T',' ','\0'},{'L','S','E','T',' ','\0'},{'M','O','R','E',' ','\0'},{'I','N','F','L',' ','\0'},
			}
		},
		{0x74,{
		{'-','S','P','D',' ','\0'},{'+','S','P','D',' ','\0'},{'M','R','G' ,'N',' ','\0'},{'S','A','V','E',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x75,{
		{'L','E','F','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'R','G','H','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			  }
		},
		{0x78,{
		{'F','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'L','S','E','T',' ','\0'},{'S','Y','M','B',' ','\0'},{'I','N','F','L',' ','\0'},
			  }
		},
		{0x88,{
		{'F','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'L','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'I','N','F','L',' ','\0'},
			  }
		},
		{0x38,{
		{'F','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'L','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'I','N','F','L',' ','\0'},
			  }
		},
		{0x08,{
		{'F','S','E','T',' ','\0'},{'M','A','I','N',' ','\0'},{'L','S','E','T',' ','\0'},{'P','A','R','M',' ','\0'},{'C','A','L','I','B','\0'},
			}
		},
		{0x0A,{
		{'C','F','G','1',' ','\0'},{'C','F','G','2',' ','\0'},{' ',' ',' ',' ',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			  }
		},
		{0x09,{
		{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},
			  }
		},
		{0x13,{
		{'M','O','R','E',' ','\0'},{' ',' ',' ',' ',' ','\0'},{' ','A','D','J',' ','\0'},{'M','B','I','T',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x0C,{
		{' ','E','N','G','+','\0'},{' ','E','N','G','-','\0'},{' ',' ',' ',' ',' ','\0'},{'J','O','Y','S',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x02,{
		{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},
			  }
		},
		{0x1A,{
		{'A','X','I','S',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'S','A','V','E',' ','\0'},{'R','E','A','D',' ','\0'},{'Q','U','I','T',' ','\0'},
			  }
		},
		{0x0E,{
		{' ',' ',35,' ',' ','\0',},{' ',' ',33,' ',' ','\0'},{' ',' ',38,' ',' ','\0'},{' ',' ',39,' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},//que shao jian tou
		{0x18,{
		{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},
			  }
		},
		{0x19,{
		{' ','T','V','S',' ','\0',},{'F','L','I','R',' ','\0'},{'S','A','V','E',' ','\0'},{'A','U','T','O',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},
		{0x0F,{
		{' ',' ',35,' ',' ','\0',},{' ',' ',33,' ',' ','\0'},{' ',' ',38,' ',' ','\0'},{' ',' ',39,' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},//que shao jian tou
		{0x11,{
		{'F','S','E','T',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'L','S','E','T',' ','\0'},{'P','A','R','M',' ','\0'},{'C','A','L','I','B','\0'},
			}
		},
		{0x03,{
		{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},
			  }
		},
		{0x04,{
		{'B','K','W','D',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'S','A','V','E',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			  }
		},
		{0x05,{
		{' ',' ',35,' ',' ','\0',},{' ',' ',33,' ',' ','\0'},{' ',' ',38,' ',' ','\0'},{' ',' ',39,' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			}
		},//que shao jian tou
		{0x06,{
		{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},{' ',' ','*',' ',' ','\0',},
			  }
		},
		{0x07,{
		{'A','U','T','O',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'S','A','V','E',' ','\0'},{' ',' ',' ',' ',' ','\0'},{'Q','U','I','T',' ','\0'},
			  }
		},		
};

#if 0
char gWorkStr[9][10] =
{     
	{'I','N','I','T',' ',' ',' ',' ',' ','\0',},
	{'H','I','D','E',' ',' ',' ',' ',' ','\0',},
	{'C','A','G','E',' ',' ',' ',' ',' ','\0',},
	{'M','A','N','U','A','L',' ',' ',' ','\0'},
	{'S','C','A','N',' ',' ',' ',' ',' ','\0',},
	{'T','R','A','C','K',' ',' ',' ',' ','\0',},
	{'G','P','C',' ','T','R','A','C','K','\0'},
	{'S','L','A','V','E',' ',' ',' ',' ','\0',},
	{'S','E','R','V','I','C','E',' ',' ','\0'},
};
#else

#endif

Osd_SysInit_t gSysInit[16] = 
{
	{0x11,
		{'T','V','S','\0'}
	},
	{0x12,
		{'I','R','\0',}
	},
	{0x13,
		{'L','D','R','\0'}
	},
	{0x14,
		{'T','L','\0'}
	},
	{0x15,
		{'D','R','V','\0'}
	},
	{0x16,
		{'S','E','R','V','O','I','O','\0'}
	},
	{0x17,
		{'S','E','R','V','O','\0'}
	},
	{0x21,
		{'M','A','I','N','S','B','C','\0'}
	},
	{0x22,
		{'C','O','M','\0',}
	},
	{0x23,
		{'A','F','D','X','\0',}
	},
	{0x24,
		{'A','V','T','\0',}
	},
	{0x25,
		{'P','S','B','\0',}
	},
	{0x31,
		{'O','C','U','\0',}
	},
	{0x41,
		{'.','.','.','.','.','.','\0',}
	},
	{0x42,
		{'P','R','E','S','S',' ','K','5',' ','T','O',' ','D','E','B','U','G','\0'}
	},
	{0x43,
		{'W','A','I','T','I','N','G',' ','F','O','R',' ','D','E','B','U','G','\0'}
	},
};

 char gSysInit_1[4][9] =
{
	{'B','I','T',' ','O','K',' ',' ','\0'},
	{'B','I','T',' ','F','A','I','L','\0'},
	{'C','O','M',' ','O','K',' ',' ','\0'},
	{'C','O','M',' ','F','A','I','L','\0'},
};

char gFovChoose[6] = 
{'Z','S','N','M','W',38};//que shao jian tou

char gTvStat[4][4] = 
{
	{'T','V',' ','\0'},
	{' ',' ',' ','\0'},
	{'T','V',' ','\0'},
	{'T','V','x','\0'},
};

char gVpsstat[2][5] = 
{
	{'+','V','C','P','\0'},
	{' ',' ',' ',' ','\0',}, 
};

osd_id_t g_OsdId[WINID_MAX] =
{
	{   60, eSen_TV,  0, grpx_ObjId_Cross    },
	{   61, eSen_FR,  0, grpx_ObjId_Cross    },
	{   62, eSen_TV,  1, grpx_ObjId_Rect_gap },
	{   63, eSen_FR,  1, grpx_ObjId_Rect_gap },

	{   64, eSen_TV,  2, grpx_ObjId_Rect     },
	{   65, eSen_TV,  3, grpx_ObjId_Cross    },
	{   66, eSen_TV,  4, grpx_ObjId_Cross    },
	{   67, eSen_TV,  5, grpx_ObjId_Cross    },
	{   68, eSen_TV,  6, grpx_ObjId_Cross    },
	{   69, eSen_TV,  7, grpx_ObjId_Cross    },
	{   70, eSen_TV,  8, grpx_ObjId_Cross    },
	{   71, eSen_TV,  9, grpx_ObjId_Cross    },
	{   72, eSen_TV, 10, grpx_ObjId_Cross    },
	{   73, eSen_TV, 11, grpx_ObjId_Cross    },

	{   74, eSen_FR,  2, grpx_ObjId_Rect     },
	{   75, eSen_FR,  3, grpx_ObjId_Cross    },
	{   76, eSen_FR,  4, grpx_ObjId_Cross    },
	{   77, eSen_FR,  5, grpx_ObjId_Cross    },
	{   78, eSen_FR,  6, grpx_ObjId_Cross    },
	{   79, eSen_FR,  7, grpx_ObjId_Cross    },
	{   80, eSen_FR,  8, grpx_ObjId_Cross    },
	{   81, eSen_FR,  9, grpx_ObjId_Cross    },
	{   82, eSen_FR, 10, grpx_ObjId_Cross    },
	{   83, eSen_FR, 11, grpx_ObjId_Cross    },

	{   84, eSen_TV,  12, grpx_ObjId_Cross},
	{   85, eSen_FR,  12, grpx_ObjId_Cross},
	{   86, eSen_TV,  13, grpx_ObjId_Cross},
	{   87, eSen_FR,  13, grpx_ObjId_Cross},
	
        {   88, eSen_TV,  14, grpx_ObjId_Cross},
        {   89, eSen_FR,  14, grpx_ObjId_Cross},
        {   90, eSen_TV,  15, grpx_ObjId_Cross},
        {   91, eSen_FR,  15, grpx_ObjId_Cross},

	{   94, eSen_TV, 16, grpx_ObjId_Text    },
	{   95, eSen_FR, 16, grpx_ObjId_Text    },
	{   96, eSen_TV, 17, grpx_ObjId_Text    },
	{   97, eSen_FR, 17, grpx_ObjId_Text    },
	{   98, eSen_TV, 18, grpx_ObjId_Text    },
	{   99, eSen_FR, 18, grpx_ObjId_Text    },
	{   100, eSen_TV, 19, grpx_ObjId_Text    },
	{   101, eSen_FR, 19, grpx_ObjId_Text    },
	{   102, eSen_TV, 20, grpx_ObjId_Text    },
	{   103, eSen_FR, 20, grpx_ObjId_Text    },

	{   104, eSen_TV, 21, grpx_ObjId_Text     },
	{   105, eSen_FR, 21, grpx_ObjId_Text     },
	{   106, eSen_TV, 22, grpx_ObjId_Text     },
	{   107, eSen_FR, 22, grpx_ObjId_Text     },
	{   108, eSen_TV, 23, grpx_ObjId_Text     },
	{   109, eSen_FR, 23, grpx_ObjId_Text     },

	{  110, eSen_TV, 24, grpx_ObjId_Text     },
	{  111, eSen_FR, 24, grpx_ObjId_Text     },
	{  112, eSen_TV, 25, grpx_ObjId_Text     },
	{  113, eSen_FR, 25, grpx_ObjId_Text     },
	{  114, eSen_TV, 26, grpx_ObjId_Text     },
	{  115, eSen_FR, 26, grpx_ObjId_Text     },
	{  116, eSen_TV, 27, grpx_ObjId_Text     },
	{  117, eSen_FR, 27, grpx_ObjId_Text     },
};

char gGrap_view[7] = {35,33,38,39,92,126,32};/*xia jian tou  shang zuo you \  wujiaoxing  kongge*/

//static int lrf_preid = 0;
static unsigned int lrf_count  = 0;
static unsigned int SysInfo_count = 0;
/*****************************************************************************
* FunctionName : convert_fixed32_to_float
* Description  :
* ReturnValue  :
* Parameter[0] :
* Parameter[1] :
* Author : xavier
* Date : 2016-05-20
*****************************************************************************/
inline float convert_fixed32_to_float(int iTmp)
{
    float  fTmp = 0.0;

    if((iTmp>>12) < 0xFFFFE/2+1)
    {
        fTmp = (float)(iTmp);
        fTmp /= 4096.0;
    }
    else
    {
        fTmp = (float)(iTmp>>12) - 0x100000;
        fTmp = (float)(((float)(iTmp&0xFFF))/4096.0);
        fTmp+= fTmp;
    }
    return fTmp;
}
/*****************************************************************************
* FunctionName : sysinfo_loop_deal
* Description  :
* ReturnValue  :
* Parameter[0] :
* Parameter[1] :
* Author : xavier
* Date : 2016-05-20
*****************************************************************************/
void sysinfo_loop_deal(char *pStr,int *lineStrLen,int sysinfo_subsys,int sysinfo_bit )
{
	int i;
	//	sysinfo_subsys=0x21;
	for(i = 0;i < 16;i ++)
	{
		if(sysinfo_subsys ==  gSysInit[i].id)
		{
			//printf("%s    sysinfo_subsys=%d\n ",__func__,sysinfo_subsys);
			if(sysinfo_subsys <0x40)
			{
			 sprintf(pStr + *lineStrLen,"%s %s          ",&gSysInit[i].SysInit[0],&gSysInit_1[sysinfo_bit - 1][0]);
			}
			else
			{
			sprintf(pStr + *lineStrLen,"%s           ",&gSysInit[i].SysInit[0]);
			}
		   return ;
		}
		else
			sprintf(pStr + *lineStrLen ,"                           ");
		
		if(i == 15 &&(sysinfo_subsys !=  gSysInit[i].id))
			return ;
	}
	
	return ;
}

/*****************************************************************************
* FunctionName : APP_text_genContext
* Description  : Osd context genneration.
* ReturnValue  : 0 ot -1
* Parameter[0] : int chId, tv 0 flir 1
* Parameter[1] : int winId, osdCR_winId[WINID_SYSTIME,WINID_MAX)
* Parameter[2] : int str, context return out;
* Parameter[3] : int strBufLen, buf size
*              : bit0->1st line bit1->2nd line bit1->3rd line
* Parameter[5] : int *textLen[3]:return line used length,
* Author : xavier
* Date : 2016-05-20
*****************************************************************************/
int APP_text_genContext(int chId, int winId, char * str, int strBufLen, Int16 *textLen)
{
    int iRet = 0;
    int lineStrLen = 0;

    short textLineUsed = 0;

    //char strTemp[128] = {0};
    //char *pStr = strTemp;
    char *pStr = str;
	
    Int16 textLines = 0;
  //  int i = 0;
    Int16 *textValid = &textLines;

    OSA_assert((str != NULL) && (strBufLen > 0));
    OSA_assert(textValid != NULL && textLen != NULL);
    OSA_assert(winId < WINID_TEXT_MAX/2);
    //winId = winId % (WINID_GRAPHIC_MAX/2);

    *textValid  = 0;
    textLen[0] = 0;
    textLen[1] = 0;
    textLen[2] = 0;
//printf("**************************the winid =%d\n",winId);
    switch (winId*2)
    {
    case WINID_TV_SOFTVER:
        {
	
	     int year = 0,month = 0,day = 0,enableShow = 0;
	     int ver1 = 0,ver2 = 0;
	     int work = (gDXD_info.sysConfig[CFGID_OC25_MODECHOOSE] - 1) % 10;//w

    		sprintf(pStr,"    ");   // for reserve
		lineStrLen = strlen(pStr);
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;

		ver1 = gDXD_info.sysConfig[CFGID_OC25_SOFTVER_0];
		ver2 = gDXD_info.sysConfig[CFGID_OC25_SOFTVER_1];
		year = gDXD_info.sysConfig[CFGID_OC25_SOFTVER_YEAR];
		month =gDXD_info.sysConfig[CFGID_OC25_SOFTVER_MON];
		day  = gDXD_info.sysConfig[CFGID_OC25_SOFTVER_DAY];
              enableShow  = gDXD_info.sysConfig[CFGID_OC25_SOFTVER_EN];//w
		
		if(work == 0 && enableShow != 0)
			sprintf(pStr+lineStrLen,"SSDS SW V%d.%02d 20%02d %02d %02d",ver1,ver2,year,month,day);
		else
			sprintf(pStr+lineStrLen,"                         ");
        
		//sprintf(pStr+lineStrLen,"SSDS SW V%d.%02d 20%02d %02d %02d",ver1,ver2,year,month,day);
		lineStrLen = strlen(pStr) -lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;

		lineStrLen = strlen(pStr);
           	OSA_assert(strBufLen >= lineStrLen);
        }
        break;

    case WINID_TV_SENSANGLE:
        {  
            short Pan_ang,Til_ang;
            unsigned int laser_dist = gDXD_info.sysConfig[CFGID_OC25_TAR_RNG];

            if (chId == eSen_TV)
            {
                Pan_ang = (short)gDXD_info.sysConfig[CFGID_RTS_TV_SEN_PAN_ANG];
                Til_ang = (short)gDXD_info.sysConfig[CFGID_RTS_TV_SEN_TIL_ANG];
            }
            else if (chId == eSen_FR)
            {
                Pan_ang  = (short)gDXD_info.sysConfig[CFGID_RTS_FR_SEN_PAN_ANG];
                Til_ang = (short)gDXD_info.sysConfig[CFGID_RTS_FR_SEN_TIL_ANG];
            }
            else
            {
                iRet = OSA_EFAIL;
                break;
            }

            
            sprintf(pStr,"AZ:%+03.02f ",Pan_ang * 180/3.1415 / 4000);		
            lineStrLen = strlen(pStr);
            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            sprintf(pStr + lineStrLen,"EL:%+03.02f ",Til_ang * 180/3.1415 /4000);	
            lineStrLen = strlen(pStr) - lineStrLen;
            *textValid = BIT_SET(textLineUsed,1);
            textLen[1] = lineStrLen;

            lineStrLen = strlen(pStr);
            sprintf(pStr + lineStrLen,"RNG:%05d ",laser_dist);	
            lineStrLen = strlen(pStr) - lineStrLen;
            *textValid = BIT_SET(textLineUsed,2);
            textLen[2] = lineStrLen;

            lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);

        }
        break;

    case WINID_TV_WORKMOD:
        {

	
            int work = gDXD_info.sysConfig[CFGID_OC25_MODECHOOSE];//w
            int Vps_Stat = gDXD_info.sysConfig[CFGID_OC25_GTSSTAT] - 1;//w
            int tv_id = (gDXD_info.sysConfig[CFGID_OC25_TVSTAT] - 1)%4;
            int ir_Power = gDXD_info.sysConfig[CFGID_OC25_IRPOWER];
            work = (work == 11)?11:((work -1)%10);

		
	//printf("!!!!!!!work = %d\n ",work);	
	
            if(Vps_Stat == 1 && work != 3)
                sprintf(pStr, "%s      ",&gWorkStr[work][0]);
            else if(Vps_Stat == 0 && work == 3)
                sprintf(pStr,"%s%s",&gWorkStr[work][0],&gVpsstat[Vps_Stat][0]);
            else if(work == 11)
                sprintf(pStr,"          ");
            else
                sprintf(pStr,"%s      ",&gWorkStr[work][0]);

            lineStrLen =  strlen(pStr);

            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            lineStrLen = strlen(pStr);
            if(tv_id == 1 && gDXD_info.sysConfig[CFGID_OSD_TEXT_1_TOP_Y(4 + 1)] == 170)
                sprintf(pStr+lineStrLen," ");
            else if(ir_Power == 2 && gDXD_info.sysConfig[CFGID_OSD_TEXT_1_TOP_Y(4 + 1)] == 190)
                sprintf(pStr+lineStrLen," ");
            else
                sprintf(pStr+lineStrLen,"%c",39);//you jian tou
            lineStrLen = strlen(pStr) - lineStrLen;
            *textValid = BIT_SET(textLineUsed,1);
            textLen[1] = lineStrLen;

            lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);

        }
        break;
		
    case WINID_TV_UPDATESTAT:
        {
		
		int tv_id = (gDXD_info.sysConfig[CFGID_OC25_TVSTAT] - 1)%4;
        
                int ir_Power = gDXD_info.sysConfig[CFGID_OC25_IRPOWER];//w
		int ir_Stat = gDXD_info.sysConfig[CFGID_OC25_IRSTAT];
                int ir_Pal =  gDXD_info.sysConfig[CFGID_OC25_IRPAL];
                
		int Lrf_Power = gDXD_info.sysConfig[CFGID_OC25_LRFPOWER];
		int Lrf_Stat = gDXD_info.sysConfig[CFGID_OC25_LRFSTAT];
		int Lrf_NoStat = gDXD_info.sysConfig[CFGID_OC25_LRFNSTAT];
		int Lrf_Ready = gDXD_info.sysConfig[CFGID_OC25_LRFREADY];
		int Lrf_Bright =1; gDXD_info.sysConfig[CFGID_OC25_LRFBRIGHT];
		int Lrf_Run = gDXD_info.sysConfig[CFGID_OC25_LRFRUN];
		int Lrf_Allow = gDXD_info.sysConfig[CFGID_OC25_LRFALLOW];
		int Lrf_EngLev = gDXD_info.sysConfig[CFGID_OC25_LRFENGLEV];
		
		int stat1,stat3,stat4,stat5,stat6;
                char irText[2] = {' ','x'};
                char irText1[2] = {'B','W'};
		char *lrfText[2] = {" ", "x"};
		char *lrfText1[3] = {" ","*",&gGrap_view[5]};
		char lrfText2[2] = {'L','H'};
		char templrf[2] = {' ','x'};
		
		if(lrf_count > 0xFFFFFFFF)
			lrf_count = 0;
		lrf_count ++;
		
		sprintf(pStr,"%s",&gTvStat[tv_id][0]);
		lineStrLen = strlen(pStr);
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;	

                if(ir_Power == 0x01)
                {
                    sprintf(pStr+lineStrLen,"IR%c%c",irText[(ir_Stat - 1) %2],irText1[(ir_Pal - 1)%2]);
                }
                else
                {
                    sprintf(pStr+lineStrLen,"    ");
                    gDXD_info.sysConfig[CFGID_OC25_IRSTAT] = 1;
                    gDXD_info.sysConfig[CFGID_OC25_IRPAL] = 1;
                }

		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;
			
		lineStrLen = strlen(pStr);

		if(Lrf_Power == 0x00 || Lrf_Power == 0x02)
		{
                    sprintf(pStr + lineStrLen,"       ");
                    if(Lrf_Stat != 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFSTAT] = 0;
                    if(Lrf_NoStat != 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFNSTAT] = 0;
                    if(Lrf_Ready!= 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFREADY] = 0;
                    if(Lrf_Run!= 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFRUN] = 0;
                    if(Lrf_Bright != 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFBRIGHT] = 0;
                    if(Lrf_Allow != 0)
                    	 gDXD_info.sysConfig[CFGID_OC25_LRFALLOW] = 0;	
                    if(Lrf_EngLev!= 0)
                    	gDXD_info.sysConfig[CFGID_OC25_LRFENGLEV] = 0;	
		}
		else if(Lrf_Power == 0x01)
		{
                    if(Lrf_Ready != 0)
                    	stat1 = Lrf_Ready % 2;  //->
                    else
                    	stat1 = 0;

                    if(Lrf_Bright % 2 == 0)
                    	stat3 = 0;
                    else
                    	stat3 = Lrf_Run % 3; //*

                    if(Lrf_NoStat != 0)
                    	stat4 =(Lrf_NoStat - 1) % 2; //x
                    else
                    	stat4 = 0;

                    if(Lrf_Allow != 0)
                    	stat5 = Lrf_Allow % 2;
                    else
                    	stat5 = 0;

                    if(Lrf_EngLev != 0)
                    	stat6 = Lrf_EngLev % 2;
                    else
                    	stat6 = 0;

                    if(stat5 == 0)
                    	sprintf(pStr+lineStrLen,"%cL",gGrap_view[6]);
                    else
                    	sprintf(pStr+lineStrLen,"%c|",gGrap_view[6]);

                    strcat(pStr+lineStrLen,lrfText1[stat3]);
                    strcat(pStr+lineStrLen,lrfText[stat4]);

                    if(stat1)
                    {	
                            if(stat5 == 0)
                    		sprintf(pStr+lineStrLen,"%cL %c%c",gGrap_view[3],templrf[stat4],lrfText2[stat6]);
                    	else
                    		sprintf(pStr+lineStrLen,"%c| %c%c",gGrap_view[3],templrf[stat4],lrfText2[stat6]);
                    }
                    else
                    {
                        if(stat5 == 0)
                    		sprintf(pStr+lineStrLen," L %c%c",templrf[stat4],lrfText2[stat6]);
                    	else
                    		sprintf(pStr+lineStrLen," | %c%c",templrf[stat4],lrfText2[stat6]);
                    }
                    

                    if((stat3 == 2) && Lrf_Bright == 1)
                    {
                    	if(stat4 == 0)
                    	{
                    		if(lrf_count % 16 <= 7)
                    			 sprintf(pStr+lineStrLen,"%cL%c %c",gGrap_view[3],gGrap_view[5],lrfText2[stat6]); 
                    		 else
                    	 		 sprintf(pStr+lineStrLen,"%cL  %c",gGrap_view[3],lrfText2[stat6]); 
                    	}
                    	else
                    	{
                    		if(lrf_count % 16 <= 7)
                    			 sprintf(pStr+lineStrLen,"%cL%cx%c",gGrap_view[3],gGrap_view[5],lrfText2[stat6]); 
                    		 else
                    	 		 sprintf(pStr+lineStrLen,"%cL x%c",gGrap_view[3],lrfText2[stat6]); 
                    	}
                    }

                    if((stat3 == 1) && Lrf_Bright == 1)
                    {
                    	if(stat4 == 0)
                    	{
                    		if(lrf_count % 16 <= 7)
                    		    sprintf(pStr+lineStrLen,"%cL* %c",gGrap_view[3],lrfText2[stat6]);
                    		else
                    		    sprintf(pStr+lineStrLen,"%cL  %c",gGrap_view[3],lrfText2[stat6]);
                    	}
                    	else
                    	{
                    		if(lrf_count % 16 <= 7)
                    			 sprintf(pStr+lineStrLen,"%cL*x%c",gGrap_view[3],lrfText2[stat6]); 
                    		 else
                    	 		 sprintf(pStr+lineStrLen,"%cL x%c",gGrap_view[3],lrfText2[stat6]); 
                    	}
                    }

                }
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,2);
		textLen[2] = lineStrLen;	

                lineStrLen = strlen(pStr);
                OSA_assert(strBufLen >= lineStrLen);
        }
        break;

    case WINID_TV_AXIS:
        {
	    
	  //  printf("WINID_TV_AXIS the winid =%d\n",winId);
            float iaxisx =convert_fixed32_to_float( gDXD_info.sysConfig[CFGID_RT_MAIN_SEN_BROESIGHT_X]);
            float iaxisy = convert_fixed32_to_float(gDXD_info.sysConfig[CFGID_RT_MAIN_SEN_BROESIGHT_Y]);

            sprintf(pStr, "Bx:%+04.0f", iaxisx);
            lineStrLen = strlen(pStr);

            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            sprintf(pStr + lineStrLen, "By:%+04.0f", iaxisy);
            lineStrLen = strlen(pStr) - lineStrLen;

            *textValid = BIT_SET(textLineUsed,1);
            textLen[1] = lineStrLen;

            lineStrLen = strlen(pStr);
	   //  printf("the len[0]=%d  len[1]=%d\n",textLen[0],textLen[1]);

            OSA_assert(strBufLen >= lineStrLen);

        }
        break;
 case WINID_TV_TARGET_COORD:
        {
		float Lon_data  = (float)gDXD_info.sysConfig[CFGID_OC25_TAR_LON];
		float Lat_data  = (float)gDXD_info.sysConfig[CFGID_OC25_TAR_LAT];
		short Alt_data = gDXD_info.sysConfig[CFGID_OC25_TAR_ALT];
                
		sprintf(pStr,"LON:%+03.05f ",(Lon_data / 100000));
		lineStrLen = strlen(pStr);
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;

		sprintf(pStr+lineStrLen,"LAT:%+03.05f ",(Lat_data / 100000));
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;

		lineStrLen = strlen(pStr);
		sprintf(pStr+lineStrLen,"ALT:%+04d.0",Alt_data);
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,2);
		textLen[2] = lineStrLen;	

		lineStrLen = strlen(pStr);
		OSA_assert(strBufLen >= lineStrLen);
        }
        break;

    case WINID_TV_FOV_CHOOSE_0:
        {
		sprintf(pStr,"%c",gFovChoose[0]);
		lineStrLen = strlen(pStr);
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;

		sprintf(pStr+lineStrLen,"%c",gFovChoose[1]);
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;
		
		lineStrLen = strlen(pStr);
		sprintf(pStr+lineStrLen,"%c",gFovChoose[2]);
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,2);
		textLen[2] = lineStrLen;	

            lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);
        }
        break;
		
    case WINID_TV_FOV_CHOOSE_1:
        {
		sprintf(pStr,"%c",gFovChoose[3]);
		lineStrLen = strlen(pStr);
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;

		sprintf(pStr+lineStrLen,"%c",gFovChoose[4]);
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;

		lineStrLen = strlen(pStr);
		sprintf(pStr+lineStrLen,"%c",gFovChoose[5]);
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,2);
		textLen[2] = lineStrLen;	

            lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);
        }
        break;

    case WINID_TV_UPDATE_MENU_0:
        {
		int update_menu_id0 =  gDXD_info.sysConfig[CFGID_OC25_MENUINFO];
		int i ;
		//  break;
		for(i = 0 ; i< 33; i ++)
		{
			if(update_menu_id0 == gMenu_Info[i].id)
			{
				sprintf(pStr,"%s",&gMenu_Info[i].Menu[0][0]);
				lineStrLen = strlen(pStr);
				*textValid = BIT_SET(textLineUsed,0);
				textLen[0] = lineStrLen;

				sprintf(pStr+lineStrLen,"%s",&gMenu_Info[i].Menu[1][0]);
				lineStrLen = strlen(pStr) - lineStrLen;
				*textValid = BIT_SET(textLineUsed,1);
				textLen[1] = lineStrLen;

				lineStrLen = strlen(pStr);
				sprintf(pStr+lineStrLen,"%s",&gMenu_Info[i].Menu[2][0]);
				lineStrLen = strlen(pStr) - lineStrLen;
				*textValid = BIT_SET(textLineUsed,2);
				textLen[2] = lineStrLen;	
			}
		}
		
		lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);
        }
        break;

    case WINID_TV_UPDATE_MENU_1:
        {
		int update_menu_id1 =  gDXD_info.sysConfig[CFGID_OC25_MENUINFO];
		int i ;
		//  break;
		for(i = 0 ; i < 33; i ++)
		{
			if(update_menu_id1 == gMenu_Info[i].id)
			{
				sprintf(pStr,"%s",&gMenu_Info[i].Menu[3][0]);
				lineStrLen = strlen(pStr);
				*textValid = BIT_SET(textLineUsed,0);
				textLen[0] = lineStrLen;

				sprintf(pStr+lineStrLen,"%s",&gMenu_Info[i].Menu[4][0]);
				lineStrLen = strlen(pStr) - lineStrLen;
				*textValid = BIT_SET(textLineUsed,1);
				textLen[1] = lineStrLen;	
			}
		}
		
		lineStrLen = strlen(pStr);
		OSA_assert(strBufLen >= lineStrLen);
        }
        break;

     case WINID_TV_SYSINFO_LOOP_0:
        {
            int sysinfo_subsys[3];
            int sysinfo_bit[3];

	    //printf("WINID_TV_SYSINFO_LOOP_0\n");

            sysinfo_subsys[0] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SUBSYS0];
            sysinfo_subsys[1] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SUBSYS1];
            sysinfo_subsys[2] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SUBSYS2];

            sysinfo_bit[0] =  gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SYSSTAT0];
            sysinfo_bit[1] =  gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SYSSTAT1];
            sysinfo_bit[2] =  gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SYSSTAT2];

            sysinfo_loop_deal( pStr, &lineStrLen, sysinfo_subsys[0], sysinfo_bit[0]);

            lineStrLen = strlen(pStr);
            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            sysinfo_loop_deal( pStr, &lineStrLen, sysinfo_subsys[1], sysinfo_bit[1]);
            lineStrLen = strlen(pStr) -lineStrLen;
            *textValid = BIT_SET(textLineUsed,1);
            textLen[1] = lineStrLen;

            lineStrLen = strlen(pStr);

            sysinfo_loop_deal( pStr, &lineStrLen, sysinfo_subsys[2], sysinfo_bit[2]);	
            lineStrLen = strlen(pStr) -lineStrLen;
            *textValid = BIT_SET(textLineUsed,2);
            textLen[2] = lineStrLen;	

            lineStrLen = strlen(pStr);
            OSA_assert(strBufLen >= lineStrLen);
        }
        break;

     case WINID_TV_SYSINFO_LOOP_1:
	{
		int sysinfo_subsys[2];
		int sysinfo_bit[2];
		
		sysinfo_subsys[0] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SUBSYS3];
		sysinfo_subsys[1] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SUBSYS4];

		sysinfo_bit[0] =  gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SYSSTAT3];
		sysinfo_bit[1] =  gDXD_info.sysConfig[CFGID_OC25_SYSINFO_SYSSTAT4];

		sysinfo_loop_deal( pStr, &lineStrLen, sysinfo_subsys[0], sysinfo_bit[0]);	
		lineStrLen = strlen(pStr) - lineStrLen;
		*textValid = BIT_SET(textLineUsed,0);
		textLen[0] = lineStrLen;

		sysinfo_loop_deal( pStr, &lineStrLen, sysinfo_subsys[1], sysinfo_bit[1]);	
		lineStrLen = strlen(pStr) -lineStrLen;
		*textValid = BIT_SET(textLineUsed,1);
		textLen[1] = lineStrLen;
		
		lineStrLen = strlen(pStr);
		OSA_assert(strBufLen >= lineStrLen);
        }
        break;		
    case WINID_TV_VCP_SYS:
        {
            int Sys_Infoid= gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STATE];
            int Sys_InfoStat_0 = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_DATA_0];
            int Sys_InfoStat_1 = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_DATA_1];
            int Sys_InfoEn = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_ENABLE];
            int Sys_String[5];
            char String_buf[21];
            char TempString[11];
            int StringLen = 0;
            int i = 0, j = 0;

            memset(String_buf,0,21);
            Sys_String[0] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRING];
            Sys_String[1] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRING1];
            Sys_String[2] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRING2];
            Sys_String[3] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRING3];
            Sys_String[4] = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRING4];
            StringLen = gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STRINGLEN];
            
	    if(SysInfo_count >  0xFFFFFFFF)
	         	SysInfo_count = 0;

            for(j = 0; j < 5;j ++)
            {
                for(i = 0;i < 4;i++)
                {
                    String_buf[i + 4 * j] = (Sys_String[j] >>(8 * i))& 0xFF;
                    //printf("char:%c\n",String_buf[i + 4 * j] );
                }
            }
            String_buf[20] = '\0';
            
            for(j = 0;j < 10;j++)
                TempString[j] = 32;
            TempString[j] = '\0';
#if 0
            if(work == 0x03)
            sprintf(pStr, "%s",&gVpsstat[Vps_Stat][0]);
            else
#endif
            sprintf(pStr,"    ");

            lineStrLen = strlen(pStr);

            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            if(Sys_Infoid >= 0 && Sys_Infoid <= 3 && Sys_InfoEn)
            {	       
                SysInfo_count = 0;	
                gDXD_info.sysConfig[CFGID_OC25_SYSINFO_ENABLE] = 0;
            }  
            
            SysInfo_count ++; 
	 
	  if(Sys_Infoid == 0x00)
	   	sprintf(pStr+lineStrLen,"     TVS FOCUS:%d      ",(((Sys_InfoStat_1<<8)&0XFF00)|(Sys_InfoStat_0 &0XFF)));
	   else if(Sys_Infoid == 0x01)
	   	sprintf(pStr+lineStrLen,"     FLIR FOCUS:%d      ",(((Sys_InfoStat_1<<8)&0XFF00)|(Sys_InfoStat_0 &0XFF)));
	   else if(Sys_Infoid == 0x02)
	   {
	   	switch(Sys_InfoStat_0>>4)
	   	{
                    case 0x01:
                        sprintf(pStr+lineStrLen,"     OCU VER:%d.%d%d     ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                        break;
                    case 0x02:
                    	sprintf(pStr+lineStrLen,"    IR VER:%d.%d%d    ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                    	break;
                    case 0x03:
                    	sprintf(pStr+lineStrLen,"     TV VER:%d.%d%d     ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                    	break;
                    case 0x04:
                    	sprintf(pStr+lineStrLen,"      AVT VER:%d.%d%d     ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                    	break;
                    case 0x05:
                    	sprintf(pStr+lineStrLen,"      LD VER:%d.%d%d     ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                    	break;
                    case 0x0A:
                    	sprintf(pStr+lineStrLen,"     SER VER:%d.%d%d     ",Sys_InfoStat_0&0xF,(Sys_InfoStat_1 >> 4)&0xF,Sys_InfoStat_1&0xF);
                    	break;
                    default:
                    	sprintf(pStr+lineStrLen,"%s%s  ",TempString,TempString);
                    	break;
	   	}
	}
	else if(Sys_Infoid == 0x03)
	{
	        for(i = 0;i < (20 - StringLen) / 2;i++)
                    TempString[i] = 32;
                TempString[i] = '\0';
		sprintf(pStr+lineStrLen,"%s%s%s  ",TempString,String_buf,TempString);
	}
	else
		sprintf(pStr+lineStrLen,"%s%s  ",TempString,TempString);

	if(SysInfo_count /100 != 0)
	{
	       gDXD_info.sysConfig[CFGID_OC25_SYSINFO_STATE] = 4;
	}

	    lineStrLen = strlen(pStr) - lineStrLen;

	    *textValid = BIT_SET(textLineUsed,1);
	    textLen[1] = lineStrLen;

	    lineStrLen = strlen(pStr);
	    OSA_assert(strBufLen >= lineStrLen);

        }
        break;
    case WINID_TV_TRK_TBE:
        {
            float idbgx = convert_fixed32_to_float(gDXD_info.sysConfig[CFGID_RT_MAIN_SEN_TRACK_TBE_X]);
            float idbgy = convert_fixed32_to_float(gDXD_info.sysConfig[CFGID_RT_MAIN_SEN_TRACK_TBE_Y]);

            sprintf(pStr, "TRx:%+07.02f", idbgx);
            lineStrLen = strlen(pStr);

            *textValid = BIT_SET(textLineUsed,0);
            textLen[0] = lineStrLen;

            sprintf(pStr + lineStrLen, "TRy:%+07.02f", idbgy);
            lineStrLen = strlen(pStr) - lineStrLen;

            *textValid = BIT_SET(textLineUsed,1);
            textLen[1] = lineStrLen;

            lineStrLen = strlen(pStr);

            OSA_assert(strBufLen >= lineStrLen);
        }
        break;
		
    default:
        iRet = OSA_EFAIL;
        break;
    }

    iRet = textLines;

    return iRet;

}
//#endif
/************************************** The End Of File **************************************/


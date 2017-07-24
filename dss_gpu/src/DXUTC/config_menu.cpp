#include <stdio.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>

#include <dx.h>
#include <dx_priv.h>
#include <arpa/inet.h>

extern Int32 gCurStartUpId;
static Int32 cSaveFlag = 0;

#define clip(rslt, a, b)  (rslt>b)?(b):((rslt<a)?a:rslt)

char Dx_getChar(void)
{
    char buffer[128];
    fflush(stdin);
    fgets(buffer, 128, stdin);
    return(buffer[0]);
}

static Int32 Dx_getInt32(void)
{
    char buffer[128];
    buffer[0] = '\0';

    fflush(stdin);
    fgets(buffer, 128, stdin);

    if(strlen(buffer) <= 1)
        return -1;

    if(buffer[0] == 'e' ||buffer[0] == 'E')
        return -2;

    return atoi(buffer);
}

static void  Dx_configSetup(SCRIPT_FIELD *field)
{
    Int32 value=0;
    char buffer[128];

    printf("\r\n Change %s", field->lab);
    if(field->dataFmt == DX_INTERGER) //"INTEGER"
    {
        printf("\r\n Blk%d Field%d [%d - %d] %d --> ", field->blkid, field->fldno, field->imin, field->imax,
               gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)]);
    }
    else if(field->dataFmt == DX_FIXED32)//"FIXED(32,12)"
    {
        float  fTmp;
        int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];

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

        printf("\r\n Blk%d Field%d [%f - %f] %f --> ", field->blkid, field->fldno, field->fmin, field->fmax,fTmp);
    }
    else if(field->dataFmt == DX_IPADDRS)//"IPADDR"
    {
        int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];
        sprintf(buffer,"%3d.%3d.%3d.%3d",(iTmp>>24)&0xFF,(iTmp>>16)&0xFF,(iTmp>>8)&0xFF,iTmp&0xFF);
        printf("\r\n Blk%d Field%d %s --> ", field->blkid, field->fldno,buffer);
    }
    else if(field->dataFmt == DX_BYTES)//"BYTE"
    {
        printf("\r\n Blk%d Field%d %c --> ", field->blkid, field->fldno,
               (char)gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)]);
    }
    else if(field->dataFmt == DX_TIMES)//"DATE/TIME"
    {
        int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];
        sprintf(buffer,"%4d:%2d:%2d",(iTmp>>16)&0x0000FFFF,(iTmp>>8)&0x000000FF,(iTmp&0x000000FF));
        printf("\r\n Blk%d Field%d %s --> ", field->blkid, field->fldno,buffer);
    }

    fflush(stdin);
    fgets(buffer, 128, stdin);

    if(strlen(buffer) > 1)
    {
        if(field->mode == 0)
            cSaveFlag = 1;

        if(field->dataFmt == DX_INTERGER)      //"INTEGER"
        {
            value = atoi(buffer);
            if(field->imax > field->imin)
            {
            	value = clip(value, field->imin, field->imax);
            }
        }
        else if(field->dataFmt == DX_FIXED32)//"FIXED(32,12)"
        {
            float fTmp;
            float left;

            fTmp = atof(buffer);
            left = fTmp - (int)(fTmp);
            value = (int)(fTmp);
            value <<= 12;
            value += (int)(left * 4096.0);
        }
        else if(field->dataFmt == DX_IPADDRS)//"IPADDR"
        {
            value = inet_addr(buffer);
            value = ntohl(value);
        }
        else if(field->dataFmt == DX_BYTES)//"BYTE"
        {
            value = buffer[0];
        }
        else if(field->dataFmt == DX_TIMES)
        {
            int a,b,c;
            sscanf(buffer,"%d:%d:%d",&a,&b,&c);
            value = (a << 16)|(b<<8)|(c);
        }

        gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)] = value;

        if(gDXD_info.fxnsCfg[CFGID_BUILD(field->blkid,field->fldno)] != NULL)
        {
            SCRIPT_OBJ* script;
            gDXD_info.fxnsCfg[CFGID_BUILD(field->blkid,field->fldno)](field->blkid,field->fldno);
            script = (SCRIPT_OBJ*)(gDXD_info.scripts);

            OSA_assert(script != NULL);

            if(script->m_script.dataBlk[field->blkid].field[field->fldno].mode == 1)
            {
                gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)] = 0;
            }
        }

        if(field->blkid== 9 && (field->fldno == 6 ||field->fldno == 7 ))
        {
            cSaveFlag = 0;
        }
    }
}

static void Dx_configMenuSub(DATABLK *blk)
{
    int i, iSel=-1;
    SCRIPT_FIELD *field;

    while(iSel != -2)
    {
        printf("\r\n =============");
        printf("\r\n %s", blk->text);
        printf("\r\n =============");
        printf("\r\n");
        for(i=0; i<16; i++)
        {
            if(blk->field[i].blkid >= 0 && blk->field[i].fldno >= 0 && blk->field[i].fldno < 16 )
            {
                OSA_assert(gDXD_info.sysConfig != NULL);
                field = &blk->field[i];

                if(field->dataFmt == DX_FIXED32)//"FIXED(32,12)"
                {
                    float  fTmp;
                    int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];

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

                    printf("\r\n %d: \t%s - %0.3f", blk->field[i].fldno, blk->field[i].lab,fTmp);
                }
                else if(field->dataFmt == DX_IPADDRS)//"IPADDR"
                {
                    char buffer[128];
                    int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];
                    sprintf(buffer,"%3d.%3d.%3d.%3d",(iTmp>>24)&0xFF,(iTmp>>16)&0xFF,(iTmp>>8)&0xFF,iTmp&0xFF);
                    printf("\r\n %d: \t%s - %s", blk->field[i].fldno, blk->field[i].lab, buffer);
                }
                else if(field->dataFmt == DX_TIMES)//"DATE/TIME"
                {
                    char buffer[128];
                    int iTmp = gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)];
                    sprintf(buffer,"%4d:%2d:%2d",(iTmp>>16)&0x0000FFFF,(iTmp>>8)&0x000000FF,(iTmp&0x000000FF));
                    printf("\r\n %d: \t%s - %s", blk->field[i].fldno, blk->field[i].lab, buffer);
                }
                else
                {
                    printf("\r\n %d: \t%s - %10d", blk->field[i].fldno, blk->field[i].lab, gDXD_info.sysConfig[CFGID_BUILD(field->blkid,field->fldno)]);
                }
            }
        }
        printf("\r\n");
        printf("\r\n e: Exit Configure");
        printf("\r\n");
        printf("\r\n Enter Choice: ");

        iSel =  Dx_getInt32();

        if(iSel >= 0)
        {
            if(iSel < 16 && blk->field[iSel].blkid >= 0 && blk->field[iSel].fldno >= 0 && blk->field[iSel].fldno < 16 && blk->field[iSel].blkid < 129)
            {
                Dx_configSetup(&blk->field[iSel]);
            }
        }
    }
}

void Dx_configMenu(Int32 *script)
{
    int i = 0, iBak = 0;
    int iPage = 0;
    Int32 iSel = -1;

    SCRIPT_OBJ *pScript =(SCRIPT_OBJ*) script;
  // printf("****************************************the pscript=%p  scrip=%p\n",pScript,script);
    if(pScript == NULL)
        return;

    cSaveFlag = 0;

    while(iSel != -2)
    {
 
        if(i == 256)
            i=0;
        printf("\r\n =====================");
        printf("\r\n Select Configure Page");
        printf("\r\n =====================");
        printf("\r\n");
        iBak = i;
        iPage = 0;
        for(i; i<256; i++)
        {
            if(pScript->m_script.dataBlk[i].bValid)
            {
                printf("\r\n %d: \t%s", i, pScript->m_script.dataBlk[i].label);
                iPage++;
                if(iPage > 16)
                    break;
            }
        }
   //printf("****************************************the pscript=%p  scrip=%p  the i=%d\n",pScript,script,i);
        printf("\r\n");
        if(i != 256)
        {
            printf("\r\n Enter: Next page");
        }
        else if(iBak != 0)
        {
            printf("\r\n Enter: First page");
        }

        printf("\r\n e: Exit Configure");
        printf("\r\n");
        printf("\r\n Enter Choice: ");

        iSel =  Dx_getInt32();
        if(iSel >= 0)
        {
            i = iBak;
            if(iSel < 256 && pScript->m_script.dataBlk[iSel].bValid)
            {
                Dx_configMenuSub(&pScript->m_script.dataBlk[iSel]);
            }
        }
    }

    if(cSaveFlag && gDXD_info.fxnsCfg[CFGID_BUILD(9,7)] != NULL )
    {
        char gc;
        printf("\r\n");
        printf("\r\n system configure data changed,  save as startup %d ?", gCurStartUpId);
        printf("\r\n Enter Choice (y/n): ");
        gc =  Dx_getChar();
        if((gc == 'y') || (gc == 'Y'))
        {
            FIELD_ITEM_SYS(CFGID_SYS_SAVE_STARTUP_AS) = gCurStartUpId;
            gDXD_info.fxnsCfg[CFGID_BUILD(9,7)](9, 7);
        }
    }
}



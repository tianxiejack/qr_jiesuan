#include "dx.h"
#include "script.h"

static char *gScriptFile = "config/default.txt";

static int script_Init(SCRIPT_OBJ *pObj, char *scriptFile);
static void script_ParseLine(SCRIPT_OBJ *pObj, char *line);
static void script_ParseFieldLine(SCRIPT_OBJ *pObj, char *key, char *line);

Int32 script_create(char *scriptfile, SCRIPT_OBJ** ppObj)
{
    Int32 iRet = 0;
    SCRIPT_OBJ *pObj;
    pObj = (SCRIPT_OBJ *)malloc(sizeof(SCRIPT_OBJ));

    if(pObj == NULL)
        return -1;

    memset(pObj, 0, sizeof(*pObj));

    script_Init(pObj, scriptfile);

    if(ppObj != NULL)
        *ppObj = pObj;

    return iRet;
}

void   script_destroy(SCRIPT_OBJ *pObj)
{
    UTILS_assert(pObj != NULL);

    free(pObj);
}

static int script_Init(SCRIPT_OBJ *pObj, char *scriptFile)
{
    FILE *stream;
    char line[256];
    int iLine =0;
    int i, j;

    memset(&pObj->m_script, 0, sizeof(APP_SCRIPT));
    memset(pObj->m_enumTab, 0, sizeof(ENUMTAB)*100);

    for(i=0; i<256; i++)
        for(j=0; j<16; j++)
            pObj->m_script.dataBlk[i].field[j].blkid = -1;

    if(scriptFile == NULL)
        scriptFile = gScriptFile;

    if( (stream  = fopen( scriptFile, "r" )) == NULL )
    {
        printf( "The script file %s was not opened !!!\n", scriptFile);
        return 0;
    }
    OSA_printf("%s:%d %s",__func__,__LINE__,scriptFile);

    do
    {
        memset(line, 0, 128);
        if(fgets(line, 128, stream) == NULL)
            break;
        iLine ++;
        //TRACE("[%03d]:   %s\n", iLine, line);
        script_ParseLine(pObj, line);
    }
    while(1);

    fclose( stream );

    return 0;
}


static void script_ParseLine(SCRIPT_OBJ *pObj, char *line)
{
    int i, len;
    char *label = NULL;
    char *text = NULL;
    BOOL bX=FALSE, bY=FALSE;
    if((len = strlen(line)) < 3)
        return ;

    for(i=0; i<len; i++)
    {
        if(line[i] == ';')
            return;
        if(line[i] != ' ')
        {
            label = &line[i];
            break;
        }
    }
    if(label == NULL)
        return ;
    len -= label-line;

    if(strncmp(label, "ENUMTAB", 7) == 0)
    {
        if(pObj->m_nEnumTab == 128)
        {
            printf("Error: enumtab out of 128 !\n");
            return;
        }
        pObj->m_enumTab[pObj->m_nEnumTab].tablenum = atoi(&label[7]);
        pObj->m_nEnumTab ++;
        return ;
    }

    if(strncmp(label, "ENUMDEF", 7) == 0)
    {
        if(pObj->m_nEnumTab == 0)
            return ;
        ENUMTAB *pEnumTab = &pObj->m_enumTab[pObj->m_nEnumTab-1];
        len -= 7;
        label += 7;
        text = NULL;
        for(i=0; i<len; i++)
        {
            if(label[i] == '"')
            {
                if(text == NULL)
                    text = &label[i+1];
                else
                {
                    label[i] = '\0';
                }
            }
            if(text != NULL && label[i] == '=')
            {
                pEnumTab->def[pEnumTab->idef].value = atoi(&label[i+1]);
                strcpy(pEnumTab->def[pEnumTab->idef].text, text);
                pEnumTab->idef ++;
                break;
            }
        }
        return ;
    }

    if(strncmp(label, "SCREEN", 6) == 0)
    {
        len -= 7;
        label += 7;
        text = NULL;
        pObj->m_iCurDataBlk = atoi(label);
        pObj->m_script.dataBlk[pObj->m_iCurDataBlk].number = pObj->m_iCurDataBlk;
        for(i=0; i<len; i++)
        {
            if(label[i] == '"')
            {
                if(text == NULL)
                {
                    text = &label[i+1];
                }
                else
                {
                    label[i] = '\0';
                    pObj->m_script.dataBlk[pObj->m_iCurDataBlk].bValid = TRUE;
                    strcpy(pObj->m_script.dataBlk[pObj->m_iCurDataBlk].label, text);
                }
            }
        }
        pObj->m_script.nDataBlk++;
        return ;
    }

    if(strncmp(label, "HEADING", 7) == 0)
    {
        len -= 7;
        label += 7;
        text = NULL;
        pObj->m_script.dataBlk[pObj->m_iCurDataBlk].x = atoi(label);
        for(i=0; i<len; i++)
        {
            if(label[i] == '"')
            {
                if(text == NULL)
                {
                    text = &label[i+1];
                }
                else
                {
                    label[i] = '\0';
                    strcpy(pObj->m_script.dataBlk[pObj->m_iCurDataBlk].text, text);
                    return ;
                }
            }

            if(bX && bY)
                continue;
            if(label[i] == ' ')
            {
                if(!bX)
                    bX = TRUE;
                else if(label[i-1] != ' ')
                {
                    bY = TRUE;
                    pObj->m_script.dataBlk[pObj->m_iCurDataBlk].y = atoi(&label[i]);
                }
            }
        }
        return ;
    }

    if(strncmp(label, "FIELD", 5) == 0)
    {
        len -= 5;
        label += 5;
        script_ParseFieldLine(pObj, "FIELD", label);
        return ;
    }
    if(strncmp(label, "WRITE_ONLY", 10) == 0)
    {
        len -= 10;
        label += 10;
        script_ParseFieldLine(pObj, "WRITE_ONLY", label);
        return ;
    }
}

char *strNotchr(char *str, char n)
{
    int len, i;
    if(str == NULL)
        return NULL;
    len = strlen(str);
    for(i=0; i<len; i++)
    {
        if(str[i] != n)
            return &str[i];
    }
    return NULL;
}
#define NEXT_KEY	\
{	\
	tmp = strchr(tmp, ' ');	\
	if(tmp == NULL){	\
		return ;	\
	}	\
	tmp = strNotchr(tmp, ' ');	\
	if(tmp == NULL){	\
		return ;	\
	}	\
}
static void script_ParseFieldLine(SCRIPT_OBJ *pObj, char *key, char *line)
{
    int len, i;
    SCRIPT_FIELD field;
    char *tmp = line;
    if((len = strlen(line)) < 3)
        return ;
    memset(&field, 0, sizeof(SCRIPT_FIELD));

    if(strcmp(key, "WRITE_ONLY") == 0)
        field.mode = 1;

    tmp = strNotchr(tmp, ' ');
    field.blkid = atoi(tmp);
    NEXT_KEY;
    field.fldno = atoi(tmp);
    char *lab = strchr(tmp, '"');
    lab ++;
    tmp = strchr(lab, '"');
    tmp[0] = '\0';
    strcpy(field.lab, lab);
    tmp ++;
    NEXT_KEY;
    field.scr = atoi(tmp);
    NEXT_KEY;
    field.x = atoi(tmp);
    NEXT_KEY;
    field.y = atoi(tmp);

    NEXT_KEY;

    if(strncmp(tmp, "FIXED(32,12)", 12) == 0)
    {
        field.dataFmt = DX_FIXED32;
        tmp += 12;
    }
    else if(strncmp(tmp, "WORD", 4) == 0)
    {
        field.dataFmt = DX_WORDS;
        tmp += strlen("WORD");
    }
    else if(strncmp(tmp,"BYTE",4) == 0)
    {
    	field.dataFmt = DX_BYTES;
    	tmp += strlen("BYTE");
    }
    else if(strncmp(tmp,"IPADDRESS",9) == 0)
    {
    	field.dataFmt = DX_IPADDRS;
    	tmp += strlen("IPADDRESS");
    }
    else if(strncmp(tmp,"TIME",4) == 0)
    {
        field.dataFmt = DX_TIMES;
        tmp += strlen("TIME");
    }
    else
    {
        field.dataFmt = DX_INTERGER;
        tmp += strlen("INTEGER");
    }

    tmp = strNotchr(tmp, ' ');
    if(tmp[0] == '[')
    {
        if(field.dataFmt)
        {
            field.fmin = (float)atof(&tmp[1]);
            tmp = strchr(tmp, ',');
            field.fmax = (float)atof(&tmp[1]);
        }
        else
        {
            field.imin = atoi(&tmp[1]);
            tmp = strchr(tmp, ',');
            field.imax = atoi(&tmp[1]);
        }
        tmp = strchr(tmp, ']');
        tmp ++;
    }
    tmp = strNotchr(tmp, ' ');
    if(field.dataFmt)
        field.fInit = (float)atof(tmp);
    else
        field.iInit = atoi(tmp);

    tmp = strchr(tmp, ' ');
    tmp = strNotchr(tmp, ' ');
    if(tmp != NULL)
    {
        if(strncmp(tmp, "ENUMS", 5) == 0)
        {
            NEXT_KEY;
            field.enumtabno = atoi(tmp);
            for(i=0; i<pObj->m_nEnumTab; i++)
            {
                if(pObj->m_enumTab[i].tablenum == field.enumtabno)
                {
                    field.enumtab = &pObj->m_enumTab[i];
                    break;
                }
            }
        }
        else if(strncmp(tmp, "CHECKBOX", 8) == 0)
        {
            field.bCheckBox = TRUE;
        }
    }

    memcpy(&pObj->m_script.dataBlk[field.blkid].field[field.fldno], &field, sizeof(SCRIPT_FIELD));

    /*
    printf("\t%02d-%02d <format %d> <init %d>  '%s'\n",
    	field.blkid, field.fldno, field.format, field.iInit, field.lab);
    */
}

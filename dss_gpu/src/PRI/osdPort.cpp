

#include "osdPort.h"
#include "osdProcess.h"


#define abs(a) (((a)>0)?(a):(0-(a)))


extern OSDCTRL_OBJ * pCtrlObj;

void moveUpXposition()
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	int i;
	SDK_ASSERT(pCtrlObj!=NULL);
	
	//if(pCtrlObj->uInit==0)
	//	return;

	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+3)%4)*35;
	//pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+6)%7)*35;
	return;
}

void moveDownXPosition()
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;
	int i;
	SDK_ASSERT(pCtrlObj!=NULL);
	//if(pCtrlObj->uInit==0)
	//	return;
	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	//pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+1)%7)*35;
	pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+1)%4)*35;
}

int getXPosition()
{
	int i;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return -1;
	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	return i;
}

void initilWeatherParam()
{
	getWeatherParam();
	return ;
}

void getWeatherParam()
{
	int value;
	Temparature = gWeatherTable.Temparature;
	Pressure	= gWeatherTable.Pressure;
	Weather[0] = (Temparature>0)?1:0;
	value = abs(Temparature);
	Weather[1] = value/10;
	Weather[2] = value%10;
	Weather[3] = Pressure/1000000%10;
	Weather[4] = Pressure/100000%10;
	Weather[5] = Pressure/10000%10;
	Weather[6] = Pressure/1000%10;
	Weather[7] = Pressure/100%10;
	Weather[8] = Pressure/10%10;
	Weather[9] = Pressure%10;

	return ;
}


bool isXatSave()
{
	//OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return FALSE;
	
	return (pTextObj[eCalibMenu_Save].osdInitY == pTextObj[eCursorX].osdInitY);
}





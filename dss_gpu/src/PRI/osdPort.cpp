

#include "osdPort.h"
#include "osdProcess.h"

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








#include "statCtrl.h"
#include "osdProcess.h"


void processCMD_MODE_AIM_LAND(LPARAM lParam)
 {
 	Posd[eAimType] = AimOsd[0];
	return ;
 }


void processCMD_MODE_AIM_SKY(LPARAM lParam)
 {
 	Posd[eAimType] = AimOsd[1];
	return ;
 }





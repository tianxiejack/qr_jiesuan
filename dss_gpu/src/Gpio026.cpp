
#include "Gpio026.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gpio_rdwr.h"
#include "osa.h"


int hard_flag=0;
void Hard_Init()
{
	Hard_displayreset();
	Hard_Gpiocreate();
	hard_flag=1;
	
}


void Hard_displayreset()
{
	//gpio t02
	
	GPIO_create(152,GPIO_DIRECTION_OUT);
	
	GPIO_set(152,0);
	OSA_waitMsecs(100);
	GPIO_set(152,1);

	GPIO_close(152);	

}

void Hard_Gpiocreate()
{
	//ccd  i2
	GPIO_create(66,GPIO_DIRECTION_IN);
	//fir k2
	GPIO_create(86,GPIO_DIRECTION_IN);


}

int Hard_getccdstatus()
{
	//ccd
	if(hard_flag==0)
		return 1;
	
	return GPIO_get(66);
	
}

int Hard_getfirstatus()
{
	//fir
	if(hard_flag==0)
		return 1;
	return GPIO_get(86);
	
}
#ifndef _GPIO_RDWR_H_
#define _GPIO_RDWR_H_
//#include <ti_media_std.h>

#define GPIO_DIRECTION_IN   0
    /**< GPIO pin direction to in */
#define   GPIO_DIRECTION_OUT  1
    /**< GPIO pin direction to out */

int GPIO_create(unsigned int num, unsigned int mode);
int GPIO_close(unsigned int num);
int GPIO_ioctl(unsigned int num, unsigned int setValue);
int GPIO_get(unsigned int nPin);
int GPIO_set(unsigned int nPin, unsigned int nValue);

#endif

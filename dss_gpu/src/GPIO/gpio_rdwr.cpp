#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gpio_rdwr.h"
#include "osa.h"

#define BUFFER_SIZE 64
#define GPIO_NUMBER_MAX 256

#define GPIO_PIN_MAX		256
static int gDirection[GPIO_PIN_MAX] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

unsigned int GPIO_DIRECTION_TAB[GPIO_NUMBER_MAX + 1] =
        { 0 };

/* create mode: 0-in read 1-out write */
int GPIO_create(unsigned int num, unsigned int mode)
{
    int file, size;
    char buffer[BUFFER_SIZE];

    // Export control of the selected GPIO to userspace
    file = open("/sys/class/gpio/export", O_WRONLY);
    if (file < 0)
    {
        perror("gpio/export");
        return -1;
    }
    if (num <= 0 || num > GPIO_NUMBER_MAX)
    {
        printf("gpio num out of range\r\n");
        return -1;
    }
    size = snprintf(buffer, sizeof(buffer), "%d", num);
    write(file, buffer, size);
    close(file);
    // Set the GPIO direction
    size = snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%d/direction",
            num);
    file = open(buffer, O_WRONLY);
    if (file < 0)
    {
        perror("gpio/direction");
        return -1;
    }
    if (0 == mode)
    {
        size = snprintf(buffer, sizeof(buffer), "in");
    }
    else
    {
        size = snprintf(buffer, sizeof(buffer), "out");
    }

    write(file, buffer, size);

    close(file);

    GPIO_DIRECTION_TAB[num] = mode;

    return 0;
}

int GPIO_close(unsigned int num)
{
    int file, size;
    char buffer[BUFFER_SIZE];

    // Export control of the selected GPIO to userspace
    file = open("/sys/class/gpio/unexport", O_WRONLY);
    if (file < 0)
    {
        perror("gpio/unexport");
        return -1;
    }
    if (num <= 0 || num > GPIO_NUMBER_MAX)
    {
        printf("gpio num out of range\r\n");
        return -1;
    }
    size = snprintf(buffer, sizeof(buffer), "%d", num);
    write(file, buffer, size);
    close(file);
    GPIO_DIRECTION_TAB[num] = 0;
    return 0;
}


int GPIO_set(UInt32 nPin, UInt32 nValue)
{
	int fd, nRet;
	char strSysfs[128];

	if(nPin >= GPIO_PIN_MAX)
	{
		printf("gpio nPin out of range\r\n");
		return -1;
	}

	nRet = snprintf(strSysfs, sizeof(strSysfs), "/sys/class/gpio/gpio%d/value", nPin);
	if(GPIO_DIRECTION_OUT == gDirection[nPin]){
		fd = open(strSysfs, O_WRONLY);
		if (fd < 0)
		{
			perror("gpio/value");
			return -1;
		}
		nRet = snprintf(strSysfs, sizeof(strSysfs), "%d", nValue);
		write(fd, strSysfs, nRet);
		close(fd);
		return 0;
	}
	return -1;
}

int GPIO_get(UInt32 nPin)
{
	int fd, nRet;
	char strSysfs[128];

	if(nPin >= GPIO_PIN_MAX)
	{
		printf("gpio nPin out of range\r\n");
		return -1;
	}

	nRet = snprintf(strSysfs, sizeof(strSysfs), "/sys/class/gpio/gpio%d/value", nPin);
	if(GPIO_DIRECTION_IN == gDirection[nPin]){
		fd = open(strSysfs, O_RDONLY);
		if (fd < 0)
		{
			perror("gpio/value");
			return -1;
		}
		nRet = read(fd, strSysfs, 128);
		close(fd);
		strSysfs[1] = '\0';
		nRet = atoi(strSysfs);
		return nRet;
	}
	return -1;
}
int GPIO_ioctl(unsigned int num, unsigned int setValue)
{
    int file, size;
    char buffer[BUFFER_SIZE];

    if (num <= 0 || num > GPIO_NUMBER_MAX)
    {
        printf("gpio num out of range\r\n");
        return -1;
    }

    size = snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%d/value",
            num);
    if (GPIO_DIRECTION_TAB[num] == 0)
    {
        // direction is in, read value
        file = open(buffer, O_RDONLY);
        if (file < 0)
        {
            perror("gpio/value");
            return -1;
        }

        // The GPIO toggles
        size = read(file, buffer, BUFFER_SIZE);
        close(file);
        buffer[1] = '\0';
        size = atoi(buffer);
        return size;
    }
    else
    {
        // direction is out, write value
        file = open(buffer, O_WRONLY);
        if (file < 0)
        {
            perror("gpio/value");
            return -1;
        }

        // The GPIO toggles
        size = snprintf(buffer, sizeof(buffer), "%d", setValue);

        write(file, buffer, size);
        close(file);
    }
    return 0;
}


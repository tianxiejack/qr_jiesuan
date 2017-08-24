#ifndef SERVO_CONTROL_OBJ_H
#define SERVO_CONTROL_OBJ_H
#include "If_servo_control.h"
#include "app_global.h"


#define CAN_CMD_SIZE_SHORT	6
#define CAN_CMD_SIZE_LONG    	10
#define CAN_CMD_SIZE_MINI		4

PIF_servo_control getMachGunServoContrlObj();
PIF_servo_control getGrenadeServoContrlObj();
void processCMD_MACHSERVO_MOVESPEED(LPARAM lParam);
void processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam);
void processCMD_MACHSERVO_STOP(LPARAM lParam);
void processCMD_MACHSERVO_MOVEOFFSET(LPARAM lParam);
void processCMD_GRENADESERVO_MOVEOFFSET(LPARAM lParam);

void initAllServos();

extern long cmd_machservo_moveoffset_tmp;
extern long cmd_grenadeservo_moveoffset_tmp;
#endif


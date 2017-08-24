#ifndef IF_SERVO_CONTROL_H
#define IF_SERVO_CONTROL_H

typedef void (* PFUNC_MOVE_OFFSET)(float xOffset,float yOffset);
typedef void (* PFUNC_MOVE_SPEED)(float xSpeed,float ySpeed);
typedef void (* PFUNC_STOP)();

typedef struct _IF_servo_control{
	PFUNC_MOVE_OFFSET moveOffset;
	PFUNC_MOVE_SPEED moveSpeed;
	PFUNC_STOP stop;
} IF_servo_control, *PIF_servo_control;

#endif


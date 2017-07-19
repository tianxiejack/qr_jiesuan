#if 0

#include <osa_thr.h>
#include "Tasklist.h"
#include <glut.h>

static OSA_ThrHndl GrenadePORTHndl;
static OSA_ThrHndl LaserPORTHndl;
static OSA_ThrHndl PositionPORTHndl;
static OSA_ThrHndl TurretPosPORTHndl;
static OSA_ThrHndl WeaponPORTHndl;
static OSA_ThrHndl MachGunPORTHndl;




int mainCtrl(void)
{
	taskCreate();
	glutMainLoop();

	taskDestroy();
	return 0;
}

int taskCreate(void)
{
	int status = -1;

#if 1
	#if 1
	status = OSA_thrCreate(
				&GrenadePORTHndl,
				GrenadePORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
	
	status = OSA_thrCreate(
				&LaserPORTHndl,
				LaserPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);

	status = OSA_thrCreate(
				&PositionPORTHndl,
				PositionPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);

	status = OSA_thrCreate(
				&TurretPosPORTHndl,
				TurretPosPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
	
	status = OSA_thrCreate(
				&MachGunPORTHndl,
				MachGunPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
	
	status = OSA_thrCreate(
				&WeaponPORTHndl,
				WeaponCtrlPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
#endif
}

int taskDestroy(void)
{

	int status;
	status = OSA_thrDelete(&GrenadePORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&LaserPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&PositionPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&TurretPosPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&WeaponPORTHndl);
	OSA_assert(status == OSA_SOK);

	//status = OSA_thrDelete(&MsgdrivHndl);
	//OSA_assert(status == OSA_SOK);
#endif
	return 0;
}

#endif
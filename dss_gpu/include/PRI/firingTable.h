#ifndef FIRING_TABLE_H
#define FIRING_TABLE_H

#include "permanentStorage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRENADE_KILL_DISTANCE_MAX		1791
#define GRENADE_GAS_DISTANCE_MAX			364
#define MACHINEGUN_DISTANCE_MAX			1500
enum {
		K_minus_One,
		K,
		K_plus_One,
		K_COUNT
	};

typedef struct _RowItems {
	double distance;		//\u5c04\u7a0b
	double elevationAngle;	//\u7784\u51c6\u89d2
	double duration;		//\u98de\u884c\u65f6\u95f4
	double bias;			//\u504f\u6d41\u89d2
}RowItems;

typedef struct _FiringTable {
	unsigned int count;
	const RowItems* table;
}FiringTable;

FiringTable* getFiringTable(PROJECTILE_TYPE type);

#ifdef __cplusplus
}
#endif
#endif


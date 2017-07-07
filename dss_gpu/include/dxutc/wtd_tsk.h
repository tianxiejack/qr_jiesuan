/**
*  
*
*  \brief .
*
*/

#ifndef _WTD_TSK_H_
#define _WTD_TSK_H_
enum {
	WTD_MSGID_CREATE = 0,
	WTD_MSGID_DELETE,
    WTD_MSGID_CLEAR,
	WTD_MSGID_MAX
};

int wtd_tsk_create(Uint16 timeout);
void wtd_tsk_destroy();
int wtd_tsk_clear();
#endif




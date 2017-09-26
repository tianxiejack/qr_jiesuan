#ifndef RECORD_LOG_H
#define RECORD_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include "osa.h"

enum record_id{
	record_log_roter,
	record_log_decode,
	record_log_bak2,
	record_log_mirror,
	record_log_vcode,
	record_log_bak1,
	record_log_test,
	record_log_hcode,
	record_log_can,
	record_log_can_send,
	record_log_max,
};

//初始化日记记录，返回串口id号。
int init_record_log(int com_id, char *filename);

//关闭日记记录
int uninit_record_log();

//发送数据
int record_log_send_data(int com_id, int size, unsigned char * buffer);

#endif

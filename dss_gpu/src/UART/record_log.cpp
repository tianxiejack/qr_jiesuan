
#include "record_log.h"


#define RECV_NUM_SIZE 	(100)
#define LOG_SIZE		(128)


typedef struct
{
    int size;  //接收到一组数据的大小
    unsigned char recv[LOG_SIZE]; //存储接收到的一组数据

}RECV_DATA;



typedef struct
{
    int com_id;     //串口id
    int start_record_log_flag;  //线程使能标志
    RECV_DATA recv_data[RECV_NUM_SIZE];    //设置可以存50组数据
    FILE *fp;       //存储路径
    pthread_t tid;  //线程id
    int print_num ; //计数打印次数
    int record_w_index; //计数要存的数据到第几组中
    int record_r_index; //计数读到第几组数据
    int record_w_full;  //标记数组写满了，重新开始写。

}RECORD_LOG;

static char open_filename[record_log_max][50];  //记录已经打开的文件
static RECORD_LOG record_log[record_log_max];   //设置可以初始化5个串口设备
static int index_record_log = 0;   //初始化到第几个串口的标记
static int start_pthread_flag = 0;	//打开线程的标志
static int close_pthread_flag = 1;



//线程处理
void* thread_record_log_data(void * prm)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int r_index = 0;
	int recv_size = 0;

    while(start_pthread_flag)
    {
    	for(i=0; i<index_record_log; i++)
    	{
    		//printf("hello world %d\n",index_record_log);
    		if(record_log[i].fp == NULL)
    		{
    			printf("record_log[%d].fp == NULL\n",i);
			continue;
		}
    			


			if(record_log[i].record_r_index >= RECV_NUM_SIZE)
			{
				record_log[i].record_r_index = 0;
				record_log[i].record_w_full  = 0;
				//printf(" world hello %d\n", i);
			}

			r_index = record_log[i].record_r_index;
			//to do : 数据来不及读走，输出丢失数据
			//if(record_log[i].record_r_index > record_log[i].record_w_index )

			if(record_log[i].record_w_full == 1 )
			{
				recv_size = sizeof(record_log[i].recv_data)/sizeof(RECV_DATA);
			}
			else
			{
				recv_size = record_log[i].record_w_index;
			}

			for(j=r_index; j<recv_size; j++)
			{
				 int recv_data_size = record_log[i].recv_data[j].size;
				 if(recv_data_size == 0)
					 continue;
				 fprintf(record_log[i].fp,"%05d: ", ++record_log[i].print_num);
				 for(k=0; k<recv_data_size; k++)
				 {
					 fprintf(record_log[i].fp, "0x%02x ", record_log[i].recv_data[j].recv[k]);
				 }
				 record_log[i].recv_data[j].size=0;
				 fputc('\n', record_log[i].fp);
				 record_log[i].record_r_index++;
				 fflush( record_log[i].fp );
			}


    	}
    	//usleep(5*1000);
    	OSA_waitMsecs(5);
    }

    close_pthread_flag = 1;
    return NULL;
}



//初始化日记记录，返回串口id号。
int init_record_log(int com_id, const char *filename)
{
    FILE * fp = NULL;
    time_t timep;
    int max_index = sizeof(record_log)/sizeof(RECORD_LOG);
    int pthread_rt = 0;

    timep = time(NULL);

    if(index_record_log >= max_index)
    {
        printf("Initialization failed, cannot more than %d !!!\n", max_index);
        return -1;
    }

    if(filename == NULL)
    {
    	printf("Initialization failed, filename illegal !!!\n");
    	return -1;
    }

    //打开文件
    fp = fopen(filename, "a+");

    if(fp == NULL)
	{
		printf("%s: %s\n",__func__, strerror(errno));
		return -1;
	}

    fprintf(fp, "\n//..................start record..................//\n");
    fprintf(fp, "record time: %s\n\n", ctime(&timep));
    fflush(fp);
    //初始化只创建一个线程
    if(index_record_log == 0)
    {
    	//开启线程
       	pthread_rt = pthread_create(&record_log[index_record_log].tid, NULL, thread_record_log_data, NULL);
        //开启线程失败，关闭文件
        if(pthread_rt != 0)
        {
        	printf("%s: %s\n",__func__, strerror(pthread_rt));
        	fclose(fp);
        	return -1;
        }
        start_pthread_flag = 1;
        close_pthread_flag = 0;
    }

    record_log[index_record_log].fp = fp;
	record_log[index_record_log].com_id = com_id;
	record_log[index_record_log].start_record_log_flag = 1;

    memcpy(&open_filename[index_record_log], filename, strlen(filename));
    index_record_log++;

    return 0;
}


//发送数据
int record_log_send_data(int com_id, int size, unsigned  char * buffer)
{
    //获取空id, 写入数据
    //发送信号
	int i=0;
	int j=0;
	//if(size<=0)
	//	return 0;
	
	for(i=0; i<index_record_log ; i++)
	{
		if(record_log[i].com_id == com_id)
		{
			if(record_log[i].fp == NULL)
				continue;

			int index = record_log[i].record_w_index++;

			if(record_log[i].record_w_index >= RECV_NUM_SIZE)
			{
				record_log[i].record_w_index = 0;
				record_log[i].record_w_full = 1;
				//printf("hello world %d\n",i);
			}


			if(size > sizeof(record_log[i].recv_data[1].recv)/sizeof(char) )
				size = sizeof(record_log[i].recv_data[1].recv);

			record_log[i].recv_data[index].size = size;

			for(j=0; j<size; j++)
			{
				record_log[i].recv_data[index].recv[j] = buffer[j];
				//if(record_log_can == com_id)
				    // printf("record_log_can buffer=%02x \n",buffer[j]);
			}


		}

	}

    return 0;
}

//关闭日记记录
int uninit_record_log()
{
     //关闭文件 停止线程
	 int i = 0 ;

     start_pthread_flag = 0;
     while(!close_pthread_flag)
     {
    	// usleep(40*1000);
    	 OSA_waitMsecs(40);
     }

     for(i=0; i<index_record_log; i++)
     {
    	 if(record_log[index_record_log].start_record_log_flag)
    	 {
    		 fclose(record_log[index_record_log].fp);
    		 record_log[index_record_log].fp = NULL;
    		 record_log[index_record_log].com_id = -1;
    		 record_log[index_record_log].start_record_log_flag = 0;
    	 }
     }
     return 0;
}




static int test_record_log()
{

    FILE *fp = NULL;
    int i=0,j=0;
    int recv_size      = 0;
    int recv_data_size = 0;
    int print_num = 0;
    RECV_DATA recv_data[50];
    time_t timep;

    init_record_log(0, "123.txt");
    init_record_log(1, "1234.txt");
    init_record_log(2, "12345.txt");

    unsigned char buffer[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    unsigned char buffer_2[20] = {0,11, 2, 3, 4, 5, 6, 7, 8, 9};

    int kkk=100;
    while(kkk--)
    {
    	 buffer[0] = kkk;
    	 buffer_2[0] = kkk+1;
    	 record_log_send_data( 0, 10, buffer);
    	 record_log_send_data( 1, 10, buffer_2);
    	// usleep(10*1000);
    }


 //   sleep(2);
    uninit_record_log();

    return 0;
}



// HwaFcSelfTest.cpp : 定义控制台应用程序的入口点。
//

#ifdef VXWORKS_OPS
//#include "stdafx.h"
#include "HwaFcDriver.h"
#include <vxWorks.h>
#include <taskLib.h>
#include "HwaFcStruct.h"

#include <semLib.h>
#include <intLib.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <vmLib.h>					
#include <time.h>

typedef int DWORD;
typedef int LPVOID;
#define WINAPI
#define gets_s gets
#define ResetEvent semGive
#define  HANDLE int 
#define CloseHandle taskDelete
// HwaFcDrvTest.cpp :
#else
#include "stdafx.h"
#include "windows.h"
#include "HwaFcDriver.h"
#include "fcntl.h"
#include "sys/types.h"
#include "io.h"
#endif

#define DebugMsg printf
#define compareB 0   
#define BUFNOR  0 
                    //0:normal 1:for fpga
///*********For:Windows Test create task = Vxworks******///
//!!!线程函数注意定义：DWORD WINAPI send_task_1(LPVOID pPara)
#ifdef WINDOWS_TEST
	#define	 FUNCPTR LPTHREAD_START_ROUTINE
	#define  taskSpawn(taskName,Prj,res,stack,pfunc,nCardNum,handle,var3,var4,var5,var6,var7,var8,var9,var10) (handle = CreateThread(NULL,stack,pfunc,(LPVOID)(nCardNum),0,NULL));
	#define  taskDelay(x) Sleep(x);
	#define  taskDelete(TaskID); CloseHandle(TaskID);

	#define semTake(hEvent,millis) WaitForSingleObject(hEvent,millis);  
	#define semGive(hEvent)	SetEvent(hEvent)//ResetEvent(hEvent);
	#define logMsg printf
	#define WAIT_FOREVER INFINITE

#endif
///****************************************************///

/*****  OutputInfo(char* taskName,int nNum,int recvLength,int compNum,int realRecv,int expectRecv)  *****/
#define OutputInfoErr(taskName,nNum,recvLength,compNum,realRecv,expectRecv)  printf("%s data compare Error! %d Packet recv %d bytes, Compare Num is %d, real recv is %d, expect is %d\n",taskName,nNum,recvLength,compNum,realRecv,expectRecv);

#define PrtBuf(taskName,num,buf) printf("%s  buf[%d]=%d, buf[%d]=%d, buf[%d]=%d, buf[%d]=%d\n",taskName,num-1,buf[num-1],num,buf[num],num+1,buf[num+1],num+2,buf[num+2]);

/////////////////////////////////////////////
HANDLE sendtask_handle[10] = {0};
	HANDLE sendtask_handle_1 = 0;
	HANDLE sendtask_handle_2 = 0;
	HANDLE sendtask_handle_3 = 0;
	HANDLE sendtask_handle_4 = 0;
	HANDLE sendtask_handle_5 = 0;
	HANDLE sendtask_handle_6 = 0;
	HANDLE sendtask_handle_7 = 0;
	HANDLE sendtask_handle_8 = 0;

	HANDLE recvtask_handle[10] = {0};
	HANDLE recvtask_handle_1 = 0;
	HANDLE recvtask_handle_2 = 0;
	HANDLE recvtask_handle_3 = 0;
	HANDLE recvtask_handle_4 = 0;
	HANDLE recvtask_handle_5 = 0;
	HANDLE recvtask_handle_6 = 0;
	HANDLE recvtask_handle_7 = 0;
	HANDLE recvtask_handle_8 = 0;

	
#ifdef WINDOWS_TEST 
	HANDLE task_SEMID_1 = NULL;
	HANDLE task_SEMID_2 = NULL;
	HANDLE task_SEMID_3 = NULL;
	HANDLE task_SEMID_4 = NULL;
	HANDLE task_SEMID_5 = NULL;
	HANDLE task_SEMID_6 = NULL;
	HANDLE task_SEMID_7 = NULL;
	HANDLE task_SEMID_8 = NULL;

	
#elif defined VXWORKS_TEST
	SEM_ID task_SEMID_1 = NULL;
	SEM_ID task_SEMID_2 = NULL;
	SEM_ID task_SEMID_3 = NULL;
	SEM_ID task_SEMID_4 = NULL;
	SEM_ID task_SEMID_5 = NULL;
	SEM_ID task_SEMID_6 = NULL;
	SEM_ID task_SEMID_7 = NULL;
	SEM_ID task_SEMID_8 = NULL;
#else
   #error "unknown operation system."
#endif

//////////////send buf////////////////////
unsigned char send_buf_1[2096] = {0};
unsigned char send_buf_2[2096] = {0};
unsigned char send_buf_3[2096] = {0};
unsigned char send_buf_4[2096] = {0};
unsigned char send_buf_5[2096] = {0};
#if 1
unsigned char send_buf_6[20960] = {0};
unsigned char send_buf_7[20960] = {0};
unsigned char send_buf_8[20960] = {0};
#else
unsigned char* send_buf_6;
unsigned char* send_buf_7;
unsigned char* send_buf_8;
#endif

//////////////recv buf////////////////////
unsigned char recv_buf_1[2096] = {0};
unsigned char recv_buf_2[2096] = {0};
unsigned char recv_buf_3[2096] = {0};
unsigned char recv_buf_4[2096] = {0};
unsigned char recv_buf_5[2096] = {0};
#if 1
unsigned char recv_buf_6[20960] = {0};
unsigned char recv_buf_7[20960] = {0};
unsigned char recv_buf_8[20960] = {0};
#else
unsigned char* recv_buf_6;
unsigned char* recv_buf_7;
unsigned char* recv_buf_8;
#endif 

//////////////port list/////////////////
typedef struct
{
	THwaFcPortCfg nHwaPortpara;
	THwaFcPortHandle nPortHandle;
}nHwaPortList;

nHwaPortList nHwaPortSend[] = 
{
	/*	 portid         porttype                 dir         sid        did      oxid       portsel     msgtype   msgid        seqid   frgseq   fragoff       pri  maxlen */  	
	{{0,    eTypeNormal,	eDirSend,	1, 		1, 	   1,   ePortAB, 	0x49,     1,       1,     1,     0,    0,   2096},0},
	{{1,    eTypeNormal,	eDirSend,	1, 		1, 	   1,   ePortAB, 	0x49,     2,       1,     1,     0,    0,   2096},0},
	{{2,    eTypeNormal,	eDirSend,	1, 		1, 	   1,   ePortAB, 	0x49,     3,       1,     1,     0,    0,   2096},0},
	{{3,    eTypeNormal,	eDirSend,	1, 		1, 	   1,   ePortAB, 	0x49,     4,       1,     1,     0,    0,   2096},0},
	{{4,    eTypeNormal,	eDirSend,	1, 		1, 	   1,   ePortAB, 	0x49,     5,       1,     1,     0,    0,   2096},0},
	{{5,    eTypeBlock, 	eDirSend,	6, 	    6,     6,	ePortAB, 	0x49,	  6,       1,     1,	 0,    0,   8192},0},
	{{6,    eTypeBlock, 	eDirSend,	7,      7,	   7,	ePortAB, 	0x49,	  7,	   1,	  1,	 0,    0,	8192},0},
	{{7,    eTypeBlock,	    eDirSend,	8,      8,     8,	ePortAB, 	0x49,	  8,       1,     1,	 0,    0,   8192},0},
};

nHwaPortList nHwaPortRecv[] = 
{
	/*	 portid 		porttype		      dir		 sid		did 	 oxid	portsel 	msgtype     msgid 	       seqid   frgseq	   fragoff 	  pri  maxlen */	
	{{0,    eTypeNormal,    eDirRecv,	1,		1,	   1,	ePortAB,	0x49,	  1,	   1,	  1,	 0,    0,	2096},0},
	{{1,    eTypeNormal,    eDirRecv,	1,		1,	   1,	ePortAB,	0x49,	  2,	   1,	  1,	 0,    0,	2096},0},
	{{2,    eTypeNormal,    eDirRecv,	1,		1,	   1,	ePortAB,	0x49,	  3,	   1,	  1,	 0,    0,	2096},0},
	{{3,    eTypeNormal,    eDirRecv,	1,		1,	   1,	ePortAB,	0x49,	  4,	   1,	  1,	 0,    0,	2096},0},
	{{4,    eTypeNormal,    eDirRecv,	1,		1,	   1,	ePortAB,	0x49,	  5,	   1,	  1,	 0,    0,	2096},0},
	{{5,    eTypeBlock,	    eDirRecv,	6,      6,     6,	ePortAB, 	0x49,	  6,       1,     1,	 0,    0,   8192},0},
	{{6,    eTypeBlock,	    eDirRecv,	7,      7,     7,	ePortAB, 	0x49,	  7,       1,     1,	 0,    0,   8192},0},
	{{7,    eTypeBlock, 	eDirRecv,	8,      8,     8,	ePortAB, 	0x49,	  8,       1,     1,	 0,    0,   8192},0},
};

#define Send_Port_Cnt (sizeof(nHwaPortSend)/sizeof(nHwaPortSend[0]))
#define Recv_Port_Cnt (sizeof(nHwaPortRecv)/sizeof(nHwaPortRecv[0]))


int HwaFcDownloadPortCfg(int nCardNum)
{
	int ret = 0;
	int i;
	for(i = 0;i< Recv_Port_Cnt;i++)
	{
		if((ret = HwaFcAddPort(nCardNum,&(nHwaPortRecv[i].nHwaPortpara),1)) != HWA_FC_OP_OK)
		{	
			DebugMsg("Add Recv port failed[%s]\n",HwaFcGetLastErr());
			HwaFcDelPort(nCardNum,nHwaPortRecv[i].nHwaPortpara.nPortId,eDirRecv);

		}

		if((ret = HwaFcOpenPort(nCardNum,nHwaPortRecv[i].nHwaPortpara.nPortId,&(nHwaPortRecv[i].nPortHandle),eDirRecv)) != HWA_FC_OP_OK )
		{
			DebugMsg("Open Send port %d failed[%s]\n",i,HwaFcGetLastErr());
			HwaFcClosePort(nCardNum,nHwaPortRecv[i].nPortHandle,eDirRecv);
		}	
	}

	for(i = 0; i< Send_Port_Cnt; i++)
	{

		if((ret = HwaFcAddPort(nCardNum,&(nHwaPortSend[i].nHwaPortpara),1)) != HWA_FC_OP_OK)
		{	
			DebugMsg("Add Send port failed[%s]\n",HwaFcGetLastErr());
			HwaFcDelPort(nCardNum,nHwaPortSend[i].nHwaPortpara.nPortId,eDirSend);
		}

		if((ret = HwaFcOpenPort(nCardNum,nHwaPortSend[i].nHwaPortpara.nPortId,&(nHwaPortSend[i].nPortHandle),eDirSend)) != HWA_FC_OP_OK )
		{
			DebugMsg("Open Send port %d failed[%s]\n",i,HwaFcGetLastErr());
			HwaFcClosePort(nCardNum,nHwaPortSend[i].nPortHandle,eDirSend);
		}
	}
	return ret;;
}

long long power(int x)
{
	int i;
	long long equal = 1;
	for (i = 0;i<x;i++)
	{
		equal = 2*equal;
	}
	return equal;
}

void InitTask()
{
	int i;
#ifdef WINDOWS_TEST 
	task_SEMID_1 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_1");
	task_SEMID_2 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_2");
	task_SEMID_3 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_3");
	task_SEMID_4 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_4");
	task_SEMID_5 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_5");
	task_SEMID_6 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_6");
	task_SEMID_7 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_7");
	task_SEMID_8 = CreateEvent(NULL,FALSE,TRUE,"task_SEMID_8");

#elif defined VXWORKS_TEST
#if 1
	task_SEMID_1 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_2 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_3 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_4 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_5 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_6 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_7 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	task_SEMID_8 = semBCreate(SEM_Q_PRIORITY,SEM_FULL);
#endif
#else
	#error "unknown operation system."
#endif

#if BUFNOR
	for(i = 0;i<2096;i++)
	{
		send_buf_1[i] = (i+1)&0xff; 
		send_buf_2[i] = (i+1)&0xff; 
		send_buf_3[i] = (i+1)&0xff; 
		send_buf_4[i] = (i+1)&0xff; 
		send_buf_5[i] = (i+1)&0xff; 
	}

	for(i = 0;i<20960;i++)
	{
		send_buf_6[i] = (i+1)&0xff; 
		send_buf_7[i] = (i+1)&0xff; 
		send_buf_8[i] = (i+1)&0xff; 
	}
#else


	for(i = 0;i<2096/8;i++)
	{

		send_buf_1[i*8+7] = i;
		send_buf_1[i*8+6] = i/power(8);
		send_buf_1[i*8+5] = i/power(16);
		send_buf_1[i*8+4] = i/power(24);
		send_buf_1[i*8+3] = i/power(32);
		send_buf_1[i*8+2] = i/power(32);
		send_buf_1[i*8+1] = i/power(32);
		send_buf_1[i*8+0] = i/power(32);

		send_buf_2[i*8+7] = i;
		send_buf_2[i*8+6] = i/power(8);
		send_buf_2[i*8+5] = i/power(16);
		send_buf_2[i*8+4] = i/power(24);
		send_buf_2[i*8+3] = i/power(32);
		send_buf_2[i*8+2] = i/power(32);
		send_buf_2[i*8+1] = i/power(32);
		send_buf_2[i*8+0] = i/power(32);

		send_buf_3[i*8+7] = i;
		send_buf_3[i*8+6] = i/power(8);
		send_buf_3[i*8+5] = i/power(16);
		send_buf_3[i*8+4] = i/power(24);
		send_buf_3[i*8+3] = i/power(32);
		send_buf_3[i*8+2] = i/power(32);
		send_buf_3[i*8+1] = i/power(32);
		send_buf_3[i*8+0] = i/power(32);

		send_buf_4[i*8+7] = i;
		send_buf_4[i*8+6] = i/power(8);
		send_buf_4[i*8+5] = i/power(16);
		send_buf_4[i*8+4] = i/power(24);
		send_buf_4[i*8+3] = i/power(32);
		send_buf_4[i*8+2] = i/power(32);
		send_buf_4[i*8+1] = i/power(32);
		send_buf_4[i*8+0] = i/power(32);

		send_buf_5[i*8+7] = i;
		send_buf_5[i*8+0] = i/power(8);
		send_buf_5[i*8+5] = i/power(16);
		send_buf_5[i*8+4] = i/power(24);
		send_buf_5[i*8+3] = i/power(32);
		send_buf_5[i*8+2] = i/power(32);
		send_buf_5[i*8+1] = i/power(32);
		send_buf_5[i*8+0] = i/power(32);

	}
	
	for(i = 0;i<20960/8;i++)
	{
	
		send_buf_6[i*8+7] = i;
		send_buf_6[i*8+6] = i/power(8);
		send_buf_6[i*8+5] = i/power(16);
		send_buf_6[i*8+4] = i/power(24);
		send_buf_6[i*8+3] = i/power(32);
		send_buf_6[i*8+2] = i/power(32);
		send_buf_6[i*8+1] = i/power(32);
		send_buf_6[i*8+0] = i/power(32);

		send_buf_7[i*8+7] = i;
		send_buf_7[i*8+6] = i/power(8);
		send_buf_7[i*8+5] = i/power(16);
		send_buf_7[i*8+4] = i/power(24);
		send_buf_7[i*8+3] = i/power(32);
		send_buf_7[i*8+2] = i/power(32);
		send_buf_7[i*8+1] = i/power(32);
		send_buf_7[i*8+0] = i/power(32);

		send_buf_8[i*8+7] = i;
		send_buf_8[i*8+6] = i/power(8);
		send_buf_8[i*8+5] = i/power(16);
		send_buf_8[i*8+4] = i/power(24);
		send_buf_8[i*8+3] = i/power(32);
		send_buf_8[i*8+2] = i/power(32);
		send_buf_8[i*8+1] = i/power(32);
		send_buf_8[i*8+0] = i/power(32);

	}
#endif
	return;
}



DWORD WINAPI send_task_1(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	unsigned int send_num_1 = 0;
	unsigned int send_length_1 = 0;
	
	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[0].nHwaPortpara.ePortPortSel;
	int i;
	while(1)
	//for (i=0;i<10000;i++)
	{

		semTake(task_SEMID_1,WAIT_FOREVER);

		//ResetEvent(task_SEMID_1);
		//send_length_1 = send_num_1%2097;
		send_length_1 = 1024;
		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[0].nPortHandle,(char*)send_buf_1,send_length_1,nCfg))< 0)
		{
			//while(1)
			{
				HwaFcGetLastErr();
				logMsg("send_task_1 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_1,send_length_1,ret,0,0,0);
				taskDelay(100);
				break;
			}
		}
		else
		{
			send_num_1++;
			//taskDelay(2);
			//getchar();
			//break;
			if(send_num_1%100 == 0)
			{
				logMsg("send_task_1:PortID %d send %d\n",nHwaPortSend[0].nHwaPortpara.nPortId,send_num_1,0,0,0,0);
				taskDelay(30);
			}
			
		}
	}
    return 0;
}

DWORD WINAPI send_task_2(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	unsigned int send_num_2 = 0;
	unsigned int send_length_2 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[1].nHwaPortpara.ePortPortSel;
	
	int i;
	while(1)
	//for (i=0;i<10000;i++)
	{

		semTake(task_SEMID_2,WAIT_FOREVER);
		ResetEvent(task_SEMID_2);
		send_length_2 = send_num_2%2097;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[1].nPortHandle,(char*)send_buf_2,send_length_2,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_2 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_2,send_length_2,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_2++;
			//taskDelay(2);
			if(send_num_2%10000 == 0)
			{
				logMsg("send_task_2:PortID %d send %d\n",nHwaPortSend[1].nHwaPortpara.nPortId,send_num_2,0,0,0,0);
				taskDelay(30);
			}
		}
	}


	return 0;
}

DWORD WINAPI send_task_3(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	unsigned int send_num_3 = 0;
	unsigned int send_length_3 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[2].nHwaPortpara.ePortPortSel;
	int i;
	while(1)
	//for (i=0;i<10000;i++)
	{

		semTake(task_SEMID_3,WAIT_FOREVER);
		ResetEvent(task_SEMID_3);
		send_length_3 = send_num_3%2097;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[2].nPortHandle,(char*)send_buf_3,send_length_3,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_3 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_3,send_length_3,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_3++;
			if(send_num_3%10000 == 0)
			{
				logMsg("send_task_3:PortID %d send %d\n",nHwaPortSend[2].nHwaPortpara.nPortId,send_num_3,0,0,0,0);
				taskDelay(30);
			}
		}
	}
	return 0;
}


DWORD WINAPI send_task_4(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	unsigned int send_num_4 = 0;
	unsigned int send_length_4 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[3].nHwaPortpara.ePortPortSel;

	while(1)
	{

		semTake(task_SEMID_4,WAIT_FOREVER);
		ResetEvent(task_SEMID_4);
		send_length_4 = send_num_4%2097;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[3].nPortHandle,(char*)send_buf_4,send_length_4,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_4 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_4,send_length_4,ret);
				taskDelay(90);
			}
		}
		else
		{
			send_num_4++;
			if(send_num_4%10000 == 0)
			{
				logMsg("send_task_4:PortID %d send %d\n",nHwaPortSend[3].nHwaPortpara.nPortId,send_num_4,0,0,0,0);
				taskDelay(30);
			}
		}
	}
	return 0;
}

DWORD WINAPI send_task_5(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	unsigned int send_num_5 = 0;
	unsigned int send_length_5 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[4].nHwaPortpara.ePortPortSel;

	while(1)
	{

		semTake(task_SEMID_5,WAIT_FOREVER);
		ResetEvent(task_SEMID_5);
		send_length_5 = send_num_5%2097;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[4].nPortHandle,(char*)send_buf_5,send_length_5,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_5 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_5,send_length_5,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_5++;
			if(send_num_5%10000 == 0)
			{
				logMsg("send_task_5:PortID %d send %d\n",nHwaPortSend[4].nHwaPortpara.nPortId,send_num_5,0,0,0,0);
				taskDelay(30);
			}
		}
	}
	return 0;
}


int send_task_6(int nCardNum)
{
	int ret = 0;
	unsigned int send_num_6 = 0;
	unsigned int send_length_6 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[5].nHwaPortpara.ePortPortSel;

	while(1)
	{

		semTake(task_SEMID_6,WAIT_FOREVER);
		send_length_6 = send_num_6%20961;
		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[5].nPortHandle,(char*)send_buf_6,send_length_6,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_6 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_6,send_length_6,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_6++;
			//if(send_num_6 == 2097)
				//printf("ret = %d\n",ret);
			if(send_num_6%10000 == 0)
			{
				//taskDelay(5);
				logMsg("send_task_6:PortID %d send %d\n",nHwaPortSend[5].nHwaPortpara.nPortId,send_num_6,0,0,0,0);
				taskDelay(40);

			}
		}
	}
}

int send_task_7(int nCardNum)
{
	int ret = 0;
	unsigned int send_num_7 = 0;
	unsigned int send_length_7 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[6].nHwaPortpara.ePortPortSel;

	while(1)
	{

		semTake(task_SEMID_7,WAIT_FOREVER);
		send_length_7 = send_num_7%20961;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[6].nPortHandle,(char*)send_buf_7,send_length_7,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_7 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_7,send_length_7,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_7++;
			if(send_num_7%10000 == 0)
			{
				//taskDelay(5);
				logMsg("send_task_7:PortID %d send %d\n",nHwaPortSend[6].nHwaPortpara.nPortId,send_num_7,0,0,0,0);
				taskDelay(40);

			}
		}		
	}
}

int send_task_8(int nCardNum)
{
	int ret = 0;
	unsigned int send_num_8 = 0;
	unsigned int send_length_8 = 0;

	THwaSendCfg nCfg;
	nCfg.nIsAsm = 0;
	nCfg.nPktPri = 1;
	nCfg.nChSel = nHwaPortSend[7].nHwaPortpara.ePortPortSel;

	while(1)
	{

		semTake(task_SEMID_8,WAIT_FOREVER);
		send_length_8 = send_num_8%20961;

		if((ret = HwaFcSendData(nCardNum,nHwaPortSend[7].nPortHandle,(char*)send_buf_8,send_length_8,nCfg))< 0)
		{
			while(1)
			{
				HwaFcGetLastErr();
				printf("send_task_8 Error.Packet %d Want to send length is %d,real Error is %d\n",send_num_8,send_length_8,ret);
				taskDelay(100);
			}
		}
		else
		{
			send_num_8++;
			if(send_num_8%10000 == 0)
			{
				logMsg("send_task_8:PortID %d send %d\n",nHwaPortSend[7].nHwaPortpara.nPortId,send_num_8,0,0,0,0);
				taskDelay(40);
			}
		}
	}
}


DWORD WINAPI recv_task_1(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_1 = 0;
	unsigned int recv_length_1 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{


		recv_length_1 = 1024;//recv_num_1%2097;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[0].nPortHandle,(char*)recv_buf_1,&nInfo,2096,WAIT_FOREVER))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_1 Error.Packet %d real Error is %d\n",recv_num_1,ret);
			while(1)
				taskDelay(100);
		}
		//else if(ret>0)
		else if(ret == recv_length_1)
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_1; CompNum++)
			{
				if(recv_buf_1[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_1",recv_num_1,ret,CompNum,recv_buf_1[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_1",CompNum,recv_buf_1);
					PrtBuf("Send_task_1",CompNum,send_buf_1);
					printf("\a");
					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_1++;
			if(recv_num_1%100 == 0)
			{
				logMsg("recv_task_1:PortID %d recv %d\n",nHwaPortRecv[0].nHwaPortpara.nPortId,recv_num_1,0,0,0,0);
				taskDelay(10);
			}			
			semGive(task_SEMID_1);
						
		}
		else
		{
			printf("frame num %d err:Recv_task_1 Error.Ret = %d, recv_length_1 = %d.Not Equal\n",recv_num_1,ret,recv_length_1);
			printf("\a");

			while(1)
			taskDelay(100);
		}
	}

	return 0;

}


DWORD WINAPI recv_task_2(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_2 = 0;
	unsigned int recv_length_2 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_2 = recv_num_2%2097;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[1].nPortHandle,(char*)recv_buf_2,&nInfo,2096,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_2 Error.Packet %d real Error is %d\n",recv_num_2,ret);
			printf("\a");

			while(1)
				taskDelay(100);
		}
		//else if(ret>=0)// == recv_length_2)
		else if(ret == recv_length_2)
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_2; CompNum++)
			{
				if(recv_buf_2[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_2",recv_num_2,ret,CompNum,recv_buf_2[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_2",CompNum,recv_buf_2);
					PrtBuf("Send_task_2",CompNum,send_buf_2);
					printf("\a");
					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_2++;
			if(recv_num_2%10000 == 0)
			{
				logMsg("recv_task_2:PortID %d recv %d\n",nHwaPortRecv[1].nHwaPortpara.nPortId,recv_num_2,0,0,0,0);
				taskDelay(10);
			}			
			semGive(task_SEMID_2);
		}
		else
		{
			printf("Recv_task_2 Error.Ret = %d, recv_length_2 = %d.Not Equal\n",ret,recv_length_2);
			printf("\a");

			while(1)
				taskDelay(100);
		}
	}

	return 0;

}

DWORD WINAPI recv_task_3(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_3 = 0;
	unsigned int recv_length_3 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_3 = recv_num_3%2097;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[2].nPortHandle,(char*)recv_buf_3,&nInfo,2096,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_3 Error.Packet %d real Error is %d\n",recv_num_3,ret);
			while(1)
				taskDelay(100);
		}
		//else if(ret >=0)//== recv_length_3)
		else if(ret == recv_length_3)

		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_3; CompNum++)
			{
				if(recv_buf_3[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_3",recv_num_3,ret,CompNum,recv_buf_3[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_3",CompNum,recv_buf_3);
					PrtBuf("Send_task_3",CompNum,send_buf_3);
					printf("\a");

					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_3++;
			if(recv_num_3%10000 == 0)
			{
				logMsg("recv_task_3:PortID %d recv %d\n",nHwaPortRecv[2].nHwaPortpara.nPortId,recv_num_3,0,0,0,0);
				taskDelay(10);
			}			
			semGive(task_SEMID_3);
		}
		else
		{
			printf("Recv_task_3 Error.Ret = %d, recv_length_3 = %d.Not Equal\n",ret,recv_length_3);
			printf("\a");

			while(1)
				taskDelay(100);
		}
	}

	return 0;

}


DWORD WINAPI recv_task_4(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_4 = 0;
	unsigned int recv_length_4 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_4 = recv_num_4%2097;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[3].nPortHandle,(char*)recv_buf_4,&nInfo,2096,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_4 Error.Packet %d real Error is %d\n",recv_num_4,ret);
			while(1)
				taskDelay(100);
		}
		//else if(ret >=0)//== recv_length_4)
		else if(ret == recv_length_4)
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_4; CompNum++)
			{
				if(recv_buf_4[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_4",recv_num_4,ret,CompNum,recv_buf_4[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_4",CompNum,recv_buf_4);
					PrtBuf("Send_task_4",CompNum,send_buf_4);
					printf("\a");

					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_4++;
			if(recv_num_4%10000 == 0)
			{
				logMsg("recv_task_4:PortID %d recv %d\n",nHwaPortRecv[3].nHwaPortpara.nPortId,recv_num_4,0,0,0,0);
				taskDelay(10);
			}			
			semGive(task_SEMID_4);
		}
		else
		{
			printf("Recv_task_4 Error.Ret = %d, recv_length_4 = %d.Not Equal\n",ret,recv_length_4);
			printf("\a");

			while(1)
				taskDelay(100);
		}
	}

	return 0;

}



DWORD WINAPI recv_task_5(LPVOID pPara)
{	
	int nCardNum = (int)pPara;
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_5 = 0;
	unsigned int recv_length_5 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_5 = recv_num_5%2097;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[4].nPortHandle,(char*)recv_buf_5,&nInfo,2096,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_5 Error.Packet %d real Error is %d\n",recv_num_5,ret);
			while(1)
				taskDelay(100);
		}
		//else if(ret >=0)//== recv_length_5)
		else if(ret == recv_length_5)
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_5; CompNum++)
			{
				if(recv_buf_5[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_5",recv_num_5,ret,CompNum,recv_buf_5[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_5",CompNum,recv_buf_5);
					PrtBuf("Send_task_5",CompNum,send_buf_5);
					printf("\a");

					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_5++;
			if(recv_num_5%10000 == 0)
			{
				logMsg("recv_task_5:PortID %d recv %d\n",nHwaPortRecv[4].nHwaPortpara.nPortId,recv_num_5,0,0,0,0);
				taskDelay(10);
			}			
			semGive(task_SEMID_5);
		}
		else
		{
			printf("Recv_task_5 Error.Ret = %d, recv_length_5 = %d.Not Equal\n",ret,recv_length_5);
			printf("\a");

			while(1)
				taskDelay(100);
		}
	}

	return 0;

}


int recv_task_6(int nCardNum)
{
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_6 = 0;
	unsigned int recv_length_6 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_6 = recv_num_6%20961;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[5].nPortHandle,(char*)recv_buf_6,&nInfo,30000,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_6 Error.Packet %d real Error is %d\n",recv_num_6,ret);
			printf("\a");

			while(1)
				taskDelay(100);
		}
		else
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_6; CompNum++)
			{
				if(recv_buf_6[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_6",recv_num_6,ret,CompNum,recv_buf_6[CompNum],(CompNum+1)&0xff);			
					PrtBuf("Recv_task_6",CompNum,recv_buf_6);
					PrtBuf("Send_task_6",CompNum,send_buf_6);
					printf("\a");
                     break;
					while(1)
						taskDelay(1000);
				}
			}		
#endif
			recv_num_6++;
			if(recv_num_6%10000 == 0)
			{
				logMsg("recv_task_6:PortID %d send %d\n",nHwaPortRecv[5].nHwaPortpara.nPortId,recv_num_6,0,0,0,0);
				taskDelay(50);
			}
			semGive(task_SEMID_6);
		}
	}
}

int recv_task_7(int nCardNum)
{
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_7 = 0;
	unsigned int recv_length_7 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_7 = recv_num_7%20961;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[6].nPortHandle,(char*)recv_buf_7,&nInfo,30000,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_7 Error.Packet %d real Error is %d\n",recv_num_7,ret);
			printf("\a");

			while(1)
				taskDelay(100);
		}
		else
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_7; CompNum++)
			{
				if(recv_buf_7[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_7",recv_num_7,ret,CompNum,recv_buf_7[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_7",CompNum,recv_buf_7);
					PrtBuf("Send_task_7",CompNum,send_buf_7);
					printf("\a");
					break;

					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_7++;
			if(recv_num_7%10000 == 0)
			{
				logMsg("recv_task_7:PortID %d send %d\n",nHwaPortRecv[6].nHwaPortpara.nPortId,recv_num_7,0,0,0,0);
				taskDelay(50);
			}
			semGive(task_SEMID_7);
		}
		
	}
}

int recv_task_8(int nCardNum)
{
	int ret = 0;
	int CompNum = 0;
	unsigned int recv_num_8 = 0;
	unsigned int recv_length_8 = 0;
	THwaFcRecvInfo nInfo;

	while(1)
	{

		recv_length_8 = recv_num_8%20961;
		if((ret = HwaFcRecvData(nCardNum,nHwaPortRecv[7].nPortHandle,(char*)recv_buf_8,&nInfo,30000,-1))<0)
		{
			if(ret == HWA_FC_READ_MSG_TIMEDOUT)
			{
				continue;
			}
			printf("Recv_task_8 Error.Packet %d real Error is %d\n",recv_num_8,ret);
			printf("\a");

			while(1)
				taskDelay(100);
		}
		else
		{		   
#if compareB
			for(CompNum = 0; CompNum < recv_length_8; CompNum++)
			{
				if(recv_buf_8[CompNum] != ((CompNum+1)&0xff))
				{
					OutputInfoErr("Recv_task_8",recv_num_8,ret,CompNum,recv_buf_8[CompNum],(CompNum+1)&0xff);
					PrtBuf("Recv_task_8",CompNum,recv_buf_8);
					PrtBuf("Send_task_8",CompNum,send_buf_8);
					printf("\a");
					break;

					while(1)
						taskDelay(100);
				}
			}		
#endif
			recv_num_8++;
			if(recv_num_8%10000 == 0)
			{
				logMsg("recv_task_8:PortID %d send %d\n",nHwaPortRecv[7].nHwaPortpara.nPortId,recv_num_8,0,0,0,0);
				taskDelay(60);
			}
			semGive(task_SEMID_8);
		}
	}
}



int CreateTask(int nCardNum)
{
	char buf_task[10]={0};
	int buf_num = 0;
	printf("There are 8 test task,1-5 is normal,6-8 is block.\nSelect Create task\n");
	printf("(Example:12345678)\n");
	printf("Please enter your choice:");
	buf_task[0]='1';
	//gets(buf_task);
	while((buf_num < 8)&&(buf_task[buf_num])!= NULL)
	{
		switch(buf_task[buf_num])
		{
		case NULL:
			break;
		case'1':
			
			taskSpawn("send_task_1",110,0,1024*100,(FUNCPTR)send_task_1,nCardNum,sendtask_handle[0],3,4,5,6,7,8,9,10);
			taskSpawn("recv_task_1",110,0,1024*100,(FUNCPTR)recv_task_1,nCardNum,recvtask_handle[0],3,4,5,6,7,8,9,10);

		//	while(1){
			//	send_task_1(LPVOID(nCardNum));
		//	}
			
			printf("Create test task 1.\n");
			break;
		case'2':
			taskSpawn("recv_task_2",110,0,16384*1,(FUNCPTR)recv_task_2,nCardNum,recvtask_handle[1],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_2",110,0,16384*1,(FUNCPTR)send_task_2,nCardNum,sendtask_handle[1],3,4,5,6,7,8,9,10);
			printf("Create test task 2.\n");
			break;
		case'3':
			taskSpawn("recv_task_3",110,0,16384*1,(FUNCPTR)recv_task_3,nCardNum,sendtask_handle[2],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_3",110,0,16384*1,(FUNCPTR)send_task_3,nCardNum,recvtask_handle[2],3,4,5,6,7,8,9,10);
			printf("Create test task 3.\n");
			break;
	
		case'4':
			taskSpawn("recv_task_4",110,0,16384*1,(FUNCPTR)recv_task_4,nCardNum,sendtask_handle[3],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_4",110,0,16384*1,(FUNCPTR)send_task_4,nCardNum,recvtask_handle[3],3,4,5,6,7,8,9,10);
			printf("Create test task 4.\n");
			break;
		case'5':				
			taskSpawn("recv_task_5",110,0,16384*1,(FUNCPTR)recv_task_5,nCardNum,sendtask_handle[4],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_5",110,0,16384*1,(FUNCPTR)send_task_5,nCardNum,recvtask_handle[4],3,4,5,6,7,8,9,10);
			printf("Create test task 5.\n");
			break;
	#if 1
		case'6':
			taskSpawn("recv_task_6",110,0,16384*3,(FUNCPTR)recv_task_6,nCardNum,sendtask_handle[5],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_6",110,0,16384*3,(FUNCPTR)send_task_6,nCardNum,recvtask_handle[5],3,4,5,6,7,8,9,10);
			printf("Create test task 6.\n");
			break;
		case'7':
			taskSpawn("recv_task_7",110,0,16384*3,(FUNCPTR)recv_task_7,nCardNum,sendtask_handle[6],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_7",110,0,16384*3,(FUNCPTR)send_task_7,nCardNum,recvtask_handle[6],3,4,5,6,7,8,9,10);
			printf("Create test task 7.\n");
			break;
		case'8':
			taskSpawn("recv_task_8",110,0,16384*3,(FUNCPTR)recv_task_8,nCardNum,sendtask_handle[7],3,4,5,6,7,8,9,10);
			taskSpawn("send_task_8",110,0,16384*3,(FUNCPTR)send_task_8,nCardNum,recvtask_handle[7],3,4,5,6,7,8,9,10);
			printf("Create test task 8.\n");
			break;
#endif
		default:
			printf("buf_task[%d] is %c, enter error!\n",buf_num,buf_task[buf_num]);
			break;
		}
		if(buf_task[buf_num]>'8'||buf_task[buf_num]<'1')
		{		
			if(buf_task[buf_num] != NULL)
			{
				printf("Please enter your choice again:");
				memset(buf_task,0,sizeof(buf_task));
				gets(buf_task);
				buf_num = 0;
			}
		}
		else
			buf_num++;
		taskDelay(30);
	}

	printf("success!\n");
	return 0;
}
#ifdef WINDOWS_TEST
	typedef struct{
		int buf_task_have;
		HANDLE send_task_id[10];
		HANDLE recv_task_id[10];
	}tTaskCtrl;

#else
	typedef struct{
		int buf_task_have;
		int send_task_id[10];
		int recv_task_id[10];
	}tTaskCtrl;
#endif

int DeleteTask(int nCardNum)
{
	char buf_task[10]={0};
	int nbuf_task = 0;
	char sbuf_task;
	int buf_num = 0;
	char buf_name[50] = {0};
	tTaskCtrl CrtlTask;
	memset(&CrtlTask,0,sizeof(CrtlTask));
	int ret;
#ifdef WINDOWS_TEST
	int i;
	for (i = 0; i< 10;i++)
	{
		CrtlTask.send_task_id[i] = sendtask_handle[i];
		if (CrtlTask.send_task_id[i] != 0)
		{
			CrtlTask.buf_task_have++;
		}
		CrtlTask.recv_task_id[i] = recvtask_handle[i];

		if (CrtlTask.recv_task_id[i] != 0)
		{
			CrtlTask.buf_task_have++;
		}

	}

#else
	for(buf_num = 0;buf_num < 8; buf_num++)
	{
		memset(buf_name,0,sizeof(buf_name));
		sprintf(buf_name,"send_task_%d",buf_num+1);
		if((CrtlTask.send_task_id[buf_num] = taskNameToId(buf_name))!=ERROR)
		{
			CrtlTask.buf_task_have++;
		}

		memset(buf_name,0,sizeof(buf_name));
		sprintf(buf_name,"recv_task_%d",buf_num+1);
		if((CrtlTask.recv_task_id[buf_num] = taskNameToId(buf_name))!=ERROR)
		{
			CrtlTask.buf_task_have++;
		}
	}
#endif

	if(CrtlTask.buf_task_have != 0)
	{

		printf("running task total:%d\n",CrtlTask.buf_task_have);
		for(buf_num = 0;buf_num < 8;buf_num++)
		{
			if(CrtlTask.send_task_id[buf_num] != ERROR )
			{
				printf("\nrunning send task:");
				printf("%d ,CrtlTask.send_task_id[buf_num] = %d",buf_num+1,CrtlTask.send_task_id[buf_num]);
			}
			if(CrtlTask.recv_task_id[buf_num] != ERROR )
			{
				printf("\nrunning recv task:");
				printf("%d ,CrtlTask.recv_task_id[buf_num] = %d",buf_num+1,CrtlTask.recv_task_id[buf_num]);
			}
		}
		printf("\nWhich test you want to stop");
		printf("(Example:12345678)\n");
		printf("Please enter your choice:");
		gets(buf_task);
		buf_num = 0;

		while((buf_num < 8)&&buf_task[buf_num]!= 0)
		{
			sbuf_task = buf_task[buf_num];
			nbuf_task = atoi(&sbuf_task);
			printf("nbuf_task = %d\n",nbuf_task);
			if(CrtlTask.send_task_id[nbuf_task-1]!=0)
			{
				
				ret = CloseHandle(CrtlTask.send_task_id[nbuf_task-1]);
			//	taskDelete(CrtlTask.send_task_id[nbuf_task-1]);
				CrtlTask.buf_task_have--;
				printf("taskDelete send %d\n",nbuf_task);
			}

			if(CrtlTask.recv_task_id[nbuf_task-1]!=0)
			{
				taskDelete(CrtlTask.recv_task_id[nbuf_task-1]); 		
				CrtlTask.buf_task_have--;
				printf("taskDelete recv %d\n",nbuf_task);
			}

			if(buf_task[buf_num]>'8'||buf_task[buf_num]<'1')
			{ 	  
				if(buf_task[buf_num] != NULL)
				{
					printf("Please enter your choice again:");
					memset(buf_task,0,sizeof(buf_task));
					gets(buf_task);
					buf_num = 0;
				}
			}
			else
				buf_num++;
		}

	}

	printf("running task total:%d\n",CrtlTask.buf_task_have);
	printf("success!\n");
	return 0;


}






#if 1

void  HwaFcDumpMibInfo(void *pData,THwaFcMib eMibType,char nPort)
{
	if(eMibType == eMibRx)
	{
		THwaFcRXMib *pRx = (THwaFcRXMib *)pData;
		DebugMsg("---------------RX MIBS(%c)-----------------\n",nPort);
		DebugMsg("[0]pRx->nRXAllByte[0]   :   0x%08x(%u)\n",pRx->nRXAllByte0,pRx->nRXAllByte0);
		//	DebugMsg("[1]pRx->nRXAllByte[1]   :   0x%08x(%u)\n",pRx->nRxAllByte1,pRx->nRxAllByte1);
		DebugMsg("[2]pRx->nRXAllFrm       :   0x%08x(%u)\n",pRx->nRXAllFrm,pRx->nRXAllFrm);
		DebugMsg("[3]pRx->nRX0To127       :   0x%08x(%u)\n",pRx->nRX0To127,pRx->nRX0To127);
		DebugMsg("[4]pRx->nRX128To255     :   0x%08x(%u)\n",pRx->nRX128To255,pRx->nRX128To255);
		DebugMsg("[5]pRx->nRX256To511     :   0x%08x(%u)\n",pRx->nRX256To511,pRx->nRX256To511);
		DebugMsg("[6]pRx->nRX512To1023    :   0x%08x(%u)\n",pRx->nRX512To1023,pRx->nRX512To1023);
		DebugMsg("[7]pRx->nRX1024To2112   :   0x%08x(%u)\n",pRx->nRX1024To2112,pRx->nRX1024To2112);
		DebugMsg("[8]pRx->nRXShortError   :   0x%08x(%u)\n",pRx->nRXShortErr,pRx->nRXShortErr);
		DebugMsg("[9]pRx->nRXLongError    :   0x%08x(%u)\n",pRx->nRXLongErr,pRx->nRXLongErr);
		DebugMsg("[10]pRx->nRxCrcError    :   0x%08x(%u)\n",pRx->nRxCrcError,pRx->nRxCrcError);
		DebugMsg("[11]pRx->nRXByteFlux    :   0x%08x(%u)\n",pRx->nRXByteFlux,pRx->nRXByteFlux);
		DebugMsg("[12]pRx->nRXFrmFlux     :   0x%08x(%u)\n",pRx->nRXFrmFlux,pRx->nRXFrmFlux);
		DebugMsg("[13]pRx->nRxPortIdError :   0x%08x(%u)\n",pRx->nRxPortIdError,pRx->nRxPortIdError);
		DebugMsg("[14]pRx->nRxPortEnaError:   0x%08x(%u)\n",pRx->nRxPortEnableError,pRx->nRxPortEnableError);
		DebugMsg("[15]pRx->nRxOffsetError :   0x%08x(%u)\n",pRx->nRxOffsetError,pRx->nRxOffsetError);
		DebugMsg("[16]pRx->nRxSeqIDError  :   0x%08x(%u)\n",pRx->nRxSeqIDError,pRx->nRxSeqIDError);

	}else if(eMibType == eMibTx)
	{
		THwaFcTxMib *pTx = (THwaFcTxMib *)pData;
		DebugMsg("---------------TX MIBS(%c)-----------------\n",nPort);
		DebugMsg("[32]pTx->nTXAllByte[0] :   0x%08x(%u)\n",pTx->nTXAllByte0,pTx->nTXAllByte0);
		//			DebugMsg("[33]pTx->nTXAllByte[1] :   0x%08x(%u)\n",pTx->nTXAllByte1,pTx->nTXAllByte1);
		DebugMsg("[34]pTx->nTXAllFrm     :   0x%08x(%u)\n",pTx->nTXAllFrm,pTx->nTXAllFrm);
		DebugMsg("[35]pTx->nTx0To127     :   0x%08x(%u)\n",pTx->nTx0To127,pTx->nTx0To127);
		DebugMsg("[36]pTx->nTx128To255   :   0x%08x(%u)\n",pTx->nTx128To255,pTx->nTx128To255);
		DebugMsg("[37]pTx->nTx256To511   :   0x%08x(%u)\n",pTx->nTx256To511,pTx->nTx256To511);
		DebugMsg("[38]pTx->nTx512To1023  :   0x%08x(%u)\n",pTx->nTx512To1023,pTx->nTx512To1023);
		DebugMsg("[39]pTx->nTx1024To2112 :   0x%08x(%u)\n",pTx->nTx1024To2112,pTx->nTx1024To2112);
		DebugMsg("[40]pTx->nTxByteFlux   :   0x%08x(%u)\n",pTx->nTxByteFlux,pTx->nTxByteFlux);
		DebugMsg("[41]pTx->nTxFrmFlux    :   0x%08x(%u)\n",pTx->nTxFrmFlux,pTx->nTxFrmFlux);
	}else if(eMibType == eMibSys)
	{
		THwaFcSysMib *pSys = (THwaFcSysMib *)pData;
		DebugMsg("--------------SYS MIBS-----------------\n");
		DebugMsg("pSys->nRXCrcErr   :   0x%08x(%d)\n",pSys->nRXCrcErr,pSys->nRXCrcErr);
		DebugMsg("pSys->nRxDrop     :   0x%08x(%d)\n",pSys->nRxDrop,pSys->nRxDrop);
		DebugMsg("pSys->nRxerr      :   0x%08x(%d)\n",pSys->nRxerr,pSys->nRxerr);
		DebugMsg("pSys->nRXMaxErr   :   0x%08x(%d)\n",pSys->nRXMaxErr,pSys->nRXMaxErr);
		DebugMsg("pSys->nRXMinErr   :   0x%08x(%d)\n",pSys->nRXMinErr,pSys->nRXMinErr);
	}
}

int GetContinue()
{
	char buf[10] = {0};
	printf("Press Enter to continue,Q to quit....\n");
	printf("Enter:");
	gets(buf);
	if((buf[0] == 'Q') || (buf[0] == 'q'))
	{
		return 0;
	}
	return 1;
}

int TestGetMibdebug(int nCardNum)
{
	THwaFcRXMib nRxMib[2]={0};
	THwaFcTxMib nTxMib[2]={0};
	THwaFcSysMib nSysMib={0};

	if(HwaFcGetRxMibs(nCardNum,&nRxMib[0],ePortA) != HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		return 0;
	}
	HwaFcDumpMibInfo(&nRxMib[0],eMibRx,'A');
	if(!GetContinue())
	{
		return 0;
	}
	if(HwaFcGetRxMibs(nCardNum,&nRxMib[1],ePortB) != HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		return 0;
	}
	HwaFcDumpMibInfo(&nRxMib[1],eMibRx,'B');
	if(!GetContinue())
	{
		return 0;
	}
	if(HwaFcGetTxMibs(nCardNum,&nTxMib[0],ePortA)!= HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		return 0;
	}
	HwaFcDumpMibInfo(&nTxMib[0],eMibTx,'A');
	if(!GetContinue())
	{
		return 0;
	}
	if(HwaFcGetTxMibs(nCardNum,&nTxMib[1],ePortB)!= HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		return 0;
	}
	HwaFcDumpMibInfo(&nTxMib[1],eMibTx,'B');
	if(!GetContinue())
	{
		return 0;
	}
	//printf("---------------Soft Counter-----------------\n");
	//printf("Total Receive %d\nTotal Send %d\n",recv_cnt,send_cnt);
	return 0;
}

int TestclearMibdebug(int nCardNum)
{
	//send_cnt= 0;
	//recv_cnt = 0;
	HwaFcClearMibs(nCardNum);
	printf("\nClear MIB Success.\n");
	return 0;
}


#endif


int HwaFcTestA(void)
{
	int nCardNum = 0;
	int i = 0;
	int nHardVer = 0;
	int nSoftVer = 0;
	char choice[5] = {0};

	THwaFcCardCfg nCfg;
	THwaFcPortStatus nPortStatus = {0};
	memset(&nCfg,0,sizeof(nCfg));

	DebugMsg("Test App Release:%s-%s\n",__DATE__,__TIME__);
	//pciConfigOutByte(4,13,0,0x3c,0x2);
	if((nCardNum = HwaFcInit()) < 0)
	{
		DebugMsg("%s\n",HwaFcGetLastErr());
		getchar();
		return 0;
	}

#if 1
	for(i = 0; i < nCardNum; i++)
	{
		ThwaFcDevInfo nDevInfo;
		if(HwaFcGetCardInfo(i,&nDevInfo) != HWA_FC_OP_OK)
		{
			DebugMsg("%s\n",HwaFcGetLastErr());
			return 0;
		}
		HwaFcDumpCardInfo(i,nDevInfo);
	}
#else
	nCardNum = 1;
	HwaFcDriverGetDeivceVersion(nCardNum,&nHardVer,&nSoftVer);
	printf("HardWare Ver: 0x%08x,  SoftWare Ver: 0x%08x\n",nHardVer,nSoftVer);
#endif

#if 1
	if(nCardNum > 0)
	{
		char buf[10];
		printf("Select Card:");
		//gets(buf);
		sprintf(buf,"%s","1");
		nCardNum = strtol(buf,NULL,0);
		//nCardNum = 1;
		printf("Selected Card %d\n",nCardNum);
	}

#endif
#if 1
	if(HwaFcOpenCard(nCardNum) < 0)
	{
		printf("[Err]%s\n",HwaFcGetLastErr());
		getchar();
		goto ErrOpen;
		return 0;
	}
	DebugMsg("Open Card success \n");

	/*get speed cfg*/
#if 0
	{
		char bufs[10];
		printf("Select Card Speed:\n");
		printf("1     : Select 1G\n");
		printf("other : Select 2G\n");
		printf("Enter your choice:");

		gets(bufs);

		if(bufs[0] == '1')
		{
			nCfg.eSpeed = eSpeed1G;
		}else{
			nCfg.eSpeed = eSpeed2G;
		}
	}
	nCfg.eWorkMode = eModeNormal;
#else
	nCfg.eSpeed = eSpeed2G;
	nCfg.eWorkMode = eModeNormal;
	nCfg.eIrigbMod = eModeMaster;
#endif
	if(HwaFcSetCardCfg(nCardNum,nCfg) != HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		getchar();		
		goto ErrNeedClose;	
	}

	if(HwaFcLoadCfg(nCardNum,"bootHost:FcCfg.xml") != HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		getchar();
		goto ErrNeedClose;
	}
#endif
	if(HwaFcGetPortStatus(nCardNum,&nPortStatus) != HWA_FC_OP_OK)
	{
		printf("%s\n",HwaFcGetLastErr());
		getchar();
		goto ErrNeedClose;
	}
	else
	{
		DebugMsg("PortLink Status:\nPortA : %s \nPortB : %s\nSpeed : %s\n",
			(nPortStatus.nLinkStatus & 0x1 ) ? "Link Up": "Link Down" ,
			(nPortStatus.nLinkStatus & 0x2 ) ? "Link Up": "Link Down", 
			nPortStatus.nFcSpeed == eSpeed1G ? "1G\n" : "2G\n");
	}
	if(HwaFcDownloadPortCfg(nCardNum) < 0)
		goto ErrNeedClose;
	else
		printf("Fc Card Phase Cfg Success!\n");

	taskDelay(10);
	InitTask();
	CreateTask(nCardNum);

	while(1)
	{
		memset(choice,0,sizeof(choice));
		gets(choice);
		printf("---input-------\n");
		
		if(choice[0] =='Q'||choice[0] == 'q')
			break;
		else if(choice[0] == 'T'||choice[0] == 't')
			TestGetMibdebug(nCardNum);
		else if(choice[0] == 'D'||choice[0] == 'd')
			DeleteTask(nCardNum);
		else if(choice[0] == 'R'||choice[0] == 'r')
			CreateTask(nCardNum);
	}
	
	return 0;
ErrNeedClose:
	HwaFcCloseCard(nCardNum);
ErrOpen:
	return 0;
}

DWORD WINAPI test_taskDelay(LPVOID pPara){

	int i = 0 ;

	while(i++ < 10){
		logMsg("-------%d-----------\n",i,0,0,0,0,0);
		taskDelay(30);
	}
}

int test_yzy(){
	HANDLE handle;
	taskSpawn("test_yzy",80,0,1024*100,(FUNCPTR)test_taskDelay,0,handle,3,4,5,6,7,8,9,10);
}

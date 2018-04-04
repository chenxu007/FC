//
//==============================================================================
//
// Title:       HwaFcMoniter.h.
// Purpose:     A short description of the interface.
//
// Created on:  2012-03-06 at 15:05:06 by lanxingdong.
// Copyright:   hwa. All Rights Reserved.
//
//==============================================================================
//

#ifndef _HWA_FC_MONITER_H_
#define _HWA_FC_MONITER_H_



#ifdef WIN32  /* For Windows */

#ifdef _FC_MONITER_EXPORT
#ifdef __cplusplus
#define _FC_MONITER_DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define _FC_MONITER_DLL_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __cplusplus
#define _FC_MONITER_DLL_EXPORT extern "C" __declspec(dllimport)
#else
#define _FC_MONITER_DLL_EXPORT __declspec(dllimport)
#endif
#endif

#else  /* For VxWorks */

#define _FC_MONITER_DLL_EXPORT

#endif

#define HWA_FC_MON_OK                          0

#define HWA_FC_MON_ERR_PARA						-2001
#define HWA_FC_MON_ERR_NOCARD					-2002
#define HWA_FC_MON_ERR_CARD						-2003
#define HWA_FC_MON_ERR_INTER					-2004
#define HWA_FC_MON_ERR_TIMEDOUT					-2005
#define HWA_FC_MON_ERR_DISK						-2006
#define HWA_FC_MON_ERR_SEND						-2007
#define HWA_FC_MON_ERR_ST						-2008
#define HWA_FC_MON_ERR_DINTER					-2009
#define HWA_FC_MON_ERR_UNKNOW					-2010

//YES?????
#define HWA_FC_MON_ERR_CREATE_THREAD            -2010

#if 0
#define HWA_FC_MON_DEVSTATUS					0x1001
#define HWA_FC_MON_START						166//0x1002
#define HWA_FC_MON_PAUSE						0x1004
#define HWA_FC_MON_RESUME						0x1005
#define HWA_FC_MON_STOP							0x1003
#endif

#define HWA_FC_MON_CREAT_PRJ					0x1006
#define HWA_FC_MON_DEL_PRJ						0x1007
#define HWA_FC_MON_COVER_PRJ					0x1008
#define HWA_FC_MON_DEV_ST						0x1101
#define HWA_FC_MON_REPORT_ST					0x1102
#define HWA_FC_MON_CHECK_STATUS                 0x1103


#if 0

MessageCommand_CMD_COLLECT_START,
MessageCommand_CMD_COLLECT_PAUSE,
MessageCommand_CMD_COLLECT_RESUME,
MessageCommand_CMD_COLLECT_STOP,
MessageCommand_CMD_COLLECT_QUERY_CARD,

MessageCommand_RET_COLLECT_START_OK,
MessageCommand_RET_COLLECT_START_ERROR,
MessageCommand_RET_COLLECT_CONNECT_ANALYSE_SYSTEM_ERROR,
MessageCommand_RET_COLLECT_PAUSE_OK,
MessageCommand_RET_COLLECT_PAUSE_ERROR,
MessageCommand_RET_COLLECT_RESUME_OK,
MessageCommand_RET_COLLECT_RESUME_ERROR,
MessageCommand_RET_COLLECT_STOP_OK,
MessageCommand_RET_COLLECT_STOP_ERROR,
MessageCommand_RET_COLLECT_QUERY_CARD,



typedef enum{
	HWA_FC_MON_START,
	HWA_FC_MON_PAUSE,
	HWA_FC_MON_RESUME,
	HWA_FC_MON_STOP,
	HWA_FC_MON_CARD_NUM,
	HWA_FC_MON_START_OK,
	HWA_FC_MON_START_ERROR,
	HWA_FC_MON_CONNECT_ANALYSE_SYSTEM_ERROR,
	HWA_FC_MON_PAUSE_OK,
	HWA_FC_MON_PAUSE_ERROR,
	HWA_FC_MON_STOP_OK,
	HWA_FC_MON_STOP_ERROR,
}THwaFcMonCommand;

#endif
   

/*
#define HWA_FC_MON_START   185
#define HWA_FC_MON_PAUSE   186
#define HWA_FC_MON_RESUME 187
#define HWA_FC_MON_STOP 188


#define HWA_FC_MON_CARD_NUM 189
#define HWA_FC_MON_START_OK 190
#define HWA_FC_MON_START_ERROR 191
#define HWA_FC_MON_CONNECT_ANALYSE_SYSTEM_ERROR 192
#define HWA_FC_MON_PAUSE_OK 193
#define HWA_FC_MON_PAUSE_ERROR 194
#define HWA_FC_MON_RESUME_OK 195
#define HWA_FC_MON_RESUME_ERROR 196
#define HWA_FC_MON_STOP_OK 197
#define HWA_FC_MON_STOP_ERROR 198
#define HWA_FC_MON_QUERY_CARD 199
#define HWA_FC_MON_DATA_IN_TIME 200
#define HWA_FC_MON_STATUS_IN_TIME 201
*/



typedef enum{
	HWA_FC_SEND_TEST_DATA = 94,
	HWA_FC_MON_START = 186,
	HWA_FC_MON_PAUSE,
	HWA_FC_MON_RESUME,
	HWA_FC_MON_STOP,
	HWA_FC_MON_CARD_NUM,
	HWA_FC_MON_START_OK,
	HWA_FC_MON_START_ERROR,
	HWA_FC_MON_CONNECT_ANALYSE_SYSTEM_ERROR,
	HWA_FC_MON_PAUSE_OK,
	HWA_FC_MON_PAUSE_ERROR,
	HWA_FC_MON_RESUME_OK,
	HWA_FC_MON_RESUME_ERROR,
	HWA_FC_MON_STOP_OK,
	HWA_FC_MON_STOP_ERROR,
	HWA_FC_MON_QUERY_CARD,
	HWA_FC_MON_DATA_IN_TIME,
	HWA_FC_MON_STATUS_IN_TIME,
	HWA_FC_MON_STATUS_IN_TIME_INDEX,
	HWA_FC_MON_QUIT,
	
}THwaFcMonCommand;

typedef unsigned int  THwaFcMonHandle;

/*port information  ip and port number*/
typedef struct{
	char sIpAddr[32];
	unsigned int nPort;
	BOOL nFlag;/*1:give port 
			   0:anto port	*/
}THwaMonInterInfo;



_FC_MONITER_DLL_EXPORT int HwaFcMonSysInit(THwaFcMonHandle *pHwaFcMonHandle, THwaMonInterInfo *nLocal,  THwaMonInterInfo *nRemote, char *pDataDir,int nTimedOut);



_FC_MONITER_DLL_EXPORT  int HwaFcMonitor(THwaFcMonHandle nHandle);




_FC_MONITER_DLL_EXPORT  int HwaFcMonHandleDeInit(THwaFcMonHandle nHandle);


// _FC_MONITER_DLL_EXPORT int HwaFcMonStartCap(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonStopCap(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonPauseCap(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonResumeCap(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// 
// 
// 
// _FC_MONITER_DLL_EXPORT DWORD WINAPI HwaFcMonThread(LPVOID  pPara);
// 
// _FC_MONITER_DLL_EXPORT DWORD WINAPI HwaFcMonStThread(LPVOID pPara);
// 
// 
// 
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonCreatePrj(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonDelPrj(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);
// 
// 
// _FC_MONITER_DLL_EXPORT int HwaFcMonCoverPrj(THwaFcMonHandle nHandle, void *pInputData, int nInPutLen, void *pOutData, int nOutLen);



#endif
#ifndef _HWA_FC_DRIVER_LIB_H_
#define _HWA_FC_DRIVER_LIB_H_
/************************************************************************
*  File: HwaFcDriverLib.h
*
*  Library for accessing HwaFcDriver devices.
*  The code accesses hardware using WinDriver's WDC library.
*  Code was generated by DriverWizard v10.20.
*
*  Jungo Confidential. Copyright (c) 2012 Jungo Ltd.  http://www.jungo.com
*************************************************************************/


#include "HwaFcStruct.h"
#include "HwaFcDriver.h"
#include "HwaFcReg.h"


/*************************************************************
  General definitions
 *************************************************************/
/* Default vendor and device IDs */
#define HWA_FC_VENDOR_ID 0x10EE /* Vendor ID */
#define HWA_FC_DEVICE_ID 0x7 /* Device ID */

/*************************************************************
  Function prototypes
 *************************************************************/
int HwaFcDriverInit(void);

int HwaFcDriverDeInit(void);

int HwaFcDriverOpenDevice(int nCardNum);

int HwaFcDriverCloseDevice( int nCardNum);

int HwaFcDriverInitDevStr(int nCardNum);

int HwaFcDriverDeInitDevStr(int nCardNum);

int HwaFcDriverEnableInt(int nCardNum);

int HwaFcDriverDisableInt(int nCardNum);

int HwaFcDriverCreateRecvPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle);

int HwaFcDriverFindPortByID(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle,THwaFcPortDir eDir);

int HwaFcDriverCreateSendPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle);

int HwaFcDriverDeleteSendPort(int nCardNum,THwaFcPortHandle nPortHandle);

int HwaFcDriverDeleteRecvPort(int nCardNum,THwaFcPortHandle nPortHandle);

THwaFcMsgHandle HwaFcDriverCreateMsg(LPSTR pMsgName);

THwaFcMsgHandle HwaFcDriverOpenMsg(LPSTR pMsgName);

int HwaFcDriverSendMsg(THwaFcMsgHandle nHandle,void *pBuf, int nBufLen);

int HwaFcDriverReadMsg(THwaFcMsgHandle nHandle,void *pBuf,int nBufLen,int *pReadLen,int time);

int HwaFcDriverSetSendCfg(int nCardNum,THwaFcPortHandle nPortHandle,unsigned int nSendLen,THwaSendCfg nCfg);

int HwaFcDriverSendData(int nCardNum,void *pBuf,int nSendLen);

int HwaFcDriverCheckOverFlow(int nCardNum,THwaFcPortHandle nPortHandle,THwaFcPortSel ePort);

int HwaFcDriverRecvData(int nCardNum,THwaFcPortHandle nPortHandle,void *pBuf,THwaFcRecvInfo *pTime,int nRecvLen,int nWaitTime);

int HwaFcDriverMonRecvData(int nCardNum,void *pBuf,int nRecvLen,int nWaitTime,int *pDataType);

int HwaFcDriverStartDma(THwaFcDevList *pDevCtx,unsigned int nSize,THwaFcPortDir eDir);

int HwaFcDriverGetCardNum(void);

int HwaFcDriverGetRecvLock(int nCardNum);

int HwaFcDriverReleaseRecvLock(int nCardNum);

int HwaFcDriverGetSendLock(int nCardNum);

int HwaFcDriverReleaseSendLock(int nCardNum);

int HwaFcDriverReadReg(int nCardNum,THwaBar eBar,unsigned int nAddr);

int HwaFcDriverWriteReg(int nCardNum,THwaBar eBar,unsigned int nAddr,unsigned int nValue);

int HwaFcDriverReadCfgReg(int nCardNum,unsigned int nAddr,void *pData,unsigned int nBytes);

int HwaFcDriverWriteCfgReg(int nCardNum,unsigned int nAddr,void *pData,unsigned int nByte);

const char *HwaFcDriverGetLastErr(void);

void HwaFcDriverDmaDone(THwaFcDevList *pDevCtx);

int HwaFcDriverGetCardInfo(int nCardNum,ThwaFcDevInfo *pDevInfo);

int HwaFcDriverGetDeivceVersion(int nCardNum);

int HwaFcDriverGetSerialNum(int nCardNum,char *pSerial,int nCnt);

int HwaFcDriverGetSpePortHandle(int nCardNum,THwaFcPortHandle *pRecv,THwaFcPortHandle *pSend);

#ifdef SUPPORT_XML
int HwaFcDriverPhraseXml(int nCardNum,char *pFileName);
#endif


int HwaFcDriverFillPort(THwaFcPort * pPort,THwaFcPortCfg nCfg);

int HwaFcDriverAddPort(int nCardNum,THwaFcPort nPort);

int HwaFcDriverDelPort(int nCardNum,unsigned int nPortId,THwaFcPortDir ePortDir);

int HwaFcDriverLoadCfg(int nCardNum);

int HwaFcDriverGetMibs(int nCardNum,THwaFcMib eMibs,THwaFcPortSel ePortType, void *pBuf);

int HwaFcDriverClearMib(int nCardNum);

int HwaFcDriverListenLinkStatus(int nCardNum,THwaFcPortStatus *pStatus);

#ifdef WINDOWS_OPS
DWORD WINAPI HwaFcDriverRecvFunc(LPVOID);

int HwaFcDriverOpenDma(int nCardNum);

void ErrLogWin(const char *sFormat, ...);

void HwaFcDriverDbg(char *sFormat,...);
int HwaFcDriverMonStartDma(THwaFcDevList *pDevCtx,unsigned int nSize);
#elif defined VXWORKS_OPS
	int HwaFcDriverRecvFunc(int nCardNum);
#else
	#error "unknown operation system"
#endif
void HwaFcDriverInitDbg();

int HwaFcDriverMonStopRecv(int nCardNum);

int HwaFcDriverMonStartRecv(int nCardNum);

int HwaFcDriverMonPauseRecv(int nCardNum);

int HwaFcDriverSetMonCfg(int nCardNum,THwaFcMonCfg nCfg);

int HwaFcDriverDelMonFilter(int nCardNum,unsigned int nFilterId);

int HwaFcDriverAddMonFilter(int nCardNum,THwaFcMonFilter nFilter);

int HwaFcDriverInitFifo(THwaFcSoftFifo *pFifo,unsigned int nFifoSize);

int HwaFcDriverDeInitFifo(THwaFcSoftFifo *pFifo);

int HwaFcDriverWriteFifo(THwaFcSoftFifo *pFifo,char *pBuf,unsigned int len);

int HwaFcDriverReadFifo(THwaFcSoftFifo *pFifo,char *pBuf,unsigned int nBufLen,unsigned int len);

int HwaFcDriverSetTime(int nCardNum);

int HwaFcDriverInitDone(int nCardNum);

int HwaFcDriverGetTime(int nCardNum,THwaFcTime *pTime);

int HwaFcDriverStartRecvThread(int nCardNum);

int HwaFcDriverEnableRecvIntr(int nCardNum);

int HwaFcDriverSetCardCfg(int nCardNum,THwaFcCardCfg nCfg);

int HwaFcDriverGetPortStatus(int nCardNum,THwaFcPortStatus *pPortStatus);

int HwaFcDriverGetSendOverFlow(int nCardNum,unsigned int nPortIndex);

int HwaFcDriverGetRecvOverFlow(int nCardNum,unsigned int nPortIndex);

THwaFcLockHandle HwaFcInitLock(char *pLockName,char *pSerial);

int HwaFcDriverLogin(int nCardNum,THwaFcPortSel ePort);

void TraceLog(const char * sFormat,...);

int HwaFcDriverGetIoLevel(int nCardNum,int *pLevel);

int HwaFcDriverSetIoLevel(int nCardNum,int nLevel);

int HwaFcDriverSetIrigMode(int nCardNum,THwaFcIrigMode eMode);

int HwaFcDriverSetCaptureLoopMode(int nCardNum,THwaFcLoopMode eMode);

//added by yzy
int HwaFcDriverMonStartDma(THwaFcDevList *pDevCtx,unsigned int nSize);
int HwaFcDriverSetTime(int nCardNum);
int HwaFcDriverGetTime(int nCardNum,THwaFcTime *pTime);
int GetSystemTime(THwaFcSysTime *pTime);
int HwaFcDriverOpenDma(int nCardNum);
#endif

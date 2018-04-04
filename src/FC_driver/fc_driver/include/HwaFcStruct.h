#ifndef __HWA_FC_STRUCT__
#define __HWA_FC_STRUCT__

#ifdef VXWORKS_OPS
#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <drv/pci/pciConfigLib.h>
#include <drv/pci/pciIntLib.h>
#include <semLib.h>
#include <intLib.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <vmLib.h>					
#include <time.h>
#include "private/vmLibP.h"			/*  VM_PAGE_SIZE_GET() */
#include <cacheLib.h>  				 /* DMA no cache */
#include "netinet/in.h"
#include "string.h" // added by yzy
#include <vxBusLib.h>
#include <vxbParamSys.h>
#include <vxbPciLib.h>
#endif


#define FC_DBG_NO		0x0
#define FC_DBG_ERROR	0x1
#define FC_DBG_WARNING	0x2
#define FC_DBG_INFOR	0x4

extern int nHwaFcDevCnt;
extern char sHwaFcLastErr[256];
extern char sDbgInfo[200];
extern unsigned int nHwaFcDbgFd;
extern struct sockaddr_in nDstAddr;



#include "HwaFcDriver.h"
#define MAX_DMA_SIZE			(1024*1024*17)//(0.1 * 1024 * 1024)	//16M
#define MAX_LPS_SIZE			32
#define MAX_SEND_LEN			(10240)
#define MAX_RECV_LEN             (1024*1024*16) //add wxb
#define MAX_DMA_SIZE_NEED_ADD   512
#define MAX_TIMED_OUT			(4000*1000*1000) //3000ms
#define MAX_SEND_LPS_SIZE			32
#define MAX_RECV_LPS_SIZE			4
#define CAP_MODE_DMA_BUF_CNT	2
#define FC_HEADER_LEN			24
#define FC_ASM_HEADER_LEN		16
#define MAX_PAYLOAD_LEN			(2112 - 16) //FC Payload - ASM　Header
#define FC_CRC_LEN				4
#define FC_SOF_LEN				4
#define FC_EOF_LEN				4
#define FC_MAX_FRM_LEN			(FC_HEADER_LEN + FC_ASM_HEADER_LEN + MAX_PAYLOAD_LEN +FC_CRC_LEN + FC_SOF_LEN + FC_EOF_LEN)
#define DMA_LPS_SIZE_CAP		4
#define HWA_FC_BLOCK_FIFO_SIZE	(80 * 1024 * 1024) //80M 5 packets
#define HWA_FC_FIFO_SIZE		(1024 * 2112) //1K packets 

#define HWA_FC_MSG_CNT			(1000)
#define HWA_FC_MSG_NAME_LEN 	(100)
#define HWA_FC_SPE_PORT_S_INDEX (65535)
#define HWA_FC_SPE_PORT_R_INDEX (65536)

#define HWA_FC_DELAY_CNT 1000000
#define PKT_FILE_NAME "PKT.data"

#define SWAP_32(x) (((x) & 0x000000ff) << 24) |\
					((x) & 0x0000ff00) << 8)  |\
					((x) & 0x00ff0000) >> 8)  |\
					((x) & 0xff000000) >>24))


#ifdef WINDOWS_OPS
#define WAIT_EVENT(x) do {\
						if((x) != HWA_FC_INVALIED_HANDLE){\
							if(WAIT_OBJECT_0 != WaitForSingleObject((x), INFINITE))\
							{\
								ErrLog("Wait for event Failed.");\
								ReleaseMutex((x));\
								return -1;\
							}\
						}else{\
							ErrLog("Lock event is NULL.");\
							return -1;\
						}\
					}while(0)


#define RESET_EVENT(x) do{\
						if((x) != HWA_FC_INVALIED_HANDLE){\
							ResetEvent((x));\
						}else{\
							ErrLog("Lock event is NULL.");\
						return -1;\
						}\
					}while(0)

#define RELEASE_LOCK(x) do{\
							if((x) != HWA_FC_INVALIED_HANDLE){\
								ReleaseMutex((x));\
							}else{\
								ErrLog("Lock event is NULL.");\
								return -1;\
							}\
						}while(0)

#define SET_EVENT(x) do{\
						if(x != HWA_FC_INVALIED_HANDLE)\
						{\
							SetEvent((x));\
						}\
					}while(0)


#define SAFE_CLOSE(x)do{\
						if((x) != HWA_FC_INVALIED_HANDLE){\
							CloseHandle((x));\
							(x)=HWA_FC_INVALIED_HANDLE;\
						}\
					}while(0)

#define SAFE_CLOSE_MSG(x) SAFE_CLOSE(x)

#define HWA_FC_WRITE_32(drv,bar,addr,value) WDC_WriteAddr32(drv.pDevHandle,bar,addr,value);

#define HWA_FC_READ_32(drv,bar,addr,value) WDC_ReadAddr32(drv.pDevHandle,bar,addr,value);

#define SPRINTF sprintf_s

#define ErrLog ErrLogWin

typedef HANDLE THwaFcMsgHandle;

typedef HANDLE THwaFcEventHandle;

typedef HANDLE THwaFcLockHandle;


#define INVAILED_HWA_FC_XML_STR "" 

#define HWA_FC_INVALIED_HANDLE INVALID_HANDLE_VALUE

#define START_THREAD(id,handle,nCardNum,pfunc) (handle = CreateThread(NULL,0,pfunc,(LPVOID)(nCardNum),0,(LPDWORD)(id)));

#define SLEEP Sleep



#elif defined VXWORKS_OPS	/*#ifdef WINDOWS_OPS*/

#define WAIT_EVENT(x) do {\
							if(x)\
							{\
								if(semTake(x,WAIT_FOREVER) != OK)\
								{\
									ErrLog("%s","Wait for  event Failed.");\
									return -1;\
								}\
							}else{\
								ErrLog("%s","Event is null.");\
								return -1;\
							}\
						}while(0)


#define RESET_EVENT(x) do{\
							if(x){\
								semGive(x);\
							}else{\
								ErrLog("%s","Event is null.");\
								return -1;\
							}\
					}while(0)

#define SET_EVENT(x) do{\
						if(x != NULL)\
						{\
							semGive((x));\
						}\
					}while(0)

#define RELEASE_LOCK(x) do{\
							if(x){\
								semGive(x);\
							}\
						}while(0)

#define SAFE_CLOSE(x)do{\
						if(x != NULL){\
							semDelete(x);\
							x = NULL;\
						}\
					}while(0)
					
#define SAFE_CLOSE_MSG(x)do{\
						if(x != NULL){\
							msgQDelete(x);\
							x = NULL;\
						}\
					}while(0)

#define HWA_RECV_TASK_PRIORITY 	101

#define HWA_RECV_TASK_STACK 	16384

typedef MSG_Q_ID 	THwaFcMsgHandle;

typedef SEM_ID 		THwaFcEventHandle;

typedef SEM_ID 		THwaFcLockHandle;

typedef void *		LPSTR;


#define INVAILED_HWA_FC_XML_STR NULL

#define HWA_FC_INVALIED_HANDLE 0

#define SLEEP taskDelay

#if 0					
#define START_THREAD(id,handle,nCardNum) do{\
											(*(id)) = taskSpawn("HwaFcRecvTask",HWA_RECV_TASK_PRIORITY,0,HWA_RECV_TASK_STACK,(FUNCPTR)HwaFcDriverRecvFunc,(nCardNum),0,0,0,0,0,0,0,0,0);\
											handle = 1;\
	
}while(0)
#else
	#define START_THREAD(id,nCardNum) do{\
										(*(id)) = taskSpawn("HwaFcRecvTask",HWA_RECV_TASK_PRIORITY,0,HWA_RECV_TASK_STACK,(FUNCPTR)HwaFcDriverRecvFunc,(nCardNum),0,0,0,0,0,0,0,0,0);\
									}while(0)
#endif


						
#define convert(x)	((((x) & 0x000000ff) << 24) | \
									 (((x) & 0x0000ff00) <<  8) | \
									 (((x) & 0x00ff0000) >>  8) | \
									 (((x) & 0xff000000) >> 24))
						

extern void sMsSleep (int ms);

#define PCIE_WRITE32(bar, offset, value) \
								sMsSleep(50);\
								*(unsigned int *)(bar + offset) = convert(value);

#define PCIE_READ32(bar, offset, pValue) \
								sMsSleep(50);\
								*pValue = convert(*(unsigned int *)(bar + offset))


#define HWA_FC_WRITE_32(drv,bar,addr,value) PCIE_WRITE32(drv.nBarAddr[bar],addr,value)
#define HWA_FC_READ_32(drv,bar,addr,value) PCIE_READ32(drv.nBarAddr[bar],addr,value)
						
#define SPRINTF(buff,size,fmt,args...) sprintf(buff,fmt,##args)
#define ErrLog(fmt,args...) sprintf(sHwaFcLastErr,fmt,##args)

#else /*#elif defined VXWORKS_OPS*/

#error "unknown operation system."

#endif


#ifdef FC_DBG
#ifdef WINDOWS_OPS
#define HWA_FC_DBG HwaFcDriverDbg
#elif defined VXWORKS_OPS
#define HWA_FC_DBG(fmt,args...) do{\
										sprintf(sDbgInfo,fmt,##args);\
										if(nHwaFcDbgFd == 0){\
											HwaFcDriverInitDbg();\
										}\
										sendto(nHwaFcDbgFd,sDbgInfo,strlen(sDbgInfo+1),0,(struct sockaddr *) &nDstAddr,sizeof(nDstAddr));\	
									}while(0)
#else
#error "unknown operator system"
#endif
#else
#define HWA_FC_DBG
#endif

#ifdef FC_DBG_FUNC_CALL_EN
#define FC_DBG_FUNC_CALL HWA_FC_DBG("%s called.\n",__func__);
#else
#define FC_DBG_FUNC_CALL
#endif

#define CHECK_CARD_ST(x) do{\
							if(!nHwaFcDev[(x)].nDevInitFlag)\
								{\
								ErrLog("Card %d not opened.",nCardNum);\
								return FALSE;\
								}\
							}while(0)

#define CHECK_CARD_ID(x) do{\
							if(((x) < 0) ||((x) >= nHwaFcDevCnt))\
							{\
								ErrLog("Wrong card id.\n");\
								return HWA_FC_ERR_PARA;\
							}\
						}while(0)


#define CHECK_PORT_ID(x) do{\
							if(((x) < 0) ||((x) >= HWA_FC_MAX_PORT_CNT))\
							{\
								ErrLog("Wrong port id.\n");\
								return HWA_FC_ERR_PARA;\
							}\
						}while(0)

#define CHECK_PORT_HANDLE(x) do{\
							if(((x) < 0) ||((x) >= HWA_FC_MAX_PORT_CNT))\
							{\
								ErrLog("Wrong port handle.");\
								return HWA_FC_ERR_PARA;\
							}\
						}while(0)

#define SAFE_FREE(x)do{\
						if((x) != NULL)\
						{\
							free(x);\
							(x)=NULL;\
						}\
					}while(0)

#define GOTO_ERR(x)do{ret = x;goto ERR;}while(0)

#define GOTO_LABEL(label,err)do{ret = (err);goto label;}while(0)

#define SET_MSG_NAME(buff,name,index) SPRINTF((buff),HWA_FC_MSG_NAME_LEN,"\\\\.\\pipe\\FCMsg%s-%d",(name),(index))
#define SET_LOCK_NAME(buff,name,index) SPRINTF((buff),HWA_FC_MSG_NAME_LEN,"%s-PortIndex-%d",(name),(index))

#define CLEAR_MSG_NAME(buff) memset((buff),0,HWA_FC_MSG_NAME_LEN)



#define SET_DAYS(aim,days,hours) aim = ((((days) & TIME_DAYS_MASK) << TIME_DAYS_BIT) |\
										(((hours) & TIME_HOURS_MASK) << TIME_HOURS_BIT))

#define SET_MIN(aim,min,sec,msec,usec) aim =	((((min) & TIME_MIN_MASK) << TIME_MIN_BIT)|\
												(((sec) & TIME_SEC_MASK) << TIME_SEC_BIT)|\
												(((msec) & TIME_MSEC_MASK) << TIME_MSEC_BIT)|\
												(((min) & TIME_USEC_MASK) << TIME_USEC_BIT))

#define HWA_FC_DAY(x)	(((x) >> TIME_DAYS_BIT) & TIME_DAYS_MASK)
#define HWA_FC_HOUR(x)	(((x) >> TIME_HOURS_BIT) & TIME_HOURS_MASK)
#define HWA_FC_MIN(x)	(((x) >> TIME_MIN_BIT) & TIME_MIN_MASK)
#define HWA_FC_SEC(x)	(((x) >> TIME_SEC_BIT) & TIME_SEC_MASK)
#define HWA_FC_MSEC(x)	(((x) >> TIME_MSEC_BIT) & TIME_MSEC_MASK)
#define HWA_FC_USEC(x)	(((x) >> TIME_USEC_BIT) & TIME_USEC_MASK)

#define LEAP_YEAR(x) (((x) % 4 == 0) && ((x) % 100 != 0) || ((x) % 400 == 0))

#define IS_SIMULATE (nCardNum == 0)



typedef enum{
	eBar0=0,
	eBar1,
	eBar2,
	eBar3,
	eBar4,
	eBar5,
	eBar6,
	eBarNr
}THwaBar;



typedef struct {
	unsigned int nBufSize;
	char *pReadPos;
	char *pWritePos;
	char *pBuf;
	char *pLast;
	unsigned int nDataCnt;
	char sLockName[HWA_FC_MSG_NAME_LEN];
	THwaFcLockHandle  nLock;
	int  nInit;

}THwaFcSoftFifo;



typedef struct{
		unsigned int nPortId;
		unsigned int nPortIndex;
		unsigned int nPortStat; //端口配置信息已经有效
		unsigned int nLoaded;	//端口配置信息已经被加载到硬件
		unsigned int nPhyIndex;
		THwaFcPortType ePortTye;
		THwaFcPortDir ePortDir;
		unsigned int nSid;
		unsigned int nDid;
		unsigned int nOxId;
		THwaFcPortSel nPortSel;
		unsigned int nType;
		unsigned int nMsgId;
		unsigned int nSeqId;
		union {
			struct{
				int nRecvEnable;
				int nFragOffset;
				int nFragSeq;
				THwaFcMsgHandle nRecvMsgHandle;
				THwaFcMsgHandle nPipeHandle;
				THwaFcSoftFifo nPortFifo;
				char sMsgName[HWA_FC_MSG_NAME_LEN];
			}nRecvPortCfg;
			struct{
				unsigned int nPri;
				unsigned int nMaxLen;
			}nSendPortCfg;
		}nPortCfg;
}THwaFcPort;


typedef struct  
{
	int nLogicPortIndex;
}THwaFcPhyPort;


#ifdef WINDOWS_OPS
typedef enum{
	eDmaCh1,
	eDmaCh2,
	eDmaChNr
}THwaFcDmaCh;


typedef struct tHwaDmaStruct {
    WD_DMA *pDma;
    THwaFcDmaCh nDmaCh;
    DWORD nDmaCsr;
    DWORD nDmaMode;
    DWORD nDmaPadr;
    DWORD nDmaLadr;
    DWORD nDmaPr;
    DWORD nDmaSize;
	unsigned int nPhyAddr;
	unsigned int nIndex;
	struct tHwaDmaStruct *pNext;
} THwaDmaStruct;

typedef struct{
	WDC_DEVICE_HANDLE pDevHandle;
	WD_TRANSFER *pIntTransCmds;
	WD_PCI_CARD_INFO nPciDevInfo;
	void *pBufIn;
	void *pBufOut;
	THwaFcEventHandle nDmaLock;
	THwaFcEventHandle nPortLock;
	THwaFcEventHandle nDmaEvent;
	THwaFcEventHandle nRecvEvent;
	THwaFcEventHandle nTrigEvent;
	THwaFcEventHandle nLinkEvent;
	THwaDmaStruct  nDmaIn;
	THwaDmaStruct  nDmaOut;
/*used for CAP mode*/
	void *pDmaBuf[CAP_MODE_DMA_BUF_CNT];
	THwaDmaStruct nCapModeDma[CAP_MODE_DMA_BUF_CNT];
	THwaFcEventHandle nDmaUsedEvent[CAP_MODE_DMA_BUF_CNT];
	THwaDmaStruct *pCurDma;
}THwaFcDrvHandle;
#else
typedef struct{
	unsigned int nBarAddr[eBarNr];
	char nIrqId;
	char nPciIntPing;
	void *pBufIn;
	void *pBufOut;
	THwaFcEventHandle nDmaLock;
	THwaFcEventHandle nPortLock;
	THwaFcEventHandle nDmaEvent;
	THwaFcEventHandle nRecvEvent;
	THwaFcEventHandle nTrigEvent;
	THwaFcEventHandle nLinkEvent;
}THwaFcDrvHandle;
#endif

typedef struct 
{
	unsigned int nDataLen;
	THwaFcDevTime nRecvTime;
	unsigned int nMsgInfo;
	unsigned int nRes;
}THwaFcRecvMsg;

typedef struct {
	unsigned int nDataLen;
	unsigned int nAddr;
	/*res for keep same size with RecvMsg*/
	unsigned int nDataType;
	unsigned int nDmaIndex;
}THwaFcMonRecvMsg;

typedef struct {
	char nMsgName[HWA_FC_MSG_NAME_LEN];
	THwaFcMsgHandle nPipeHandle;
	THwaFcMsgHandle nMonRecvHandle;
	THwaFcSoftFifo nMonFifo;
}THwaFcMonPort;

typedef enum{
	eMonStop,
	eMonRunning,
	eMonPause,
}THwaFcMonStat;


typedef struct{
	unsigned char nId[2];
}THwaFcOxID; 

typedef struct{
	unsigned char nId[3];
}THwaFcDeviceID; 

typedef enum{
	eLoginToSw,
	eLoginToNport,
}THwaFcPLoginType;


typedef struct{
	unsigned int nVailed;
	unsigned int isInDma;
	unsigned int isRecvData;
	unsigned int nDevInitFlag;
	unsigned int nExitFlag;
#ifdef 	VXWORKS_OPS
	int nRecvThreadId;
	int nLoginThreadId; 
#else
	THwaFcEventHandle nRecvThreadId;
	THwaFcEventHandle nLoginThreadId;
#endif
	
	THwaFcMonStat eMonStat;
	THwaFcMonCfg  nMonCfg;
	ThwaFcDevInfo nDevInfo;
	
	THwaFcDevMode eFcDevMode;
	THwaFcDevSpeed eFcDevSpeed;
	THwaFcIrigMode eFcIrigMode;
	
	/*add by sxy 20130416_for normal recv block mode*/
	THwaFcRecvMode eRecvMode;

	THwaFcLockHandle nSendLock;
	THwaFcLockHandle nRecvLock;
	THwaFcLockHandle nRecvIntrLock;

	THwaFcMonPort nMonRecvPort;

	THwaFcMonFilter nMonFilter[HWA_FC_MAX_FILTERCNT];
	unsigned int nMonFilterMask[8];

	THwaFcPort nRecvPort[HWA_FC_MAX_PORT_CNT];
	unsigned int nRecvPortMask[HWA_FC_PORT_MASK_CNT];
	
	THwaFcPort nSendPort[HWA_FC_MAX_PORT_CNT];
	unsigned int nSendPortMask[HWA_FC_PORT_MASK_CNT];
	
	THwaFcPhyPort nRecvPhyPort[HWA_FC_MAX_PORT_CNT];
	unsigned int nRecvPhyPortMask[HWA_FC_PORT_MASK_CNT];

	THwaFcPhyPort nSendPhyPort[HWA_FC_MAX_PORT_CNT];
	unsigned int nSendPhyPortMask[HWA_FC_PORT_MASK_CNT];
	
	unsigned int nDevUsedCnt;
	THwaFcDrvHandle nHwaFcDrvHandle;
	unsigned int nCardSt; 
	THwaFcPortHandle nSpeSendPort;
	THwaFcPortHandle nSpeRecvPort;
	THwaFcDeviceID nSrcId[2];
	unsigned char nNodeName[8];
	char nDevName[50];
	int nSendLpsSize;
	int nRecvLpsSize;
	THwaFcPortStatus nLinkStatus;
/*for debug*/
	int nRecvCnt;
	int nIntrCnt;
}THwaFcDevList;

#endif



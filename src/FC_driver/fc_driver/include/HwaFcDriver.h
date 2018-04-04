#ifndef _HWA_FC_DRIVER_H_
#define _HWA_FC_DRIVER_H_
#ifdef WINDOWS_OPS
	#ifdef _HWA_FC_EXPORT
		#ifdef __cplusplus
			#define __HWA_FC_DLL_EXPORT extern "C" __declspec(dllexport)
		#else
			#define __HWA_FC_DLL_EXPORT __declspec(dllexport)
		#endif
	#else
		#ifdef __cplusplus
			#define __HWA_FC_DLL_EXPORT extern "C" __declspec(dllimport)
		#else
			#define __HWA_FC_DLL_EXPORT __declspec(dllimport)
		#endif
	#endif
#else
	#define __HWA_FC_DLL_EXPORT
#endif
/*error code defines*/
#define HWA_FC_OP_OK				0x0
#define HWA_FC_ERR_PARA				-1
#define HWA_FC_ERR_INIT				-2
#define HWA_FC_ERR_UNINIT			-3
#define HWA_FC_ERR_OPEN				-4
#define HWA_FC_ERR_CLOSE			-5
#define HWA_FC_ERR_SEND				-6
#define HWA_FC_ERR_RECV				-7
#define HWA_FC_ERR_SEND_OVERFLOW	-8
#define HWA_FC_ERR_XML				-9
#define HWA_FC_ERR_CFG				-10
#define HWA_FC_ERR_MSG				-11
#define HWA_FC_ERR_FIFO				-12
#define HWA_FC_ERR_PORT				-13
#define HWA_FC_ERR_LOGIN			-14
#define HWA_FC_ERR_PORT_LINK		-15
#define HWA_FC_READ_MSG_TIMEDOUT    -16
#define HWA_FC_ERR_RECV_BLOCK		-17  
//add by sxy normal recv block mode

/*common defines*/
#define SERIAL_LEN 16
#define HWA_FC_MAX_NOR_PORT	256
#define HWA_FC_MAX_BLK_PORT	16
#define HWA_FC_MAX_SPE_PORT	1
#define HWA_FC_MAX_PORT_CNT (HWA_FC_MAX_NOR_PORT + HWA_FC_MAX_BLK_PORT +HWA_FC_MAX_SPE_PORT)
#define HWA_FC_PORT_MASK_CNT 9
#define HWA_FC_MAX_DEV_CNT 8
#define HWA_FC_ASM_MAX_PAYLOAD 	2096
#define HWA_FC_NASM_MAX_PAYLOAD	2148
#define HWA_FC_BLK_MAX_PAYLOAD 16*1024*1024
#define HWA_FC_MAX_FILTERCNT 256
#define HWA_FC_FILTER_MASKCNT 8
#define HWA_FC_FILTER_FILED_CNT 6
/*struct*/
typedef int THwaFcPortHandle;

#define IS_ASM 0
#define NOT_ASM (!(IS_ASM))
typedef struct{
	unsigned int nIsAsm;
	unsigned int nPktPri;
	unsigned int nChSel;
}THwaSendCfg;


typedef struct{
	unsigned int nVendorID;
	unsigned int nDeviceID;
	unsigned int nBusNum;
	unsigned int nSlotNum;
	unsigned int nFuncNum;
	unsigned int nHwVer;
	char sFcSerial[SERIAL_LEN];
}ThwaFcDevInfo;

typedef struct{
	unsigned int nLinkStatus;
	unsigned int nFcSpeed;
	unsigned int nFiberStatus;
	unsigned char nRes[8];
}THwaFcPortStatus;


typedef struct{
	unsigned int nRXAllFrm;
	unsigned int nRXAllByte0;
	unsigned int nRxAllByte1;
	unsigned int nRX0To127;
	unsigned int nRX128To255;
	unsigned int nRX256To511;
	unsigned int nRX512To1023;
	unsigned int nRX1024To2112;
	unsigned int nRXShortErr;
	unsigned int nRXLongErr;
	unsigned int nRxCrcError;
	unsigned int nRXByteFlux;
	unsigned int nRXFrmFlux;
	unsigned int nRxPortIdError;
	unsigned int nRxPortEnableError;
	unsigned int nRxOffsetError;
	unsigned int nRxSeqIDError;
}THwaFcRXMib;

typedef enum{
	eRxMib0,
	eRxMib1,
	eRxMib2,
	eRxMib3,
	eRxMib4,
	eRxMib5,
	eRxMib6,
	eRxMib7,
	eRxMib8,
	eRxMib9,
	eRxMib10,
	eRxMib11,
	eRxMib12,
	eRxMib13,
	eRxMib14,
	eRxMib15,
	eRxMib16,
	eRxMibNr
}THwaFcRxMibIndex;


typedef struct{
	unsigned int nTXAllFrm;
	unsigned int nTXAllByte0;
	unsigned int nTxAllByte1;
	unsigned int nTx0To127;
	unsigned int nTx128To255;
	unsigned int nTx256To511;
	unsigned int nTx512To1023;
	unsigned int nTx1024To2112;
	unsigned int nTxByteFlux;
	unsigned int nTxFrmFlux;
}THwaFcTxMib;

typedef enum{
	eTxMib0 = 0x20,
	eTxMib1,
	eTxMib2,
	eTxMib3,
	eTxMib4,
	eTxMib5,
	eTxMib6,
	eTxMib7,
	eTxMib8,
	eTxMib9,
	eTxMibNr
}THwaFcTxMibIndex;

typedef struct{
	unsigned int nRXMinErr;
	unsigned int nRXMaxErr;
	unsigned int nRXCrcErr;
	unsigned int nRxerr;
	unsigned int nRxDrop;
}THwaFcSysMib;

typedef enum{
	eSysMib0,
	eSysMib1,
	eSysMib2,
	eSysMib3,
	eSysMib4,
	eSysMibNr
}THwaFcSysMibIndex;

typedef enum{
	eMibRx,
	eMibTx,
	eMibSys,
	eMibNr
}THwaFcMib;

typedef enum{
	eDirSend,
	eDirRecv,
	ePortDirNr,
}THwaFcPortDir;

typedef enum{
	eTypeNormal,
	eTypeBlock,
	eTypeSpecial,
	ePortTypeNr,
}THwaFcPortType;


typedef enum{
	ePortA = 1,
	ePortB,
	ePortAB,
}THwaFcPortSel;

typedef enum{
	eTypeAsm,
	eTypeOther,
	eMsgTypeNr,
}THwaFcMsgType;

typedef enum{
	eSys2Dev,
	eDev2Fc,
}THwaFcTimeDir;

typedef enum{
	eModeNormal,
	eModeCapture,
	eDevModeNr
}THwaFcDevMode;

typedef enum{
	eNormalRecvNor,
	eNormalRecvBlk,
	eRecvNr,
}THwaFcRecvMode;

typedef enum{
	eSpeed1G,
	eSpeed2G,
	eSpeedNr
}THwaFcDevSpeed;

typedef enum{
	eModeMaster,
	eModeSlave,
	eIrigModeNr
}THwaFcIrigMode;

typedef struct {
	unsigned int nHighTime;
	unsigned int nLowTime;
}THwaFcDevTime;

typedef enum{
	eDivMode,
	eLoopMode,
	eLoopModeNr,
}THwaFcLoopMode;

typedef struct{
	int nDays;
	int nHours;
	int nMins;
	int nSecs;
	int nMsecs;
	int nUsecs;
}THwaFcTime;

typedef struct{
	THwaFcDevMode eWorkMode;
	THwaFcDevSpeed eSpeed;
	THwaFcIrigMode eIrigbMod;
	THwaFcRecvMode eRecvMode;
	int nSendLpsSize;
	int nRecvLpsSize;
}THwaFcCardCfg;

typedef struct{
		unsigned int nSid;
		unsigned int nDid;
		unsigned int nType;
		unsigned int nMsgId;
		unsigned int nOxId;
		unsigned int nSeqId;
		unsigned int nPortSel;
		unsigned char nFilterFiledSel[HWA_FC_FILTER_FILED_CNT];
}THwaFcMonFilter;


typedef struct
{
	unsigned int nPortId;
	THwaFcPortType ePortType;
	THwaFcPortDir ePortDir;
	unsigned int nSid;
	unsigned int nDid;
	unsigned int nOxId;
	THwaFcPortSel ePortPortSel;
	unsigned int nPortMsgType;
	unsigned int nMsgId;
	unsigned int nSeqId;
	unsigned int nFragSeq;
	unsigned int nFragOffset;
	unsigned int nPri;
	unsigned int nMaxLen;
}THwaFcPortCfg;

typedef struct{
	THwaFcPortSel ePort;
	THwaFcDevTime nRecvTime;
	THwaFcMsgType eMsgType;
}THwaFcRecvInfo;

typedef enum{
	eStopRecv,
	eStartRecv,
	ePauseRecv,
	eMonOpNr,
}THwaFcMonOperation;

typedef enum{
	eMonSelect,
	eMonFilter,
	eMonModeNr,
}THwaFcMonMode;

typedef struct{
	unsigned int nMonThresdHold;
	THwaFcMonMode nWorkMode;
	unsigned int nMonTimedOut;
}THwaFcMonCfg;

#ifdef VXWORKS_OPS
typedef struct{
   int wYear;
   int wMonth;
   int wDayOfWeek;
   int wDay;
   int wHour;
   int wMinute;
   int wSecond;
   int wMilliseconds;
} THwaFcSysTime;
#elif defined WINDOWS_OPS
typedef  SYSTEMTIME THwaFcSysTime; 
#else 
	#error "unknown operation system"  
#endif


/*export functions*/
__HWA_FC_DLL_EXPORT int HwaFcInit(void);

__HWA_FC_DLL_EXPORT int HwaFcDeInit(void);

__HWA_FC_DLL_EXPORT int HwaFcOpenCard(int nCardNum);

__HWA_FC_DLL_EXPORT int HwaFcCloseCard(int nCardNum);

__HWA_FC_DLL_EXPORT int HwaFcLoadCfg(int nCardNum,char *pFileName);

__HWA_FC_DLL_EXPORT int HwaFcSetCardCfg(int nCardNum,THwaFcCardCfg nCfg);

__HWA_FC_DLL_EXPORT int HwaFcAddPort(int nCardNum,THwaFcPortCfg * pPortList,int nPortCnt);

__HWA_FC_DLL_EXPORT int HwaFcDelPort(int nCardNum,unsigned int nPortId,THwaFcPortDir ePortDir);

__HWA_FC_DLL_EXPORT int HwaFcOpenPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle,THwaFcPortDir eDir);

__HWA_FC_DLL_EXPORT int HwaFcClosePort(int nCardNum,THwaFcPortHandle nPortHandle,THwaFcPortDir eDir);

__HWA_FC_DLL_EXPORT int HwaFcSendData(int nCardNum,THwaFcPortHandle nHandle,char *pBuf,unsigned int nBufSize,THwaSendCfg nCfg);

__HWA_FC_DLL_EXPORT int HwaFcRecvData(int nCardNum,THwaFcPortHandle nPortHandle,char *pBuf,THwaFcRecvInfo *pInfo, int nBufSize,int nWaitTime);

#ifdef WINDOWS_OPS
__HWA_FC_DLL_EXPORT int HwaFcSetMonOperation(int nCardNum,THwaFcMonOperation eOperation);

__HWA_FC_DLL_EXPORT int HwaFcMonRecvData(int nCardNum,char *pBuf, int nBufSize,int nWaitTime,int *pDataType);

__HWA_FC_DLL_EXPORT int HwaFcSetMonCfg(int nCardNum,THwaFcMonCfg nCfg);

__HWA_FC_DLL_EXPORT int HwaFcAddMonFilter(int nCardNum,int nFilterCnt,THwaFcMonFilter *pMonFilter);

__HWA_FC_DLL_EXPORT int HwaFcDelMonFilter(int nCardNum,int nIndex);

#endif

__HWA_FC_DLL_EXPORT int HwaFcGetCardNum(void);

__HWA_FC_DLL_EXPORT int HwaFcGetCardInfo(int nCardNum,ThwaFcDevInfo *pDevInfo);

__HWA_FC_DLL_EXPORT int HwaFcGetPortStatus(int nCardNum,THwaFcPortStatus * pPortStatus);

__HWA_FC_DLL_EXPORT void HwaFcDumpCardInfo(int nCardNum,ThwaFcDevInfo nDevInfo);

__HWA_FC_DLL_EXPORT  int HwaFcReadReg(int nCardNum,int nAddr);

__HWA_FC_DLL_EXPORT int HwaFcWriteReg(int nCardNum,int nAddr,unsigned int nValue);

__HWA_FC_DLL_EXPORT const char *HwaFcGetLastErr(void);

__HWA_FC_DLL_EXPORT int HwaFcWriteCfgReg(int nCardNum,int nAddr,unsigned int nValue);

__HWA_FC_DLL_EXPORT int HwaFcReadCfgReg(int nCardNum,int nAddr);

__HWA_FC_DLL_EXPORT int HwaFcGetTxMibs(int nCardNum,THwaFcTxMib *pMibs,THwaFcPortSel ePort);

__HWA_FC_DLL_EXPORT int HwaFcGetRxMibs(int nCardNum,THwaFcRXMib *pMibs,THwaFcPortSel ePort);

__HWA_FC_DLL_EXPORT int HwaFcGetSysMibs(int nCardNum,THwaFcSysMib *pMibs);

__HWA_FC_DLL_EXPORT int HwaFcClearMibs(int nCardNum);

__HWA_FC_DLL_EXPORT int HwaFcGetPortOverFlow(int nCardNum,unsigned int nPortIndex,THwaFcPortDir eDir);

__HWA_FC_DLL_EXPORT int HwaFcDriverChangeTime(THwaFcSysTime *pSysTime,THwaFcDevTime *pDevTime,THwaFcTime *pFcTime,THwaFcTimeDir eDir);

__HWA_FC_DLL_EXPORT int HwaFcLogin(int nCardNum,THwaFcPortSel ePort);

__HWA_FC_DLL_EXPORT int HwaFcGetSpePortHandle(int nCardNum,THwaFcPortHandle *pRecv,THwaFcPortHandle *pSend);

__HWA_FC_DLL_EXPORT int HwaFcSetIolevel(int nCardNum, int nLevel);

__HWA_FC_DLL_EXPORT int HwaFcSetIrigMode(int nCardNum,THwaFcIrigMode eMode);

__HWA_FC_DLL_EXPORT int HwaFcGetIolevel(int nCardNum, int *pLevel);

__HWA_FC_DLL_EXPORT int HwaFcListenLinkStatus(int nCardNum,THwaFcPortStatus *pStatus);

__HWA_FC_DLL_EXPORT int HwaFcSetCaptureLoopMode(int nCardNum,THwaFcLoopMode eMode);
__HWA_FC_DLL_EXPORT void test_pcie_dma(int nCardNum);

#endif


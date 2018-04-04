#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifdef VXWORKS_OPS
#include "iv.h" 
#include "drv/pci/pciConfigLib.h"
#include "string.h"
#elif defined WINDOWS_OPS
#include <io.h>
#else
#error "unknown operation system"
#endif

#include "HwaFcStruct.h"
#include "HwaFcDriver.h"
#include "HwaFcReg.h"

#include "HwaFcDriverLib.h"






#define HWAFC_DEFAULT_LICENSE_STRING "6C3CC2CFE89E7AD0424A070D434A6F6DC4950E31.hwacreate"
#define HWAFC_DEFAULT_DRIVER_NAME WD_DEFAULT_DRIVER_NAME_BASE


THwaFcLockHandle nHwaFcDevListLock = HWA_FC_INVALIED_HANDLE;
int nHwaFcDevCnt = 0;
int nHwaFcDriverInitFlag = 0;
THwaFcDevList nHwaFcDev[HWA_FC_MAX_DEV_CNT + 1];
#ifdef WINDOWS_OPS
static DWORD LibInit_count = 0;
static BOOL DeviceValidate(const PWDC_DEVICE pDev);
static void DLLCALLCONV HwaFcIntHandler(PVOID pData);
#endif

#define FC_DEV(x) nHwaFcDev[x].nHwaFcDrvHandle

/* TODO: You can add fields to store additional device-specific information */

char sHwaFcLastErr[256];


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
THwaFcEventHandle HwaFcInitEvent(char *pLockName,char *pSerial)
{
	THwaFcEventHandle pTmpHandle = NULL;
	FC_DBG_FUNC_CALL;

#ifdef WINDOWS_OPS	
	char LockName[100] = {0};
	if((pLockName == NULL) || (pSerial == NULL)){
		return HWA_FC_INVALIED_HANDLE;
	}
	SPRINTF(LockName,sizeof(LockName),"HWA-FC-NIC-%s-%s",pSerial,pLockName);
	pTmpHandle = CreateEvent(NULL,FALSE,FALSE,(LPCSTR)LockName);

#elif defined VXWORKS_OPS
	pTmpHandle = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

#else
	#error "unknown operation system."
#endif

	if(pTmpHandle == HWA_FC_INVALIED_HANDLE)
	{
		ErrLog("%s","Create event failed.");
		return HWA_FC_INVALIED_HANDLE;
	}	

	return pTmpHandle;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
THwaFcLockHandle HwaFcInitLock(char *pLockName,char *pSerial)
{	
	THwaFcEventHandle pTmpHandle = NULL;
	FC_DBG_FUNC_CALL;

#ifdef WINDOWS_OPS
	char LockName[100] = {0};
	if((pLockName == NULL) || (pSerial == NULL)){
		return HWA_FC_INVALIED_HANDLE;
	}

	SPRINTF(LockName,sizeof(LockName),"HWA-FC-NIC-%s-%s",pSerial,pLockName);
	pTmpHandle = CreateMutex(NULL,FALSE,(LPCSTR)LockName);

#elif defined VXWORKS_OPS
	pTmpHandle = semBCreate(SEM_Q_FIFO, SEM_FULL);
#else
	#error "unknown operation system."
#endif
	if(pTmpHandle == HWA_FC_INVALIED_HANDLE)
	{
		ErrLog("%s","Create lock failed.");
		return HWA_FC_INVALIED_HANDLE;
	}	

	return pTmpHandle;
}

#ifdef WINDOWS_OPS
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    FC_DBG_FUNC_CALL;
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        SPRINTF(sHwaFcLastErr, sizeof(sHwaFcLastErr) - 1, "%s: NULL device %s\n",
            sFunc, !pDev ? "handle" : "context");
        ErrLog(sHwaFcLastErr);
        return FALSE;
    }

    return TRUE;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static BOOL DeviceValidate(const PWDC_DEVICE pDev)
{
	DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;
	FC_DBG_FUNC_CALL;
	for (i = 0; i < dwNumAddrSpaces; i++)
	{
		if (WDC_AddrSpaceIsActive(pDev, i))
		{
			return TRUE;
		}
	}
	ErrLog("Device does not have any active memory or I/O address spaces\n");
	return FALSE;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
BOOL HwaFcDriverIsIntEnabled(WDC_DEVICE_HANDLE hDev)
{
	FC_DBG_FUNC_CALL;
	if (!IsValidDevice((PWDC_DEVICE)hDev, "HwaFcDriver_IntIsEnabled"))
		return FALSE;

	return WDC_IntIsEnabled(hDev);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item)
{
	DWORD i;
	DWORD dwNumItems = pDev->cardReg.Card.dwItems;
	FC_DBG_FUNC_CALL;

	for (i=0; i<dwNumItems; i++)
	{
		if (pDev->cardReg.Card.Item[i].item == item)
			return TRUE;
	}

	return FALSE;
}
#endif

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverRead32(THwaFcDrvHandle nDrv,THwaBar eBar, unsigned int nAddr)
{
	unsigned int nReadVal = 0;
#if 0
	FC_DBG_FUNC_CALL;
#else
	HWA_FC_DBG("Read Addr 0x%x\n",nAddr);
#endif
#ifdef WINDOWS_OPS
	WDC_ReadAddr32(nDrv.pDevHandle,eBar,nAddr,&nReadVal);

#elif defined VXWORKS_OPS
	HWA_FC_READ_32(nDrv,eBar,nAddr,&nReadVal);
#else
	#error "unknown operation system."	
#endif
	return nReadVal;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverWrite32(THwaFcDrvHandle nDrv,THwaBar eBar, unsigned int nAddr,unsigned int nValue)
{
	FC_DBG_FUNC_CALL;
#ifdef WINDOWS_OPS
	WDC_WriteAddr32(nDrv.pDevHandle,eBar,nAddr,nValue);

#elif defined VXWORKS_OPS
	HWA_FC_WRITE_32(nDrv,eBar,nAddr,nValue);
#else
	#error "unknown operation system."	
#endif	
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
extern int nsend;
extern int nrecv;
extern int reset_send;
extern int reset_recv;
inline void HwaFcDriverDmaDone(THwaFcDevList *pDevCtx,int nLen)
{
	unsigned int nValue = 0;
	int i = 0;
	int x = 0;
	static int test_num = 0;

	FC_DBG_FUNC_CALL;
	test_num++;

	do{
		for(i = 0 ; i < nLen; i++)
		{
			x++;
		}
		HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_DONE,&nValue);
		if (nHwaFcDev[1].nExitFlag == 1)
		{
			printf("send = %d  recv = %d\n",nsend,nrecv);
			printf("reset_send = %d ,reset_recv = %d\n",reset_send,reset_recv);
		}
	}while(!(nValue & PCIE_DMA_SEND_DONE));

	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_SEND_RST);
	//HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_SEND_NRST);
	reset_send++;
	

}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
inline void HwaFcDriverRecvDmaDone(THwaFcDevList *pDevCtx)
{
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;
	do{
		HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_DONE,&nValue);
		//HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_RECV_DONE,&nValue);

	}while(!(nValue & PCIE_DMA_RECV_DONE));
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RECV_RST);
	//HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RECV_NRST);
}


#ifdef WINDOWS_OPS
	#include "HwaFcDriverWin.cpp"
#elif defined VXWORKS_OPS
	#include "HwaFcDriverVxWorks.c"
#else
#error "unknown operation system."
#endif

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverInitDevStr(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	int j;
	int i;
	FC_DBG_FUNC_CALL;
	if(!pDevCtx->nVailed)
	{
		ErrLog("%s","Card not valid.");
		return HWA_FC_ERR_PARA;
	}
/*set default value*/	
	pDevCtx->eFcDevMode = eModeNormal;
	pDevCtx->eFcDevSpeed = eSpeed1G;
	pDevCtx->eMonStat = eMonStop;
	pDevCtx->nMonCfg.nMonThresdHold = 4*1024;//4M
	pDevCtx->nMonCfg.nWorkMode = eMonFilter;
	pDevCtx->nMonCfg.nMonTimedOut = 1000*1000*200; //200ms,max value is 1s	
/*clear all port cfg*/
	memset(pDevCtx->nSendPort,0,sizeof(pDevCtx->nSendPort));
	memset(pDevCtx->nSendPortMask,0,sizeof(pDevCtx->nSendPortMask));
	memset(pDevCtx->nRecvPort,0,sizeof(pDevCtx->nRecvPort));
	memset(pDevCtx->nRecvPortMask,0,sizeof(pDevCtx->nRecvPortMask));

	memset(pDevCtx->nSendPhyPort,0,sizeof(pDevCtx->nSendPhyPort));
	memset(pDevCtx->nSendPhyPortMask,0,sizeof(pDevCtx->nSendPhyPortMask));
	memset(pDevCtx->nRecvPhyPort,0,sizeof(pDevCtx->nRecvPhyPort));
	memset(pDevCtx->nRecvPhyPortMask,0,sizeof(pDevCtx->nRecvPhyPortMask));
/*clear all filter cfg*/
	memset(pDevCtx->nMonFilter,0,sizeof(pDevCtx->nMonFilter));
	memset(pDevCtx->nMonFilterMask,0,sizeof(pDevCtx->nMonFilterMask));
/*clear src ID*/
	memset(&pDevCtx->nSrcId,0,sizeof(THwaFcDeviceID));
	///////////////////////////physis init/////////////////////////////////////

	for(j = 0;j < HWA_FC_MAX_PORT_CNT;j++)
	{
		
		for(i = 0; i < TX_PAYLOAD_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_data(i),0);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_addr,j);
		/*config fragment */ 
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_data,0);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_addr,j);
	}
	for(j = 0;j < HWA_FC_MAX_PORT_CNT;j++)
	{
		for(i = 0; i < RXANA_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_data(i),0);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_addr,j);
	}

	//////////////////////////////////////////////////////////////////////////
/*init Lock and event*/
	if((pDevCtx->nHwaFcDrvHandle.nDmaLock == HWA_FC_INVALIED_HANDLE) ||(pDevCtx->nHwaFcDrvHandle.nDmaLock == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nDmaLock = HwaFcInitLock("HWA-FC-NIC-DMA-LOCK",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nHwaFcDrvHandle.nPortLock == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nHwaFcDrvHandle.nPortLock == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nPortLock = HwaFcInitLock("HWA-FC-NIC-PORT-LOCK",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nHwaFcDrvHandle.nDmaEvent == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nHwaFcDrvHandle.nDmaEvent == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nDmaEvent = HwaFcInitEvent("HWA-FC-NIC-DMA-EVENT",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nHwaFcDrvHandle.nRecvEvent == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nHwaFcDrvHandle.nRecvEvent == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nRecvEvent = HwaFcInitEvent("HWA-FC-NIC-RECV-EVENT",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nHwaFcDrvHandle.nLinkEvent == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nHwaFcDrvHandle.nLinkEvent == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nLinkEvent = HwaFcInitEvent("HWA-FC-NIC-LINK-EVENT",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nSendLock == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nSendLock == NULL))
	{
		if((pDevCtx->nSendLock = HwaFcInitLock("HWA-FC-NIC-SEND-LOCK",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nRecvLock == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nRecvLock == NULL))
	{
		if((pDevCtx->nRecvLock  = HwaFcInitLock("HWA-FC-NIC-RECV-LOCK",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nRecvIntrLock == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nRecvIntrLock == NULL))
	{
		if((pDevCtx->nRecvIntrLock  = HwaFcInitLock("HWA-FC-NIC-RECVINTR-LOCK",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}

	if((pDevCtx->nHwaFcDrvHandle.nTrigEvent == HWA_FC_INVALIED_HANDLE)||(pDevCtx->nHwaFcDrvHandle.nTrigEvent == NULL))
	{
		if((pDevCtx->nHwaFcDrvHandle.nTrigEvent= HwaFcInitEvent("HWA-FC-NIC-TRIGGER-EVENT",pDevCtx->nDevInfo.sFcSerial)) == HWA_FC_INVALIED_HANDLE)
		{
			return HWA_FC_ERR_INIT;
		}
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDeInitDevStr(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
	pDevCtx->eMonStat = eMonStop;
	pDevCtx->nExitFlag = 1;
#ifdef WINDOWS_OPS
	if((pDevCtx->nRecvThreadId != HWA_FC_INVALIED_HANDLE) && 
		(pDevCtx->nRecvThreadId != NULL))
	{
		SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
		WAIT_EVENT(pDevCtx->nRecvThreadId);
	}
	if(pDevCtx->nLoginThreadId != HWA_FC_INVALIED_HANDLE && pDevCtx->nLoginThreadId != NULL)
	{
		WAIT_EVENT(pDevCtx->nLoginThreadId);
	}
#endif	/*DeInit DMA information*/
	if(nCardNum)
	{
		if(HwaFcDriverCloseDma(pDevCtx) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_UNINIT;
		}
	}
	/*DeInit Lock and event*/
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nPortLock);
	SAFE_CLOSE(pDevCtx->nSendLock);
	SAFE_CLOSE(pDevCtx->nRecvLock);
	SAFE_CLOSE(pDevCtx->nRecvIntrLock);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nTrigEvent);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nTrigEvent);
	SAFE_CLOSE(pDevCtx->nHwaFcDrvHandle.nLinkEvent);
	pDevCtx->nExitFlag = 0;
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverRstFpga(int nCardNum)
{
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;
/*write reset reg*/
	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_device_soft_reset,RST_FPGA);
	
	do{
		nValue = HwaFcDriverRead32(FC_DEV(nCardNum),eBar0,FPGA_REG_device_state);
		if(nValue == 0xffffffff)
		{
			return HWA_FC_ERR_INIT;
		}
		SLEEP(100);
	}while((nValue & RST_FPGA_SUCC_MASK) != RST_FPGA_SUCC);

	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:add by sxy 20130416 for normal blocked recv and single packet recv 
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetCardRecvMode(int nCardNum,THwaFcRecvMode eRecvMode )
{
	if (eRecvMode == eNormalRecvBlk)
	{
		HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_Rx_rec_Mode,eNormalRecvBlk);
	}
	else
	{
		HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_Rx_rec_Mode,eNormalRecvNor);
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetCardMode(int nCardNum,THwaFcDevMode eMode)
{
		unsigned int nValue = 0;
		THwaFcDevMode eModeTmp = eDevModeNr;
		FC_DBG_FUNC_CALL;
		CHECK_CARD_ST(nCardNum);

/*get hardware mode*/
#if 1
		nValue = HwaFcDriverRead32(FC_DEV(nCardNum),eBar0,FPGA_REG_device_version);
		if((nValue >> 24 & 0xff) < 0x3){
			eModeTmp = eModeNormal;
		}else{
			eModeTmp = eModeCapture;
		}
		if(eMode != eModeTmp)
		{
				ErrLog("%s","Config mode and hardware mode not match.");
				return HWA_FC_ERR_XML;
		}
#endif
		return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetCardSpeed(int nCardNum,THwaFcDevSpeed eSpeed)
{
	int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	switch(eSpeed)
	{
		case eSpeed1G:
				nValue = eSpeed1G;
				break;
		case eSpeed2G:
				nValue = eSpeed2G;
				break;
		default:
				nValue = eSpeedNr;
			break;
	}
	if(nValue == eSpeedNr)
	{
		ErrLog("%s","Unknown device speed.");
		return HWA_FC_ERR_CFG;
	}
/*Stop FC*/
	HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_FC_start,0x0);
/*set FC Speed*/
	//HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_FC_speed,nValue);
	HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_FC_speed,0x3);//4G FC

/*set FC start*/
	HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_FC_start,0x1);
/*set rm mode -add wenxiaobo0409*/
	HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_Rm_mode,0x3);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetIrigMode(int nCardNum,THwaFcIrigMode eMode)
{
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;	
	CHECK_CARD_ST(nCardNum);

	switch(eMode)
	{
	case eModeMaster:
		nValue = IRIG_MASTER;
		break;
	case eModeSlave:
		nValue = IRIG_SLAVER;
		break;
	default:
		nValue = eIrigModeNr;
		break;
	}
	if(nValue == eIrigModeNr)
	{
		ErrLog("%s","Unknown IRIG-B mode.");
		return HWA_FC_ERR_CFG;
	}
	HwaFcDriverWrite32(FC_DEV(nCardNum),eBar0,FPGA_REG_IRIG_B_set,nValue);
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetIoLevel(int nCardNum,int nLevel)
{
	int nAddr = FPGA_REG_set_out_io_addr;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CHECK_CARD_ST(nCardNum);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,nAddr,nLevel);
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetIoLevel(int nCardNum,int *pLevel)
{
	int nAddr = FPGA_REG_get_in_io_addr;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CHECK_CARD_ST(nCardNum);
	if(pLevel == NULL)
	{
		ErrLog("Parameter error.");
		return HWA_FC_ERR_PARA;
	}
	HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,nAddr,(unsigned int *)pLevel);
	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetCaptureLoopMode(int nCardNum,THwaFcLoopMode eMode)
{	
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_reset_ctrl,0x100);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_FC_start,0x0);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_loopback_en,(unsigned int) eMode);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_reset_ctrl,0x0);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_FC_start,0x1);
	return 0;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverLoadMonCfg(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_Mon_intr_rx_threshold,pDevCtx->nMonCfg.nMonThresdHold);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_Mon_work_mode,pDevCtx->nMonCfg.nWorkMode);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_Mon_intr_rx_timedout,pDevCtx->nMonCfg.nMonTimedOut);	
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetSendCfg(int nCardNum,THwaFcPortHandle nPortHandle,unsigned int nSendLen,THwaSendCfg nCfg)
{
	THwaFcPort *pPort = NULL;
	unsigned int nCfgVal = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
#if 0
	if(nHwaFcDev[nCardNum].eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}
#endif

	pPort = &nHwaFcDev[nCardNum].nSendPort[nPortHandle];
	if(((pPort->ePortTye == eTypeNormal) && (nSendLen > HWA_FC_ASM_MAX_PAYLOAD))
		||((pPort->ePortTye == eTypeSpecial) &&(nSendLen > HWA_FC_NASM_MAX_PAYLOAD))
		||((pPort->ePortTye == eTypeBlock) && (nSendLen > HWA_FC_BLK_MAX_PAYLOAD))
		||(nSendLen < 0))
	{
		ErrLog("Payload length out of range.");
		return HWA_FC_ERR_PARA;
	}
	if(pPort->nPortStat == 0)
	{
		ErrLog("%s","Port Not been Created.");
		return HWA_FC_ERR_SEND;
	}
	nCfgVal =	(((nCfg.nIsAsm & TX_FLAG_MSG_TYPE_MASK) << TX_FLAG_MSG_TYPE_BIT) | 
				((nCfg.nChSel & TX_FLAG_PORT_SEL_MASK) << TX_FLAG_PORT_SEL_BIT) |
				((nCfg.nPktPri & TX_FLAG_PORT_SEL_MASK) << TX_FLAG_PRI_BIT) |
				((pPort->nPhyIndex & TX_FLAG_PORT_INDEX_MASK) << TX_FLAG_PORT_INDEX_BIT));
/*set tx port id
	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_TX_port_id,pPort->nPhyIndex);
*/

/*set tx flag*/
	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_TX_flag,nCfgVal);
/*set tx_frame len*/
	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_TX_frame_len,nSendLen);
	
	return HWA_FC_OP_OK;

}

int HwaFcDriverGetMibs(int nCardNum,THwaFcMib eMibs,THwaFcPortSel ePortType, void *pBuf)
{
	unsigned int nSysMibAddr[eSysMibNr] = {
		FPGA_REG_RX_frame_short_err,
		FPGA_REG_RX_frame_long_err,
		FPGA_REG_RX_frame_crc_err,
		FPGA_REG_RX_frame_err,
		FPGA_REG_RX_frame_drop,
	};
	unsigned int *pData = (unsigned int *)pBuf;
	unsigned int nMibCnt  = 0;
	unsigned int nMibAddr = 0;
	unsigned int nMibData = 0;
	unsigned int i = 0;
	FC_DBG_FUNC_CALL;
	
	CHECK_CARD_ST(nCardNum);


	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}

/*get system MIB Info*/
	if(eMibs == eMibSys)
	{
		unsigned int i = 0;
		for(;i < eSysMibNr; i++)
		{
			HWA_FC_READ_32(FC_DEV(nCardNum),eBar0,nSysMibAddr[i],&pData[i]);
		}
		return HWA_FC_OP_OK;
	}

/*get port mib info*/
	if(ePortType == ePortA)
	{
		nMibAddr = FPGA_REG_FCA_MIB_addr;
		nMibData = FPGA_REG_FCA_MIB_data;
	}else if(ePortType == ePortB)
	{
		nMibAddr = FPGA_REG_FCB_MIB_addr;
		nMibData = FPGA_REG_FCB_MIB_data;
	}else{
		ErrLog("Unknown port type.");
		return HWA_FC_ERR_PARA;
	}

	if(eMibs == eMibTx)
	{
		nMibCnt = eTxMibNr - eTxMib0;
	}else if(eMibs == eMibRx)
	{
		nMibCnt = eRxMibNr;
	}else{
		ErrLog("Unknown MIB type.");
		return HWA_FC_ERR_PARA;
	}
	for( i = 0; i < nMibCnt; i++)
	{
		HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,nMibAddr,((eMibs == eMibTx) ? (i + eTxMib0) : i));
		HWA_FC_READ_32(FC_DEV(nCardNum),eBar0,nMibData,&pData[i]);
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverClearMib(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];

	CHECK_CARD_ST(nCardNum);


	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_mibs_clear,CLEAR_MIB);
	SLEEP(1);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_mibs_clear,CLEAR_MIB_STOP);
	return HWA_FC_OP_OK;
}




/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverCheckOverFlow(int nCardNum,THwaFcPortHandle nPortHandle,THwaFcPortSel ePort)
{
	unsigned int nVal = 0;
	static int a =0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
#if 0 //not have ddr
	do{
		HWA_FC_READ_32(FC_DEV(nCardNum),eBar0,FPGA_REG_TX_buf_overflow,&nVal);
		if((nVal&0x08) == 0)
		{
			a++;
 			if (a == 4)
 			{
 				//printf("\a");
				HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,0x28,0xcccccccc);
				a = 0;

 			}
		}
	}while(!((nVal >>TX_OVER_FLOW_VAILD_BIT) & TX_OVER_FLOW_VAILD_MASK));
	a = 0;
	if(((nVal >> TX_OVER_FLOW_BIT) & TX_OVER_FLOW_MASK) == TX_OVER_FLOW)
	{
		ErrLog("Send data buffer over flow.");
		return TX_OVER_FLOW;
		//return HWA_FC_ERR_SEND_OVERFLOW;
	}
#ifdef NEW_TX_OVER_FORMAT
	switch(ePort)
	{
		case ePortAB:
			if(((nVal >> PORT_LINK_BIT) & PORT_LINK_MASK) != 0x3)
			{
				ErrLog("Port not link up.");
				return PORT_NOT_LINK;
			}
			break;
		case ePortA:
			if(((nVal >> PORT_LINK_BIT) & PORT_A_LINK_MASK) != 0x1)
			{
				ErrLog("Port not link up.");
				return PORT_NOT_LINK;
			}
			break;
		case ePortB:
			if(((nVal >> PORT_LINK_BIT) & PORT_B_LINK_MASK) != 0x2)
			{
				ErrLog("Port not link up.");
				return PORT_NOT_LINK;
			}
			break;
	}
#endif
#endif
	return 0;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSendData(int nCardNum,void *pBuf,int nSendLen)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	int nSendTmp = nSendLen;
	int nNeedSend = 0;
	int nSendTotal = 0;
	int nRemain = 0;
	nSendLen = nSendLen + ((nSendLen%4 == 0) ? 0 :(4 - nSendLen % 4));
#ifdef NEW_TX_OVER_FORMAT	
	if(nSendLen > MAX_SEND_LEN)
	{	
		nNeedSend = MAX_SEND_LEN;
		nRemain = nSendLen;
		while(nRemain  > 0)
		{
			memcpy((char *)pDevCtx->nHwaFcDrvHandle.pBufOut,(char *)((char *)(pBuf) + nSendTotal),nNeedSend);
			HwaFcDriverStartDma(pDevCtx,nNeedSend,eDirSend);
			nSendTotal += nNeedSend;
			nRemain = nSendLen - nSendTotal;
			nNeedSend = ((nRemain > MAX_SEND_LEN) ?  MAX_SEND_LEN : nRemain);
		}
	}else
#endif	
	{
		if(nSendLen > MAX_DMA_SIZE){
			ErrLog("%s","Send data length too large.\n");
			return HWA_FC_ERR_SEND;
		}

		memcpy(pDevCtx->nHwaFcDrvHandle.pBufOut,pBuf,nSendLen);
		HwaFcDriverStartDma(pDevCtx,nSendLen,eDirSend);
	}
	return nSendTmp;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverRecvData(int nCardNum,THwaFcPortHandle nPortHandle,void *pBuf,THwaFcRecvInfo  *pInfo,int nRecvLen,int nWaitTime)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	THwaFcPort *pPort = &nHwaFcDev[nCardNum].nRecvPort[nPortHandle];
	int  nRealLen = 0;
	int nMsgReadLen = 0;
	static int time = 0;
	THwaFcRecvMsg nMsg = {0};
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	if(pDevCtx->eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}

	if(IS_SIMULATE)
	{
		SPRINTF((char *)pBuf,nRecvLen,"%s","this is not a real data,it's created by simulate driver\n");
		pInfo->eMsgType = eTypeAsm;
		pInfo->ePort = ePortA;
		pInfo->nRecvTime.nHighTime= 0;
		pInfo->nRecvTime.nLowTime +=  10;
		SLEEP(10);
		return nRecvLen;
	}
#ifdef WINDOWS_OPS	
	if(HwaFcDriverReadMsg(pPort->nPortCfg.nRecvPortCfg.nPipeHandle,&nMsg,sizeof(nMsg),&nMsgReadLen,nWaitTime) != HWA_FC_OP_OK)
#elif defined VXWORKS_OPS
	if(HwaFcDriverReadMsg(pPort->nPortCfg.nRecvPortCfg.nRecvMsgHandle,&nMsg,sizeof(nMsg),&nMsgReadLen,nWaitTime) != HWA_FC_OP_OK)
#else
#error "unknown operation system."
#endif
	{
	  	ErrLog("%s","Receive data timed out.");
		return HWA_FC_READ_MSG_TIMEDOUT;
	}

	if(nMsgReadLen != sizeof(nMsg))
	{
		ErrLog("%s-%d","Got wrong message len.",nMsgReadLen);
		return HWA_FC_ERR_RECV;
	}
#if 1
	if((nRealLen = HwaFcDriverReadFifo(&(pPort->nPortCfg.nRecvPortCfg.nPortFifo),(char *)pBuf,nRecvLen,nMsg.nDataLen)) < 0)
	{
		return HWA_FC_ERR_RECV;
	}
	memcpy(&pInfo->nRecvTime,&nMsg.nRecvTime,sizeof(nMsg.nRecvTime));
	pInfo->eMsgType = (THwaFcMsgType)((nMsg.nMsgInfo >> RX_FLAG_TYPE_BIT) & RX_FLAG_TYPE_MASK);
	pInfo->ePort = (THwaFcPortSel)((nMsg.nMsgInfo >> RX_FLAG_CH_BIT) & RX_FLAG_CH_MASK);
#endif
	return nRealLen;
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverChangeTime(THwaFcSysTime *pSysTime,THwaFcDevTime *pDevTime,THwaFcTime *pFcTime,THwaFcTimeDir eDir)
{
	unsigned int nDays = pSysTime->wDay;
	unsigned int nDayList[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	int i = 0;
	FC_DBG_FUNC_CALL;
	switch(eDir)
	{
		case eSys2Dev:
			for(i = 0; i < (pSysTime->wMonth - 1);i++)
			{
				nDays += nDayList[i];
			}
			
			if((pSysTime->wMonth > 2) &&(LEAP_YEAR(pSysTime->wYear)))
			{
				nDays++;
			}
			SET_DAYS(pDevTime->nHighTime,nDays,pSysTime->wHour);
			SET_MIN(pDevTime->nLowTime,pSysTime->wMinute,pSysTime->wSecond,pSysTime->wMilliseconds,0);
			break;
		case eDev2Fc:
				pFcTime->nDays = HWA_FC_DAY(pDevTime->nHighTime);
				pFcTime->nHours = HWA_FC_HOUR(pDevTime->nHighTime);
				pFcTime->nMins = HWA_FC_MIN(pDevTime->nLowTime);
				pFcTime->nSecs = HWA_FC_SEC(pDevTime->nLowTime);
				pFcTime->nMsecs = HWA_FC_MSEC(pDevTime->nLowTime);
				pFcTime->nUsecs = HWA_FC_USEC(pDevTime->nLowTime);
			break;
		default:
			return HWA_FC_ERR_PARA;
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetTime(int nCardNum)
{
	THwaFcDevTime nDevTime = {0};
	THwaFcSysTime nSystime = {0};
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}

	CHECK_CARD_ST(nCardNum);
#ifdef WINDOWS_OPS
	GetLocalTime(&nSystime);
#else
	GetSystemTime(&nSystime);
#endif
	if(HwaFcDriverChangeTime(&nSystime,&nDevTime,NULL,eSys2Dev) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_PARA;
	}

	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_host_time1,nDevTime.nLowTime);
	HWA_FC_WRITE_32(FC_DEV(nCardNum),eBar0,FPGA_REG_host_time2,nDevTime.nHighTime);
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetTime(int nCardNum,THwaFcTime *pTime)
{
	THwaFcDevTime nDevTime = {0};
	FC_DBG_FUNC_CALL;

	CHECK_CARD_ST(nCardNum);

	HWA_FC_READ_32(FC_DEV(nCardNum),eBar0,FPGA_REG_host_time1,&nDevTime.nLowTime);
	HWA_FC_READ_32(FC_DEV(nCardNum),eBar0,FPGA_REG_host_time2,&nDevTime.nHighTime);

	if(HwaFcDriverChangeTime(NULL,&nDevTime,pTime,eDev2Fc) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_PARA;
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverLoadCfg(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	THwaFcPortCfg nSpePort[] =
	{
		{HWA_FC_SPE_PORT_S_INDEX,    eTypeSpecial,eDirSend,	0,	0, 0,	ePortA,		0,	  0,   0,  0,	 0,    0, HWA_FC_NASM_MAX_PAYLOAD},
		{HWA_FC_SPE_PORT_R_INDEX,    eTypeSpecial,eDirRecv,	0,	0, 0,	ePortAB,	0,	  0,   0,  0,	 0,    0, HWA_FC_NASM_MAX_PAYLOAD},
	};
	FC_DBG_FUNC_CALL;

	CHECK_CARD_ST(nCardNum);


	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}

	if(HwaFcDriverRstFpga(nCardNum) != HWA_FC_OP_OK)
	{
		ErrLog("FPGA reset failed.\n");
		return HWA_FC_ERR_INIT;
	}
	

/*set card mode*/
#if 1
	if(HwaFcDriverSetCardMode(nCardNum,pDevCtx->eFcDevMode)!= HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_CFG;
	}
#endif
/*set card speed*/
	if(HwaFcDriverSetCardSpeed(nCardNum,pDevCtx->eFcDevSpeed) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_PARA;
	}
/*set IRIG-B Mode speed*/
	if(HwaFcDriverSetIrigMode(nCardNum,pDevCtx->eFcIrigMode) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_PARA;
	}
/*set system time*/
	if(HwaFcDriverSetTime(nCardNum) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_INIT;
	}
/*set card nomal recv mode(single or block)*/       //add by sxy 20130146
	if (pDevCtx->eFcDevMode == eModeNormal)
	{
			HwaFcDriverSetCardRecvMode(nCardNum,pDevCtx->eRecvMode);

	}

/*if is monitor mode ,set monitor cfg*/
	if(pDevCtx->eFcDevMode == eModeCapture)
	{
		if(HwaFcDriverLoadMonCfg(nCardNum) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_INIT;
		}
	}

/*write cfg done*/
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_Cfg_done,CFG_DONE);
/*set a DMA rst before strart recv*/
	HwaFcDriverWriteReg(nCardNum,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);	
	//HwaFcDriverWriteReg(nCardNum,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
	//reset_send++;
	//reset_recv++;
/*enable recv interrupt*/
#ifndef SUPPORT_POLL
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_RX_intr_poll_set,0x1);
#endif
#if 1
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_intr_RX_frame_en,RECV_INTR_ENABLE);
#endif

#ifdef SUPPORT_LOGIN
	if(pDevCtx->eFcDevMode != eModeCapture)
	{
/*add and open special port for login */
		HwaFcAddPort(nCardNum,nSpePort,2);
		HwaFcDriverCreateRecvPort(nCardNum,HWA_FC_SPE_PORT_R_INDEX,&pDevCtx->nSpeRecvPort);
		HwaFcDriverCreateSendPort(nCardNum,HWA_FC_SPE_PORT_R_INDEX,&pDevCtx->nSpeSendPort);
	}
#endif
	return HWA_FC_OP_OK;
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSecureICRead(int nCardNum,unsigned int nSecureAddr,unsigned int *pData)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

/*write addr to hardware*/
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_secure_rd_addr,nSecureAddr);
	SLEEP(100);
/*read data*/
	*pData = HwaFcDriverReadReg(nCardNum,eBar0,FPGA_REG_secure_rd_data);
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSecureICWrite(int nCardNum,unsigned int nSecureAddr,unsigned int nValue)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

/*write addr to hardware*/
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_secure_wr_addr,nSecureAddr);
/*write data*/
	HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_secure_rd_data,nValue);
	return HWA_FC_OP_OK;
}


#ifdef WINDOWS_OPS

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverMonRecvData(int nCardNum,void *pBuf,int nRecvLen,int nWaitTime,int *pDataType)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int  nRealLen = 0;
	int nMsgReadLen = 0;
	THwaFcMonRecvMsg nMsg = {0};
	CHECK_CARD_ST(nCardNum);

	if(pDevCtx->eFcDevMode != eModeCapture)
	{
		ErrLog("Work mode not support.");
		return HWA_FC_ERR_PARA;
	}

	if(pDevCtx->eMonStat != eMonRunning)
	{
		if(pDevCtx->eMonStat == eMonStop)
		{
			ErrLog("Receive data not be started.");
		}else{
			ErrLog("Receive data was paused.");
		}
		return HWA_FC_ERR_RECV;
	}

	if(IS_SIMULATE)
	{
		struct stat nFilestat;
		static int nRecvCnt = 0;
		if(stat(PKT_FILE_NAME,&nFilestat) != 0)
		{
			ErrLog("Get packet data failed.");
			return HWA_FC_ERR_RECV;
		}
		if(nRecvLen >= nFilestat.st_size)
		{
			int nFd = _open(PKT_FILE_NAME,O_RDONLY | O_BINARY);
			if(nFd == 0)
			{
				ErrLog("Open packet file failed.");
				return HWA_FC_ERR_RECV;
			}
			if(_read(nFd,pBuf,nFilestat.st_size) != nFilestat.st_size)
			{
				ErrLog("Open packet file failed.");
				_close(nFd);
				return HWA_FC_ERR_RECV;
			}
			_close(nFd);
			if(((nRecvCnt++) % 13 == 0) && (nRecvCnt != 0))
			{
				*pDataType = MON_DATA_REAL;
			}else{
				*pDataType = MON_DATA_NOT_REAL;
			}
			return nFilestat.st_size;
		}else{
			ErrLog("No enough buffer.");
			return HWA_FC_ERR_RECV;
		}
	}

	if(HwaFcDriverReadMsg(pDevCtx->nMonRecvPort.nPipeHandle,&nMsg,sizeof(nMsg),&nMsgReadLen,nWaitTime) != HWA_FC_OP_OK)
	{
		return 0;
	}

	if(nMsgReadLen != sizeof(nMsg))
	{
		ErrLog("Got wrong message.");
		return HWA_FC_ERR_RECV;
	}
#if 0
	if((nRealLen = HwaFcDriverReadFifo(&(pDevCtx->nMonRecvPort.nMonFifo),(char *)pBuf,nRecvLen,nMsg.nDataLen)) < 0)
	{
		return HWA_FC_ERR_RECV;
	}
	*pDataType = nMsg.nDataType;
	return nRealLen;
#else
	nRealLen = (nRecvLen < nMsg.nDataLen ? nRecvLen : nMsg.nDataLen);
	memcpy(pBuf,(void *)nMsg.nAddr,nRealLen);
	*pDataType = nMsg.nDataType;
	SetEvent(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[nMsg.nDmaIndex]);
	return nRealLen;
#endif
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverMonStartRecv(int nCardNum)
{
	int ret = 0;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);


	if(pDevCtx->eFcDevMode != eModeCapture)
	{
		ErrLog("Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
	
	if((pDevCtx->eMonStat != eMonStop) && (pDevCtx->eMonStat != eMonPause))
	{
		ErrLog("Receive data is been started.");
		return HWA_FC_ERR_PARA;
	}

	if(pDevCtx->eMonStat == eMonStop)
	{
/*init msg*/
		SET_MSG_NAME(pDevCtx->nMonRecvPort.nMsgName,nHwaFcDev[nCardNum].nDevInfo.sFcSerial,0xffff);
#ifdef WINDOWS_OPS
		if((pDevCtx->nMonRecvPort.nPipeHandle = HwaFcDriverCreateMsg(pDevCtx->nMonRecvPort.nMsgName)) == NULL)
		{
			return HWA_FC_ERR_MSG;
		}
#endif
		
		if((pDevCtx->nMonRecvPort.nMonRecvHandle = HwaFcDriverOpenMsg(pDevCtx->nMonRecvPort.nMsgName)) == NULL)
		{
			GOTO_LABEL(ERR_PIPE,HWA_FC_ERR_MSG);
		}

		SET_LOCK_NAME(pDevCtx->nMonRecvPort.nMonFifo.sLockName,nHwaFcDev[nCardNum].nDevInfo.sFcSerial,0xffff);
		if(HwaFcDriverInitFifo(&pDevCtx->nMonRecvPort.nMonFifo,HWA_FC_BLOCK_FIFO_SIZE) != HWA_FC_OP_OK)
		{
			GOTO_LABEL(ERR_FIFO,HWA_FC_ERR_FIFO);
		}
	}
	if(!IS_SIMULATE)
	{
		HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_Mon_RX_START,MON_START);
	}
	pDevCtx->eMonStat = eMonRunning;
	goto DONE;
ERR_FIFO:
	SAFE_CLOSE(pDevCtx->nMonRecvPort.nMonRecvHandle);
ERR_PIPE:
	SAFE_CLOSE(pDevCtx->nMonRecvPort.nPipeHandle);
DONE:
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverMonPauseRecv(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	if(pDevCtx->eFcDevMode != eModeCapture)
	{
		ErrLog("Work mode not support.");
		return HWA_FC_ERR_PARA;
	}

	if(pDevCtx->eMonStat != eMonRunning)
	{
		ErrLog("Receive data isn't running.");
		return HWA_FC_ERR_PARA;
	}
	if(!IS_SIMULATE)
	{
		HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_Mon_RX_START,MON_STOP);
	}
	pDevCtx->eMonStat = eMonPause;
	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverMonStopRecv(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);


	if(pDevCtx->eFcDevMode != eModeCapture)
	{
		ErrLog("Work mode not support.");
		return HWA_FC_ERR_PARA;
	}

	if((pDevCtx->eMonStat != eMonRunning) && (pDevCtx->eMonStat != eMonPause))
	{
		ErrLog("Receive data not been start.");
		return HWA_FC_ERR_PARA;
	}
	
	if(!IS_SIMULATE)
	{
		HwaFcDriverWriteReg(nCardNum,eBar0,FPGA_REG_Mon_RX_START,MON_STOP);
	}
	SAFE_CLOSE(pDevCtx->nMonRecvPort.nMonRecvHandle);
	SAFE_CLOSE(pDevCtx->nMonRecvPort.nPipeHandle);
	HwaFcDriverDeInitFifo(&pDevCtx->nMonRecvPort.nMonFifo);
	pDevCtx->eMonStat = eMonStop;
	SetEvent(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[0]);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetMonCfg(int nCardNum, THwaFcMonCfg nCfg)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

#if 0
	if(pDevCtx->eFcDevMode != eModeCapture)
	{
		ErrLog("Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
#endif
	if(nCfg.nMonThresdHold > MAX_DMA_SIZE)
	{
		ErrLog("Error capture thread hold value.");
		return HWA_FC_ERR_PARA;
	}
	if((nCfg.nWorkMode < eMonSelect) || (nCfg.nWorkMode >= eMonModeNr))
	{
		ErrLog("Error capture mode value.");
		return HWA_FC_ERR_PARA;
	}
	if((nCfg.nMonTimedOut <= 0) ||(nCfg.nMonTimedOut > MAX_TIMED_OUT))
	{
		ErrLog("Timed out value out of range.");
		return HWA_FC_ERR_PARA;
	}
	pDevCtx->nMonCfg.nMonThresdHold = nCfg.nMonThresdHold;
	pDevCtx->nMonCfg.nWorkMode = nCfg.nWorkMode;
	pDevCtx->nMonCfg.nMonTimedOut = nCfg.nMonTimedOut;
	return HWA_FC_OP_OK;
	
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverFindUnusedFilter(THwaFcDevList *pDevCtx)
{
	int i = 0;
	int j = 0;
	FC_DBG_FUNC_CALL;
	for(i =0 ; i < HWA_FC_FILTER_MASKCNT; i++)
	{
		if(pDevCtx->nMonFilterMask[i] != 0xffffffff)
		{
			for(j = 0; j < 32; j++)
			{
				if((pDevCtx->nMonFilterMask[i] & (0x1 << j)) == 0)
				{
					return i*32 + j;
				}
			}
		}
	}
	return -1;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverUseFilter(THwaFcDevList *pDevCtx,int nFilterIndex)
{
	int i = 0;
	int j = 0;
	FC_DBG_FUNC_CALL;
	pDevCtx->nMonFilterMask[nFilterIndex / 32] |= (0x1 << (nFilterIndex % 32));
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverUnuseFilter(THwaFcDevList *pDevCtx,int nFilterIndex)
{
	int i = 0;
	int j = 0;
	FC_DBG_FUNC_CALL;
	pDevCtx->nMonFilterMask[nFilterIndex / 32] &= (~(0x1 << (nFilterIndex % 32)));
	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverAddMonFilter(int nCardNum,THwaFcMonFilter nFilter)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nFilterValue[RXANA_DATA_CNT] = {0};
	int i = 0;
	unsigned int index=0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	nFilterValue[0] |= (0x1 & RX_ANA_ENABLE_MASK) << RX_ANA_ENABLE_BIT; //used 1 bit ,left 31
	nFilterValue[0] |= (nFilter.nPortSel & RX_ANA_CH_SEL_MASK) << RX_ANA_CH_SEL_BIT; //used 3bit,left 29
	//res in filter mode
	nFilterValue[0] |= (0x0 & RX_ANA_FRG_OFF_MASK) << RX_ANA_FRG_OFF_BIT; //used 4.left 28
	nFilterValue[0] |= (0x0 & RX_ANA_FRG_SEQ_MASK) << RX_ANA_FRG_SEQ_BIT; //used 5.left 27
	//res 3   used 8,left 24
	nFilterValue[0] |= (nFilter.nSid & RX_ANA_SID_MASK) << RX_ANA_SID_BIT;	//used 32,left 0

	nFilterValue[1] |= (nFilter.nDid & RX_ANA_DID_MASK) << RX_ANA_DID_BIT; //used 24,left 8
	nFilterValue[1] |= (nFilter.nType & RX_ANA_TYPE_MASK) << RX_ANA_TYPE_BIT; //used 32,left 0

	nFilterValue[2] |= (nFilter.nMsgId & RX_ANA_MSG_ID_MASK) << RX_ANA_MSG_ID_BIT; //used 32,left 0

	nFilterValue[3] |= (nFilter.nOxId & RX_ANA_OXID_MASK) << RX_ANA_OXID_BIT; //used 16bit,left 16
	nFilterValue[3] |= (nFilter.nSeqId & RX_ANA_SEQID_MASK) << RX_ANA_SEQID_BIT; //used 24 bit,left 0

	for(i = 0; i < HWA_FC_FILTER_FILED_CNT; i++)
	{
		nFilterValue[3] |= (((nFilter.nFilterFiledSel[i] == 1) ? 0x1 : 0x0) << (24 + i));
	}
	
	if((index = HwaFcDriverFindUnusedFilter(pDevCtx)) < 0)
	{
		ErrLog("Too many filters.");
		return HWA_FC_ERR_PARA;
	}
	for(i = 0; i < RXANA_DATA_CNT; i++)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_data(i),nFilterValue[i]);
	}
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_addr,index);

	memcpy(&pDevCtx->nMonFilter[index],&nFilter,sizeof(nFilter));
	HwaFcDriverUseFilter(pDevCtx,index);
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDelMonFilter(int nCardNum,unsigned int nIndex)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nFilterValue[RXANA_DATA_CNT] = {0};
	int i = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
	if((nIndex < 0) || (nIndex > HWA_FC_MAX_FILTERCNT))
	{
		ErrLog("Filter index out of range.");
		return HWA_FC_ERR_PARA;
	}
	if(!IS_SIMULATE)
	{
	for(i = 0; i < RXANA_DATA_CNT; i++)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_data(i),nFilterValue[i]);
	}
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_addr,nIndex);
	}
	memset(&pDevCtx->nMonFilter[nIndex],0, sizeof(THwaFcMonFilter));
	HwaFcDriverUnuseFilter(pDevCtx,nIndex);
	return HWA_FC_OP_OK;
}
#endif

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverReadReg(int nCardNum,THwaBar eBar,unsigned int nAddr)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}
	return HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar,nAddr);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverWriteReg(int nCardNum,THwaBar eBar,unsigned int nAddr,unsigned int nValue)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	
	if(nCardNum == 0)
	{
		return HWA_FC_OP_OK;
	}
	
	return HwaFcDriverWrite32(pDevCtx->nHwaFcDrvHandle,eBar,nAddr,nValue);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverReadCfgReg(int nCardNum,unsigned int nAddr,void *pData,unsigned int nBytes)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);


	if(nBytes !=0 && (pData == NULL))
	{
		return HWA_FC_ERR_PARA;
	}

	if(pDevCtx->nDevInitFlag == 0)
	{
		return HWA_FC_ERR_PARA;
	}
	
	if(nCardNum == 0)
	{
		return HWA_FC_OP_OK;
	}

#ifdef WINDOWS_OPS
	WDC_PciReadCfg(pDevCtx->nHwaFcDrvHandle.pDevHandle,nAddr,pData,nBytes);
#elif defined VXWORKS_OPS
	pciConfigInLong(pDevCtx->nDevInfo.nBusNum,pDevCtx->nDevInfo.nDeviceID,pDevCtx->nDevInfo.nFuncNum,nAddr,(unsigned int *)pData);
#else
#error "unkonwn operation system"
#endif
	return 0;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverWriteCfgReg(int nCardNum,unsigned int nAddr,void *pData,unsigned int nBytes)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);

	
	if(nCardNum == 0)
	{
		return HWA_FC_OP_OK;
	}

#ifdef WINDOWS_OPS
	return WDC_PciWriteCfg(pDevCtx->nHwaFcDrvHandle.pDevHandle,nAddr,pData,nBytes);
#elif defined VXWORKS_OPS
	nValue = *((int *)pData);
	pciConfigOutLong(pDevCtx->nDevInfo.nBusNum,pDevCtx->nDevInfo.nDeviceID,pDevCtx->nDevInfo.nFuncNum,nAddr,nValue);
#endif
	return 0;
}


/*port manager*/
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverFindPortByID(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle,THwaFcPortDir eDir)
{
	int i = 0 ;
	THwaFcPort *pPortList = (eDir == eDirRecv) ? nHwaFcDev[nCardNum].nRecvPort : nHwaFcDev[nCardNum].nSendPort;
	FC_DBG_FUNC_CALL;
	for(i = 0 ; i < HWA_FC_MAX_PORT_CNT; i++)
	{
		if(pPortList[i].nPortStat)
		{
			if(pPortList[i].nPortId == nPortId)
			{
				*pPortHandle = i;
				return HWA_FC_OP_OK;
			}
		}
	}
	ErrLog("%s","Port id not found.");
	return HWA_FC_ERR_PARA;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static void HwaFcDriverUnusePhyPort(THwaFcDevList *pDevCtx,int nPhyIndex,THwaFcPortDir eDir)
{
	unsigned int *pMask = (eDir == eDirRecv) ? pDevCtx->nRecvPhyPortMask : pDevCtx->nSendPhyPortMask;
	FC_DBG_FUNC_CALL;
	pMask[nPhyIndex/32] &= ~(0x1 << (nPhyIndex % 32));
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverFindUnusedPhyPort(THwaFcDevList *pDevCtx,THwaFcPortDir eDir,THwaFcPortType ePortType)
{
	int j  = 0;
	int i = 0;
	unsigned int *pMask = (eDir == eDirRecv) ? pDevCtx->nRecvPhyPortMask : pDevCtx->nSendPhyPortMask;
	FC_DBG_FUNC_CALL;
	if(ePortType == eTypeSpecial)
	{
		if((pMask[HWA_FC_PORT_MASK_CNT - 1] >> 16) & 0x1)
		{
			return -1;
		}else{
			pMask[HWA_FC_PORT_MASK_CNT - 1] |= (0x1 << 16);
			return HWA_FC_MAX_PORT_CNT - 1;
		}
	}else if(ePortType == eTypeBlock){
			for( j = 0; j < HWA_FC_MAX_BLK_PORT; j++)
			{
				if(!((pMask[HWA_FC_PORT_MASK_CNT - 1] >> j) & 0x1))
				{
					pMask[HWA_FC_PORT_MASK_CNT - 1] |= (0x1 << j);
					return HWA_FC_MAX_NOR_PORT + j;
				}
			}
			return -1;
	}

	for(i = 0; i < HWA_FC_MAX_NOR_PORT; i++)
	{
			if(pMask[i] != 0xffffffff)
			{
				for(j = 0; j < 32; j++)
				{
					if(((pMask[i] >> j) & 0x1) == 0)
					{
						pMask[i] |= (0x1 << j); 
						return (i * 32 + j);
					}
				}
			}
			return -1;
	}
	return -1;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverClearPortCfg(THwaFcDevList *pDevCtx,THwaFcPort *pPort)
{
	unsigned int nValue = 0;
	unsigned int nRXValue[RXANA_DATA_CNT] = {0};
	unsigned int nTxValue[TX_PAYLOAD_DATA_CNT] = {0};
	unsigned int nFragData = 0;
	unsigned int nPhyIndex = 0;
	int i = 0;
	FC_DBG_FUNC_CALL;

	pPort->nLoaded = 0;
	HwaFcDriverUnusePhyPort(pDevCtx,pPort->nPhyIndex,pPort->ePortDir);
	
	nPhyIndex = pPort->nPhyIndex;

	do{
		nValue = HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_busy_reset_port);
	}while(nValue != 0);
	if(pPort->ePortDir == eDirSend){
		/*config payload*/
		for(i = 0; i < TX_PAYLOAD_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_data(i),nTxValue[i]);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_addr,nPhyIndex);

		/*config fragment */ 
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_data,nFragData);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_addr,nPhyIndex);
	}else if(pPort->ePortDir == eDirRecv){
		/*config analyze */ 
		for(i = 0; i < RXANA_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_data(i),nRXValue[i]);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_addr,nPhyIndex);
	}else{
		ErrLog("%s","Got unknown port direction.");
		return HWA_FC_ERR_PARA;
	}

/*rst port cfg*/
#if 0
	if(pPort->ePortDir == eDirRecv)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_port_id_clr,CFG_RST << CFG_RST_BIT | (nPhyIndex & CFG_RST_PORTID_MASK) << CFG_RST_PORTID_BIT);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_port_id_clr,CFG_STOP_RST);
	}
#endif
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverLoadPortCfg(THwaFcDevList *pDevCtx,THwaFcPort *pPort)
{
	unsigned int nValue = 0;
	unsigned int nRXValue[RXANA_DATA_CNT] = {0};
	unsigned int nTxValue[TX_PAYLOAD_DATA_CNT] = {0};
	unsigned int nFragData = 0;
	int i = 0;
	FC_DBG_FUNC_CALL;
	pPort->nPhyIndex = 0xffffffff;

	switch(pPort->ePortDir)
	{
	case eDirRecv:
			nRXValue[0] |= (pPort->nPortCfg.nRecvPortCfg.nRecvEnable & RX_ANA_ENABLE_MASK) << RX_ANA_ENABLE_BIT; //used 1 bit ,left 31
			nRXValue[0] |= (pPort->nPortSel & RX_ANA_CH_SEL_MASK) << RX_ANA_CH_SEL_BIT; //used 3bit,left 29
			nRXValue[0] |= (pPort->nPortCfg.nRecvPortCfg.nFragOffset & RX_ANA_FRG_OFF_MASK) << RX_ANA_FRG_OFF_BIT; //used 4.left 28
			nRXValue[0] |= (pPort->nPortCfg.nRecvPortCfg.nFragSeq & RX_ANA_FRG_SEQ_MASK) << RX_ANA_FRG_SEQ_BIT; //used 5.left 27
																				//res 3   used 8,left 24
			nRXValue[0] |= (pPort->nSid & RX_ANA_SID_MASK) << RX_ANA_SID_BIT;	//used 32,left 0
			
			nRXValue[1] |= (pPort->nDid & RX_ANA_DID_MASK) << RX_ANA_DID_BIT; //used 24,left 8
			nRXValue[1] |= (pPort->nType & RX_ANA_TYPE_MASK) << RX_ANA_TYPE_BIT; //used 32,left 0

			nRXValue[2] |= (pPort->nMsgId & RX_ANA_MSG_ID_MASK) << RX_ANA_MSG_ID_BIT; //used 32,left 0

			nRXValue[3] |= (pPort->nOxId & RX_ANA_OXID_MASK) << RX_ANA_OXID_BIT; //used 16bit,left 16
			nRXValue[3] |= (pPort->nSeqId & RX_ANA_SEQID_MASK) << RX_ANA_SEQID_BIT; //used 32 bit,left 0

		break;
	case eDirSend:
			nTxValue[0] |= (pPort->nSid & TX_PAYLOAD_SID_MASK) << TX_PAYLOAD_SID_BIT; //used 24bit,left 12bit
			nTxValue[0] |= (pPort->nDid & TX_PAYLOAD_DID_MASK) << TX_PAYLOAD_DID_BIT; //used 32bit,need more 16bit
			
			nTxValue[1] |= ((pPort->nDid & TX_PAYLOAD_DID_MASK) >> 8); //used 16 bit,left 16 bit*/
			nTxValue[1] |= (pPort->nType & TX_PAYLOAD_TYPE_MASK) << TX_PAYLOAD_TYPE_BIT; //used 24 bit,left 8
			nTxValue[1] |= (pPort->nMsgId & TX_PAYLOAD_MSGID_MASK) << TX_PAYLOAD_MSGID_BIT; //used 32,need more 24bit

			nTxValue[2] |= (pPort->nMsgId & TX_PAYLOAD_MSGID_MASK) >> 8; //used 24,left 8
			nTxValue[2] |= (pPort->nOxId & TX_PAYLOAD_OXID_MASK) << TX_PAYLOAD_OXID_BIT; //used 32 .need 8

			nTxValue[3] |= (pPort->nOxId & TX_PAYLOAD_OXID_MASK) >> 8; //used 8,left 24
			nTxValue[3] |= (pPort->nSeqId & TX_PAYLOAD_SEQID_MASK) << TX_PAYLOAD_SEQID_BIT; //used 16,left 16
			nTxValue[3] |= (pPort->nPortCfg.nSendPortCfg.nPri & TX_PAYLOAD_PRIOI_MASK) << TX_PAYLOAD_PRIOI_BIT; //used 24,left 8
			nTxValue[3] |= (pPort->nPortSel & TX_PAYLOAD_CH_MASK) << TX_PAYLOAD_CH_BIT; //used 26,left 6

			nFragData = pPort->nPortCfg.nSendPortCfg.nMaxLen & TX_PAYLOAD_MAXFRM_LEN_MASK << TX_PAYLOAD_MAXFRM_LEN_BIT;
		break;
	default:
		ErrLog("%s","Got unknown port direction.");
		return HWA_FC_ERR_CFG;
		break;
	}
	int nPhyIndex = HwaFcDriverFindUnusedPhyPort(pDevCtx,pPort->ePortDir,pPort->ePortTye);
	if(nPhyIndex == -1)
	{
		ErrLog("%s","No enough physic port.");
		return HWA_FC_ERR_CFG;
	}

	pPort->nPhyIndex = nPhyIndex;

	do{
		nValue = HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_busy_reset_port);
	}while(nValue != 0);
	if(pPort->ePortDir == eDirSend){
/*config payload*/
		for(i = 0; i < TX_PAYLOAD_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_data(i),nTxValue[i]);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TXpayload_cs_addr,nPhyIndex);

/*config fragment */ 
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_data,nFragData);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_fragment_cs_addr,nPhyIndex);
	}else{
/*config analyze */ 
		for(i = 0; i < RXANA_DATA_CNT; i++)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_data(i),nRXValue[i]);
		}
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RXanalyze_cs_addr,nPhyIndex);
	}

/*rst port cfg*/
	if(pPort->ePortDir == eDirRecv)
	{
//		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_port_id_clr,CFG_RST << CFG_RST_BIT | (nPhyIndex & CFG_RST_PORTID_MASK) << CFG_RST_PORTID_BIT);
//		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_port_id_clr,CFG_STOP_RST);
	}
	pPort->nLoaded = 1;
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverEnableRecvIntr(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_intr_RX_frame_en,RECV_INTR_ENABLE);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDeleteSendPort(int nCardNum,THwaFcPortHandle nPortHandle)
{
	int ret = HWA_FC_OP_OK;
	THwaFcPort *pPort = NULL;
#if 0
	if(nHwaFcDev[nCardNum].eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
#endif
	CHECK_CARD_ST(nCardNum);

	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);

	pPort = &nHwaFcDev[nCardNum].nSendPort[nPortHandle];
/*check port status*/
	if(pPort->nLoaded == 0)
	{
		ErrLog("%s","Port not been created.");
		GOTO_ERR(HWA_FC_ERR_PARA);
	}

/*load port config to hardware*/
	if(nCardNum != 0)
	{
		if(HwaFcDriverClearPortCfg(&nHwaFcDev[nCardNum],pPort)!= HWA_FC_OP_OK)
		{
			GOTO_ERR(HWA_FC_ERR_CFG);
		}
	}
	pPort->nLoaded = 0;
	nHwaFcDev[nCardNum].nSendPhyPort[pPort->nPhyIndex].nLogicPortIndex = -1;
ERR:
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return ret;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverCreateSendPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle)
{
	int ret = HWA_FC_OP_OK;
	THwaFcPort *pPort = NULL;
	FC_DBG_FUNC_CALL;

#if 0
	if(nHwaFcDev[nCardNum].eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
#endif
	CHECK_CARD_ST(nCardNum);


	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);

/*find an port by port id*/
	if(HwaFcDriverFindPortByID(nCardNum,nPortId,pPortHandle,eDirSend) != HWA_FC_OP_OK)
	{
		GOTO_ERR(HWA_FC_ERR_PARA);
	}
	pPort = &nHwaFcDev[nCardNum].nSendPort[*pPortHandle];
	
/*check port status*/
	if(pPort->nLoaded == 1)
	{
		ErrLog("%s","Port already created.");
		GOTO_ERR(HWA_FC_ERR_PARA);
	}

	if(pPort->nPortStat == 0)
	{
		ErrLog("%s","Port not been initialized");
		GOTO_ERR(HWA_FC_ERR_PARA);
	}

/*load port config to hardware*/
	if(!IS_SIMULATE)
	{
		if(HwaFcDriverLoadPortCfg(&nHwaFcDev[nCardNum],pPort)!= HWA_FC_OP_OK)
		{
			GOTO_ERR(HWA_FC_ERR_CFG);
		}
	}
	pPort->nLoaded = 1;
	nHwaFcDev[nCardNum].nSendPhyPort[pPort->nPhyIndex].nLogicPortIndex = *pPortHandle;

ERR:
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return ret;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDeleteRecvPort(int nCardNum,THwaFcPortHandle nPortHandle)
{
	int ret = HWA_FC_OP_OK;
	THwaFcPort *pPort = NULL;
	FC_DBG_FUNC_CALL;
	if(nHwaFcDev[nCardNum].eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
	CHECK_CARD_ST(nCardNum);

	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	
	pPort = &nHwaFcDev[nCardNum].nRecvPort[nPortHandle];

/*check port status*/
	if(pPort->nLoaded == 0)
	{
		ErrLog("%s","Port not been created..");
		GOTO_ERR(HWA_FC_ERR_PARA);
	}

/*clear port config to hardware*/
	if(!IS_SIMULATE)
	{	
		if(HwaFcDriverClearPortCfg(&nHwaFcDev[nCardNum],pPort)!= HWA_FC_OP_OK)
		{
			GOTO_ERR(HWA_FC_ERR_CFG);
		}
	}
	pPort->nLoaded = 0;
/*DeInit data FIFO*/
	if(HwaFcDriverDeInitFifo(&(pPort->nPortCfg.nRecvPortCfg.nPortFifo)) != HWA_FC_OP_OK)
	{
		GOTO_ERR(HWA_FC_ERR_PARA);
	}
/*Deinit data message*/
	SAFE_CLOSE_MSG(pPort->nPortCfg.nRecvPortCfg.nRecvMsgHandle);
	SAFE_CLOSE_MSG(pPort->nPortCfg.nRecvPortCfg.nPipeHandle);

	CLEAR_MSG_NAME(pPort->nPortCfg.nRecvPortCfg.sMsgName);

	nHwaFcDev[nCardNum].nRecvPhyPort[pPort->nPhyIndex].nLogicPortIndex = -1;
ERR:
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return ret;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverCreateRecvPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle)
{
	int ret = HWA_FC_OP_OK;
	THwaFcPort *pPort = NULL;
	FC_DBG_FUNC_CALL;
	if(nHwaFcDev[nCardNum].eFcDevMode == eModeCapture)
	{
		ErrLog("%s","Work mode not support.");
		return HWA_FC_ERR_PARA;
	}
	CHECK_CARD_ST(nCardNum);

	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
/*find an port by port id*/
	if(HwaFcDriverFindPortByID(nCardNum,nPortId,pPortHandle,eDirRecv) != HWA_FC_OP_OK)
	{
		GOTO_ERR(HWA_FC_ERR_PARA);
	}
	pPort = &nHwaFcDev[nCardNum].nRecvPort[*pPortHandle];

/*check port status*/
	if(pPort->nLoaded == 1)
	{
		ErrLog("%s","Port already created.");
		GOTO_ERR(HWA_FC_ERR_PARA);
	}
	
	if(pPort->nPortStat == 0)
	{
		ErrLog("%s","Port not been initialized");
		return HWA_FC_ERR_PARA;
	}

/*load port config to hardware*/
	if(nCardNum != 0)
	{
		if(HwaFcDriverLoadPortCfg(&nHwaFcDev[nCardNum],pPort)!= HWA_FC_OP_OK)
		{
			GOTO_ERR(HWA_FC_ERR_CFG);
		}
	}
	pPort->nLoaded = 1;
/*init data FIFO*/
	SET_LOCK_NAME(pPort->nPortCfg.nRecvPortCfg.nPortFifo.sLockName,nHwaFcDev[nCardNum].nDevInfo.sFcSerial,pPort->nPhyIndex);
	if(HwaFcDriverInitFifo((&pPort->nPortCfg.nRecvPortCfg.nPortFifo),(pPort->ePortTye) == 
						eTypeBlock ? HWA_FC_BLOCK_FIFO_SIZE : HWA_FC_FIFO_SIZE) != HWA_FC_OP_OK)
	{
		GOTO_LABEL(ERRLoad,HWA_FC_ERR_PARA);
	}

/*init data message*/
#ifdef WINDOWS_OPS
	SET_MSG_NAME(pPort->nPortCfg.nRecvPortCfg.sMsgName,nHwaFcDev[nCardNum].nDevInfo.sFcSerial,pPort->nPhyIndex);
	if((pPort->nPortCfg.nRecvPortCfg.nPipeHandle = HwaFcDriverCreateMsg(pPort->nPortCfg.nRecvPortCfg.sMsgName)) == HWA_FC_INVALIED_HANDLE)
	{
		GOTO_LABEL(ERRFifo,HWA_FC_ERR_PARA);
	}
#endif

	if((pPort->nPortCfg.nRecvPortCfg.nRecvMsgHandle = HwaFcDriverOpenMsg(pPort->nPortCfg.nRecvPortCfg.sMsgName)) == HWA_FC_INVALIED_HANDLE)
	{
		GOTO_LABEL(ERRPipe,HWA_FC_ERR_PARA);
	}
	nHwaFcDev[nCardNum].nRecvPhyPort[pPort->nPhyIndex].nLogicPortIndex = *pPortHandle;
	goto ERR;

ERRPipe:
	SAFE_CLOSE_MSG(pPort->nPortCfg.nRecvPortCfg.nPipeHandle);
#ifdef WINDOWS_OPS
ERRFifo:
	HwaFcDriverDeInitFifo((&pPort->nPortCfg.nRecvPortCfg.nPortFifo));
#endif
ERRLoad:
	HwaFcDriverClearPortCfg(&nHwaFcDev[nCardNum],pPort);
ERR:
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return ret;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverFillPort(THwaFcPort *pPort,THwaFcPortCfg nCfg)
{
	FC_DBG_FUNC_CALL;
	pPort->ePortDir = nCfg.ePortDir;
	pPort->nPortSel = nCfg.ePortPortSel;
	pPort->ePortTye = nCfg.ePortType;
	pPort->nDid = nCfg.nDid;
	if(pPort->ePortDir == eDirSend)
	{
		pPort->nPortCfg.nSendPortCfg.nMaxLen = nCfg.nMaxLen;
		pPort->nPortCfg.nSendPortCfg.nPri = nCfg.nPri;
	}else if(pPort->ePortDir == eDirRecv)
	{
		pPort->nPortCfg.nRecvPortCfg.nFragSeq = nCfg.nFragSeq;
		pPort->nPortCfg.nRecvPortCfg.nFragOffset = nCfg.nFragOffset;
		pPort->nPortCfg.nRecvPortCfg.nRecvEnable = 1;
	}
	pPort->nMsgId = nCfg.nMsgId;
	pPort->nOxId = nCfg.nOxId;
	pPort->nPortId = nCfg.nPortId;
	pPort->nType = nCfg.nPortMsgType;
	pPort->nSeqId = nCfg.nSeqId;
	pPort->nSid = nCfg.nSid;
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetSendLock(int nCardNum)
{
	WAIT_EVENT(nHwaFcDev[nCardNum].nSendLock);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int  HwaFcDriverReleaseSendLock(int nCardNum)
{
	RELEASE_LOCK(nHwaFcDev[nCardNum].nSendLock);
	return HWA_FC_OP_OK;
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetRecvLock(int nCardNum)
{
	FC_DBG_FUNC_CALL;

	WAIT_EVENT(nHwaFcDev[nCardNum].nRecvLock);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverReleaseRecvLock(int nCardNum)
{
	RELEASE_LOCK(nHwaFcDev[nCardNum].nRecvLock);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetDeivceVersion(int nCardNum)
{
	int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ST(nCardNum);
	nValue = HwaFcDriverReadReg(nCardNum,eBar0,FPGA_REG_device_version);
	return nValue;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetSerialNum(int nCardNum,char *pSerial,int nCnt)
{
	FC_DBG_FUNC_CALL;
	SPRINTF(pSerial,SERIAL_LEN,"%s-%d","HWA-FC-CARD",nCardNum);
	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetCardInfo(int nCardNum,ThwaFcDevInfo *pDevInfo)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	if(!pDevCtx->nVailed)
	{
		ErrLog("%s","Card not find.");
		return HWA_FC_ERR_PARA;
	}

	memcpy(pDevInfo,&pDevCtx->nDevInfo,sizeof(ThwaFcDevInfo));
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetPortStatus(int nCardNum,THwaFcPortStatus *pPortStatus)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	if(IS_SIMULATE)
	{
		pPortStatus->nLinkStatus = 0x3;
		pPortStatus->nFcSpeed = pDevCtx->eFcDevSpeed;
	}else{
		pPortStatus->nLinkStatus = HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_FC_status);
		pPortStatus->nFcSpeed = HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_FC_speed);
		pPortStatus->nFiberStatus = HwaFcDriverRead32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_FC_bb_credit);
	}
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetSendOverFlow(int nCardNum,unsigned int nPortIndex)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	if(IS_SIMULATE)
	{
		return 0;
	}
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TX_buf_overflow_port_id,nPortIndex);
	HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_TX_buf_overflow_num,&nValue);
	return nValue;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetRecvOverFlow(int nCardNum,unsigned int nPortIndex)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	if(IS_SIMULATE)
	{
		return 0;
	}
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RX_buf_overflow_port_id,nPortIndex);
	HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_RX_buf_overflow_num,&nValue);
	return nValue;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSetCardCfg(int nCardNum,THwaFcCardCfg nCfg)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if((nCfg.eSpeed < eSpeed1G) || (nCfg.eSpeed >= eSpeedNr))
	{
		ErrLog("Device work speed is %d not supported",nCfg.eSpeed);
		return HWA_FC_ERR_PARA;
	}

	if((nCfg.eWorkMode < eModeNormal) || (nCfg.eSpeed >= eDevModeNr))
	{
		ErrLog("Device work mode %d not supported",nCfg.eWorkMode);
		return HWA_FC_ERR_PARA;
	}
	if((nCfg.eIrigbMod < eModeMaster) || (nCfg.eSpeed >= eIrigModeNr))
	{
		ErrLog("IRIG-B work mode %d not supported",nCfg.eIrigbMod);
		return HWA_FC_ERR_PARA;
	}
	pDevCtx->eRecvMode = nCfg.eRecvMode;
	pDevCtx->eFcDevMode = nCfg.eWorkMode;
	pDevCtx->eFcDevSpeed = nCfg.eSpeed;
	pDevCtx->eFcIrigMode = nCfg.eIrigbMod;
	pDevCtx->nSendLpsSize = ((nCfg.nSendLpsSize != 0) ? nCfg.nSendLpsSize : MAX_SEND_LPS_SIZE);
	pDevCtx->nRecvLpsSize = ((nCfg.nRecvLpsSize != 0) ? nCfg.nRecvLpsSize : MAX_RECV_LPS_SIZE);
	return HWA_FC_OP_OK;
	
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverInitDone(int nCardNum)
{
	nHwaFcDev[nCardNum].nDevInitFlag = 1;
	return 0;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDeInit(void)
{

	int ret = 0;
	FC_DBG_FUNC_CALL;
	if(nHwaFcDevListLock){
		WAIT_EVENT(nHwaFcDevListLock);
	}

	if(nHwaFcDriverInitFlag == 0){
		ErrLog("%s","Device driver not initialed.");
		GOTO_ERR(HWA_FC_ERR_UNINIT);
	}

	nHwaFcDevCnt = 0;
	memset(nHwaFcDev,0,sizeof(nHwaFcDev));

	/*DeInit Win Driver LIB*/
#ifdef WINDOWS_OPD
	if(HwaFcDriverUninitLib() != WD_STATUS_SUCCESS)
	{
		GOTO_ERR(HWA_FC_ERR_UNINIT);
	}
#endif

	nHwaFcDriverInitFlag = 0;
	RESET_EVENT(nHwaFcDevListLock);	
	SAFE_CLOSE(nHwaFcDevListLock);
	return HWA_FC_OP_OK;

ERR:
	RESET_EVENT(nHwaFcDevListLock);
	return ret;

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverInit(void)
{

	int ret = 0;
	FC_DBG_FUNC_CALL;
	if(nHwaFcDevListLock != HWA_FC_INVALIED_HANDLE){
		WAIT_EVENT(nHwaFcDevListLock);
	}

	if(nHwaFcDriverInitFlag == 1){
		return HWA_FC_OP_OK;
	}
	/*init Hwa FC Driver struct init */
	nHwaFcDevListLock = HwaFcInitLock("HwaFcDevList","00000000");
	if(nHwaFcDevListLock == NULL)
	{
		ErrLog("%s","Create devList lock failed.");
		GOTO_ERR(HWA_FC_ERR_INIT);
	}
	nHwaFcDevCnt = 0;
	memset(nHwaFcDev,0,sizeof(nHwaFcDev));
	/*init Win Driver LIB*/
#ifdef WINDOWS_OPS
	if(HwaFcDriverInitLib() != WD_STATUS_SUCCESS)
	{
		GOTO_ERR(HWA_FC_ERR_INIT);
	}
#elif defined VXWORKS_OPS
//	HwaFcPciInit();
#endif
	/*List the Hwa FC devlist*/
	if(HwaFcDriverListDev() != HWA_FC_OP_OK)
	{
		GOTO_ERR(HWA_FC_ERR_INIT);
	}
	nHwaFcDriverInitFlag = 1;
	ret = nHwaFcDevCnt;
ERR:
	RELEASE_LOCK(nHwaFcDevListLock);
	return ret;

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
-----------------------------------------------*/
int HwaFcDriverGetCardNum(void)
{
	if(nHwaFcDriverInitFlag)
	{
		return nHwaFcDevCnt;
	}
	ErrLog("Driver not initional.");
	return HWA_FC_ERR_INIT;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
-----------------------------------------------*/
int HwaFcDriverStartRecvThread(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	int nId = 0;
	FC_DBG_FUNC_CALL;
#ifdef VXWORKS_OPS
	int nThreadId = 0;
	if((nThreadId = taskNameToId("HwaFcRecvTask")) != 0)
	{
		taskDelete(nThreadId);
	}
#endif
	START_THREAD(&(pDevCtx->nRecvThreadId),nCardNum);
	if(pDevCtx->nRecvThreadId == HWA_FC_INVALIED_HANDLE)
	{
		ErrLog("%s","Start receive thread failed.");
		return HWA_FC_ERR_OPEN;
	}
	return HWA_FC_OP_OK;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverListenLinkStatus(int nCardNum,THwaFcPortStatus *pStatus)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nLinkEvent);
	if(pStatus != NULL)
	{
		memcpy(pStatus,&pDevCtx->nLinkStatus,sizeof(THwaFcPortStatus));
	}
	return 0;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverGetSpePortHandle(int nCardNum,THwaFcPortHandle *pRecv,THwaFcPortHandle *pSend)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CHECK_CARD_ID(nCardNum);
	CHECK_CARD_ST(nCardNum);
	*pRecv = pDevCtx->nSpeRecvPort;
	*pSend = pDevCtx->nSpeSendPort;
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
const char *HwaFcDriverGetLastErr(void)
{
    return sHwaFcLastErr;
}





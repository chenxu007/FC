// HwaFcNicDriver.cpp :
//

#include "stdafx.h"

#include "HwaFcDriverLib.h"
#include "HwaFcDriver.h"

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcInit(void)
{
	FC_DBG_FUNC_CALL;
	return HwaFcDriverInit();
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDeInit(void)
{
	FC_DBG_FUNC_CALL;
	return HwaFcDriverDeInit();
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcOpenCard(int nCardNum)
{
	int ret = 0;
	CHECK_CARD_ID(nCardNum);
	FC_DBG_FUNC_CALL;
	HWA_FC_DBG("Open Card %d\n",nCardNum);
/*open device*/
	if((ret = HwaFcDriverOpenDevice(nCardNum)) != HWA_FC_OP_OK)
	{
		goto DONE;
	}
/*init Device struct*/
	if((ret = HwaFcDriverInitDevStr(nCardNum)) != HWA_FC_OP_OK)
	{
		goto ERR_OPEN;;
	}
#ifdef WORK_MODE_INTERRUPT
/*enable interrupt*/
	if((ret = HwaFcDriverEnableInt(nCardNum)) != HWA_FC_OP_OK)
	{
		goto ERR_STR;
	}
#endif
	HwaFcDriverInitDone(nCardNum);
	goto DONE;
ERR_STR:
	HwaFcDriverDeInitDevStr(nCardNum);
ERR_OPEN:
	HwaFcDriverCloseDevice(nCardNum);
DONE:
	return ret;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcCloseCard(int nCardNum)
{
	int ret = 0;
	int i = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	
/*close all port*/
	for(i = 0; i < HWA_FC_MAX_PORT_CNT; i++)
	{
		HwaFcDriverDeleteRecvPort(nCardNum,i);
		HwaFcDriverDelPort(nCardNum,i,eDirRecv);
		HwaFcDriverDeleteRecvPort(nCardNum,i);
		HwaFcDriverDelPort(nCardNum,i,eDirSend);
	}
/*stop mon recv*/
	HwaFcDriverMonStopRecv(nCardNum);

/*wait interrupt clear all data*/ 
	SLEEP(10);
	
	if((ret = HwaFcDriverDisableInt(nCardNum)) != HWA_FC_OP_OK)
	{
		return ret;
	}	

	if((ret = HwaFcDriverDeInitDevStr(nCardNum)) != HWA_FC_OP_OK)
	{
		return ret;
	}

	if((ret = HwaFcDriverCloseDevice(nCardNum)) != HWA_FC_OP_OK)
	{
		return ret;
	}

	return HWA_FC_OP_OK;
}

int HwaFcSetCardCfg(int nCardNum,THwaFcCardCfg nCfg)
{
	int ret = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return HwaFcDriverSetCardCfg(nCardNum,nCfg);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcAddPort(int nCardNum,THwaFcPortCfg *pPortList,int nPortCnt)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	THwaFcPort nPort = {0};
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if((nPortCnt != 0) && (pPortList == NULL))
	{
		ErrLog("Get wrong parameter.\n");
		return HWA_FC_ERR_PARA;
	}
	for(i = 0; i < nPortCnt; i++)
	{
		HwaFcDriverFillPort(&nPort,pPortList[i]);
		if((ret = HwaFcDriverAddPort(nCardNum,nPort)) != HWA_FC_OP_OK)
		{
			for(j = 0; j < i; j++)
			{
				HwaFcDriverDelPort(nCardNum,pPortList[i].nPortId,pPortList[i].ePortDir);
			}
			return ret;
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
int HwaFcDelPort(int nCardNum,unsigned int nPortId,THwaFcPortDir ePortDir)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return HwaFcDriverDelPort(nCardNum,nPortId,ePortDir);
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcLoadCfg(int nCardNum,char *pFileName)
{
	int ret = 0;
	int n = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	//THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
/*	
	if(pFileName == NULL)
	{
		ErrLog("Got wrong config file name.");
		return HWA_FC_ERR_PARA;
	}
*/
#ifdef SUPPORT_XML
	if(pFileName != NULL)
	{
		if((ret = HwaFcDriverPhraseXml(nCardNum,pFileName)) != HWA_FC_OP_OK)
		{
			goto ERR;
		}
	}
#endif
	/*Init DMA information*/
	if(nCardNum != 0)
	{
#ifdef 	VXWORKS_OPS //yzy
		if(HwaFcDriverOpenDma(nCardNum) != HWA_FC_OP_OK)
		{
#else
		if(HwaFcDriverOpenDma(nCardNum) != HWA_FC_OP_OK)
		{
#endif
			return HWA_FC_ERR_INIT;
		}
	}

/*start receive data thread*/
	if((ret = HwaFcDriverStartRecvThread(nCardNum)) != HWA_FC_OP_OK)
	{
		goto ERR;
	}


	if((ret = HwaFcDriverLoadCfg(nCardNum)) != HWA_FC_OP_OK)
	{
		goto ERR;
	}



ERR:
	return ret;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcOpenPort(int nCardNum,unsigned int nPortId,THwaFcPortHandle *pPortHandle,THwaFcPortDir eDir)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_PORT_ID(nPortId);
	if(pPortHandle == NULL)
	{
		ErrLog("Got wrong port handler pointer.");
		return HWA_FC_ERR_PARA;
	}

	if(eDir == eDirSend)
	{
		return HwaFcDriverCreateSendPort(nCardNum,nPortId,pPortHandle);
	}else if(eDir == eDirRecv)
	{
		return HwaFcDriverCreateRecvPort(nCardNum,nPortId,pPortHandle);
	}else{
		ErrLog("Got wrong port dir.");
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
int HwaFcClosePort(int nCardNum,THwaFcPortHandle nPortHandle,THwaFcPortDir eDir)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_PORT_HANDLE(nPortHandle);

	if(eDir == eDirRecv)
	{
		return HwaFcDriverDeleteRecvPort(nCardNum,nPortHandle);
	}else if(eDir == eDirSend)
	{
		return HwaFcDriverDeleteSendPort(nCardNum,nPortHandle);
	}else{
		ErrLog("Got wrong port dir.");
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
int HwaFcLogin(int nCardNum,THwaFcPortSel ePort)
{
	THwaFcPortStatus nSt;
	int n = 0;
	int ret = 0;
	nSt.nLinkStatus = 0;
	HwaFcGetPortStatus(nCardNum,&nSt);
	/*start login*/
	if(nSt.nLinkStatus & ePort)
	{
		if((ret = HwaFcDriverLogin(nCardNum,ePort)) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_LOGIN;
		}
	}else{
		ErrLog("Port not link up.");
		return HWA_FC_ERR_LOGIN;
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
int HwaFcSendData(int nCardNum,THwaFcPortHandle nPortHandle,char *pBuf,unsigned int nBufSize,THwaSendCfg nCfg)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_PORT_HANDLE(nPortHandle);
	int ret = 0;
	if((pBuf == 0) && (nBufSize != 0))
	{
		ErrLog("Got wrong parameters.");
		return HWA_FC_ERR_PARA;
	}
	
	if(IS_SIMULATE)
	{
		return nBufSize;
	}

	if(HwaFcDriverGetSendLock(nCardNum) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_SEND;
	}

	if((ret = HwaFcDriverSetSendCfg(nCardNum,nPortHandle,nBufSize,nCfg)) != HWA_FC_OP_OK)
	{
		GOTO_LABEL(DONE, ret);
	}
	if((ret = HwaFcDriverSendData(nCardNum,pBuf,nBufSize)) <0)
	{
		GOTO_LABEL(DONE, ret);
	}
	if((ret = HwaFcDriverCheckOverFlow(nCardNum,nPortHandle,(THwaFcPortSel)nCfg.nChSel)) != HWA_FC_OP_OK)
	{
		GOTO_LABEL(DONE, ret);
	}
	ret = nBufSize;
DONE:
	HwaFcDriverReleaseSendLock(nCardNum);
	return ret;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcRecvData(int nCardNum,THwaFcPortHandle nPortHandle,char *pBuf,THwaFcRecvInfo *pInfo, int nBufSize,int nWaitTime)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	CHECK_PORT_HANDLE(nPortHandle);

	if((pBuf == 0) && (nBufSize != 0) || (pInfo == NULL))
	{
		ErrLog("Got wrong pointer.");
		return HWA_FC_ERR_PARA;
	}
	return HwaFcDriverRecvData(nCardNum,nPortHandle,pBuf,pInfo,nBufSize,nWaitTime);
}

#ifdef WINDOWS_OPS
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcMonRecvData(int nCardNum,char *pBuf, int nBufSize,int nWaitTime,int *pDataType)
{
	CHECK_CARD_ID(nCardNum);

	if(((pBuf == 0) && (nBufSize != 0)) || (pDataType == NULL))
	{
		ErrLog("Got wrong pointer.");
		return HWA_FC_ERR_PARA;
	}
	return HwaFcDriverMonRecvData(nCardNum,pBuf,nBufSize,nWaitTime,pDataType);
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcSetMonOperation(int nCardNum,THwaFcMonOperation eOperation)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(eOperation == eStartRecv)
	{
		return HwaFcDriverMonStartRecv(nCardNum);
	}else if(eOperation == eStopRecv)
	{
		return HwaFcDriverMonStopRecv(nCardNum);
	}else if(eOperation == ePauseRecv)
	{
		return HwaFcDriverMonPauseRecv(nCardNum);
	}
	ErrLog("Got unknown monitor operation");
	return HWA_FC_ERR_PARA;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcSetMonCfg(int nCardNum, THwaFcMonCfg nCfg)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return HwaFcDriverSetMonCfg(nCardNum,nCfg);	

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcAddMonFilter(int nCardNum,int nFilterCnt,THwaFcMonFilter *pMonFilter)
{
	int i = 0;
	int j = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(((nFilterCnt > 0) && (pMonFilter == NULL)) ||(nFilterCnt > HWA_FC_MAX_FILTERCNT))
	{
		ErrLog("Got error filter para.");
		return HWA_FC_ERR_PARA;
	}
	for(i= 0; i < nFilterCnt; i++)
	{
		if(HwaFcDriverAddMonFilter(nCardNum,pMonFilter[i]) != HWA_FC_OP_OK)
		{
			for(j = 0; j < i; j++)
			{
				HwaFcDriverDelMonFilter(nCardNum,j);
			}
			return HWA_FC_ERR_PARA;
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
int HwaFcDelMonFilter(int nCardNum,int nIndex)
{
	int i = 0;
	int j = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return HwaFcDriverDelMonFilter(nCardNum,nIndex);

}

#endif
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetTxMibs(int nCardNum,THwaFcTxMib *pMibs,THwaFcPortSel ePort)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(pMibs == NULL)
	{
		ErrLog("Got wrong pointer.");
		return HWA_FC_ERR_PARA;
	}
	if(HwaFcDriverGetMibs(nCardNum,eMibTx,ePort,pMibs) != HWA_FC_OP_OK)
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
int HwaFcGetRxMibs(int nCardNum,THwaFcRXMib *pMibs,THwaFcPortSel ePort)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(pMibs == NULL)
	{
		ErrLog("Got wrong pointer.");
		return HWA_FC_ERR_PARA;
	}
	
	if(HwaFcDriverGetMibs(nCardNum,eMibRx,ePort,pMibs) != HWA_FC_OP_OK)
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
int HwaFcGetSysMibs(int nCardNum,THwaFcSysMib *pMibs)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(pMibs == NULL)
	{
		ErrLog("Got wrong pointer.");
		return HWA_FC_ERR_PARA;
	}
	if(HwaFcDriverGetMibs(nCardNum,eMibSys,ePortAB,pMibs) != HWA_FC_OP_OK)
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
int HwaFcClearMibs(int nCardNum)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return HwaFcDriverClearMib(nCardNum);
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetCardNum(void)
{
	FC_DBG_FUNC_CALL;
	return HwaFcDriverGetCardNum();
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetCardInfo(int nCardNum,ThwaFcDevInfo *pDevInfo)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(pDevInfo == NULL)
	{
		return HWA_FC_ERR_PARA;
	}
	return  HwaFcDriverGetCardInfo(nCardNum,pDevInfo);

}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetPortStatus(int nCardNum,THwaFcPortStatus *pPortStatus)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(pPortStatus == NULL)
	{
		return HWA_FC_ERR_PARA;
	}
	return  HwaFcDriverGetPortStatus(nCardNum,pPortStatus);

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetPortOverFlow(int nCardNum,unsigned int nPortIndex, THwaFcPortDir eDir)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(nPortIndex > HWA_FC_MAX_PORT_CNT)
	{
		ErrLog("Port index out of range.");
		return HWA_FC_ERR_PARA;
	}
	if(eDir == eDirSend)
	{
		return  HwaFcDriverGetSendOverFlow(nCardNum,nPortIndex);
	}else if(eDir == eDirRecv)
	{
		return	HwaFcDriverGetRecvOverFlow(nCardNum,nPortIndex);

	}
	ErrLog("Wrong port direction.");
	return HWA_FC_ERR_PARA;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */


int HwaFcSetIrigMode(int nCardNum,THwaFcIrigMode eMode)
{
	return HwaFcDriverSetIrigMode(nCardNum,eMode);
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcSetIolevel(int nCardNum, int nLevel)
{
	return HwaFcDriverSetIoLevel(nCardNum,nLevel);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */

int HwaFcGetIolevel(int nCardNum, int *pLevel)
{
	return HwaFcDriverGetIoLevel(nCardNum,pLevel);
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcSetCaptureLoopMode(int nCardNum,THwaFcLoopMode eMode)
{
	if((eMode < eLoopMode) || (eMode >= eLoopModeNr))
	{
		ErrLog("Loop mode error.");
		return HWA_FC_ERR_PARA;
	}
	return HwaFcDriverSetCaptureLoopMode(nCardNum,eMode);
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
void HwaFcDumpCardInfo(int nCardNum,ThwaFcDevInfo nDevInfo)
{
	printf("************************\n");
	printf("[Device %d]\n",nCardNum);
	printf("Vendor ID   :   0x%08x\n",nDevInfo.nVendorID);
	printf("Device ID   :   0x%08x\n",nDevInfo.nDeviceID);
	printf("Function ID :   0x%08x\n",nDevInfo.nFuncNum);
	printf("Bus Num     :   0x%08x\n",nDevInfo.nBusNum);
	printf("Slot Num    :   0x%08x\n",nDevInfo.nSlotNum);
	printf("Hardware Ver:   0x%08x\n",nDevInfo.nHwVer);
	printf("Serial Num  :   %s\n",nDevInfo.sFcSerial);
	printf("************************\n");
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcReadReg(int nCardNum,int nAddr)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return  HwaFcDriverReadReg(nCardNum,eBar0,nAddr);

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcWriteReg(int nCardNum,int nAddr,unsigned int nValue)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return  HwaFcDriverWriteReg(nCardNum,eBar0,nAddr,nValue);

}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcReadCfgReg(int nCardNum,int nAddr)
{
	unsigned int nValue = 0;
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	if(HwaFcDriverReadCfgReg(nCardNum,nAddr,&nValue,sizeof(nValue)))
	{
		return -1;
	}
	return nValue;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcWriteCfgReg(int nCardNum,int nAddr,unsigned int nValue)
{
	FC_DBG_FUNC_CALL;
	CHECK_CARD_ID(nCardNum);
	return  HwaFcDriverWriteCfgReg(nCardNum,nAddr,&nValue,sizeof(nValue));
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:s
OutPut			:
return			:
----------------------------------------------- */
int HwaFcListenLinkStatus(int nCardNum,THwaFcPortStatus *pStatus)
{
	return HwaFcDriverListenLinkStatus(nCardNum,pStatus);
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:s
OutPut			:
return			:
----------------------------------------------- */
int HwaFcGetSpePortHandle(int nCardNum,THwaFcPortHandle *pRecv,THwaFcPortHandle *pSend)
{
	return HwaFcDriverGetSpePortHandle(nCardNum,pRecv,pSend);
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
const char *HwaFcGetLastErr(void)
{
	FC_DBG_FUNC_CALL;
	return  HwaFcDriverGetLastErr();
}

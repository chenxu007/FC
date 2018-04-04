/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
#ifndef PCI_MAX_FUNC
#define PCI_MAX_FUNC 15
#endif

#include "logLib.h"

IMPORT VXB_DEVICE_ID globalBusCtrlID;
VXB_DEVICE_ID vxb_handle=globalBusCtrlID;

#if 0

#define PCIE_CTL_MAX_NUM 5

static int HwaFcDriverListDev(void)
{
	int i = 0;
	int pcie_ctl_num = 0;
	int find_fc_dvice = 0;
	int nDeviceCnt = 0;
	int nBusNo = 0;
	int nSlotNo=0;
	int nFuncNo = 0;
	int ret = 0;
	
	FC_DBG_FUNC_CALL;
	//VXB_DEVICE_ID vxb_handle
	vxb_handle=globalBusCtrlID;
	for(pcie_ctl_num = 0 ; pcie_ctl_num < PCIE_CTL_MAX_NUM;pcie_ctl_num++){
	
			if(find_fc_dvice)
				break;
			//if(vxb_handle == NULL){
#if 0				
				//vxb_handle = vxbInstByNameFind("pciExpressHost",pcie_ctl_num);
				if(vxb_handle == NULL)
					continue;
				pciConfigEnable(vxb_handle);
			//}
#endif
			//pciConfigEnable(vxb_handle);
			for (i = 0; i < PCI_MAX_FUNC; i++)
			{
		/* find card and get bus, device, func number */
				//
		#ifdef USE_VXBUS
				
					ret = vxbPciFindDevice(vxb_handle,HWA_FC_VENDOR_ID,HWA_FC_DEVICE_ID,i,&nBusNo, &nSlotNo, &nFuncNo);
					printf("---------------ret=%d--------------\n",ret);
					 if (ret == 0){
						 
		#else			
			if (pciFindDevice (HWA_FC_VENDOR_ID, HWA_FC_DEVICE_ID, i, &nBusNo, &nSlotNo, &nFuncNo) == OK)
				{
		#endif
					find_fc_dvice = 1;
					nHwaFcDev[nDeviceCnt + 1].nDevInfo.nBusNum = nBusNo;
					nHwaFcDev[nDeviceCnt + 1].nDevInfo.nFuncNum = nFuncNo;
					nHwaFcDev[nDeviceCnt + 1].nDevInfo.nSlotNum = nSlotNo;
					nHwaFcDev[nDeviceCnt + 1].nDevInfo.nVendorID = HWA_FC_VENDOR_ID;
					nHwaFcDev[nDeviceCnt + 1].nDevInfo.nDeviceID = HWA_FC_DEVICE_ID;
		#ifndef USE_VXBUS
		/*read pci bar0 address*/
					pciConfigInLong(nBusNo,nSlotNo,nFuncNo,PCI_CFG_BASE_ADDRESS_0,&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nBarAddr[0]));
		/*get irq number*/
					pciConfigInLong(nBusNo,nSlotNo,nFuncNo,PCI_CFG_DEV_INT_LINE,(unsigned int *)&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nIrqId));
		#else
		/*read pci bar0 address*/
					vxbPciConfigInLong (vxb_handle, nBusNo, nSlotNo, nFuncNo,PCI_CFG_BASE_ADDRESS_0,&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nBarAddr[0]));
		/*get irq number*/
					vxbPciConfigInLong (vxb_handle, nBusNo, nSlotNo, nFuncNo,PCI_CFG_DEV_INT_LINE,(unsigned int *)&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nIrqId));
		#endif
					printf("HwaFcBar0Addr = 0x%x\n",nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nBarAddr[0]);
					printf("HwaFcIrq = %d\n",nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nIrqId);
					
					nHwaFcDev[nDeviceCnt + 1].nVailed = 1;

					nDeviceCnt++;

				}
			}
	}
	
	/*for simulate*/
	nHwaFcDev[0].nDevInfo.nSlotNum = 0;
	nHwaFcDev[0].nDevInfo.nBusNum = 1;
	nHwaFcDev[0].nDevInfo.nFuncNum = 2;
	nHwaFcDev[0].nDevInfo.nVendorID = 0x1234;
	nHwaFcDev[0].nDevInfo.nDeviceID = 0xffee;
	nHwaFcDev[0].nVailed = 1;
	SPRINTF(nHwaFcDev[0].nDevInfo.sFcSerial,SERIAL_LEN,"%s","FC SIMULATE");

	nHwaFcDevCnt = nDeviceCnt + 1;	
	return HWA_FC_OP_OK;
}

#else
	
static int HwaFcDriverListDev(void)
{
	int i = 0;
	int nDeviceCnt = 0;
	int nBusNo = 0;
	int nSlotNo=0;
	int nFuncNo = 0;
	
	int nBusNo_bak = 0;
	int nSlotNo_bak=0;
	int nFuncNo_bak = 0;
		
	FC_DBG_FUNC_CALL;

	for (i = 0; i < PCI_MAX_FUNC; i++)
	{
/* find card and get bus, device, func number */

		if (vxbPciFindDevice(globalBusCtrlID,HWA_FC_VENDOR_ID,HWA_FC_DEVICE_ID,i,&nBusNo, &nSlotNo, &nFuncNo))
		{
			if(
				nBusNo_bak == nBusNo
				&& nSlotNo_bak == nSlotNo
				&& nFuncNo_bak == nFuncNo
			){
				continue;
			}
			
			nBusNo_bak = nBusNo;
			nSlotNo_bak = nSlotNo;
			nFuncNo_bak = nFuncNo;
			
			nHwaFcDev[nDeviceCnt + 1].nDevInfo.nBusNum = nBusNo;
			nHwaFcDev[nDeviceCnt + 1].nDevInfo.nFuncNum = nFuncNo;
			nHwaFcDev[nDeviceCnt + 1].nDevInfo.nSlotNum = nSlotNo;
			nHwaFcDev[nDeviceCnt + 1].nDevInfo.nVendorID = HWA_FC_VENDOR_ID;
			nHwaFcDev[nDeviceCnt + 1].nDevInfo.nDeviceID = HWA_FC_DEVICE_ID;

	/*read pci bar0 address*/
			vxbPciConfigInLong (globalBusCtrlID, nBusNo, nSlotNo, nFuncNo,PCI_CFG_BASE_ADDRESS_0,&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nBarAddr[0]));
	/*get irq number*/
			vxbPciConfigInLong (globalBusCtrlID, nBusNo, nSlotNo, nFuncNo,PCI_CFG_DEV_INT_LINE,(unsigned int *)&(nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nIrqId));

			printf("HwaFcBar0Addr = 0x%x\n",nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nBarAddr[0]);
			printf("HwaFcIrq = %d\n",nHwaFcDev[nDeviceCnt + 1].nHwaFcDrvHandle.nIrqId);
			
			nHwaFcDev[nDeviceCnt + 1].nVailed = 1;

			nDeviceCnt++;

		}
	}
	
	
	/*for simulate*/
	nHwaFcDev[0].nDevInfo.nSlotNum = 0;
	nHwaFcDev[0].nDevInfo.nBusNum = 1;
	nHwaFcDev[0].nDevInfo.nFuncNum = 2;
	nHwaFcDev[0].nDevInfo.nVendorID = 0x1234;
	nHwaFcDev[0].nDevInfo.nDeviceID = 0xffee;
	nHwaFcDev[0].nVailed = 1;
	SPRINTF(nHwaFcDev[0].nDevInfo.sFcSerial,SERIAL_LEN,"%s","FC SIMULATE");

	nHwaFcDevCnt = nDeviceCnt + 1;	
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
int HwaFcDriverCloseDma(THwaFcDevList *pDevCtx)
{
	FC_DBG_FUNC_CALL;

	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaLock);

	if(pDevCtx->nHwaFcDrvHandle.pBufIn)
	{
		cacheDmaFree(pDevCtx->nHwaFcDrvHandle.pBufIn);
		pDevCtx->nHwaFcDrvHandle.pBufIn = NULL;
	}
	if(pDevCtx->nHwaFcDrvHandle.pBufOut)
	{
		cacheDmaFree(pDevCtx->nHwaFcDrvHandle.pBufOut);
		pDevCtx->nHwaFcDrvHandle.pBufOut = NULL;
	}
	RELEASE_LOCK(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverOpenDma(int  nCardNum)
{
	FC_DBG_FUNC_CALL;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];

	pDevCtx->nHwaFcDrvHandle.pBufIn = (void *) cacheDmaMalloc(MAX_DMA_SIZE);
	pDevCtx->nHwaFcDrvHandle.pBufOut = (void *) cacheDmaMalloc(MAX_DMA_SIZE);

	if(!(pDevCtx->nHwaFcDrvHandle.pBufIn) || !(pDevCtx->nHwaFcDrvHandle.pBufOut))
	{
		ErrLog("%s","Malloc DMA buffer failed.");
		if(pDevCtx->nHwaFcDrvHandle.pBufIn)
		{
			cacheDmaFree(pDevCtx->nHwaFcDrvHandle.pBufIn);
			pDevCtx->nHwaFcDrvHandle.pBufIn = NULL;
		}
		if(pDevCtx->nHwaFcDrvHandle.pBufOut)
		{
			cacheDmaFree(pDevCtx->nHwaFcDrvHandle.pBufOut);
			pDevCtx->nHwaFcDrvHandle.pBufOut = NULL;
		}
		return HWA_FC_ERR_INIT;
	}

	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
	return HWA_FC_OP_OK;
	
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int intrCnt = 0;
int enter =0;
int err = 0;
int other = 0;

void HwaFcIntHandler(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nValue = 0;
	static int nDataIntCnt = 0;
	FC_DBG_FUNC_CALL;
/*claer interrupt*/
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,0x1 << 8);
	HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_STAT,&nValue);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,0);
	//logMsg("-----entry interrupt----num:%d----\n",(nValue & 0x3),0,0,0,0,0);
	//logMsg(char * fmt,_Vx_usr_arg_t arg1,_Vx_usr_arg_t arg2,_Vx_usr_arg_t arg3,_Vx_usr_arg_t arg4,_Vx_usr_arg_t arg5,_Vx_usr_arg_t arg6)

	#if 0
/*read interrupt type*/
	nValue &= 0x3;
	switch(nValue)
	{
		case 0x0:
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_intr_RX_frame_en,0x1);
			break;
		case 0x1:
		/*RST DMA module*/
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
		/*Tell App DMA done*/
			SET_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
			break;
		case 0x2:
				SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
			break;
		default:
			break;
	}
	#else

		nValue = nValue>>9;
		nValue = nValue&0x3;
		switch(nValue)
		{
			case 0:
				err++;
				break;
			case 1:
#if 1

		/*Tell App DMA done*/
			//	if(pDevCtx->isInDma == 0)
				{
					SET_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
					//i1++;
					//printf("i1 = %d\n",i1);
				}
#endif
				break;
			case 2://Get Data Interrupt
				//if(pDevCtx->isRecvData == 0)
				{
					SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
					//i2++;
					//printf("i2 = %d\n",i2);
				}
				break;
			case 3:
				SET_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
				SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
				//i3++;
				//printf("i3 = %d\n",i3);
				break;
			default:
				other++;
				break;
		}
	#endif
	
}



/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverEnableInt(int nCardNum)
{
	VOIDFUNCPTR isrRoot;
	FC_DBG_FUNC_CALL;
	isrRoot = (VOIDFUNCPTR) HwaFcIntHandler;
	if(intConnect((VOIDFUNCPTR *) INUM_TO_IVEC(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nIrqId), isrRoot, nCardNum) != OK)
	{
		ErrLog("%s","Connect interrupt failed.");
		return HWA_FC_ERR_OPEN;
	}
	if(intEnable(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nIrqId) != OK)
	{
		ErrLog("%s","Enable interrupt failed.");
		return HWA_FC_ERR_OPEN;
	}
	logMsg("-----register interrupt ok--------\n",0,0,0,0,0,0);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDisableInt(int nCardNum)
{
	FC_DBG_FUNC_CALL;
	intDisable(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nIrqId);
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverCloseDevice(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;

	if(pDevCtx->nDevInitFlag == 1)
	{
		pDevCtx->nDevInitFlag = 0;
		return HWA_FC_OP_OK;
	}

	ErrLog("%s","Device not been opened.");
	if(pDevCtx->nRecvThreadId)
	{
		WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaLock);
		taskDelete(pDevCtx->nRecvThreadId);
		pDevCtx->nRecvThreadId = 0;
		RELEASE_LOCK(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	}
	return HWA_FC_ERR_CLOSE;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverOpenDevice(int nCardNum)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	FC_DBG_FUNC_CALL;

	if(pDevCtx->nDevInitFlag == 1)
	{
		return HWA_FC_OP_OK;
	}
/* config inboudn windows for dma */
#if 0
	*(volatile unsigned int *)(0xf8009dc8) = 0x00020000;
	WRS_ASM("sync");
	*(volatile unsigned int *)(0xf8009dd0) = 0x80f5501b; 
	WRS_ASM("sync");
	*(volatile unsigned int *)(0xf8009dc0) = 0x00010000;
	WRS_ASM("sync");
#endif
	pDevCtx->nDevInitFlag = 1;
	return HWA_FC_OP_OK;
}

extern int sysClkRateGet(void);

void sMsSleep (int ms)  
{  
    int m = sysClkRateGet(); 
    int i = 0;
   // m = 1000/m;  
   // m = ms/m + 1;  
    while(i++<(50*1000)); 
}  


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverStartDma(THwaFcDevList *pDevCtx,unsigned int nSize,THwaFcPortDir eDir)
{
	int nSizeTmp = nSize / 4;
	int nLpsSize = 0;
	int nLpsCnt = 1;
	FC_DBG_FUNC_CALL;
	if(nSizeTmp <= MAX_LPS_SIZE)
	{
		nLpsSize = nSizeTmp + nSizeTmp % 2; 
	}else{
		nLpsSize = MAX_LPS_SIZE;
		nLpsCnt = (nSizeTmp / nLpsSize) + (((nSizeTmp % nLpsSize) == 0) ? 0 : 1);
	}
	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaLock);

	if(eDir == eDirSend){
		cacheFlush(DATA_CACHE,(void*)pDevCtx->nHwaFcDrvHandle.pBufOut,MAX_DMA_SIZE);
	}else if(eDir == eDirRecv){
		cacheFlush(DATA_CACHE,(void*)pDevCtx->nHwaFcDrvHandle.pBufIn,MAX_DMA_SIZE);	
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RECV_REAL_LEN,(nSizeTmp + nSizeTmp % 2) - 2);
	}
	/*init Dma register*/
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);

	if(eDir == eDirSend)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPA_W,(((unsigned int)pDevCtx->nHwaFcDrvHandle.pBufOut) & 0xfffffffc));
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPS_W,nLpsSize);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPC_W,nLpsCnt);		
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_START,DMA_START_SEND);
		HwaFcDriverDmaDone(pDevCtx);
	}
	else if(eDir == eDirRecv)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPA_R,(((unsigned int)pDevCtx->nHwaFcDrvHandle.pBufIn) & 0xfffffffc));		
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPS_R,nLpsSize);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPC_R,nLpsCnt);	
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_START,DMA_START_RECV);
	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
	}
	RELEASE_LOCK(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RECV_RST);	
	return nSize;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int GetSystemTime(THwaFcSysTime *pTime)
{
	time_t nTime;
	struct tm nLocalTime;  
	FC_DBG_FUNC_CALL;
	if(time(&nTime) == (unsigned long)(-1))
	{
		ErrLog("%s","Get system time failed.");
		return HWA_FC_ERR_OPEN;
	}
	localtime_r(&nTime,&nLocalTime);
	pTime->wDay = nLocalTime.tm_yday;
	pTime->wHour = nLocalTime.tm_hour;
	pTime->wMinute = nLocalTime.tm_min;
	pTime->wSecond = nLocalTime.tm_sec;
	pTime->wMilliseconds = 0;
	return HWA_FC_OP_OK;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
void TraceLog(const char *sFormat, ...)
{
	return;
}




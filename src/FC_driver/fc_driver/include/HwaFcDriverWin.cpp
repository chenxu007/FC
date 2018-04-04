//WDC_DEVICE
//WDC_GET_ENABLED_INT_LAST_MSG
//WDC_GET_ENABLED_INT_TYPE
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverListDev(void)
{
	DWORD nDevCnt = 0;
	DWORD i = 0;
	WDC_PCI_SCAN_RESULT nPciScanResult;
	DWORD dwStatus = WDC_PciScanDevices(HWA_FC_VENDOR_ID,HWA_FC_DEVICE_ID,&nPciScanResult);

	if(!LibInit_count)
	{
		ErrLog("Hwa fc card driver not init.");
		return HWA_FC_ERR_PARA;
	}

	if(dwStatus != WD_STATUS_SUCCESS)
	{
		ErrLog("Scan pcie devices faied.Error 0x%lx",dwStatus);
		goto SIM;
	}
	for(i = 0; i < nPciScanResult.dwNumDevices;i++)
	{
		nHwaFcDev[i + 1].nDevInfo.nSlotNum = nPciScanResult.deviceSlot[i].dwSlot;
		nHwaFcDev[i + 1].nDevInfo.nBusNum = nPciScanResult.deviceSlot[i].dwBus;
		nHwaFcDev[i + 1].nDevInfo.nFuncNum = nPciScanResult.deviceSlot[i].dwFunction;
		nHwaFcDev[i + 1].nDevInfo.nVendorID = nPciScanResult.deviceId[i].dwVendorId;
		nHwaFcDev[i + 1].nDevInfo.nDeviceID = nPciScanResult.deviceId[i].dwDeviceId;
		nHwaFcDev[i + 1].nVailed = 1;
		nHwaFcDev[i + 1].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwSlot = nHwaFcDev[i + 1].nDevInfo.nSlotNum;
		nHwaFcDev[i + 1].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwFunction = nHwaFcDev[i + 1].nDevInfo.nFuncNum;
		nHwaFcDev[i + 1].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwBus = nHwaFcDev[i + 1].nDevInfo.nBusNum;

		dwStatus  = WDC_PciGetDeviceInfo(&nHwaFcDev[i+1].nHwaFcDrvHandle.nPciDevInfo);
		if(dwStatus != WD_STATUS_SUCCESS)
		{
			ErrLog("Get devide information faild.Error 0x%lx",dwStatus);
			return HWA_FC_ERR_INIT;
		}
#if 1
		if(HwaFcDriverOpenDevice( i+ 1) != HWA_FC_OP_OK)
		{
			ErrLog("Get devide information faild.");
			continue;
		}
/*Get device version*/
		nHwaFcDev[i + 1].nDevInfo.nHwVer = HwaFcDriverGetDeivceVersion(i + 1);
/*Get device serial*/
		HwaFcDriverGetSerialNum(i + 1,nHwaFcDev[i+1].nDevInfo.sFcSerial,SERIAL_LEN);
/*Set default Device name*/
		SPRINTF(nHwaFcDev[i + 1].nDevName,"%s",nHwaFcDev[i+1].nDevInfo.sFcSerial);
/*Set default node name*/
		nHwaFcDev[i+1].nNodeName[0] = 0x10;
		nHwaFcDev[i+1].nNodeName[1] = 0x00;
		nHwaFcDev[i+1].nNodeName[2] = 0x00;
		nHwaFcDev[i+1].nNodeName[3] = 0x05;
		nHwaFcDev[i+1].nNodeName[4] = 0x1e;
		nHwaFcDev[i+1].nNodeName[5] = 0xfc;
		nHwaFcDev[i+1].nNodeName[6] = 0x02;
		nHwaFcDev[i+1].nNodeName[7] = 0xe1;
		HwaFcDriverCloseDevice( i + 1);
#endif
	}
SIM:
	/*for simulate*/
	nHwaFcDev[0].nDevInfo.nSlotNum = 0;
	nHwaFcDev[0].nDevInfo.nBusNum = 1;
	nHwaFcDev[0].nDevInfo.nFuncNum = 2;
	nHwaFcDev[0].nDevInfo.nVendorID = 0x1234;
	nHwaFcDev[0].nDevInfo.nDeviceID = 0xffee;
	nHwaFcDev[0].nVailed = 1;
	nHwaFcDev[0].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwSlot = 0;
	nHwaFcDev[0].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwFunction = 2;
	nHwaFcDev[0].nHwaFcDrvHandle.nPciDevInfo.pciSlot.dwBus = 1;
	/*Set default node name*/
	nHwaFcDev[0].nNodeName[0] = 0x10;
	nHwaFcDev[0].nNodeName[1] = 0x00;
	nHwaFcDev[0].nNodeName[2] = 0x00;
	nHwaFcDev[0].nNodeName[3] = 0x20;
	nHwaFcDev[0].nNodeName[4] = 0x21;
	nHwaFcDev[0].nNodeName[5] = 0x22;
	nHwaFcDev[0].nNodeName[6] = 0x23;
	nHwaFcDev[0].nNodeName[7] = 0x24;
	SPRINTF(nHwaFcDev[0].nDevInfo.sFcSerial,SERIAL_LEN,"FC SIMULATE");
	nHwaFcDevCnt = i + 1;

	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
#define KP_HWAFCNIC_DRIVER_NAME "KP_HWAFF"
int HwaFcDriverInitLib(void)
{
	DWORD dwStatus;

	/* init only once */
	if (++LibInit_count > 1)
	{
		return HWA_FC_OP_OK;
	}

	/* Set the driver name */
#ifdef WD_DRIVER_NAME_CHANGE
	if (!WD_DriverName(HWAFC_DEFAULT_DRIVER_NAME))
	{
		ErrLog("Failed to set the driver name for WDC library.\n");
		return WD_SYSTEM_INTERNAL_ERROR;
	}
#endif
	/* Set WDC library's debug options (default: level TRACE, output to Debug Monitor) */
	dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed to initialize debug options for WDC library.\n"
			"Error 0x%lx\n", dwStatus);

		return HWA_FC_ERR_INIT;
	}

	/* Open a handle to the driver and initialize the WDC library */
	dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, HWAFC_DEFAULT_LICENSE_STRING);
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed to initialize the WDC library. Error 0x%lx\n",
			dwStatus);

		return HWA_FC_ERR_INIT;
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
DWORD HwaFcDriverUninitLib(void)
{
	DWORD dwStatus;

	if (--LibInit_count > 0)
		return HWA_FC_OP_OK;

	/* Uninit the WDC library and close the handle to WinDriver */
	dwStatus = WDC_DriverClose();
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed to uninit the WDC library. Error 0x%lx\n",
			dwStatus);
		return HWA_FC_ERR_UNINIT;
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
int HwaFcDriverCloseDma(THwaFcDevList *pDevCtx)
{

	DWORD  dwStatus = 0;

	if(pDevCtx->eFcDevMode == eModeCapture)
	{	
		for(int i = 0; i < CAP_MODE_DMA_BUF_CNT; i++)
		{
			if(pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pDma)
			{
				dwStatus = WDC_DMABufUnlock(pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pDma);
				if (WD_STATUS_SUCCESS != dwStatus)
				{
					ErrLog("Failed Unlocking a Contiguous DMA buffer0. Error 0x%lx\n", dwStatus);
					return dwStatus;
				}else{
					pDevCtx->nHwaFcDrvHandle.pDmaBuf[i] = NULL;
				}
				if(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[i]  != HWA_FC_INVALIED_HANDLE)
				{
					CloseHandle(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[i]);
					pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[i] = HWA_FC_INVALIED_HANDLE;
				}
			}

		}	
	}else{
		if(pDevCtx->nHwaFcDrvHandle.nDmaIn.pDma)
		{
			dwStatus = WDC_DMABufUnlock(pDevCtx->nHwaFcDrvHandle.nDmaIn.pDma);
			if (WD_STATUS_SUCCESS != dwStatus)
			{
				ErrLog("Failed Unlocking a Contiguous DMA buffer0. Error 0x%lx\n", dwStatus);
				return dwStatus;
			}
		}
	}
	if(pDevCtx->nHwaFcDrvHandle.nDmaOut.pDma)
	{
		dwStatus = WDC_DMABufUnlock(pDevCtx->nHwaFcDrvHandle.nDmaOut.pDma);
		if (WD_STATUS_SUCCESS != dwStatus)
		{
			ErrLog("Failed locking a Contiguous DMA buffer1. Error 0x%lx\n", dwStatus);
			return dwStatus;
		}
	}
	SAFE_FREE(pDevCtx->nHwaFcDrvHandle.pIntTransCmds);
	return 0;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverOpenDma(int nCardNum)
{

	DWORD dwStatus = 0;
	unsigned int nAddr = 0;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	int nSize = MAX_DMA_SIZE + MAX_DMA_SIZE_NEED_ADD;
	if(pDevCtx->eFcDevMode == eModeNormal)
	{
		do{ 
			dwStatus = WDC_DMAContigBufLock(pDevCtx->nHwaFcDrvHandle.pDevHandle, 
				&(pDevCtx->nHwaFcDrvHandle.pBufIn), DMA_READ_FROM_DEVICE, 
				nSize, &(pDevCtx->nHwaFcDrvHandle.nDmaIn.pDma));
			if (WD_STATUS_SUCCESS != dwStatus)
			{
				nSize /=2;
			}else{
				break;
			}
		}while(nSize > 0);

		if(nSize <= 0)
		{
			ErrLog("Failed locking a Contiguous DMA buffer0. Error 0x%lx\n", dwStatus);
			return dwStatus;

		}
		nAddr = pDevCtx->nHwaFcDrvHandle.nDmaIn.pDma->Page[0].pPhysicalAddr & 0xfffffffc;
		printf("Get Address 0x%x\n",nAddr);

		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPA_R,nAddr);		
	}else if(pDevCtx->eFcDevMode == eModeCapture)
	{
		for(int i = 0; i < CAP_MODE_DMA_BUF_CNT; i++)
		{
			dwStatus = WDC_DMAContigBufLock(pDevCtx->nHwaFcDrvHandle.pDevHandle, 
				&(pDevCtx->nHwaFcDrvHandle.pDmaBuf[i]), DMA_READ_FROM_DEVICE, 
				nSize, &(pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pDma));
			if (WD_STATUS_SUCCESS != dwStatus)
			{
				for(int j = 0; j < i ; j++)
				{
					dwStatus = WDC_DMABufUnlock(pDevCtx->nHwaFcDrvHandle.nCapModeDma[j].pDma);
					if(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j] != HWA_FC_INVALIED_HANDLE)
					{
						CloseHandle(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j]);
						pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j]  = HWA_FC_INVALIED_HANDLE;
					}
				}
				ErrLog("Failed Unlocking a Contiguous DMA buffer0. Error 0x%lx\n", dwStatus);
				return HWA_FC_ERR_OPEN;
			}
			printf("Get Phy Address :0x%x\n",pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pDma->Page[0].pPhysicalAddr);
			pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].nPhyAddr = (unsigned int )pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pDma->Page[0].pPhysicalAddr;

			pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[i] = CreateEvent(NULL,FALSE,FALSE,NULL);
			if(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[i] == HWA_FC_INVALIED_HANDLE)
			{
				for(int j = 0; j < i ; j++)
				{
					dwStatus = WDC_DMABufUnlock(pDevCtx->nHwaFcDrvHandle.nCapModeDma[j].pDma);
					if(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j] != HWA_FC_INVALIED_HANDLE)
					{
						CloseHandle(pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j]);
						pDevCtx->nHwaFcDrvHandle.nDmaUsedEvent[j]  = HWA_FC_INVALIED_HANDLE;
					}
				}
				ErrLog("Failed Unlocking a Contiguous DMA buffer0. Error 0x%lx\n", dwStatus);
				return HWA_FC_ERR_OPEN;

			}

			pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].nIndex = i;
		}
		for(int i = 0; i < CAP_MODE_DMA_BUF_CNT - 1; i++)
		{
			pDevCtx->nHwaFcDrvHandle.nCapModeDma[i].pNext =  &pDevCtx->nHwaFcDrvHandle.nCapModeDma[i + 1];
		}
		pDevCtx->nHwaFcDrvHandle.nCapModeDma[CAP_MODE_DMA_BUF_CNT - 1].pNext = &(pDevCtx->nHwaFcDrvHandle.nCapModeDma[0]);
		pDevCtx->nHwaFcDrvHandle.pCurDma = &(pDevCtx->nHwaFcDrvHandle.nCapModeDma[0]);
	}
	dwStatus = WDC_DMAContigBufLock(pDevCtx->nHwaFcDrvHandle.pDevHandle, 
		&(pDevCtx->nHwaFcDrvHandle.pBufOut), DMA_WRITE_TO_DEVICE, 
		MAX_DMA_SIZE, &(pDevCtx->nHwaFcDrvHandle.nDmaOut.pDma));
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed locking a Contiguous DMA buffer1. Error 0x%lx\n", dwStatus);
		return -1;
	}
	nAddr = (((unsigned int)pDevCtx->nHwaFcDrvHandle.nDmaOut.pDma->Page[0].pPhysicalAddr) & 0xfffffffc);
	printf("Get Address 0x%x\n",nAddr);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPA_W,nAddr);	
	return HWA_FC_OP_OK;
}

#define INTR_TYPE_BYTE		1
#define INTR_FIN_TYPE_BYTE  5
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
  int enter =0;
int err = 0;
int other = 0;
extern  int reset_send;
extern  int reset_recv;
static void DLLCALLCONV HwaFcIntHandler(PVOID pData)
{
	PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
	THwaFcDevList *pDevCtx = (THwaFcDevList *)WDC_GetDevContext(pDev);
	unsigned int nRegValue = 0;
	unsigned int nRegValue1 = 0;
	static int i1 = 0;
	static int i2 = 0;
	static int i3 = 0;

	if (!pDevCtx)
	{
		ErrLog("CardContext null");
		return;
	}	
#if 0
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,BIT8);//屏蔽中断
	HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_STAT,&nRegValue);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,0);//可以报中断
	
   
#else
	nRegValue = pDevCtx->nHwaFcDrvHandle.pIntTransCmds[INTR_TYPE_BYTE].Data.Dword & 0xffff;
#endif
	 enter++;
	///nRegValue &= 0x3;

	/*bit9:dmadone  bit10:recv event*/
	nRegValue = nRegValue>>9;
	nRegValue = nRegValue&0x3;
	switch(nRegValue)
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
			i1++;
			//printf("i1 = %d\n",i1);
		}
#endif
		break;
	case 2://Get Data Interrupt
		//if(pDevCtx->isRecvData == 0)
		{
			SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
			i2++;
			//printf("i2 = %d\n",i2);
		}
		break;
	case 3:
		SET_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
		SET_EVENT(pDevCtx->nHwaFcDrvHandle.nRecvEvent);
		i3++;
		//printf("i3 = %d\n",i3);
		break;
	default:
		other++;
		break;
	}
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
	int i = 0; 
	DWORD dwStatus;
	WD_TRANSFER *pTrans = NULL;
	WDC_ADDR_DESC *pAddrDesc = NULL;

	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	WDC_DEVICE_HANDLE hDev = pDevCtx->nHwaFcDrvHandle.pDevHandle;
	PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}

	TraceLog("HwaFcDriver_IntEnable entered. Device handle: 0x%p\n", hDev);

	if (!IsValidDevice(pDev, "HwaFcDriver_IntEnable"))
		return WD_INVALID_PARAMETER;

	if (!IsItemExists(pDev, ITEM_INTERRUPT))
		return WD_OPERATION_FAILED;

	pDevCtx = (THwaFcDevList *)WDC_GetDevContext(hDev);

	/* Check if interrupts are already enabled */
	if (WDC_IntIsEnabled(hDev))
	{
		ErrLog("Interrupts are already enabled ...\n");
		return WD_OPERATION_ALREADY_DONE;
	}

	/* Define the number of interrupt transfer commands to use */
#define NUM_TRANS_CMDS 4
#if 1
	pTrans = (WD_TRANSFER*)calloc(NUM_TRANS_CMDS, sizeof(WD_TRANSFER));
	if (!pTrans)
	{
		return WD_OPERATION_FAILED;
	}
	pAddrDesc = &(pDev->pAddrDesc[eBar0]);
	
	/*clear interrupr and disable other*/
	pTrans[i].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
	pTrans[i].dwPort = pAddrDesc->kptAddr + PCIE_REG_INTR_CLEAR;
	pTrans[i].Data.Dword = BIT8;
	i++;

	/*Get Interrupt Type*/
	pTrans[i].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;
	pTrans[i].dwPort = pAddrDesc->kptAddr + PCIE_REG_INTR_STAT;
	i++;

	/*open interrupt*/
	pTrans[i].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
	pTrans[i].dwPort = pAddrDesc->kptAddr + PCIE_REG_INTR_CLEAR;
	pTrans[i].Data.Dword = 0;
	i++;

	pTrans[i].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
	pTrans[i].dwPort = pAddrDesc->kptAddr + 0x5c;
	pTrans[i].Data.Dword = 0xaaaaaaaa;
	i++;
#endif

	dwStatus = WDC_IntEnable(hDev, pTrans, i, INTERRUPT_CMD_COPY, //| INTERRUPT_MESSAGE,
		HwaFcIntHandler, (PVOID)pDev, WDC_IS_KP(hDev));

	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed enabling interrupts. Error 0x%lx\n",
			dwStatus);
		SAFE_FREE(pTrans);
		return dwStatus;
	}

	/* TODO: You can add code here to write to the device in order
	to physically enable the hardware interrupts */

	pDevCtx->nHwaFcDrvHandle.pIntTransCmds = pTrans;
	TraceLog("HwaFcDriver_IntEnable: Interrupts enabled\n");

	return WD_STATUS_SUCCESS;
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
	DWORD dwStatus;

	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	WDC_DEVICE_HANDLE hDev = pDevCtx->nHwaFcDrvHandle.pDevHandle;
	PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

	if(IS_SIMULATE)
	{
		return HWA_FC_OP_OK;
	}

	TraceLog("HwaFcDriver_IntDisable entered. Device handle: 0x%p\n", hDev);

	if (!IsValidDevice(pDev, "HwaFcDriver_IntDisable"))
		return WD_INVALID_PARAMETER;

	pDevCtx = (THwaFcDevList *)WDC_GetDevContext(hDev);

	if (!WDC_IntIsEnabled(hDev))
	{
		ErrLog("Interrupts are already disabled ...\n");
		return WD_OPERATION_ALREADY_DONE;
	}

	/* TODO: You can add code here to write to the device in order
	to physically disable the hardware interrupts */

	/* Disable the interrupts */
	dwStatus = WDC_IntDisable(hDev);
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed disabling interrupts. Error 0x%lx\n",
			dwStatus);
	}

	SAFE_FREE(pDevCtx->nHwaFcDrvHandle.pIntTransCmds);

	return dwStatus;
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
	DWORD dwStatus = 0;	
	int ret = 0;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	WDC_DEVICE_HANDLE pDev = pDevCtx->nHwaFcDrvHandle.pDevHandle;

	CHECK_CARD_ST(nCardNum);

	if(nCardNum == 0)
	{
		return HWA_FC_OP_OK;
	}

	TraceLog("HwaFcDriver_DeviceClose entered. Device handle: 0x%p\n", pDev);

	if (!pDev)
	{
		ErrLog("HwaFcDriver_DeviceClose: Error - NULL device handle\n");
		GOTO_ERR(HWA_FC_ERR_CLOSE);
	}

	/* Close the device */
	dwStatus = WDC_PciDeviceClose(pDev);
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx\n",
			pDev, dwStatus);
		GOTO_ERR(HWA_FC_ERR_CLOSE);
	}
	pDevCtx->nDevInitFlag = 0; 
	return HWA_FC_OP_OK;
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
int HwaFcDriverOpenDevice(int nCardNum)
{
	DWORD dwStatus = 0;
	int ret = 0; 
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	WD_PCI_CARD_INFO *pDeviceInfo = &(pDevCtx->nHwaFcDrvHandle.nPciDevInfo);

	/* Validate arguments */
	if (!pDeviceInfo && pDevCtx->nVailed == 1)
	{
		ErrLog("HwaFcDriver_DeviceOpen: Error - NULL device information struct pointer\n");
		GOTO_ERR(HWA_FC_ERR_OPEN);
	}

	if(pDevCtx->nDevInitFlag == 1){
		return HWA_FC_OP_OK;
	}

	if(!IS_SIMULATE)
	{
		/* Open a WDC device handle */
		dwStatus = WDC_PciDeviceOpen(&(pDevCtx->nHwaFcDrvHandle.pDevHandle), pDeviceInfo, pDevCtx, NULL, NULL, NULL);

		if (WD_STATUS_SUCCESS != dwStatus)
		{
			ErrLog("Failed opening a WDC device handle. Error 0x%lx\n",
				dwStatus);
			GOTO_ERR(HWA_FC_ERR_OPEN);
		}

		/* Validate device information */
		if (!DeviceValidate((PWDC_DEVICE)pDevCtx->nHwaFcDrvHandle.pDevHandle))
		{
			GOTO_ERR(HWA_FC_ERR_OPEN);
		}
	}
	/* Return handle to the new device */
	TraceLog("HwaFcDriver_DeviceOpen: Opened a HwaFcDriver device (handle 0x%p)\n", pDevCtx->nHwaFcDrvHandle.pDevHandle);
	pDevCtx->nDevInitFlag = 1;  
	
	if(!IS_SIMULATE)
	{
		int sts = 0;
		if(((sts = HwaFcDriverReadReg(nCardNum,eBar0,PCIE_REG_DEVICE_STAT)) &  0xfffffffe) != PCIE_DEVICE_STATUS)
		{
			ErrLog("Check device status error with 0x%x.",sts);
		
			goto ERR;
		}
	}

	return HWA_FC_OP_OK;

ERR:    
	if (pDevCtx->nHwaFcDrvHandle.pDevHandle){
		HwaFcDriverCloseDevice(nCardNum);
	}
	return HWA_FC_ERR_OPEN;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
 int test_timein = 0;
 int test_timeout = 0;
 int test_edge = 0;
  int nsend = 0;
  int nrecv = 0;
  int reset_send = 0;
  int reset_recv = 0;
  int intMode = 0;


int HwaFcDriverStartDma(THwaFcDevList *pDevCtx,unsigned int nSize,THwaFcPortDir eDir)
{
	int nLpsSize = 0;
	int nSizeTmp = nSize / 4;  //x个 DWORD  
	int nLpsCnt = 1;
	int nCntTmp = 0;
	int nRealRecvSize = nSizeTmp;

	unsigned int nValue = 0;
	
	if(eDir == eDirRecv)
	{
		if(nSizeTmp == 0)
		{
			nSizeTmp = 1;
			nRealRecvSize = 1;
		}
		if(nSizeTmp <= pDevCtx->nRecvLpsSize)
		{
			nLpsSize = nSizeTmp + nSizeTmp % 2; 
		}else{
			nLpsSize = pDevCtx->nRecvLpsSize;
			nLpsCnt = (nSizeTmp / nLpsSize) + (((nSizeTmp % nLpsSize) == 0) ? 0 : 1);
		}
		if(nSizeTmp <= 2)
		{
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RECV_REAL_LEN,nRealRecvSize);
		}else{	
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RECV_REAL_LEN,((nSizeTmp + nSizeTmp % 2) - 2));
		}
	}else{
		if(nSizeTmp == 0)
		{
			nSizeTmp = 2;
		}
		if(nSizeTmp <= pDevCtx->nSendLpsSize)
		{
			nLpsSize = nSizeTmp + nSizeTmp % 2; 
		}else{
			nLpsSize = pDevCtx->nSendLpsSize;
			nLpsCnt = (nSizeTmp / nLpsSize) + (((nSizeTmp % nLpsSize) == 0) ? 0 : 1);
		}
	}

	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	
	if(eDir == eDirSend){
		WDC_DMASyncCpu(pDevCtx->nHwaFcDrvHandle.nDmaOut.pDma);
	}else if(eDir == eDirRecv){
		WDC_DMASyncCpu(pDevCtx->nHwaFcDrvHandle.nDmaIn.pDma);	
	}
	
	if(eDir == eDirSend)
	{

		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPS_W,nLpsSize);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPC_W,nLpsCnt);		
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_START,DMA_START_SEND);
		nsend++;
		HwaFcDriverDmaDone(pDevCtx,nSize);

	}
	else if(eDir == eDirRecv)
	{


		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPS_R,nLpsSize);
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPC_R,nLpsCnt);
		pDevCtx->isInDma = 1;
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_START,DMA_START_RECV);

		nrecv++;
#ifndef RECV_INT 
		HwaFcDriverRecvDmaDone(pDevCtx);
#else
	//	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent); /*WAITE DMA DONE INTR*/
	//	test_timein++;


// 		do{
// 			if(WAIT_OBJECT_0 != WaitForSingleObject(pDevCtx->nHwaFcDrvHandle.nDmaEvent,100))
// 			{
// 				unsigned int nValue = 0;
// 				HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_Mon_rx_rec_state,&nValue);
// 				if((nValue  & 0x1) == 0)
// 				{
// 					HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,0x0);
// 					HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);
// 					HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
// 				}
// 			}
// 		}while(1);
#if 1

			do 
			{
				if(WAIT_OBJECT_0 == WaitForSingleObject(pDevCtx->nHwaFcDrvHandle.nDmaEvent,100))
				{
					HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_DONE,&nValue);
					if(((nValue>>8)  & 0x1) == 1)
					{
						test_timein++;
						reset_recv++;
						break;
					}
					intMode++;
					printf("intMode = %d\n",intMode);
					printf("test_timein = %d test_timeout = %d  test_timein+ test_timeout=%d\n",test_timein,test_timeout,test_timein+test_timeout);
					printf("reset_recv = %d\n",reset_recv);
					printf("enter = %d\n",enter);
					goto INTERR;
				}
				else
				{

					/*check rx state*/
					HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_DONE,&nValue);

					if(((nValue>>8)  & 0x1) == 1)
					{
						test_timeout++;
						reset_recv++;
						break;

					}
				}
			} while (1);
			//HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,0x0060,0x1);	
			HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RECV_RST);	
			//HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RECV_NRST);

#endif
#endif
INTERR:
		pDevCtx->isInDma = 0;
	}
	ResetEvent(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
	RELEASE_LOCK(pDevCtx->nHwaFcDrvHandle.nDmaLock);
	return nSize;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverMonStartDma(THwaFcDevList *pDevCtx,unsigned int nSize)
{
	int nLpsSize = pDevCtx->nRecvLpsSize;
	int nMaxLpsSize = pDevCtx->nRecvLpsSize;
	int nSizeTmp = nSize / 4;
	int nLpsCnt = 1;
	int nCntTmp = 0;
	nLpsCnt = (nSizeTmp/nLpsSize) + ((nSizeTmp % nLpsSize == 0) ? 0 : 1);

	if(nSizeTmp <= 2)
	{
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RECV_REAL_LEN,2);
	}else{	
		HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RECV_REAL_LEN,((nSizeTmp + nSizeTmp % 2) - 2));
	}
	/*init Dma register*/
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);	
	//HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPS_R,nLpsSize);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPC_R,nLpsCnt);
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_RDTLPA_R,(unsigned int)(pDevCtx->nHwaFcDrvHandle.pCurDma->pDma->Page[0].pPhysicalAddr) & 0xfffffffc);
	pDevCtx->isInDma = 1;
	HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DMA_START,DMA_START_RECV);
#ifndef RECV_INT 
	HwaFcDriverRecvDmaDone(pDevCtx);
#else
	WAIT_EVENT(pDevCtx->nHwaFcDrvHandle.nDmaEvent);
// 	do{
// 		if(WAIT_OBJECT_0 != WaitForSingleObject(pDevCtx->nHwaFcDrvHandle.nDmaEvent,100))
// 		{
// 			unsigned int nValue = 0;
// 			HWA_FC_READ_32(pDevCtx->nHwaFcDrvHandle,eBar0,FPGA_REG_Mon_rx_rec_state,&nValue);
// 			if((nValue  & 0x1) == 0)
// 			{
// 				HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_INTR_CLEAR,0x0);
// 				HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_RST);
// 				HWA_FC_WRITE_32(pDevCtx->nHwaFcDrvHandle,eBar0,PCIE_REG_DCR,PCIE_DMA_NRST);
// 			}
// 		}
// 	}while(1);
#endif
	pDevCtx->isInDma = 0;
	return nSize;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
void ErrLogWin(const char *sFormat, ...)
{
    va_list argp;
    va_start(argp, sFormat);
    _vsnprintf_s(sHwaFcLastErr, sizeof(sHwaFcLastErr) - 1, sFormat, argp);
	HWA_FC_DBG("%s",sHwaFcLastErr);
#if defined(DEBUG)
    WDC_Err("HwaFcDriver lib: %s", HwaFcDriverGetLastErr());
#endif
    va_end(argp);
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
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;
    va_start(argp, sFormat);
    _vsnprintf_s(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Trace("HwaFcDriver lib: %s", sMsg);
    va_end(argp);
#endif
}

void test_pcie_dma(int nCardNum){
	HwaFcDriverStartDma(&nHwaFcDev[nCardNum],120,eDirRecv);
}
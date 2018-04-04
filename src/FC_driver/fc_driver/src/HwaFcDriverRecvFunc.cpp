#include "stdafx.h"
#include "HwaFcStruct.h"
#include "HwaFcDriver.h"
#include "HwaFcDriverLib.h"
#include "HwaFcDriverPkt.h"
extern THwaFcDevList nHwaFcDev[HWA_FC_MAX_DEV_CNT];
extern int nHwaFcDevCnt;

extern char sHwaFcLastErr[256];


#define NEW_DATA_EVENT(x) 	(nHwaFcDev[x].nHwaFcDrvHandle.nRecvEvent) 
#define INTR_DRV_HANDLE(x)  (nHwaFcDev[nCardNum].nHwaFcDrvHandle)
#define RECV_PORT_PHY_2_LOGIC(x,y) nHwaFcDev[(x)].nRecvPort[nHwaFcDev[(x)].nRecvPhyPort[(y)].nLogicPortIndex]

//add by sxy 20130416: recv single or block packet mode
#define NEW_RECV_MODE 1  
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcResNorBlk(int nCardNum,char *pBuf,int nBufLen,int nPhyIndex)
{
	int nPayLoadLen = 0;
	int nBlkLen = 0;
	int nRemain = 0;
	THwaFcMsgHandle nHandleTmp = NULL;
	char *pCur = NULL;
	int* pLen = NULL;
	char *pPayload = NULL;
	THwaFcRecvMsg nMsg = {0};
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];

	//add by sxy 20130416_for normal blocked recv :nmsg.datalen =several packets total len
	unsigned int nPktCnt_reg = 0;
	unsigned int nPktCnt = 0;

//#define GET_PACKET_CHANNEL(uByte)	((uByte[0] & 0x60) >> 7)
#define GET_FRAME_LEN(x)		((((x)&0x000000ff) << 24) | (((x)&0x0000ff00) <<  8) | (((x)&0x00ff0000) >> 8) | (((x)&0xff000000) >> 24))

	nRemain = nBufLen;
	pCur = pBuf;
	//pLen = pCur + 4;
	//pPayload = pLen  + 4;
	pLen = (int*)(pCur +4);
    pPayload = pCur + 8;

	while(nRemain > 0)
	{
		nBlkLen = nMsg.nDataLen =GET_FRAME_LEN(*pLen);  //payload lentgth :big <=> small
		nBlkLen +=8;                                   //payload length+ length size
		//nMsg.nMsgInfo |= GET_PACKET_CHANNEL(pCur); 
		if(HwaFcDriverWriteFifo(&(RECV_PORT_PHY_2_LOGIC(nCardNum,nPhyIndex).nPortCfg.nRecvPortCfg.nPortFifo),pPayload,nMsg.nDataLen) != nMsg.nDataLen)
		{
			return -1;
		}
		/*get recv channel and msg type*/
		HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_flag,&nMsg.nMsgInfo);
		nHandleTmp = pDevCtx->nRecvPort[pDevCtx->nRecvPhyPort[nPhyIndex].nLogicPortIndex].nPortCfg.nRecvPortCfg.nRecvMsgHandle;
		HwaFcDriverSendMsg(nHandleTmp,&nMsg,sizeof(nMsg));

		nBlkLen += ((nBlkLen % 8 == 0) ? 0 : (8 - nBlkLen % 8));
		pCur += nBlkLen;
		pLen = (int*)(pCur +4);
		pPayload = pCur + 8;

		//pLen = pCur + 4;
		//pPayload = pLen + 4;
		nRemain -= nBlkLen;
		nPktCnt ++;
	}

	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_intr_BlockMode_PtkCnt,&nPktCnt_reg);
	if (nPktCnt == nPktCnt_reg)
	{
		return nPktCnt;
	}
	return HWA_FC_ERR_RECV_BLOCK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
//add by sxy 2130416_THwaFcRecvMode eRecvMode for normal blocked recv  
static void inline HwaFcDriverNormalModeRecvData(int nCardNum,THwaFcRecvMode eRecvMode)
{
	unsigned int nPhyIndex = -1;
	unsigned int nValue = 0;
	THwaFcRecvMsg nMsg = {0};
	THwaFcRecvMsg nMsgTmp = {0};
	THwaFcMsgHandle nHandleTmp = NULL;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned int nRecvPortIndex = -1;
	int nResDataLen = 0;
	int nDmaLen = 0;
	int nRead = 0;
	int nLinkFlag = 0;
	
	int ret = 0; //add for test
	FC_DBG_FUNC_CALL;
/*get interrupt phy port index*/
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_intr_RX_port_id,&nPhyIndex);
	
	if((nPhyIndex & 0x80000000) == 0x80000000)
	{
		THwaFcPortStatus nStatus = {0};	
		HwaFcDriverGetPortStatus(nCardNum,&pDevCtx->nLinkStatus);
		SET_EVENT(pDevCtx->nHwaFcDrvHandle.nLinkEvent);
		nMsg.nDataLen = 0;
		nLinkFlag = 1;
		goto START_DMA;
	}

	if((nPhyIndex < 0) || (nPhyIndex > HWA_FC_MAX_PORT_CNT))
	{
		return;
	}
	nRecvPortIndex = (RECV_PORT_PHY_2_LOGIC(nCardNum,nPhyIndex)).nPortIndex;


	if(pDevCtx->nRecvPort[nRecvPortIndex].nLoaded != 1)
	{
		return;
	}

/*set port index to hardware*/
	HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_port_id,nPhyIndex);
/*start recveive frame*/
	HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_rec_frame,0x1);

/*check rx state*/
	do{
 		HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_rec_state,&nValue);
 	}while(((nValue & 0x1) != RECV_FIN) && (nValue != 0xffffffff));

/*get frame len*/
	if (eRecvMode == eNormalRecvNor)// recv packet one by one,nMsg.nDataLen is real length
	{
		HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_frame_len,&nMsg.nDataLen); 
	}
#if NEW_RECV_MODE
	else if(eRecvMode == eNormalRecvBlk)  //recv packet block(several packets):nDatalenBlock have handle 8bytes;
	{
		HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_intr_BlockMode_DataLen,&nMsg.nDataLen);
		nMsg.nDataLen *=8;
	}
	nResDataLen = nMsg.nDataLen;

#endif

START_DMA:
	do{
		nDmaLen = ((nResDataLen > MAX_RECV_LEN) ? MAX_RECV_LEN : nResDataLen);
		nResDataLen -= nDmaLen;
		if(HwaFcDriverStartDma(pDevCtx,nDmaLen + ((nDmaLen % 4 == 0) ? 0 : (4 - nDmaLen % 4)),eDirRecv) < 0)
		{
			HWA_FC_DBG("DMA Failed.");
			return;
		}

		if(nLinkFlag == 1)
		{
			return;
		}

		if (eRecvMode == eNormalRecvNor)
		{
			HwaFcDriverWriteFifo(&(RECV_PORT_PHY_2_LOGIC(nCardNum,nPhyIndex).nPortCfg.nRecvPortCfg.nPortFifo),(char *)nHwaFcDev[nCardNum].nHwaFcDrvHandle.pBufIn,nDmaLen);
// 			if(HwaFcDriverWriteFifo(&(RECV_PORT_PHY_2_LOGIC(nCardNum,nPhyIndex).nPortCfg.nRecvPortCfg.nPortFifo),(char *)nHwaFcDev[nCardNum].nHwaFcDrvHandle.pBufIn,nDmaLen) != ((int)nMsg.nDataLen))
// 			{
// 				//			return;
// 			}
		}

	}while(nResDataLen > 0);

#if NEW_RECV_MODE
	if(eRecvMode == eNormalRecvBlk)
	{
		ret = HwaFcResNorBlk(nCardNum,(char *)nHwaFcDev[nCardNum].nHwaFcDrvHandle.pBufIn,nMsg.nDataLen,nPhyIndex);
		return;
	}
#endif

/*get recv time*/
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_rec_time1,&nMsg.nRecvTime.nLowTime);
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_rec_time2,&nMsg.nRecvTime.nHighTime);

/*get recv channel and msg type*/
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_flag,&nMsg.nMsgInfo);
/*send msg*/
	nHandleTmp = pDevCtx->nRecvPort[pDevCtx->nRecvPhyPort[nPhyIndex].nLogicPortIndex].nPortCfg.nRecvPortCfg.nRecvMsgHandle;
	HwaFcDriverSendMsg(nHandleTmp,&nMsg,sizeof(nMsg));
	return;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcResABlk(int nCardNum,char *pBuf,int nBufLen)
{
	int nRemain = 0;
	char *pSelDefHeader = NULL;
	char *pPayload = NULL;
	char *pCur = NULL;
	int nFrameLen = 0;
	int nPayLoadLen = 0;
	int nBlkLen = 0;
	int nPktCnt = 0;

	THwaFcSof *pSof = NULL;
	THwaFcAeHeader *pFcHeader = NULL ;
	THwaFcAsmHeader *pAsmHeader =NULL;
	THwaFcAeCrc *pCrc = NULL;
	THwaFcEof *pEof = NULL;

	nRemain = nBufLen;
	pCur = pBuf;
	while(nRemain > 0)
	{
		pSelDefHeader = pCur;
		pSof = (THwaFcSof *)(pCur + SOF_OFFSET);
		pFcHeader = (THwaFcAeHeader *)(pCur + AE_HEADER_OFFSET);
		nFrameLen = GET_PAYLOAD_LEN(pSelDefHeader);
		nBlkLen = nFrameLen + SELF_DEF_HEADER_LEN;
		nBlkLen += ((nBlkLen % 8  == 0) ? 0 :(8 - nBlkLen %8)); 

		if(IS_ASM_PKT(pCur) == 1)
		{
			pAsmHeader = (THwaFcAsmHeader *)(pCur + ASM_HEADER_OFFSET);
			nPayLoadLen = nFrameLen - ASM_NOT_PAYLOAD_LEN;
			pPayload = pCur + ASM_PAYLOAD_OFFSET;
			pCrc = (THwaFcAeCrc *)(pCur + ASM_CRC_OFFSET(nPayLoadLen));
			pEof = (THwaFcEof *)((char *)pCrc + FC_CRC_LEN);
		}else{
			nPayLoadLen = nFrameLen - AE_NOT_PAYLOAD_LEN;
			pPayload = pCur + AE_PAYLOAD_OFFSET;
			pCrc = (THwaFcAeCrc *)(pPayload + AE_CRC_OFFSET(nPayLoadLen));
			pEof = (THwaFcEof *)((char *)pCrc + FC_CRC_LEN);
		}
		pCur += nBlkLen;
		nRemain -= nBlkLen;
		nPktCnt ++;
	}
	return nPktCnt;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static void inline HwaFcDriverCaptureModeRecvData(int nCardNum,THwaFcRecvMode eRecvMode)
{
	unsigned int nValue = 0;
	int nReadCnt = 0;
	THwaFcMonRecvMsg nMsg = {0};
	
#ifdef WINDOWS_OPS	
	THwaDmaStruct *pDma = nHwaFcDev[nCardNum].nHwaFcDrvHandle.pCurDma;
#endif
	
	FC_DBG_FUNC_CALL;
/*start recveive frame*/
	HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_rec_frame,0x1);
/*check rx state*/
	do{
		if(nHwaFcDev[nCardNum].nExitFlag)
		{
			return;
		}
		if(nReadCnt > HWA_FC_DELAY_CNT)
		{
			return;
		}
		HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_Mon_rx_rec_state,&nValue);
	}while(nValue != RECV_FIN);
/*get data type*/
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_Mon_rx_flag,&nMsg.nDataType);
/*get frame len*/
	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_Mon_rx_total_len,&nMsg.nDataLen);
	
	nMsg.nDataLen = (nMsg.nDataLen) * 8;
#ifdef WINDOWS_OPS	
	nMsg.nAddr = (unsigned int )nHwaFcDev[nCardNum].nHwaFcDrvHandle.pDmaBuf[pDma->nIndex];
	nMsg.nDmaIndex = pDma->nIndex;
#endif
	if(HwaFcDriverMonStartDma(&nHwaFcDev[nCardNum],nMsg.nDataLen) <= 0)
	{
		return;
	}
#ifdef WINDOWS_OPS	
//	HwaFcResABlk(nCardNum,(char *)nHwaFcDev[nCardNum].nHwaFcDrvHandle.pDmaBuf[pDma->nIndex],nMsg.nDataLen);
	
	if(nHwaFcDev[nCardNum].eMonStat == eMonRunning)
	{
		DWORD nRetIndex = -1;
		HwaFcDriverSendMsg(nHwaFcDev[nCardNum].nMonRecvPort.nMonRecvHandle,&nMsg,sizeof(nMsg));
		nRetIndex = WaitForMultipleObjects(CAP_MODE_DMA_BUF_CNT,nHwaFcDev[nCardNum].nHwaFcDrvHandle.nDmaUsedEvent,FALSE,INFINITE);	
		nHwaFcDev[nCardNum].nHwaFcDrvHandle.pCurDma = &nHwaFcDev[nCardNum].nHwaFcDrvHandle.nCapModeDma[nRetIndex - WAIT_OBJECT_0];	
	}
#else 
	if(HwaFcDriverWriteFifo(&(nHwaFcDev[nCardNum].nMonRecvPort.nMonFifo),(char *)nHwaFcDev[nCardNum].nHwaFcDrvHandle.pBufIn,nMsg.nDataLen) != ((int)(nMsg.nDataLen)))
		{
			return;
		}
		HwaFcDriverSendMsg(nHwaFcDev[nCardNum].nMonRecvPort.nMonRecvHandle,&nMsg,sizeof(nMsg));
#endif
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */



#ifdef WINDOWS_OPS
DWORD WINAPI HwaFcDriverRecvFunc(LPVOID pPara)
#elif defined VXWORKS_OPS
int HwaFcDriverRecvFunc(int pPara)
#else 
#error "unknow operation system"
#endif
{
	printf("------entry---HwaFcDriverRecvFunc----\n");

#ifdef WINDOWS_OPS
	int nCardNum = (int) pPara;
	unsigned int nValue = 0;
	unsigned int read_intr_cnt = 0;
	THwaFcDevMode eWorkMode = nHwaFcDev[nCardNum].eFcDevMode;
	THwaFcRecvMode eRecvMode = nHwaFcDev[nCardNum].eRecvMode;
	//for test interrupt num
#if 1
	static int a = 0;
	static int a1 = 0;
	static int i = 0;
#endif

	
	CHECK_CARD_ID(nCardNum);
	FC_DBG_FUNC_CALL;
	do{

		if(WAIT_OBJECT_0 != WaitForSingleObject(NEW_DATA_EVENT(nCardNum),50))
		{
			HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_intr_poll_state,&nValue);
			HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_intr_poll_state,&nValue);
			HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_RX_intr_poll_state,&nValue);
			if((nValue & 0x1) == 0)
			{
				continue;
			}			
			a++;
		}
		else
		{
			i++;

		}
		if(nHwaFcDev[nCardNum].nExitFlag == 1)
		{
			i--;
			break;
		}
		nHwaFcDev[nCardNum].isRecvData = 1;

		if(eWorkMode == eModeNormal)
		{
			HwaFcDriverNormalModeRecvData(nCardNum,eRecvMode);
		}
		else
		{
			HwaFcDriverCaptureModeRecvData(nCardNum,eRecvMode);			
		}
		nHwaFcDev[nCardNum].isRecvData = 0;
		ResetEvent(NEW_DATA_EVENT(nCardNum));
		HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_intr_RX_frame_en,RECV_INTR_ENABLE);
					//printf("a1 = %d\n",a1++);
	}while(1);

#if 1

	HWA_FC_READ_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_Rx_intr_cnt,&read_intr_cnt);
    getchar();
	//a = 0;
	i = 0;
#endif

	return 0;

#else //vxworks

int nCardNum = (int) pPara;
	THwaFcDevMode eWorkMode = nHwaFcDev[nCardNum].eFcDevMode;
	CHECK_CARD_ID(nCardNum);
	THwaFcRecvMode eRecvMode = nHwaFcDev[nCardNum].eRecvMode;
	FC_DBG_FUNC_CALL;
	do{
#if 1	
		WAIT_EVENT(NEW_DATA_EVENT(nCardNum));
//		static int IntrWrite_cnt=0;
//		printf("Intr Counter %d time(s)\n",++IntrWrite_cnt);
#else
		Sleep(8000);
#endif
		//printf("-------%s--------%d--------\n",__FUNCTION__,__LINE__);

		if(HwaFcDriverGetRecvLock(nCardNum) != HWA_FC_OP_OK)
		{
			HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_intr_RX_frame_en,RECV_INTR_ENABLE);
			continue;
		}
		//printf("-------%s--------%d--------\n",__FUNCTION__,__LINE__);

		if(eWorkMode == eModeNormal)
		{
		//printf("-------%s--------%d--------\n",__FUNCTION__,__LINE__);
			HwaFcDriverNormalModeRecvData(nCardNum,eRecvMode);
		}
		else
		{
			HwaFcDriverCaptureModeRecvData(nCardNum,eRecvMode);			
		}
		HwaFcDriverReleaseRecvLock(nCardNum);
		HWA_FC_WRITE_32(INTR_DRV_HANDLE(nCardNum),eBar0,FPGA_REG_intr_RX_frame_en,RECV_INTR_ENABLE);
		//printf("-------%s--------%d--------\n",__FUNCTION__,__LINE__);
	}while(nHwaFcDev[nCardNum].nExitFlag == 0);
	return 0;
#endif
}

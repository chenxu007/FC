#include "stdafx.h"
#include "HwaFcStruct.h"
#include "HwaFcDriver.h"
#include "HwaFcDriverLib.h"
#include "string.h"
extern char sHwaFcLastErr[256];
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverWriteFifo(THwaFcSoftFifo *pFifo,char *pBuf,unsigned int len)
{
	unsigned int nLeft = 0;
	unsigned int nWrite = 0;
	FC_DBG_FUNC_CALL;

	if((pBuf == NULL) || (len == 0))
	{
		return 0;
	}
	if(pFifo->nInit != 1)
	{
		return 0;
	}
	WAIT_EVENT(pFifo->nLock);
	if((pFifo->nBufSize - pFifo->nDataCnt) < len)
	{
		RELEASE_LOCK(pFifo->nLock);
		return -1;
	}else{
		if((pFifo->pWritePos + len) > (pFifo->pLast))
		{
			nLeft = len - (pFifo->pLast - pFifo->pWritePos);
			nWrite = len - nLeft;

			memcpy(pFifo->pWritePos,pBuf,nWrite);
			memcpy(pFifo->pBuf,(pBuf + nWrite),nLeft);

			pFifo->pWritePos = pFifo->pBuf + nLeft;
		}else{
			memcpy(pFifo->pWritePos,pBuf,len);
			pFifo->pWritePos += len;
		}
	}
	pFifo->nDataCnt += len;
	RELEASE_LOCK(pFifo->nLock);
	return len;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverReadFifo(THwaFcSoftFifo *pFifo,char *pBuf,unsigned int nBufLen,unsigned int len)
{
	unsigned int nLeft = 0;
	unsigned int nRead = 0;
	unsigned int nWant = 0;
	FC_DBG_FUNC_CALL;

	if(pBuf == NULL)
	{
		return HWA_FC_ERR_PARA;
	}
	if(len == 0)
	{
		return HWA_FC_OP_OK;
	}
	if(pFifo->nInit != 1)
	{
		return 0;
	}
	if(nBufLen >= len)
	{
		nWant = len;
	}else{
		nWant = nBufLen;
	}

	nWant = nWant > pFifo->nDataCnt ? pFifo->nDataCnt : nWant;

	if(pFifo->nDataCnt == 0)
	{
		return 0;
	}
	WAIT_EVENT(pFifo->nLock);	
	if((pFifo->pReadPos + nWant) > pFifo->pLast)
	{
		nLeft = nWant - (pFifo->pLast - pFifo->pReadPos);
		nRead = nWant - nLeft;
		memcpy(pBuf,pFifo->pReadPos,nRead);
		memcpy((pBuf + nRead),pFifo->pBuf,nLeft);
	}else{
		memcpy(pBuf,pFifo->pReadPos,nWant);
	}

	if((pFifo->pReadPos + len) > pFifo->pLast)
	{
		pFifo->pReadPos = pFifo->pBuf + (len - (pFifo->pLast - pFifo->pReadPos));
	}else{
		pFifo->pReadPos += len;
	}
	pFifo->nDataCnt -= len;
	
	RELEASE_LOCK(pFifo->nLock);
	return nWant;
}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverInitFifo(THwaFcSoftFifo *pFifo,unsigned int nFifoSize)
{
	FC_DBG_FUNC_CALL;
	memset(pFifo,0,sizeof(THwaFcSoftFifo));
	
	pFifo->pBuf = (char *)malloc(nFifoSize);
	
	if(pFifo->pBuf == NULL)
	{
		ErrLog("%s","No enough memory");
		return HWA_FC_ERR_PARA;
	}	
	pFifo->nBufSize = nFifoSize;
	pFifo->pLast = pFifo->pBuf + (pFifo->nBufSize - 1);
	pFifo->pReadPos = pFifo->pBuf;
	pFifo->pWritePos = pFifo->pBuf;
	if((pFifo->nLock = HwaFcInitLock(pFifo->sLockName,"0")) == HWA_FC_INVALIED_HANDLE)
	{
		return HWA_FC_ERR_MSG;
	}
	pFifo->nInit = 1;
	return 0;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDeInitFifo(THwaFcSoftFifo *pFifo)
{
	FC_DBG_FUNC_CALL;
	if(pFifo->pBuf)
	{
		SAFE_FREE(pFifo->pBuf);
	}	
	memset(pFifo,0,sizeof(THwaFcSoftFifo));
	SAFE_CLOSE(pFifo->nLock);
	return 0;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverReadMsg(THwaFcMsgHandle nHandle,void *pBuf,int nBufLen,int *pReadLen,int time)
{
	FC_DBG_FUNC_CALL;

#ifdef WINDOWS_OPS
	if(!pReadLen  || !pBuf)
	{
		ErrLog("%s","Got null pointer.");	
		return HWA_FC_ERR_PARA;
	}
	*pReadLen  = 0;

	if(nHandle  == HWA_FC_INVALIED_HANDLE) 
	{
		return HWA_FC_ERR_PARA;
	}
	
	if(time == 0)
	{
		time = INFINITE;
	}else{
		time *= 1000;
	}
	OVERLAPPED op;
	memset(&op,0,sizeof(op));
	op.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	DWORD dwBytesReaded = 0;
#if 0
	BOOL res = ReadFile(nHandle,pBuf, nBufLen,&dwBytesReaded,NULL);
	if(dwBytesReaded == nBufLen)
	{
		*pReadLen = dwBytesReaded;
		return HWA_FC_OP_OK;
	}else{
		return HWA_FC_ERR_MSG;
	}
#else
	BOOL res = ReadFile(nHandle,pBuf, nBufLen,&dwBytesReaded,&op);
	DWORD t = 0;
	if((!res) && (t = GetLastError()) != ERROR_IO_PENDING){
		return FALSE;
	}

	DWORD dwResult = WaitForSingleObject(op.hEvent,time);


	if(dwResult ==  WAIT_OBJECT_0)
	{
		GetOverlappedResult(nHandle,&op,&dwBytesReaded,FALSE);
		if(dwBytesReaded == nBufLen)
		{
			RESET_EVENT(op.hEvent);
			SAFE_CLOSE(op.hEvent);
			*pReadLen = dwBytesReaded;
			return HWA_FC_OP_OK;
		}
	}
	int mRet = GetOverlappedResult(nHandle,&op,&dwBytesReaded,FALSE);
	mRet = GetLastError();
	CancelIo(nHandle);
	RESET_EVENT(op.hEvent);
	SAFE_CLOSE(op.hEvent);
	ErrLog("Read message failed.");
	return HWA_FC_ERR_MSG;
#endif

#elif defined VXWORKS_OPS
	*pReadLen = msgQReceive(nHandle,(char *)pBuf, nBufLen, time);

	if(*pReadLen == ERROR)
	{			
		ErrLog("%s","Read message failed.");
		return HWA_FC_ERR_MSG;
	}
	return HWA_FC_OP_OK;

#else
#error "unknown operation system"
#endif
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
THwaFcMsgHandle  HwaFcDriverOpenMsg(LPSTR pMsgName)
{
	THwaFcMsgHandle nHandle = HWA_FC_INVALIED_HANDLE;
	FC_DBG_FUNC_CALL;
#ifdef WINDOWS_OPS
	nHandle = CreateFile( 
		pMsgName,		// pipe name 
		GENERIC_WRITE | GENERIC_READ, 
		0,				// no sharing 
		NULL,			// default security attributes
		OPEN_EXISTING,	// opens existing pipe 
		0,				// default attributes 
		NULL);		   // no template file 

	if(HWA_FC_INVALIED_HANDLE == nHandle){
		ErrLog("Open message failed.");
		return NULL;
	}

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if(!SetNamedPipeHandleState(nHandle,&dwMode,NULL,NULL))
	{
		ErrLog("Open message failed.");
		return NULL;
	}
#elif defined VXWORKS_OPS
	nHandle = msgQCreate(HWA_FC_MSG_CNT,sizeof(THwaFcRecvMsg),MSG_Q_FIFO);
	if(nHandle == HWA_FC_INVALIED_HANDLE)
	{
		ErrLog("%s","Create message failed.");
		return NULL;
	}
#else
#error "unknown operation system."
#endif
	return nHandle;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
#ifdef WINDOWS_OPS
THwaFcMsgHandle HwaFcDriverCreateMsg(LPSTR pMsgName)
{
	THwaFcMsgHandle nHandle = NULL;
	FC_DBG_FUNC_CALL;
	nHandle  = CreateNamedPipe( 
		pMsgName,										// pipe name 
		PIPE_ACCESS_DUPLEX| 							// read/write access 
		FILE_FLAG_OVERLAPPED,							// overlapped mode 
		PIPE_TYPE_MESSAGE |								// message-type pipe 
		PIPE_READMODE_MESSAGE |							// message-read mode 
		PIPE_WAIT,										// blocking mode 
		1,													// number of instances 
		(HWA_FC_MSG_CNT + 100) * sizeof(THwaFcRecvMsg),	// output buffer size 
		(HWA_FC_MSG_CNT + 100) * sizeof(THwaFcRecvMsg),	// input buffer size 
		INFINITE,												// client time-out 
		NULL);											// default security attributes 

	if(nHandle  == HWA_FC_INVALIED_HANDLE)
	{
		if(GetLastError() != ERROR_FILE_EXISTS)
		{
			ErrLog("Create message failed.");
			return NULL;
		}
	}
	return nHandle;
}
#endif
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverSendMsg(THwaFcMsgHandle nHandle,void *pBuf, int nBufLen)
{
	BOOL fSuccess;
	FC_DBG_FUNC_CALL;
#ifdef WINDOWS_OPS
	int dwBytesWritten;
#endif
// 	if(!pBuf){
// 		ErrLog("%s","Got wrong pointer.");
// 		return HWA_FC_ERR_PARA;
// 	}

// 	if(nHandle == HWA_FC_INVALIED_HANDLE)
// 	{
// 		ErrLog("%s","Got wrong message handle.");
// 		return HWA_FC_ERR_MSG;
// 	}
#ifdef WINDOWS_OPS
	fSuccess = WriteFile(nHandle,pBuf,nBufLen,(LPDWORD)&dwBytesWritten,NULL);
//	if(fSuccess && dwBytesWritten == nBufLen)
// 	{
// //		int ret = FlushFileBuffers(nHandle);
// 	}
#elif defined VXWORKS_OPS
	if(msgQSend (nHandle, (char *)pBuf, nBufLen, NO_WAIT, MSG_PRI_NORMAL) != OK)
	{
		fSuccess = FALSE;
	}
#else
#error "unknown operation system."
#endif
	if (!fSuccess){
		printf("Send MSG Failed.\n");
		ErrLog("%s","Send message failed.");
		return HWA_FC_ERR_MSG;
	}
	return HWA_FC_OP_OK;
}

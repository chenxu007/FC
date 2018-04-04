#include "stdafx.h"
#include "HwaFcDriverPkt.h"
#include "HwaFcDrvLoginPktTmp.h"
#include "HwaFcStruct.h"
#include "HwaFcDriverLib.h"
#define HWA_FC_DID_POS 5
#define HWA_FC_SID_POS 9
#define HWA_FC_OXID_POS 20

#define HWA_FC_FPLOGIN_PKT_NODE_POS 48
#define HWA_FC_FPLOGIN_PKT_NODE_NAME_POS 56

#define HWA_FC_LOGIN_PKT_DEV_NAME_POS (0x2c + 4 + 1)
#define HWA_FC_LOGIN_DEV_NAME_LEN_POS (0x2c + 4)

#define HWA_FC_LOGIN_RSNID_PORT_ID_POS (0x29 + 4)
#define HWA_FC_LOGIN_RSPC_REDID_POS		(0x28 + 1)

#define FS_ACC  0x8002
#define FS_RJT  0x8001

#define DIRECTORY_DID	0xfffffc
#define LOGIN_LOGIN_DID	0xfffffe
#define FABRIC_CTL_DID	0xfffffd

#define LOGIN_TIMED_OUT  5

extern THwaFcDevList nHwaFcDev[HWA_FC_MAX_DEV_CNT];

static char nCardWwn[8] = {'h','w','a','c','r','e','a','8'};

#define FILL_DIR_DID(x) do{x[0]=0xff;x[1]=0xff;x[2]=0xfc;}while(0)
#define FILL_LOGIN_DID(x) do{x[0]=0xff;x[1]=0xff;x[2]=0xfe;}while(0)
#define FILL_FABRIC_CTL_DID(x) do{x[0]=0xff;x[1]=0xff;x[2]=0xfd;}while(0)

#define FILL_OTHER_DID(x,y)do{x[0]= ((y) >> 16) & 0xff;x[1]= ((y) >> 8) & 0xff;x[2]= ((y)) & 0xff;}while(0)

static int HwaFcLoginUpdateOxID(unsigned char *pPkt)
{
	static unsigned char nOxId[2] = {0x80,0x00};
	static unsigned char nSeqId = 0;
	memcpy(pPkt + HWA_FC_OXID_POS,nOxId,2);
	nOxId[1] +=2;
	if(nOxId == 0)
	{
		nOxId[0] += 1;
	}
	pPkt[0x10] = nSeqId++;
	return HWA_FC_OP_OK;
}


static int HwaFcLoginUpdateSeqID(unsigned char *pPkt)
{
#define HWA_FC_LOGIN_SEQID_POS 0x10
	static unsigned char nSeqId = 0;
	pPkt[HWA_FC_LOGIN_SEQID_POS] = nSeqId++;
	return HWA_FC_OP_OK;
}
static int HwaFcLoginRplSetNodeName(unsigned char *pPkt,unsigned char *pNode,THwaFcPortSel ePort)
{
#define HWA_FC_RPL_PKT_NODE_NAME_POS 0x138
	char nNodeTmp[8] = {0};
	memcpy(nNodeTmp,pNode,8);
	nNodeTmp[0] += 0x10;
	if(ePort == ePortB)
	{
		nNodeTmp[7] += 0x10;
	}
	memcpy(pPkt + HWA_FC_RPL_PKT_NODE_NAME_POS,nNodeTmp,8);
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetDid(unsigned char *pPkt,THwaFcDeviceID nDid)
{
	memcpy(pPkt + HWA_FC_DID_POS, nDid.nId,sizeof(THwaFcDeviceID));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginGetDid(unsigned char *pPkt,THwaFcDeviceID *pDid)
{
	memcpy(pDid->nId,pPkt + HWA_FC_DID_POS,sizeof(THwaFcDeviceID));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetSid(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	memcpy(pPkt + HWA_FC_SID_POS, nSid.nId,sizeof(THwaFcDeviceID));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetPortId(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	memcpy(pPkt + HWA_FC_LOGIN_RSNID_PORT_ID_POS,&nSid,sizeof(nSid));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetReDidForRspc(unsigned char *pPkt,THwaFcDeviceID nId)
{
	memcpy(pPkt + HWA_FC_LOGIN_RSPC_REDID_POS,&nId,sizeof(nId));
	return HWA_FC_OP_OK;
}
static int HwaFcLoginGetSid(unsigned char *pPkt,THwaFcDeviceID *pSid)
{
	memcpy(pSid->nId,pPkt + HWA_FC_SID_POS,sizeof(THwaFcDeviceID));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetOxId(unsigned char *pPkt,THwaFcOxID nId)
{
	memcpy(pPkt + HWA_FC_OXID_POS, nId.nId,sizeof(THwaFcOxID));
	return HWA_FC_OP_OK;
}

static int HwaFcLoginGetOxId(unsigned char *pPkt,THwaFcOxID *pId)
{
	memcpy( pId->nId,pPkt + HWA_FC_OXID_POS,sizeof(THwaFcOxID));
	return HWA_FC_OP_OK;
}

static int HwaFcFPLoginPktSetNode(unsigned char *pPkt,unsigned char *pNode,THwaFcPortSel ePort)
{
	char nNodeTmp[8] = {0};
	memcpy(nNodeTmp,pNode,8);
	
	if(ePort == ePortB)
	{
		nNodeTmp[7] += 0x1;
	}
	memcpy(pPkt + HWA_FC_FPLOGIN_PKT_NODE_NAME_POS,nNodeTmp,8);
	nNodeTmp[0] = nNodeTmp[0] + 0x10;
	memcpy(pPkt + HWA_FC_FPLOGIN_PKT_NODE_POS,nNodeTmp,8);
	return HWA_FC_OP_OK;
}

static int HwaFcLoginSetDeviceName(unsigned char *pPkt,char *pDevName,int nNameLen,THwaFcPortSel ePort)
{
	unsigned int i = 0;
#ifndef FOR_BRCD
	SPRINTF((char *)(pPkt + HWA_FC_LOGIN_PKT_DEV_NAME_POS),100,"%s-%s",pDevName,((ePort == ePortA) ? "PORT-A" : "PORT-B"));
	nNameLen = strlen(pDevName) + 1 + 6;
	
	*(pPkt + HWA_FC_LOGIN_DEV_NAME_LEN_POS) = (unsigned char)(nNameLen);
	
	for(i = HWA_FC_LOGIN_PKT_DEV_NAME_POS + nNameLen;  i < sizeof(nHwaFcRspnIdPkt) - 4 - HWA_FC_LOGIN_PKT_DEV_NAME_POS - nNameLen; i++)
	{
		*(pPkt + i) = 0;
	}
#else
	if(ePort == ePortB)
	{
		*(pPkt + HWA_FC_LOGIN_PKT_DEV_NAME_POS) += 1;
	}
#endif
	return HWA_FC_OP_OK;
}

static int HwaFcLoginCheckIsLsAcc(unsigned char *pPkt,THwaFcDeviceID nDid,THwaFcDeviceID nSid,THwaFcOxID nOxId)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nReSrcID ={0};
	THwaFcDeviceID nReDstID = {0};
	THwaFcOxID	   nReOxID = {0};

	HwaFcLoginGetSid(pPkt,&nReSrcID);
	HwaFcLoginGetDid(pPkt,&nReDstID);
	HwaFcLoginGetOxId(pPkt,&nReOxID);

	if(((nReSrcID.nId[0] == nDid.nId[0]) &&(nReSrcID.nId[1] == nDid.nId[1])&&(nReSrcID.nId[2] == nDid.nId[2])) &&
		((nReDstID.nId[0] == nSid.nId[0]) &&(nReDstID.nId[1] == nSid.nId[1])&&(nReDstID.nId[2] == nSid.nId[2])) &&
		((nReOxID.nId[0] == nOxId.nId[0]) &&(nReOxID.nId[1] == nOxId.nId[1])))
	{
		if(((pPkt[nCmdPos] & 0xff) == LS_ACC) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return LS_ACC;
		}else if(((pPkt[nCmdPos]  & 0xff)== LS_RJT) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return LS_RJT;
		}
	}
	return 0;
}


static int HwaFcLoginCheckIsLsAccForFLogin(unsigned char *pPkt,THwaFcDeviceID nDid,THwaFcDeviceID *pSid,THwaFcOxID nOxId)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nReSrcID ={0};
	THwaFcDeviceID nReDstID = {0};
	THwaFcOxID	   nReOxID = {0};

	HwaFcLoginGetSid(pPkt,&nReSrcID);
	HwaFcLoginGetDid(pPkt,&nReDstID);
	HwaFcLoginGetOxId(pPkt,&nReOxID);

	if(((nReSrcID.nId[0] == nDid.nId[0]) &&(nReSrcID.nId[1] == nDid.nId[1])&&(nReSrcID.nId[2] == nDid.nId[2])) &&
		((nReOxID.nId[0] == nOxId.nId[0]) &&(nReOxID.nId[1] == nOxId.nId[1])))
	{
		if(((pPkt[nCmdPos] & 0xff) == LS_ACC) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			HwaFcLoginGetDid(pPkt,pSid);
			return LS_ACC;
		}else if(((pPkt[nCmdPos]  & 0xff)== LS_RJT) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return LS_RJT;
		}
	}
	return 0;
}

static int HwaFcLoginCheckIsPLogin(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nDid = {0};
	HwaFcLoginGetDid(pPkt,&nDid);
	if((nDid.nId[0] == nSid.nId[0]) &&(nDid.nId[1] == nSid.nId[1])&&(nDid.nId[2] == nSid.nId[2]))
	{
		if(((pPkt[nCmdPos] & 0xff) == PLOGI) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return PLOGI;
		}
	}
	return 0;
}

static int HwaFcLoginCheckIsPrli(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nDid = {0};
	HwaFcLoginGetDid(pPkt,&nDid);
	if((nDid.nId[0] == nSid.nId[0]) &&(nDid.nId[1] == nSid.nId[1])&&(nDid.nId[2] == nSid.nId[2]))
	{
		if(((pPkt[nCmdPos] & 0xff) == PRLI) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return PRLI;
		}
	}
	return 0;
}

static int HwaFcLoginCheckIsLogo(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nDid = {0};
	HwaFcLoginGetDid(pPkt,&nDid);
	if((nDid.nId[0] == nSid.nId[0]) &&(nDid.nId[1] == nSid.nId[1])&&(nDid.nId[2] == nSid.nId[2]))
	{
		if(((pPkt[nCmdPos] & 0xff) == LOGO) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REP)))
		{
			return LOGO;
		}
	}
	return 0;
}

static int  HwaFcLoginCheckIsRpsc(unsigned char *pPkt,THwaFcDeviceID nSid)
{
	int nCmdPos = sizeof(THwaFcSof) + sizeof(THwaFcAeHeader);
	int nExtenPos = sizeof(THwaFcSof);
	THwaFcDeviceID nDid = {0};
	HwaFcLoginGetDid(pPkt,&nDid);
	if((nDid.nId[0] == nSid.nId[0]) &&(nDid.nId[1] == nSid.nId[1])&&(nDid.nId[2] == nSid.nId[2]))
	{
		if(((pPkt[nCmdPos] & 0xff) == RPSC) && (pPkt[nExtenPos] = ((R_CTL_ROUTER_EXTEN << R_CTL_ROUTER_BIT) | R_CTL_INFO_REQ)))
		{
			return RPSC;
		}
	}
	return 0;

}

static int HwaFcLoginCheckIsFsAcc(unsigned char *pPkt,THwaFcDeviceID nDid,THwaFcDeviceID nSid,THwaFcOxID nOxId)
{
	int nExtendPos = sizeof(THwaFcSof);
	int ResPos = 0x24;
	THwaFcDeviceID nReSrcID ={0};
	THwaFcDeviceID nReDstID = {0};
	THwaFcOxID	   nReOxID = {0};
	HwaFcLoginGetSid(pPkt,&nReSrcID);
	HwaFcLoginGetDid(pPkt,&nReDstID);
	HwaFcLoginGetOxId(pPkt,&nReOxID);

	if(((nReSrcID.nId[0] == nDid.nId[0]) &&(nReSrcID.nId[1] == nDid.nId[1])&&(nReSrcID.nId[2] == nDid.nId[2])) &&
		((nReDstID.nId[0] == nSid.nId[0]) &&(nReDstID.nId[1] == nSid.nId[1])&&(nReDstID.nId[2] == nSid.nId[2])) &&
		((nReOxID.nId[0] == nOxId.nId[0]) &&(nReOxID.nId[1] == nOxId.nId[1])))
	{	
		if(((pPkt[nExtendPos]  & 0xff) == 0x3) && ((pPkt[ResPos]  & 0xff) == 0x80) && ((pPkt[ResPos + 1]) == 0x02))
		{
			return FS_ACC;
		}else 	if(((pPkt[nExtendPos]  & 0xff) == 0x3) && ((pPkt[ResPos]  & 0xff) == 0x80) && ((pPkt[ResPos + 1]) == 0x01))
		{
			return FS_RJT;
		}
	}
	return 0;
}


static int HwaFcLoginResGidAccPkt(unsigned char *pPkt,THwaFcDeviceID *pList, int *nDevCnt)
{
	int nDevListPos = 0x28+4;
	int i = 0;
	while(1){
		memcpy(&pList[i],pPkt + nDevListPos + 1,3);
		i++;
		if((pPkt[nDevListPos] & 0x80) != 0)
		{
			break;
		}
		nDevListPos += 4;
	}
	*nDevCnt = i;
	return HWA_FC_OP_OK;
}

static int HwaFcRhbaUpdateInfo(unsigned char*pPkt,unsigned char *pNode,THwaFcPortSel ePort)
{
#define RHBA_HBA_ID_POS		0x28 + 4
#define RHBA_PORT_NAME_POS 0x38
#define RHBA_NONE_NAME_POS 0x48
#define RHBA_MANUFAC_NAME_POS 0x54

	char nNodeTmp[8] = {0};
	memcpy(nNodeTmp,pNode,8);

	char *pManu="HwaCreate";
	if(ePort == ePortB)
	{
		nNodeTmp[7] += 0x1;
	}
	memcpy(pPkt + RHBA_NONE_NAME_POS,nNodeTmp,8);
	
	nNodeTmp[0] = nNodeTmp[0] + 0x10;
	memcpy(pPkt + RHBA_HBA_ID_POS,nNodeTmp,8);
	memcpy(pPkt + RHBA_PORT_NAME_POS,nNodeTmp,8);

	
#ifndef FOR_BRCD
	memcpy(pPkt + RHBA_MANUFAC_NAME_POS,pManu,strlen(pManu) + 1);
#endif
	return HWA_FC_OP_OK;
}

static int HwaFcRpaUpdateInfo(unsigned char *pPkt,unsigned char *pNode,THwaFcPortSel ePort)
{
#define RPA_PORT_NAME_POS (0x28 + 4)
	char nNodeTmp[8] = {0};
	memcpy(nNodeTmp,pNode,8);
	nNodeTmp[0] += 0x10;
	if(ePort == ePortB)
	{
		nNodeTmp[7] += 0x1;
	}
	memcpy(pPkt + RPA_PORT_NAME_POS,nNodeTmp,8);
	return HWA_FC_OP_OK;

}
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcSendFloginPacket(int nCardNum,THwaFcDeviceID *pSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcFloginPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,*pSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcFPLoginPktSetNode(pTmp,pNode,ePort);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcFloginPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,3) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAccForFLogin(nRecvBuf,nDid,pSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == LS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendPloginPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcPloginPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcFPLoginPktSetNode(pTmp,pNode,ePort);
	HwaFcLoginUpdateOxID(pTmp);	
	HwaFcLoginUpdateSeqID(pTmp);

	HwaFcLoginGetOxId(pTmp,&nOxId);
	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPloginPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAcc(nRecvBuf,nDid,nSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == LS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendScrPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcSCRPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcSCRPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAcc(nRecvBuf,nDid,nSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == LS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendPrliPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};

	unsigned char *pTmp = nHwaFcPrliPktT;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPrliPktT),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAcc(nRecvBuf,nDid,nSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == LS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRspcPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nReDid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcRpscPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetReDidForRspc(pTmp,nReDid);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcRpscPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAcc(nRecvBuf,nDid,nSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == LS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRsnIDPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,char *pDevName,int nNameLen,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};

	unsigned char *pTmp = nHwaFcRspnIdPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetPortId(pTmp,nSid);
	HwaFcLoginSetDeviceName(pTmp,pDevName,nNameLen,ePort);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcRspnIdPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRcsIdPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};

	unsigned char *pTmp = nHwaFcRcsIdPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetPortId(pTmp,nSid);
	HwaFcLoginUpdateSeqID(pTmp);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcRcsIdPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRftIdPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcRftIdPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetPortId(pTmp,nSid);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcRftIdPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRffIdPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};

	unsigned char *pTmp = nHwaFcRffIdPkt;
	int ret = 0;
	THwaFcOxID nOxId = {0};
	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetPortId(pTmp,nSid);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcRffIdPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendGidFtPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcPortSel ePort,THwaFcDeviceID *pList,int *nCnt)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcGidFcPkt;
	int ret = 0;

	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcGidFcPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			HwaFcLoginResGidAccPkt(nRecvBuf,pList,nCnt);
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcLoginSendPloginToManager(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcLoginPloginToManagerPkt;
	int ret = 0;

	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcFPLoginPktSetNode(pTmp,pNode,ePort);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLoginPloginToManagerPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait LS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsLsAcc(nRecvBuf,nDid,nSid,nOxId)) == LS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a LS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRhbaPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcLoginRhbaPkt;
	int ret = 0;

	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);

	HwaFcRhbaUpdateInfo(pTmp,pNode,ePort);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLoginRhbaPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRpaPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcLoginRpaPkt;
	int ret = 0;

	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcRpaUpdateInfo(pTmp,pNode,ePort);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLoginRpaPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcSendRplPacket(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcRecvInfo nIf;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	unsigned char *pTmp = nHwaFcLoginRplPkt;
	int ret = 0;

	THwaFcOxID nOxId = {0};

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginUpdateOxID(pTmp);
	HwaFcLoginRplSetNodeName(pTmp,pNode,ePort);
	HwaFcLoginGetOxId(pTmp,&nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;
	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLoginRplPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait FS_ACC timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort != ePort)
		{
			continue;
		}
		if((ret = HwaFcLoginCheckIsFsAcc(nRecvBuf,nDid,nSid,nOxId)) == FS_ACC)
		{
			break;
		}else if(ret == FS_RJT)
		{
			ErrLog("Get a FS_RJT packet.");
			return HWA_FC_ERR_RECV;
		}
		continue;
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
static int HwaFcLoginSendAccForPloginT(int nCardNum,THwaFcDeviceID nSrcId,THwaFcDeviceID nDid,THwaFcOxID nOxId,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcPLoginAccPktT;

	HwaFcLoginSetSid(pTmp,nSrcId);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcFPLoginPktSetNode(pTmp,pNode,ePort);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPLoginAccPktT),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginSendAccForPlogin(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,unsigned char *pNode,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcPloginAccPkt;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcFPLoginPktSetNode(pTmp,pNode,ePort);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPloginAccPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginSendAccForPrli(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcPrliAccPkt;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPrliAccPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginSendRjtForPrli(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcPrliRjtPkt;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPrliRjtPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginSendAccForPrliT(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcPrliAccPktT;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcPrliAccPktT),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginSendAccForLogo(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcLogoAccPkt;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLogoAccPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
	}
	return HWA_FC_OP_OK;
}

static int  HwaFcLoginSendAccForRpsc(int nCardNum,THwaFcDeviceID nSid,THwaFcDeviceID nDid,THwaFcOxID nOxId,THwaFcPortSel ePort)
{
	THwaSendCfg nCfg;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char *pTmp = nHwaFcLogoAccPkt;

	HwaFcLoginSetSid(pTmp,nSid);
	HwaFcLoginSetDid(pTmp,nDid);
	HwaFcLoginSetOxId(pTmp,nOxId);
	HwaFcLoginUpdateSeqID(pTmp);

	nCfg.nChSel = ePort;
	nCfg.nIsAsm = NOT_ASM;
	nCfg.nPktPri = 0;

	if(HwaFcSendData(nCardNum,pDevCtx->nSpeSendPort,(char *)pTmp,sizeof(nHwaFcLogoAccPkt),nCfg) <= 0)
	{
		ErrLog("Send　data failed.");
		return HWA_FC_ERR_SEND;
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
static int HwaFcLoginWaitPlogin(int nCardNum,THwaFcPortSel ePort)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	THwaFcDeviceID nDstID;
	THwaFcOxID nOxId;
	THwaFcRecvInfo nIf;
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeSendPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait P_LOGIN　 packet timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort == ePort)
		{
			if(HwaFcLoginCheckIsPLogin(nRecvBuf,pDevCtx->nSrcId[ePort - 1]) != PLOGI)
			{
				continue;
			}
			HwaFcLoginGetSid(nRecvBuf,&nDstID);
			HwaFcLoginGetOxId(nRecvBuf,&nOxId);
			if((nDstID.nId[0] == 0xff) && (nDstID.nId[1] == 0xfc) && (nDstID.nId[2] == 0x01)) 
			{
				HwaFcLoginSendAccForPlogin(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,nOxId,pDevCtx->nNodeName,ePort);
			}else{
				continue;
			}
			break;
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
static int HwaFcLoginWaitPrli(int nCardNum,THwaFcPortSel ePort)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	THwaFcDeviceID nDstID;
	THwaFcOxID nOxId;
	THwaFcRecvInfo nIf;
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeSendPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait PRLI　 packet timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort == ePort)
		{
			if(HwaFcLoginCheckIsPrli(nRecvBuf,pDevCtx->nSrcId[ePort - 1]) != PRLI)
			{
				continue;
			}
			HwaFcLoginGetSid(nRecvBuf,&nDstID);
			HwaFcLoginGetOxId(nRecvBuf,&nOxId);
			if((nDstID.nId[0] == 0xff) && (nDstID.nId[1] == 0xfc) && (nDstID.nId[2] == 0x01)) 
			{
#if 1	
				HwaFcLoginSendAccForPrli(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,nOxId,ePort);
#else
				HwaFcLoginSendRjtForPrli(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,nOxId,ePort);
#endif
			}else{
				continue;
			}
			break;
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
static int HwaFcLoginWaitLogo(int nCardNum,THwaFcPortSel ePort)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	THwaFcDeviceID nDstID;
	THwaFcOxID nOxId;
	THwaFcRecvInfo nIf;
	while(1)
	{
		if(HwaFcRecvData(nCardNum,pDevCtx->nSpeSendPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT) <= 0)
		{
			ErrLog("Wait PRLI　 packet timed out.");
			return HWA_FC_ERR_RECV;
		}
		if(nIf.ePort == ePort)
		{
			if(HwaFcLoginCheckIsLogo(nRecvBuf,pDevCtx->nSrcId[ePort - 1]) != LOGO)
			{
				continue;
			}
			HwaFcLoginGetSid(nRecvBuf,&nDstID);
			HwaFcLoginGetOxId(nRecvBuf,&nOxId);
			if((nDstID.nId[0] == 0xff) && (nDstID.nId[1] == 0xfc) && (nDstID.nId[2] == 0x01)) 
			{
				HwaFcLoginSendAccForLogo(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,nOxId,ePort);
			}else{
				continue;
			}
			break;
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
#ifdef WINDOWS_OPS
DWORD WINAPI HwaFcLoginListenProgress(LPVOID pPara)
#elif defined VXWORKS_OPS
int HwaFcLoginListenProgress(int pPara)
#else
#error "unknown operation system"
#endif
{
	THwaFcRecvInfo nIf;
	unsigned char nRecvBuf[HWA_FC_NASM_MAX_PAYLOAD] = {0};
	THwaFcDeviceID nDstId;
	THwaFcOxID nOXId;
	int nCardNum = (int)pPara;
	int nRet = 0;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];

	while(pDevCtx->nExitFlag == 0)
	{
		if((nRet = HwaFcRecvData(nCardNum,pDevCtx->nSpeRecvPort,(char *)nRecvBuf,&nIf,HWA_FC_NASM_MAX_PAYLOAD,LOGIN_TIMED_OUT)) < 0)
		{
			ErrLog("Login listen thread receive data error.");
		}else if(nRet > 0)
		{
			HwaFcLoginGetSid(nRecvBuf,&nDstId);
			HwaFcLoginGetOxId(nRecvBuf,&nOXId);
/*check is an PLOGIN pkt*/
			if((nRet = HwaFcLoginCheckIsPLogin(nRecvBuf,pDevCtx->nSrcId[nIf.ePort - 1])) == PLOGI)
			{
				HwaFcLoginSendAccForPloginT(nCardNum,pDevCtx->nSrcId[nIf.ePort - 1],nDstId,nOXId,pDevCtx->nNodeName,nIf.ePort);
			}else if((nRet = HwaFcLoginCheckIsPrli(nRecvBuf,pDevCtx->nSrcId[nIf.ePort - 1])) == PRLI)
			{
				HwaFcLoginSendAccForPrliT(nCardNum,pDevCtx->nSrcId[nIf.ePort - 1],nDstId,nOXId,nIf.ePort);
			}else if((nRet = HwaFcLoginCheckIsLogo(nRecvBuf,pDevCtx->nSrcId[nIf.ePort - 1])) == LOGO)
			{
				HwaFcLoginSendAccForLogo(nCardNum,pDevCtx->nSrcId[nIf.ePort - 1],nDstId,nOXId,nIf.ePort);
			}else if((nRet = HwaFcLoginCheckIsRpsc(nRecvBuf,pDevCtx->nSrcId[nIf.ePort - 1])) == RPSC)
			{
				HwaFcLoginSendAccForRpsc(nCardNum,pDevCtx->nSrcId[nIf.ePort - 1],nDstId,nOXId,nIf.ePort);
			}
		}
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
int HwaFcLoginGetDevList(int nCardNum,THwaFcDeviceID *pList,int *nCnt,THwaFcPortSel ePort)
{
	THwaFcDeviceID nDstId;
	FILL_DIR_DID(nDstId.nId);
	if(HwaFcSendGidFtPacket(nCardNum,nHwaFcDev[nCardNum].nSrcId[ePort - 1],nDstId,ePort,pList,nCnt) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_PARA;
	}
	return HWA_FC_OP_OK;
}
int HwaFcLoginProNPortLogin(int nCardNum,THwaFcDeviceID nDstId,THwaFcPortSel ePort)
{
/*send a PLOGIN PKT*/
	if(HwaFcSendPloginPacket(nCardNum,nHwaFcDev[nCardNum].nSrcId[ePort - 1],nDstId,nHwaFcDev[nCardNum].nNodeName,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*send a PRLI PKT*/
	if(HwaFcSendPrliPacket(nCardNum,nHwaFcDev[nCardNum].nSrcId[ePort - 1],nDstId,ePort) != HWA_FC_OP_OK)
	{
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
int HwaFcDriverLogin(int nCardNum,THwaFcPortSel ePort)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	THwaFcDeviceID nDevList[8] = {0};
	int nDevCnt = 0;
	int i = 0;
	int nId = 0;
	THwaFcDeviceID nDstID;
	
	if((pDevCtx->nLoginThreadId != HWA_FC_INVALIED_HANDLE) && (pDevCtx->nLoginThreadId != NULL))
	{
#ifdef WINDOWS_OPS
		TerminateThread(pDevCtx->nLoginThreadId,0);
		pDevCtx->nLoginThreadId = HWA_FC_INVALIED_HANDLE;
#endif
	}


/*send a FLOGIN to SW*/
	FILL_LOGIN_DID(nDstID.nId);
	if(HwaFcSendFloginPacket(nCardNum,&(pDevCtx->nSrcId[ePort - 1]),nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*send a PLOGIN to Directory Server*/
	FILL_DIR_DID(nDstID.nId);
	if(HwaFcSendPloginPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*send a State Change Registration pkt to fc control*/
//  	FILL_FABRIC_CTL_DID(nDstID.nId);
//  	if(HwaFcSendScrPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,ePort) != HWA_FC_OP_OK)
//  	{
//  		return HWA_FC_ERR_LOGIN;
//  	}
/*send a Register Symbollic Port Name pkt to directory*/
	FILL_DIR_DID(nDstID.nId);
	if(HwaFcSendRsnIDPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nDevName,strlen(pDevCtx->nDevName),ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*send a Register class server Packet to directory*/
 	if(HwaFcSendRcsIdPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,ePort) != HWA_FC_OP_OK)
 	{
 		return HWA_FC_ERR_LOGIN;
 	}
/*send a Register FC-4 Types pkt to directory*/
	FILL_DIR_DID(nDstID.nId);
	if(HwaFcSendRftIdPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*send a Register FC-4 Features - Port ID pkt to directory*/
	FILL_DIR_DID(nDstID.nId);
	if(HwaFcSendRffIdPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}

	if(HwaFcLoginGetDevList(nCardNum,nDevList,&nDevCnt,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}

// /*send a plogin packet to manager*/
 	FILL_OTHER_DID(nDstID.nId,0xfffffa);
 	if(HwaFcLoginSendPloginToManager(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
 	{
 		return HWA_FC_ERR_LOGIN;
 	}
/*send a register platform packet to manager*/

//  	if(ePort != ePortA)
//  	{
	/*send a register HBA pakcet to manager*/
 		FILL_OTHER_DID(nDstID.nId,0xfffffa);
 		if(HwaFcSendRhbaPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
 		{
 			return HWA_FC_ERR_LOGIN;
 		}
 	/*send a resiter port attr packet to manager*/
 		FILL_OTHER_DID(nDstID.nId,0xfffffa);
 		if(HwaFcSendRpaPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
 		{
 			return HWA_FC_ERR_LOGIN;
 		}
//	}
/*wait for a PLogin from FFFC01*/
	if(HwaFcLoginWaitPlogin(nCardNum,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*wait for a PRLI from FFFC01*/
	if(HwaFcLoginWaitPrli(nCardNum,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_LOGIN;
	}
/*wait for a LOGO from FFFC01*/
 	if(HwaFcLoginWaitLogo(nCardNum,ePort) != HWA_FC_OP_OK)
 	{
 		return HWA_FC_ERR_LOGIN;
 	}
//  	/*send a register platform packet to manager*/
//  	if(ePort == ePortA)
//  	{
//  		if(HwaFcSendRplPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDstID,pDevCtx->nNodeName,ePort) != HWA_FC_OP_OK)
//  		{
//  			return HWA_FC_ERR_LOGIN;
//  		}
//  	}

	/*start LOGIN service progress*/
#ifdef VXWORKS_OPS
	int nThreadId = 0;
	if((nThreadId = taskNameToId("HwaFcRecvTask")) != 0)
	{
		taskDelete(nThreadId);
	}
#endif
	
	
	if(pDevCtx->nLoginThreadId == HWA_FC_INVALIED_HANDLE || pDevCtx->nLoginThreadId==0)
	{
#ifdef VXWORKS_OPS
		START_THREAD(&(pDevCtx->nLoginThreadId),nCardNum);
		//START_THREAD(&(pDevCtx->nRecvThreadId),nCardNum);
#else
		START_THREAD(&nId,(pDevCtx->nLoginThreadId),nCardNum,HwaFcLoginListenProgress);
#endif
		
		if(pDevCtx->nLoginThreadId == HWA_FC_INVALIED_HANDLE)
		{
			ErrLog("%s","Start login server thread failed.");
			return HWA_FC_ERR_LOGIN;
		}
	}

	/*get all port list*/
	if(HwaFcLoginGetDevList(nCardNum,nDevList,&nDevCnt,ePort) != HWA_FC_OP_OK)
	{
		return HWA_FC_OP_OK;
	}
	if(nDevCnt == 1)
	{
		return HWA_FC_OP_OK;
	}
	for(i = 0; i < nDevCnt; i++)
	{
		if((nDevList[i].nId[0] == pDevCtx->nSrcId[ePort - 1].nId[0]) && (nDevList[i].nId[1] == pDevCtx->nSrcId[ePort - 1].nId[1]) && (nDevList[i].nId[2] == pDevCtx->nSrcId[ePort - 1].nId[2]))
		{
			continue;
		}else{
	/*send a report port speed cap pakcet*/
			FILL_OTHER_DID(nDstID.nId,0xfffc01);
			HwaFcSendRspcPacket(nCardNum,pDevCtx->nSrcId[ePort - 1],nDevList[i],nDstID,ePort);

			HwaFcLoginProNPortLogin(nCardNum,nDevList[i],ePort);
		}
	}
	return HWA_FC_OP_OK;
}	

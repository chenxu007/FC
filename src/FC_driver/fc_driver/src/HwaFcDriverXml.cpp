/*include */
#include "stdafx.h"

#include "HwaFcStruct.h"
#include "HwaFcDriver.h"
#include "HwaFcDriverLib.h"
#include "HwaFcReg.h"
#include "Markup.h"


#ifdef WINDOWS_OPS
using namespace std;
typedef string THwaFcXmlStr; 
#define XML_ATOI(x) atoi(x.c_str())
#define  XML_STR_FREE(x)
#define XML_GET_PROP(x) nXml.GetAttrib((x))
#elif defined VXWORKS_OPS
#include "libxml/xmlmemory.h"
#include "libxml/parser.h"
	typedef xmlChar* THwaFcXmlStr;
	#define XML_ATOI(x) atoi((const char *)(x))
#define XML_STR_FREE(x) do{if((x)){free((x)); (x)=NULL;}}while(0)
#define XML_GET_PROP(x) xmlGetProp()
#else
#error "unknown operation system."
#endif


#define XML_INTO_ELEM nXml.IntoElem()
#define XML_OUTOF_ELEM nXml.OutOfElem()

extern THwaFcDevList nHwaFcDev[HWA_FC_MAX_DEV_CNT];
extern char sHwaFcLastErr[256];
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverCheckCfg(THwaFcDevList *pDevCtx)
{
	return HWA_FC_OP_OK;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverCheckPort(THwaFcDevList *pDev,THwaFcPort *pPort,THwaFcPortDir eDir)
{
	int i = 0;
	switch(eDir)
	{
	case eDirRecv:
		for(i = 0; i < HWA_FC_MAX_PORT_CNT; i++)
		{
			if(pDev->nRecvPort[i].nPortStat)
			{
				if(pPort->nPortId == pDev->nRecvPort[i].nPortId)
				{
					ErrLog("Receive port id %d duplicated.",pPort->nPortId);
					return HWA_FC_ERR_XML;
				}
				if((pPort->nSid == pDev->nRecvPort[i].nSid) &&
					(pPort->nDid == pDev->nRecvPort[i].nDid) &&
					(pPort->nOxId == pDev->nRecvPort[i].nOxId) &&
					(pPort->nMsgId == pDev->nRecvPort[i].nMsgId) &&
					(pPort->nSeqId == pDev->nRecvPort[i].nSeqId))
				{
					ErrLog("Receive config duplicated.");
					return HWA_FC_ERR_XML;
				}
			}
		}
		break;
	case eDirSend:
		for(i = 0; i < HWA_FC_MAX_PORT_CNT; i++)
		{
			if(pDev->nSendPort[i].nPortStat)
			{
				if(pPort->nPortId == pDev->nSendPort[i].nPortId)
				{
					ErrLog("Send port id %d duplicated.",pPort->nPortId);
					return HWA_FC_ERR_XML;
				}
			}
		}
		break;
	default:
		break;
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
static void HwaFcDriverUsePort(THwaFcDevList *pDev,THwaFcPort *pPort,THwaFcPortDir eDir)
{
	int nPortIndex = pPort->nPortIndex;
	switch(eDir)
	{
		case eDirRecv:
				pDev->nRecvPortMask[nPortIndex / 32] |= (0x1 << (nPortIndex % 32));
			break;
		case eDirSend:
				pDev->nSendPortMask[nPortIndex / 32] |= (0x1 << (nPortIndex % 32));
			break;
		default:
			break;
	}
	pPort->nPortStat = 1;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static THwaFcPort * HwaFcDriverFindPortByID(THwaFcDevList *pDev,unsigned int nPortId,THwaFcPortDir eDir)
{
	int i = 0;
	switch(eDir)
	{
	case eDirRecv:
		for(i = 0; i < HWA_FC_MAX_PORT_CNT; i++)
		{
			if(pDev->nRecvPort[i].nPortStat)
			{
				if(pDev->nRecvPort[i].nPortId == nPortId)
				{
					return &pDev->nRecvPort[i];
				}
			}
		}
		break;
	case eDirSend:
		for(i = 0; i < HWA_FC_MAX_PORT_CNT; i++)
		{
			if(pDev->nSendPort[i].nPortStat)
			{
				if(pDev->nSendPort[i].nPortId == nPortId)
				{
					return &pDev->nSendPort[i];
				}
			}
		}
		break;
	default:
		break;
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
static void HwaFcDriverUnusePort(THwaFcDevList *pDev,THwaFcPort *pPort,THwaFcPortDir eDir)
{
	int nPortIndex = pPort->nPortIndex;
	switch(eDir)
	{
		case eDirRecv:
				pDev->nRecvPortMask[nPortIndex / 32] &= ~((0x1 << (nPortIndex % 32)));
			break;
		case eDirSend:
				pDev->nSendPortMask[nPortIndex / 32] &= ~((0x1 << (nPortIndex % 32)));
			break;
		default:
			return;
			break;
	}
	pPort->nPortStat = 0;
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static THwaFcPort *HwaFcDriverFindUnusedPort(THwaFcDevList *pDev,THwaFcPortDir eDir)
{
	int i = 0;
	int j = 0;
	switch(eDir)
	{
		case eDirRecv:
			for(i = 0; i < HWA_FC_PORT_MASK_CNT; i++)
			{
				if(pDev->nRecvPortMask[i] != 0xffffffff)
				{
					for(j = 0 ; j < 32; j++)
					{
						if(((pDev->nRecvPortMask[i] >> j) & 0x1) == 0)
						{
							pDev->nRecvPort[i * 32 + j].nPortIndex = i * 32 + j;
							return &pDev->nRecvPort[i * 32 + j];
						}
					}
				}
			}
			break;
		case eDirSend:
			for(i = 0; i < HWA_FC_PORT_MASK_CNT; i++)
			{
				if(pDev->nSendPortMask[i] != 0xffffffff)
				{
					for(j = 0 ; j < 32; j++)
					{
						if(((pDev->nSendPortMask[i] >> j) & 0x1) == 0)
						{
							
							pDev->nSendPort[i * 32 + j].nPortIndex = i * 32 + j;
							return &pDev->nSendPort[i * 32 + j];
						}
					}
				}
			}
			break;
		default:
			break;
	}
	return NULL;
}

#ifdef SUPPORT_XML

#ifdef WINDOWS_OPS
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverPhreaseOnePort(THwaFcDevList *pDevCtx,CMarkup nXml)
{
	THwaFcXmlStr nStr = INVAILED_HWA_FC_XML_STR;
	THwaFcPort *pPort = NULL;
	THwaFcPortDir ePortDir = ePortDirNr;
	XML_INTO_ELEM;
/*[1]get port Dir*/

	nStr = nXml.GetAttrib(FC_PORT_DIR);
	if(nStr == PORT_DIR_SEND)
	{
		ePortDir = eDirSend;
	}else if(nStr == PORT_DIR_RECV)
	{
		ePortDir = eDirRecv;
	}else
	{
		ErrLog("Couldn't find port direction config.");
		goto ERR;
	}
/*[2]find an unused port*/
	if((pPort = HwaFcDriverFindUnusedPort(pDevCtx,ePortDir)) == NULL)
	{
		ErrLog("Too many %s port.\n",(ePortDir == eDirRecv) ? "receive" : "send");
		goto ERR;
	}
	pPort->ePortDir = ePortDir;
/*[3]get port id*/
	nStr = nXml.GetAttrib(FC_PORT_ID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port ID config.");
		goto ERR;
	}else{
		pPort->nPortId = XML_ATOI(nStr);
	}
/*[4]get port type*/
	nStr = nXml.GetAttrib(FC_PORT_TYPE);
	if(nStr == PORT_TYPE_NOR)
	{
		pPort->ePortTye = eTypeNormal;
	}
	else if(nStr == PORT_TYPE_BLOCK)
	{
		pPort->ePortTye = eTypeBlock;
	}else if(nStr == PORT_TYPE_SPE)
	{
		pPort->ePortTye = eTypeSpecial;
	}else
	{
		ErrLog("Couldn't find port type config.");
		goto ERR;
	}
/*[5]get port enable*/
	if(ePortDir == eDirRecv)
	{
		nStr = nXml.GetAttrib(FC_PORT_ENABLE);
		if(nStr == PORT_ENABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 1;
		}
		else if(nStr == PORT_DISABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 0;
		}else
		{/*default to enable*/
			pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 1;
		}
	}
/*[6]get port s id*/
	nStr = nXml.GetAttrib(FC_PORT_SID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port S_ID config.");
		goto ERR;
	}else{
		pPort->nSid = XML_ATOI(nStr);
	}

/*[7]get port d id*/
	nStr = nXml.GetAttrib(FC_PORT_DID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port D_ID config.");
		goto ERR;
	}else{
		pPort->nDid = XML_ATOI(nStr);
	}

/*[8]get port OX_ID*/
	nStr = nXml.GetAttrib(FC_PORT_OXID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port OX_ID config.");
		goto ERR;
	}else{
		pPort->nOxId = XML_ATOI(nStr);
	}

/*[9]get port Message ID*/
	nStr = nXml.GetAttrib(FC_PORT_MSG_ID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port message ID config.");
		goto ERR;
	}else{
		pPort->nMsgId = XML_ATOI(nStr);
	}

/*[10]get port Message ID*/
	nStr = nXml.GetAttrib(FC_PORT_SEQ_ID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port sequence ID config.");
		goto ERR;
	}else{
		pPort->nSeqId = XML_ATOI(nStr);
	}

/*[11]get port msg type*/
	nStr = nXml.GetAttrib(FC_PORT_MSG_TYPE);
	if(nStr == MSG_TYPE_ASM)
	{
		pPort->nType = MSG_ASM;
	}
	else if(nStr != INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Unknown message type %s\n.",nStr.c_str());
		goto ERR;
	}else
	{
		ErrLog("Couldn't find port message type config.");
		goto ERR;
	}
/*[12] Frag seq*/
	if(ePortDir == eDirRecv)
	{
		nStr = nXml.GetAttrib(FC_PORT_FRAG_SEQ);
		if(nStr == PORT_ENABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nFragSeq  = 1;
		}
		else if(nStr == PORT_DISABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nFragSeq  = 0;
		}else
		{
			ErrLog("Couldn't find packet recombination based sequence ID config.");
			goto ERR;
		}
/*[13] Frag offset*/
		nStr = nXml.GetAttrib(FC_PORT_FRAG_OFF);
		if(nStr == PORT_ENABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nFragOffset  = 1;
		}
		else if(nStr == PORT_DISABLE)
		{
			pPort->nPortCfg.nRecvPortCfg.nFragOffset  = 0;
		}else
		{
			ErrLog("Couldn't find packet recombination based offset config.");
			goto ERR;
		}
	}
/*[14] pri*/
	if(ePortDir == eDirSend)
	{
		nStr = nXml.GetAttrib(FC_PORT_PRIO);
		if(nStr == INVAILED_HWA_FC_XML_STR)
		{
			ErrLog("Couldn't find port message priority config.");
			goto ERR;
		}else{
			pPort->nPortCfg.nSendPortCfg.nPri = XML_ATOI(nStr);
		}
/*[15] Max Frame len*/
		nStr = nXml.GetAttrib(FC_PORT_MAX_FRM);
		if(nStr == INVAILED_HWA_FC_XML_STR)
		{
			ErrLog("Couldn't find port max frame len config.");
			goto ERR;
		}else{
			pPort->nPortCfg.nSendPortCfg.nMaxLen = XML_ATOI(nStr);
			if(pPort->nPortCfg.nSendPortCfg.nMaxLen > MAX_PAYLOAD_LEN)
			{
				pPort->nPortCfg.nSendPortCfg.nMaxLen = MAX_PAYLOAD_LEN;
			}
		}
	}
/*[16]get channel*/
	nStr = nXml.GetAttrib(FC_PORT_CHANNEL);
	if(nStr == CHANNEL_A)
	{
		pPort->nPortSel = ePortA;
	}else if(nStr == CHANNEL_B)
	{
		pPort->nPortSel = ePortB;
	}else if(nStr == CHANNEL_AB)
	{
		pPort->nPortSel = ePortAB;
	}else{
		ErrLog("Couldn't find channel select config.");
		goto ERR;
	}
	if(HwaFcDriverCheckPort(pDevCtx,pPort,ePortDir) != HWA_FC_OP_OK)
	{
		goto ERR;
	}
	HwaFcDriverUsePort(pDevCtx,pPort,ePortDir);
	XML_OUTOF_ELEM;
	return HWA_FC_OP_OK;
ERR:
	return HWA_FC_ERR_XML;
	
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverPhrasePortCfg(THwaFcDevList *pDevCtx,CMarkup nXml)
{
	string nStr = INVAILED_HWA_FC_XML_STR;
	while(nXml.FindChildElem(PORT_CFG))
	{
		XML_INTO_ELEM;
			while(nXml.FindChildElem(FC_PORT_HEADER))
			{
				if(HwaFcDriverPhreaseOnePort(pDevCtx,nXml) != HWA_FC_OP_OK)
				{
					return HWA_FC_ERR_XML;
				}
			}
		XML_OUTOF_ELEM;
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
static int HwaFcDriverPhraseCardCfg(THwaFcDevList *pDevCtx,CMarkup nXml)
{
	string nStr = INVAILED_HWA_FC_XML_STR;
	while(nXml.FindChildElem(HWA_FC_CARD_CFG))
	{
		XML_INTO_ELEM;
		/*get card work mode*/
		nStr = nXml.GetAttrib(HWA_CARD_WORK_MODE);
		if(nStr == CARD_MODE_NORMAL)
		{
			pDevCtx->eFcDevMode = eModeNormal;
		}else if(nStr == CARD_MODE_CAP)
		{
			pDevCtx->eFcDevMode = eModeCapture;
		}
		/*get card work speed*/
		nStr = nXml.GetAttrib(HWA_CARD_SPEED);
		if(nStr == CARD_SPEED_1G)
		{
			pDevCtx->eFcDevSpeed = eSpeed1G;
		}else if(nStr == CARD_SPEED_2G)
		{
			pDevCtx->eFcDevSpeed = eSpeed2G;
		}
		/*Get IRIG-B Mode*/
		nStr = nXml.GetAttrib(HWA_CARD_IRIG_MODE);
		if(nStr == CARD_MODE_MASTER)
		{
			pDevCtx->eFcIrigMode = eModeMaster;
		}else if(nStr == CARD_MODE_SLAVE)
		{
			pDevCtx->eFcIrigMode = eModeSlave;
		}

		XML_OUTOF_ELEM;
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
static void HwaFcDriverClearConfig(THwaFcDevList *pDevCtx)
{
	if(pDevCtx	!= NULL)
	{
		pDevCtx->eFcDevMode = eModeNormal;
		pDevCtx->eFcDevSpeed = eSpeed1G;
		pDevCtx->eFcIrigMode = eModeMaster;
		memset(pDevCtx->nRecvPort,0,sizeof(pDevCtx->nRecvPort));
		memset(pDevCtx->nSendPort,0,sizeof(pDevCtx->nSendPort));
		memset(pDevCtx->nRecvPortMask,0,sizeof(pDevCtx->nRecvPortMask));
		memset(pDevCtx->nSendPortMask,0,sizeof(pDevCtx->nSendPortMask));
	}
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverPhraseXml(int nCardNum,char *pFileName)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CMarkup nXml;
	string nStr = INVAILED_HWA_FC_XML_STR;
	if((pFileName == NULL) ||(pDevCtx == NULL))
	{
		ErrLog("No file name or device struct.");
		return HWA_FC_ERR_PARA;
	}

	if(nXml.Load(pFileName) == 0)
	{
		ErrLog("Get wrong config file.");
		return HWA_FC_ERR_PARA;
	}

	while(nXml.FindElem(HWA_FC_NIC))
	{
		nStr = nXml.GetAttrib(HWA_FC_CREAT_TIME);
		if(HwaFcDriverPhraseCardCfg(pDevCtx,nXml) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_XML;
		}
#if 0
		if(pDevCtx->eFcDevMode == eModeCapture)
		{
			return HWA_FC_OP_OK;
		}
#endif
		if(HwaFcDriverPhrasePortCfg(pDevCtx,nXml) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_XML;
		}
		
		if(HwaFcDriverCheckCfg(pDevCtx) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_XML;
		}

	}
	return HWA_FC_OP_OK;
}
#elif defined VXWORKS_OPS /*#ifdef WINDOWS_OPS*/
/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverPhreaseOnePort(THwaFcDevList *pDevCtx,xmlNodePtr pNodeCur)
{
	THwaFcXmlStr nStr = INVAILED_HWA_FC_XML_STR;
	THwaFcPort *pPort = NULL;
	THwaFcPortDir ePortDir = ePortDirNr;

	if(xmlStrcmp(pNodeCur->name,(const xmlChar *)FC_PORT_HEADER))
	{
		ErrLog("Get unknown port config header.");
		return HWA_FC_ERR_XML;
	}
	/*[1]get port Dir*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_DIR);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar *) PORT_DIR_SEND))
		{
			ePortDir = eDirSend;
		}else if(!xmlStrcmp(nStr,(const xmlChar *) PORT_DIR_RECV))
		{
			ePortDir = eDirRecv;
		}else{
			ErrLog("Get unknown port direction config.");
			goto ERR;
		}
		XML_STR_FREE(nStr);
	}
	else
	{
		ErrLog("Couldn't find port direction config.");
		goto ERR;
	}

	/*[2]find an unused port*/
	if((pPort = HwaFcDriverFindUnusedPort(pDevCtx,ePortDir)) == NULL)
	{
		ErrLog("Too many %s port.\n",(ePortDir == eDirRecv) ? "receive" : "send");
		goto ERR;
	}
	pPort->ePortDir = ePortDir;
	/*[3]get port id*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_ID);
	
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port ID config.");
		goto ERR;
	}else{
		pPort->nPortId = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}
	/*[4]get port type*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_TYPE);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar *) PORT_TYPE_NOR))
		{
			pPort->ePortTye = eTypeNormal;
		}
		else if(!xmlStrcmp(nStr,(const xmlChar *)PORT_TYPE_BLOCK))
		{
			pPort->ePortTye = eTypeBlock;
		}else{
			ErrLog("Getunknown port type config.");
			goto ERR;
		}
		XML_STR_FREE(nStr);
	}
	else
	{
		ErrLog("Couldn't find port type config.");
		goto ERR;
	}
	/*[5]get port enable*/
	if(ePortDir == eDirRecv)
	{
		nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_ENABLE);
		if(nStr != NULL)
		{
			if(!xmlStrcmp(nStr,(const xmlChar *)PORT_ENABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 1;
			}
			else if(!xmlStrcmp(nStr,(const xmlChar *)PORT_DISABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 0;
			}else{
				ErrLog("Get unknown port enable config.");
				goto ERR;
			}
			XML_STR_FREE(nStr);
		}else
		{/*default to enable*/
			pPort->nPortCfg.nRecvPortCfg.nRecvEnable  = 1;
		}
	}

	/*[6]get port s id*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_SID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port S_ID config.");
		goto ERR;
	}else{
		pPort->nSid = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}


	/*[7]get port d id*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_DID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port D_ID config.");
		goto ERR;
	}else{
		pPort->nDid = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}

	/*[8]get port OX_ID*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_OXID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port OX_ID config.");
		goto ERR;
	}else{
		pPort->nOxId = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}

	/*[9]get port Message ID*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_MSG_ID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port message ID config.");
		goto ERR;
	}else{
		pPort->nMsgId = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}

	/*[10]get port sequence ID*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_SEQ_ID);
	if(nStr == INVAILED_HWA_FC_XML_STR)
	{
		ErrLog("Couldn't find port sequence ID config.");
		goto ERR;
	}else{
		pPort->nSeqId = XML_ATOI(nStr);
		XML_STR_FREE(nStr);
	}

	/*[11]get port msg type*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_MSG_TYPE);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar *)MSG_TYPE_ASM))
		{
			pPort->nType = 1;
		}
		else if(nStr != INVAILED_HWA_FC_XML_STR)
		{
			ErrLog("Unknown message type %s\n.",nStr);
			goto ERR;
		}
		XML_STR_FREE(nStr);
	}else
	{
		ErrLog("Couldn't find port message type config.");
		goto ERR;
	}
	/*[12] Frag seq*/
	if(ePortDir == eDirRecv)
	{
		nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_FRAG_SEQ);
		if(nStr != NULL)
		{
			if(!xmlStrcmp(nStr,(const xmlChar *)PORT_ENABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nFragSeq  = 1;
			}
			else if(!xmlStrcmp(nStr,(const xmlChar *) PORT_DISABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nFragSeq  = 0;
			}else{
				ErrLog("Get unknown packet recombination based sequence ID config.");
				goto ERR;
			}
			XML_STR_FREE(nStr);
		}else
		{
			ErrLog("Couldn't find packet recombination based sequence ID config.");
			goto ERR;
		}
		/*[13] Frag offset*/
		nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_FRAG_OFF);
		if(nStr != NULL)
		{
			if(!xmlStrcmp(nStr,(const xmlChar *) PORT_ENABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nFragOffset  = 1;
			}
			else if(!xmlStrcmp(nStr,(const xmlChar *) PORT_DISABLE))
			{
				pPort->nPortCfg.nRecvPortCfg.nFragOffset  = 0;
			}else{
				ErrLog("Get unknown packet recombination based offset config.");
				goto ERR;
			}
			XML_STR_FREE(nStr);
		}
		else
		{
			ErrLog("Couldn't find packet recombination based offset config.");
			goto ERR;
		}
	}
	/*[14] pri*/
	if(ePortDir == eDirSend)
	{
		nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_PRIO);
		if(nStr == INVAILED_HWA_FC_XML_STR)
		{
			ErrLog("Couldn't find port message priority config.");
			goto ERR;
		}else{
			pPort->nPortCfg.nSendPortCfg.nPri = XML_ATOI(nStr);
			XML_STR_FREE(nStr);
		}
		/*[15] Max Frame len*/
		nStr = xmlGetProp(pNodeCur,(const xmlChar *) FC_PORT_MAX_FRM);
		if(nStr == INVAILED_HWA_FC_XML_STR)
		{
			ErrLog("Couldn't find port max frame len config.");
			goto ERR;
		}else{
			pPort->nPortCfg.nSendPortCfg.nMaxLen = XML_ATOI(nStr);
			XML_STR_FREE(nStr);
		}
	}
	/*[16]get channel*/
	nStr = xmlGetProp(pNodeCur,(const xmlChar *)FC_PORT_CHANNEL);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar *) CHANNEL_A))
		{
			pPort->nPortSel = ePortA;
		}else if(!xmlStrcmp(nStr,(const xmlChar *) CHANNEL_B))
		{
			pPort->nPortSel = ePortB;
		}else if(!xmlStrcmp(nStr, (const xmlChar *) CHANNEL_AB))
		{
			pPort->nPortSel = ePortAB;
		}else{
			ErrLog("Get unknown channel select config.");
			goto ERR;
		}
		XML_STR_FREE(nStr);
	}else{
		ErrLog("Couldn't find channel select config.");
		goto ERR;
	}
	if(HwaFcDriverCheckPort(pDevCtx,pPort,ePortDir) != HWA_FC_OP_OK)
	{
		goto ERR;
	}
	HwaFcDriverUsePort(pDevCtx,pPort,ePortDir);
	return HWA_FC_OP_OK;
ERR:
	XML_STR_FREE(nStr);
	return HWA_FC_ERR_XML;

}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
static int HwaFcDriverPhrasePortCfg(THwaFcDevList *pDevCtx,xmlNodePtr pNodeCur)
{
	pNodeCur = pNodeCur->xmlChildrenNode;	
	while(pNodeCur != NULL)
	{
		if(HwaFcDriverPhreaseOnePort(pDevCtx,pNodeCur) != HWA_FC_OP_OK)
		{
			return HWA_FC_ERR_XML;
		}
		pNodeCur = pNodeCur->next;
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
static int HwaFcDriverPhraseCardCfg(THwaFcDevList *pDevCtx,xmlNodePtr pCurNode)
{
	THwaFcXmlStr nStr = INVAILED_HWA_FC_XML_STR;

	/*get card work mode*/
	nStr = xmlGetProp(pCurNode,(const xmlChar*)HWA_CARD_WORK_MODE);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar*)CARD_MODE_NORMAL))
		{
			pDevCtx->eFcDevMode = eModeNormal;
		}else if(!xmlStrcmp(nStr, (const xmlChar*)CARD_MODE_CAP))
		{
			pDevCtx->eFcDevMode = eModeCapture;
		}else{
			ErrLog("Get unknown work mode config.");
			XML_STR_FREE(nStr);
			return HWA_FC_ERR_XML;
		}
		XML_STR_FREE(nStr);
	}	
	/*get card work speed*/
	nStr = xmlGetProp(pCurNode,(const xmlChar*)HWA_CARD_SPEED);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar*)CARD_SPEED_1G))
		{
			pDevCtx->eFcDevSpeed = eSpeed1G;
		}else if(!xmlStrcmp(nStr,(const xmlChar*)CARD_SPEED_2G))
		{
			pDevCtx->eFcDevSpeed = eSpeed2G;
		}else{
			ErrLog("Get unknown work speed config.");
			XML_STR_FREE(nStr);
			return HWA_FC_ERR_XML;
		}
		XML_STR_FREE(nStr);
	}

	/*Get IRIG-B Mode*/
	nStr = xmlGetProp(pCurNode,(const xmlChar*)HWA_CARD_IRIG_MODE);
	if(nStr != NULL)
	{
		if(!xmlStrcmp(nStr,(const xmlChar*)CARD_MODE_MASTER))
		{
			pDevCtx->eFcIrigMode = eModeMaster;
		}else if(!xmlStrcmp(nStr,(const xmlChar*)CARD_MODE_SLAVE))
		{
			pDevCtx->eFcIrigMode = eModeSlave;
		}else{
			ErrLog("Get unknown IRIG-B mode config.");
			XML_STR_FREE(nStr);
			return HWA_FC_ERR_XML;
		}
		XML_STR_FREE(nStr);
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
static void HwaFcDriverClearConfig(THwaFcDevList *pDevCtx)
{
	if(pDevCtx	!= NULL)
	{
		pDevCtx->eFcDevMode = eModeNormal;
		pDevCtx->eFcDevSpeed = eSpeed1G;
		pDevCtx->eFcIrigMode = eModeMaster;
		memset(pDevCtx->nRecvPort,0,sizeof(pDevCtx->nRecvPort));
		memset(pDevCtx->nSendPort,0,sizeof(pDevCtx->nSendPort));
		memset(pDevCtx->nRecvPortMask,0,sizeof(pDevCtx->nRecvPortMask));
		memset(pDevCtx->nSendPortMask,0,sizeof(pDevCtx->nSendPortMask));
	}
}


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverPhraseXml(int nCardNum,char *pFileName)
{
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	xmlDocPtr pXml = NULL;
	xmlNodePtr pNodeCur = NULL;
	if((pFileName == NULL) ||(pDevCtx == NULL))
	{
		ErrLog("No file name or device struct.");
		return HWA_FC_ERR_PARA;
	}

	if((pXml = xmlParseFile(pFileName)) == NULL)
	{
		ErrLog("Get wrong config file.");
		return HWA_FC_ERR_PARA;
	}

	if((pNodeCur = xmlDocGetRootElement(pXml)) == NULL)
	{
		ErrLog("Get wrong config file.");
		xmlFreeDoc(pXml);
		return HWA_FC_ERR_PARA;
	}

	if(!xmlStrcmp(pNodeCur->name,(const xmlChar *)HWA_FC_NIC))
	{
		ErrLog("Get wrong config file.");
		xmlFreeDoc(pXml);
		return HWA_FC_ERR_PARA;
	}
	pNodeCur = pNodeCur->xmlChildrenNode;
	while(pNodeCur != NULL)
	{
		if(!xmlStrcmp(pNodeCur->name,(const xmlChar*)HWA_FC_CARD_CFG))
		{
			if(HwaFcDriverPhraseCardCfg(pDevCtx,pNodeCur) != HWA_FC_OP_OK)
			{
				xmlFreeDoc(pXml);
				return HWA_FC_ERR_XML;
			}
			if(pDevCtx->eFcDevMode == eModeCapture)
			{
				xmlFreeDoc(pXml);
				return HWA_FC_OP_OK;
			}
		}else if(!xmlStrcmp(pNodeCur->name,(const xmlChar *)PORT_CFG))
		{

			if(HwaFcDriverPhrasePortCfg(pDevCtx,pNodeCur) != HWA_FC_OP_OK)
			{
				xmlFreeDoc(pXml);
				return HWA_FC_ERR_XML;
			}
		}
		pNodeCur = pNodeCur->next; 
	}

	if(HwaFcDriverCheckCfg(pDevCtx) != HWA_FC_OP_OK)
	{
		return HWA_FC_ERR_XML;
	}
	return HWA_FC_OP_OK;
}
#else /*defined VXWORKS_OPS*/
#error "unknown operation system"
#endif
#endif  /*#ifdef SUPPORT_XML*/


/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverAddPort(int nCardNum,THwaFcPort nPort)
{
	THwaFcPort *pPort = NULL;
	THwaFcPortDir ePortDir = nPort.ePortDir;	
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
/*find port*/
	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	CHECK_CARD_ST(nCardNum);
	if((pPort = HwaFcDriverFindUnusedPort(pDevCtx,nPort.ePortDir)) == NULL)
	{
		ErrLog("Too many %s port.\n",(ePortDir == eDirRecv) ? "receive" : "send");
		goto ERR;
	}
	nPort.nPortIndex = pPort->nPortIndex;
	memcpy(pPort,&nPort,sizeof(nPort));
/*check port cfg*/
	if(HwaFcDriverCheckPort(pDevCtx,pPort,ePortDir) != HWA_FC_OP_OK)
	{
		goto ERR;
	}
/*use port*/
	HwaFcDriverUsePort(pDevCtx,pPort,ePortDir);
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return HWA_FC_OP_OK;
ERR:
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return HWA_FC_ERR_PORT;
}

/* -----------------------------------------------
Function Name	:
Purpose			:
InPut			:
OutPut			:
return			:
----------------------------------------------- */
int HwaFcDriverDelPort(int nCardNum,unsigned int nPortId,THwaFcPortDir ePortDir)
{
	THwaFcPort *pPort = NULL;
	THwaFcDevList *pDevCtx = &nHwaFcDev[nCardNum];
	CHECK_CARD_ST(nCardNum) ;
	WAIT_EVENT(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);	
/*find port*/
	if((pPort = HwaFcDriverFindPortByID(pDevCtx,nPortId,ePortDir)) == NULL)
	{
		ErrLog("%s port %d not existed.\n",(ePortDir == eDirRecv) ? "Receive" : "Send",nPortId);
		goto ERR;
	}

	if(pPort->nLoaded == 1)
	{
		ErrLog("%s port %d is in use.\n",(ePortDir == eDirRecv) ? "Receive" : "Send",nPortId);
		goto ERR;
	}

/*unuse port*/
	HwaFcDriverUnusePort(pDevCtx,pPort,ePortDir);
	memset(pPort,0,sizeof(THwaFcPort));
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return HWA_FC_OP_OK;
ERR:
	
	RELEASE_LOCK(nHwaFcDev[nCardNum].nHwaFcDrvHandle.nPortLock);
	return HWA_FC_ERR_PORT;
}


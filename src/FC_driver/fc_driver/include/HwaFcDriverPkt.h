#ifndef __HWA_FC_PKT__
#define __HWA_FC_PKT__

typedef struct  
{
	unsigned char nMsgID[4];
	unsigned char nVendorSS[4];
	unsigned char nReserved[4];
	unsigned char nLAndPri;
	unsigned char nMsgPayloadLen[3];
}THwaFcAsmHeader;

typedef struct{
	unsigned char nSof[4];
}THwaFcSof;

typedef struct{
	unsigned char nEof[4];
}THwaFcEof;

#define ASM_SET_MSG_PAYLOAD_LEN(len,Header) do{\
											for(int i = 0; i < 3; i++)\
											{\
												Header->nMsgPayloadLen[i] = (((len) >> ((2 - i) * 8)) & 0xff);\
											}\
										}while(0) 

#define ASM_GET_MSG_PAYLOAD_LEN(Header)		(((unsigned int)Header->nMsgPayloadLen[0] << 16) |\
											((unsigned int)Header->nMsgPayloadLen[1] << 8)  |\
											((unsigned int)Header->nMsgPayloadLen[2]))

#define ASM_GET_MSG_ID(Header) (((unsigned int)Header->nMsgID[0] << 24) |\
								((unsigned int)Header->nMsgID[1] << 16) |\
								((unsigned int)Header->nMsgID[2] << 8)	|\
								((unsigned int)Header->nMsgID[3]))

#define ASM_GET_RES(Header) (((unsigned int)Header->nReserved[0] << 24) |\
							((unsigned int)Header->nReserved[1] << 16) |\
							((unsigned int)Header->nReserved[2] << 8)	|\
							((unsigned int)Header->nReserved[3]))

#define ASM_GET_VCENSS(Header) (((unsigned int)Header->nVendorSS[0] << 24) |\
								((unsigned int)Header->nVendorSS[1] << 16) |\
								((unsigned int)Header->nVendorSS[2] << 8)	|\
								((unsigned int)Header->nVendorSS[3]))

#define ASM_L_BIT 0x7
#define ASM_SET_L_BIT(x)	(x) = (x) | (0x1 << ASM_L_BIT)
#define ASM_CLEAR_L_BIT(x)	(x) = (x) & ~(0x1 << ASM_L_BIT)
#define ASM_GET_L_BIT(x)	(((x) >> ASM_L_BIT) & 0x1)

#define ASM_PRIORITY_MASK 0x7f
#define ASM_PRIORITY_BIT  0x0
#define ASM_SET_PRIORITY(x,y) (x) |= (((y) & ASM_PRIORITY_MASK) << ASM_PRIORITY_BIT)  
#define ASM_GET_PRIORITY(x) (((x) >> ASM_PRIORITY_BIT) & ASM_PRIORITY_MASK)

											
/*
--------------------------------------------------------------------
|BITS    | 31 ......24 | 23 .......16 | 15 ...... 08 | 07 ...... 00|
|------------------------------------------------------------------|
|0(0-3)  |                  Message ID                             |
|------------------------------------------------------------------|
|1(4-7)  |           Vendor Specific-Security                      |
|------------------------------------------------------------------|
|2(8-11) |                  Reserved                               |
|------------------------------------------------------------------|
|3(12-15)| L | Priority|     Message Payload Lengths(Bytes)        |
--------------------------------------------------------------------
*/


typedef struct  
{
	unsigned char nRCtl;
	unsigned char nDid[3];
	unsigned char nCsCtl;
	unsigned char nSid[3];
	unsigned char nType;
	unsigned char nFctl[3];
	unsigned char nSeqId;
	unsigned char nDfCtl;
	unsigned char nSeqCnt[2];
	unsigned char nOxId[2];
	unsigned char nRxId[2];
	unsigned char nParameter[4];
}THwaFcAeHeader;

/*
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |     R_CTL     |                    D_ID                          |
|---------------------------------------------------------------------------|
|1(4-7)  |CS_CTL/Priority|                    S_ID                          |
|---------------------------------------------------------------------------|
|2(8-11) |     TYPE      |                    F_CTL                         |
|---------------------------------------------------------------------------|
|3(12-15)|    SEQ_ID     |      DF_CTL    |        SEQ_CNT                  |
|---------------------------------------------------------------------------|
|4(16-19)|             OX_ID              |              RX_ID              |
|---------------------------------------------------------------------------|
|5(19...)|                               Parameter                          |
-----------------------------------------------------------------------------
*/
#define FCAE_SET_DID(did,Header) do{\
									for(int i = 0; i < 3; i++)\
									{\
										Header->nDid[i] = (((did) >> ((2 - i) * 8)) & 0xff);\
									}\
								}while(0) 

#define FCAE_GET_DID(Header)	(((unsigned int)Header->nDid[0] << 16) |\
								((unsigned int)Header->nDid[1] << 8)  |\
								((unsigned int)Header->nDid[2]))

#define FCAE_SET_SID(sid,Header) do{\
									for(int i = 0; i < 3; i++)\
									{\
										Header->nSid[i] = (((sid) >> ((2 - i) * 8)) & 0xff);\
									}\
								}while(0) 

#define FCAE_GET_SID(Header)	(((unsigned int)Header->nSid[0] << 16) |\
								((unsigned int)Header->nSid[1] << 8)  |\
								((unsigned int)Header->nSid[2]))

#define FCAE_SET_FCTL(fctl,Header) do{\
										for(int i = 0; i < 3; i++)\
										{\
											Header->nFctl[i] = (((fctl) >> ((2 - i) * 8)) & 0xff);\
										}\
									}while(0) 

#define FCAE_GET_FCTL(Header)	(((unsigned int)Header->nFctl[0] << 16) |\
								((unsigned int)Header->nFctl[1] << 8)  |\
								((unsigned int)Header->nFctl[2]))

/*
unsigned char nSeqCnt[2];
unsigned char nOxId[2];
unsigned char nRxId[2];
*/
#define FCAE_GET_SEQCNT(Header) (((unsigned int)Header->nSeqCnt[0] << 8) | ((unsigned int)Header->nSeqCnt[1]))
#define FCAE_GET_OXID(Header) (((unsigned int)Header->nOxId[0] << 8) | ((unsigned int)Header->nOxId[1]))
#define FCAE_GET_RXID(Header) (((unsigned int)Header->nRxId[0] << 8) | ((unsigned int)Header->nRxId[1]))

/*R_CTL*/
#define R_CTL_ROUTER_BIT			0x4
#define R_CTL_ROUTER_MASK			0xf
#define R_CTL_ROUTER_FC_4_DEV		0x0
#define R_CTL_ROUTER_EXTEN			0x2
#define R_CTL_ROUTER_FC_4_LINK		0x3
#define R_CTL_ROUTER_VIDEO			0x4
#define R_CTL_ROUTER_BASE			0x8
#define R_CTL_ROUTER_CONTRL			0xc

#if 0
/*these are from GJB*/
#define R_CTL_INFO_BIT				0x0
#define R_CTL_INFO_MASK				0xf
#define R_CTL_INFO_UNGRP			0x0
#define R_CTL_INFO_REQ				0x1
#define R_CTL_INFO_SELF_SENDCTL		0x2
#define R_CTL_INFO_REQCTL			0x3
#define R_CTL_INFO_SELF_SENDDATA	0x4
#define R_CTL_INFO_DATA_DES			0x5
#define R_CTL_INFO_SELF_SENDCMD		0x6
#define R_CTL_INFO_CMD_STATUS		0x7
#else
/*these are from FC-LS*/
#define R_CTL_INFO_BIT 0x0
#define R_CTL_INFO_MASK 0xf
#define R_CTL_INFO_SOD 0x1
#define R_CTL_INFO_REQ 0x2
#define R_CTL_INFO_REP 0x3
#endif


typedef enum{
	LS_RJT  = 1,  
	LS_ACC,    
	PLOGI,    
	FLOGI,    
	LOGO,   
	ABTX,  
	RCS, 
	RES,       
	RSS,       
	RSI,       
	ESTS,      
	ESTC,      
	ADVC,      
	RTV,       
	RLS,       
	ECHO,      
	TEST,      
	RRQ,       
	REC,       
	RESFLEG,   
	PRLI = 0x20,      
	PRLO,      
	SCN,      
	TPLS,      
	TPRLO, 		
	LCLM, 			
	GAID , 			
	FACT = 0x40, 			
	FDACT, 		
	NACT, 			
	NDACT, 		
	QoSR, 			
	RVCS,      
	PDISC =0x50, 		
	FDISC, 		
	ADISC, 		
	RNC, 			
	FARP_REQ,  
	FARP_REPLY,
	RPS, 			
	RPL, 			
	RPBC, 			
	FAN = 0x60, 			
	RSCN, 			
	SCR, 			
	RNFT, 			
	CSR = 0x68, 			
	CSU, 			
	LINIT, 		
	LPC, 			
	LSTS, 			
	VENSEP,    
	RNID, 			
	RLIR, 			
	LIRR, 			
	SRL, 			
	SBRP, 			
	RPSC, 			
	QSA, 			
	EVFP, 			
	LKA ,      
	LCB ,      
	AUTH_ELS =0x90, 	
	RFCN =0x97, 			
	FFI_DTM = 0xA0, 	
	FFI_RTM, 	
	FFI_PSS, 	
	FFI_MUR, 	
	FFI_RMUN, 	
	FFI_SMU, 	
	FFI_RMU, 	
	eElsCmdNr,
}THwaFcElsCmd;
/*TYPE*/
/*
N-PORT / F-PORT Linker service
*/
#define TYPE_BASE_LINKER_SRV	0x0
#define TYPE_EXTEN_LINKER_SRV	0x1

/*
Video data
*/

/*
FC-4 device data or linker data
*/
#define TYPE_ISO_IEC8802_2LLC_SNAP_1	0x4
#define TYPE_ISO_IEC8802_2LLC_SNAP_2	0x5
#define TYPE_SCSI_FCP					0x8
#define TYPE_SCSI_GPP					0x9
#define TYPE_IPI_3_MASTER_1				0x11
#define TYPE_IPI_3_SLAVER_1				0x12
#define TYPE_IPI_3_PEER_1				0x13
#define TYPE_IPI_3_MASTER_2				0x15
#define TYPE_IPI_3_SLAVER_2				0x16
#define TYPE_IPI_3_PEER_2				0x17
#define TYPE_SBCCS_CHANNEL				0x19
#define TYPE_SBCCS_CTL					0x1a
#define TYPE_FC_SRV					0x20
#define TYPE_FC_FG					0x21
#define TYPE_FC_XS					0x22
#define TYPE_FC_AL					0x23
#define TYPE_SNMP					0x24
#define TYPE_HIPPI_FP				0x40

/*F_CTL*/
/*
BYTE 0
*/
#define F_CTL_SWITCH_RELATION_BIT	0x7
#define F_CTL_SWITCH_RELATION_MASK	0x1
#define F_CTL_SWITCHER		0x0
#define F_CTL_SWITCH_RECVER	0x1

#define FC_CTL_SEQ_RELATION_BIT		0x6
#define FC_CTL_SEQ_RELATION_MASK	0x1
#define FC_CTL_SEQ_BEGINNER	0x0
#define FC_CTL_SEQ_RECVER	0x1

#define FC_CTL_FIRST_SEQ_BIT	0x5
#define FC_CTL_FIRST_SEQ_MASK	0x1
#define FC_CTL_FIRST_SEQ	0x1
#define FC_CTL_OTHER_SEQ	0x0

#define FC_CTL_LAST_SEQ_BIT	0x4
#define FC_CTL_LAST_SEQ_MASK	0x1
#define FC_CTL_LAST_SEQ		0x1
#define FC_CTL_OTHER_SEQ	0x0

#define FC_CTL_END_SEQ_DATA_BIT		0x3
#define FC_CTL_END_SEQ_DATA_MASK	0x1
#define FC_CTL_END_SEQ_DATA		0x1
#define FC_CTL_OTHER_SEQ_DATA	0x0

#define FC_CTL_END_LINK_BIT		0x2
#define FC_CTL_END_LINK_MASK	0x1
#define FC_CTL_LINK				0x0
#define FC_CTL_UNDEC_CLA1_LINK	0x1

#define FC_CTL_LINK_SEQ_BIT		0x1
#define FC_CTL_LINK_SEQ_MASK	0x1
#define FC_CTL_LINK_SEQUACT		0x0
#define FC_CTL_LINK_SEQ_ACT		0x1

#define FC_CTL_SEQ_RIGHT_BIT		0x0
#define FC_CTL_SEQ_RIGHT_MASK		0x1
#define FC_CTL_SEQ_RIGHT_KEEP		0x0
#define FC_CTL_SEQ_RIGHT_DROP		0x1

/*BYTE 1*/
#define FC_CTL_XID_REASSIGN_BIT	0x7
#define FC_CTL_XID_REASSIGN_MASK	0x1
#define FC_CTL_XID_KEEP		0x0
#define FC_CTL_XID_REASSIGN	0x1

#define FC_CTL_XID_VAILED_BIT	0x6
#define FC_CTL_XID_VAILED_MASK	0x1
#define FC_CTL_XID_ASSIGN_KEEP	0x0
#define FC_CTL_XID_UNVAILED		0x1	

#define FC_CTL_RES_BIT	0x5
#define FC_CTL_RES_MASK	0xf

#define FC_CTL_TRANS_BIT	0x1
#define FC_CTL_TRANS_MASK	0x1
#define FC_CTL_TRANS_BEGINNER	0x0
#define FC_CTL_RETRANS			0x1

#define FC_CTL_TRANS_DIR_BIT	0x0
#define FC_CTL_TRANS_DIR_MASK	0x1
#define FC_CTL_SINGLE_DIR		0x0
#define FC_CTL_DOUBLE_DIR		0x1

/*BYTE 2*/
#define F_CTL_SEQ_STAT_BIT	0x7
#define F_CTL_SEQ_STAT_MASK	0x3
#define FC_CTL_SEQ_STAT_NO		0x0
#define FC_CTL_SEQ_STAT_IMM		0x1
#define FC_CTL_SEQ_STAT_SOON	0x2
#define FC_CTL_SEQ_STAT_DELAY	0x3

#define F_CTL_END_SEQ_CONDITION_BIT		0x5
#define F_CTL_END_SEQ_CONDITION_MASK	0x3
#define F_CTL_END_SEQ_CONDITION_DROP_MUL	0x0
#define F_CTL_END_SEQ_CONDITION_DROP_SIN	0x1
#define F_CTL_END_SEQ_CONDITION_WITHBUG		0x2
#define F_CTL_END_SEQ_CONDITION_DROP_MUL_RETRNA 0x3

#define F_CTL_OFFSET_VALIED_BIT		0x3
#define F_CTL_OFFSET_VALIED_MASK	0x3
#define F_CTL_OFFSET_VALIED			0x1
#define F_CTL_OFFSET_UNVALIED		0x0

#define F_CTL_PKT_REBUILD_BIT	0x2
#define F_CTL_PKT_REBUILD_MASK	0x1

#define F_CTL_FILL_BIT		0x1
#define F_CTL_FILL_MASK		0x3

/*DF_CTL*/
#define DF_CTL_EXTEND_RES_BIT 0x7
#define DF_CTL_EXTEND_RES_MASK 0x1

#define DF_CTL_SAFE_HEADER_BIT	0x6
#define DF_CTL_SAFE_HEADER_MASK 0x1
#define DF_CTL_SAFE_HEADER		0x1
#define DF_CTL_UNSAFE_HEADER	0x0

#define DF_CTL_NET_HEADER_BIT	0x5
#define DF_CTL_NET_HEADER_MASK	0x1
#define DF_CTL_NET_HEADER		0x1
#define DF_CTL_UNNET_HEADER		0x0

#define DF_CTL_RELE_HEADER_BIT	0x4
#define DF_CTL_RELE_HEADER_MASK	0x1
#define DF_CTL_RELE_HEADER		0x1
#define DF_CTL_UNRELE_HEADER		0x0

#define DF_CTL_RES_BIT 0x3
#define DF_CTL_RES_MASK 0x3

#define DF_CTL_DEV_HEADER_BIT 0x1
#define DF_CTL_DEV_HEADER_MASK 0x3
#define DF_CTL_DEV_UNHEADER 0x0
#define DF_CTL_DEV_HEADER_16 0x1
#define DF_CTL_DEV_HEADER_32 0x2
#define DF_CTL_DEV_HEADER_64 0x3


/*safe header*/
/*
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |     Timed Out Higher                                             |
|---------------------------------------------------------------------------|
|1(4-7)  |     Timed Out Lower                                              |
|---------------------------------------------------------------------------|
|2(8-11) |     SAFE TYPE |     RES        |    SAFE_LENGT                   |
|---------------------------------------------------------------------------|
|3(12-15)|                       SAFE                                       |
-----------------------------------------------------------------------------
*/
typedef struct{
	unsigned int nTimedOutHigh;
	unsigned int nTimedOutLower;
	unsigned char nSafeType;
	unsigned char nRes;
	unsigned short nSafeLen;
	unsigned int nSafeValue;
}THwaFcAeSafeHeader;


/*NET HEADER*/
/*
-----------------------------------------------------------------------------
|BITS    |31 ..28    |  27..24 | 23 ... 16 | 15 ........ 08| 07 ....... 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |  DstNet R |           Destination Address High                   |
|---------------------------------------------------------------------------|
|1(4-7)  |     Destination Address Lower                                    |
|---------------------------------------------------------------------------|
|2(8-11) |  SrcNet R |    Source Address High                               |
|---------------------------------------------------------------------------|
|3(12-15)|                       Source Address Lower                       |
-----------------------------------------------------------------------------
*/
typedef struct{
	unsigned int nDstR_DstAddrH;
	unsigned int nDstAddrL;
	unsigned int nSrcR_SrcAddrH;
	unsigned int nSrcAddrL;
}THwaFcAeNetHeader;

/*relevance Header/
/*
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  | vvvv rrrr     |   rrrr rrrrr   | rrrr rrrr     | rrrr rrrr       |
|---------------------------------------------------------------------------|
|1(4-7)  |                Sender Processing Factor                          |
|---------------------------------------------------------------------------|
|2(8-11) | rrrr rrrr     |   rrrr rrrr    | rrrr rrrr     | rrrr  rrrr      |
|---------------------------------------------------------------------------|
|3(12-15)|                Receiver Processing Factor                        |
|---------------------------------------------------------------------------|
|4(16-19)|                Sender Operation Factor High                      |
|---------------------------------------------------------------------------|
|5(20-23)|                Sender Operation Factor Lower                     |
|---------------------------------------------------------------------------|
|6(24-27)|                Operator Factor High                              |
|---------------------------------------------------------------------------|
|7(28-31)|                Operator Factor Low                               |
-----------------------------------------------------------------------------
*/
typedef struct  
{
	unsigned int nVr;
	unsigned int nSpf;
	unsigned int nR;
	unsigned int nRpf;
	unsigned int nSofH;
	unsigned int nSofL;
	unsigned int nOfH;
	unsigned int nOfL;
}THwaFcAeRelevanceHeader;

/*Login Packet*/

/*N-PORT Login Common Parameter For Login to Switch FLOGIN
---------------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 .. 12|...... 08| 07 ........ 00  |
|-------------------------------------------------------------------------------|
|0(0-3)  |     FC-PH Version - obsolete   |  Buffer-to-Buffer Credit            |
|-------------------------------------------------------------------------------|
|1(4-7)  |     Common Features            | BB_SC_N |BUF-BUF RECV DataField size|
|-------------------------------------------------------------------------------|
|2(8-11) |     Res                                                              |
|-------------------------------------------------------------------------------|
|3(12-15)|     Res                                                              |
----------------------------------------------------------------------------------
*/
typedef struct{
	unsigned short nPhVersion;
	unsigned short nB2bCredit;
	unsigned short nCommonFeatures;
	unsigned short nB2bDataSize;
	unsigned char  nRes[8];
}THwaFcLoginCommonParaF;
/*

/*N-PORT Login Common Parameter For Login to N-PORT
---------------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 .. 12|...... 08| 07 ........ 00  |
|-------------------------------------------------------------------------------|
|0(0-3)  |     FC-PH Version - obsolete   |  Buffer-to-Buffer Credit            |
|-------------------------------------------------------------------------------|
|1(4-7)  |     Common Features            | BB_SC_N |BUF-BUF RECV DataField size|
|-------------------------------------------------------------------------------|
|2(8-11) |     Res       |ConcurrentSeqAll|RelativeOffsetByInformationCategory  |
|-------------------------------------------------------------------------------|
|3(12-15)|     E_D_TOV                                                          |
----------------------------------------------------------------------------------
*/
typedef struct{
	unsigned short nPhVersion;
	unsigned short nB2bCredit;
	unsigned short nCommonCharactor;
	unsigned short nB2bDataSize;
	unsigned short nCsc;
	unsigned short nOffType;
	unsigned int nEdtov;
}THwaFcLoginCommonParaP;


/*N-PORT Login Common Parameter For ACC
---------------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 .. 12|...... 08| 07 ........ 00  |
|-------------------------------------------------------------------------------|
|0(0-3)  |     FC-PH Version - obsolete   |  Buffer-to-Buffer Credit(FC_PORT)   |
|-------------------------------------------------------------------------------|
|1(4-7)  |     Common Features            | BB_SC_N |BUF-BUF RECV DataField size|
|-------------------------------------------------------------------------------|
|2(8-11) |       R_A_TOV                                                        |
|-------------------------------------------------------------------------------|
|3(12-15)|       E_D_TOV                                                          |
----------------------------------------------------------------------------------
*/
typedef struct{
	unsigned short nPhVersion;
	unsigned short nB2bCredit;
	unsigned short nCommonCharactor;
	unsigned short nB2bDataSize;
	unsigned int nRaTov;
	unsigned int nEdtov;
}THwaFcLoginCommonParaA;

/*class service for fLogin and pLogin
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |     Service Option             |  Initiator Control              |
|---------------------------------------------------------------------------|
|1(4-7)  |     Recipient Control          | Reserved                        |
|---------------------------------------------------------------------------|
|2(8-11) |   Res         |Concurrent Seq  | Port to Port  Credit            |
|---------------------------------------------------------------------------|
|3(12-15)|   Res         |Opened Seq CNT  |               RES               |
-----------------------------------------------------------------------------
*/
typedef struct  
{
	unsigned short nSrvOpt;
	unsigned short InitiatorCtl;
	unsigned short RecipientCtl;
	unsigned char nRes1[2];
	unsigned char nCs;
	unsigned short nP2pCredit;
	unsigned char nRes2;
	unsigned char nOsc;
	unsigned char nRes3[2];
}THwaFcLoginClassSrvFP;


/*class service for pLogin ACC
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |     Service Option             |  Initiator Control              |
|---------------------------------------------------------------------------|
|1(4-7)  |     Recipient Control          | Reserved | Receive Data size    |
|---------------------------------------------------------------------------|
|2(8-11) |   Res         |Concurrent Seq  | Port to Port  Credit            |
|---------------------------------------------------------------------------|
|3(12-15)|   Res         |Opened Seq CNT  |               RES               |
-----------------------------------------------------------------------------
*/
typedef struct  
{
	unsigned short nSrvOpt;
	unsigned short InitiatorCtl;
	unsigned short RecipientCtl;
	unsigned short nResAndRds;
	unsigned char nCs;
	unsigned short nP2pCredit;
	unsigned char nRes2;
	unsigned char nOsc;
	unsigned char nRes3[2];
}THwaFcLoginClassSrvPA;

/*class service For fLogin ACC
-----------------------------------------------------------------------------
|BITS    |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|---------------------------------------------------------------------------|
|0(0-3)  |     Service Option             |  Res                            |
|---------------------------------------------------------------------------|
|1(4-7)  |     Recipient Control          |  Res                            |
|---------------------------------------------------------------------------|
|2(8-11) |                             Res                                  |
|---------------------------------------------------------------------------|
|3(12-15)|   CR_TOV                                                         |
-----------------------------------------------------------------------------
*/
typedef struct  
{
	unsigned short nSrvOpt;
	unsigned char nRes1[2];
	unsigned short RecipientCtl;
	unsigned char nRes2[6];
	unsigned int nCrTov;
}THwaFcLoginClassSrvFA;
/*
-------------------------------------------------------------------------------
|BITS      |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|-----------------------------------------------------------------------------|
|0(0-3)    |                           Login Type                             |
|-----------------------------------------------------------------------------|
|1(4-19)   |                           Common Service Parameter               |
|-----------------------------------------------------------------------------|
|2(20-27)  |                           N-Port Name                            |
|-----------------------------------------------------------------------------|
|3(28-35)  |                           Node Name                              |
|-----------------------------------------------------------------------------|
|4(36-51)  |                           Class 1 Service Parameter              |
|-----------------------------------------------------------------------------|
|5(52-67)  |                           Class 2 Service Parameter              |
|-----------------------------------------------------------------------------|
|6(68-83)  |                           Class 3 Service Parameter              |
|-----------------------------------------------------------------------------|
|7(84-99)  |                           Res                                    |
|-----------------------------------------------------------------------------|
|8(100-115)|                           Vendor ID                              |
--------------------------------------------------------------------------------
*/
typedef struct{
	unsigned int nLoginType;
	unsigned char nCommonService[16];
	unsigned char nNportName[8];
	unsigned char nNodeName[8];
	unsigned char nClass[3][16];
	unsigned char nRes[16];
	unsigned char nVendorID[16];
}THwaFcAeLoginPacket;
#define FC_LOGIN_TYPE_PLOGIN 0x03000000
#define FC_LOGIN_TYPE_FLOGIN 0x04000000

/*LS_ACC
-------------------------------------------------------------------------------
|BITS      |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|-----------------------------------------------------------------------------|
|0(0-3)    |                           ES_Command                             |
|-----------------------------------------------------------------------------|
|1(4-19)   |                           Common Service Parameter               |
|-----------------------------------------------------------------------------|
|2(20-27)  |                           N-Port Name                            |
|-----------------------------------------------------------------------------|
|3(28-35)  |                           Node Name                              |
|-----------------------------------------------------------------------------|
|4(36-51)  |                           Class 1 Service Parameter              |
|-----------------------------------------------------------------------------|
|5(52-67)  |                           Class 2 Service Parameter              |
|-----------------------------------------------------------------------------|
|6(68-83)  |                           Class 3 Service Parameter              |
|-----------------------------------------------------------------------------|
|7(84-99)  |                           Res                                    |
|-----------------------------------------------------------------------------|
|8(100-115)|                           Vendor ID                              |
--------------------------------------------------------------------------------
*/
typedef struct{
	unsigned int nCmd;
	unsigned char nCommonService[16];
	unsigned char nNportName[8];
	unsigned char nNodeName[8];
	unsigned char nClass[3][16];
	unsigned char nRes[16];
	unsigned char nVendorID[16];
}THwaFcAeEsAcc;


/*LS_RJT
-------------------------------------------------------------------------------
|BITS      |31 ........ 24 | 23 ........ 16 | 15 ........ 08| 07 ........ 00  |
|-----------------------------------------------------------------------------|
|0(0-3)    |                           ES_Command                             |
|-----------------------------------------------------------------------------|
|1(4-7)   |       Res     | Reason code    | Reason Code EX|Vendor unique     |
--------------------------------------------------------------------------------
*/
typedef struct{
	unsigned int nCmd;
	unsigned char nRes;
	unsigned char nReason;
	unsigned char nReasonEx;
	unsigned char nVendoruq;
}THwaFcAeEsRjt;


typedef enum{
	RJT_I_ELSCMD	= 0x01,
	RJT_LOG_ERR		= 0x03,
	RJT_LOG_BUSY	= 0x05,
	RJT_PRO_ERR		= 0x07,
	RJT_UP_CMD		= 0x09,
	RJT_CMD_NS		= 0x0B,
	RJT_CMD_ALP		= 0x0E,
	RJT_FIP_ERR		= 0x20,
	RJT_VFE			= 0xFF,
}THwaFcRjtReason;

typedef enum{
	RJT_EX_NON				= 0X00,
	RJT_EX_LOG_ERR			= 0X01,
	RJT_EX_INIT_CTL_ERR		= 0X03,
	RJT_EX_REC_CTL_ERR		= 0X05,
	RJT_EX_RECV_DATAF_ERR	= 0X07,
	RJT_EX_CONCUR_SEQ_ERR	= 0X09,
	RJT_EX_CREDIE_ADVC_ERR	= 0X0B,
	RJT_EX_INV_NPORT_NAME	= 0X0D,
	RJT_EX_INV_NODE_NAME	= 0X0E,
	RJT_EX_INV_COM_PARA		= 0X0F,
	RJT_EX_INV_ASS_HEADER	= 0X11,
	RJT_EX_REQ_ASS_HEADER	= 0X13,
	RJT_EX_INV_ORI_SID		= 0X15,
	RJT_EX_INV_ORX_ID		= 0X17,
	RJT_EX_CMD_INPROGRESS	= 0X19,
	RJT_EX_NPORT_LOG_REQ	= 0X1E,
	RJT_EX_INV_NPORT_ID		= 0X1F,
	RJT_EX_OBS_1			= 0X21,
	RJT_EX_OBS_2			= 0X23,
	RJT_EX_OBS_3			= 0X25,
	RJT_EX_OBS_4			= 0X27,
	RJT_EX_INS_RES_LOGIN	= 0X29,
	RJT_EX_UNSUP_REQ		= 0X2A,
	RJT_EX_REQ_UNSUP		= 0X2C,
	RJT_EX_INV_PAYLOAD_LEN	= 0X2D,
	RJT_EX_NOALIAS_ID		= 0X30,
	RJT_EX_ALIAS_NOT_ACT_N	= 0X31,
	RJT_EX_ALIAS_NOT_ACT_I	= 0X32,
	RJT_EX_ALIAS_NOT_DEC_D	= 0X33,
	RJT_EX_ALIAS_NOT_DEC_R	= 0X34,
	RJT_EX_SRV_PARA_CONF	= 0X35,
	RJT_EX_INV_ALIAS_TOK	= 0X36,
	RJT_EX_UNSUP_ALIAS_TOK	= 0X37,
	RJT_EX_ALIAS_GRP		= 0X38,
	RJT_EX_OBS_5			= 0X40,
	RJT_EX_OBS_6			= 0X41,
	RJT_EX_OBS_7			= 0X42,
	RJT_EX_INV_PN_NAME		= 0X44,
	RJT_EX_LOGIN_EX_NS		= 0X46,
	RJT_EX_AUTH_REQ			= 0X48,
	RJT_EX_PER_SCAN_NALLOW	= 0X50,
	RJT_EX_PER_SCAN_NSUP	= 0X51,
	RJT_EX_MAC_ADD_MODE_NSU	= 0X60,
	RJT_EX_PROP_MAC_INCORE	= 0X61,
}THwaFcRjtReasonEx;

typedef unsigned char * THwaFcAeAsmPayload;

typedef struct
{
	char nCrc[4];
}THwaFcAeCrc;


typedef struct{
	THwaFcAeHeader nFcAeHeader;
	THwaFcAsmHeader nAsmHeader;
	THwaFcAeAsmPayload pPayload;
	THwaFcAeCrc nCrc;
}THwaFcAeAsmPkt;



typedef struct  
{
	unsigned int nRecvTime[2];
	unsigned int nPktLen;
	unsigned char nRecvCh;
	unsigned char nPktFlag;
	unsigned char nRes[2];
}THwaFcAeMonHeader;



#define SELF_DEF_HEADER_LEN		(32)
#define FC_AE_HAEDER_LEN		(24)
#define FC_AE_ASM_HEADER_LEN	(16)
#define ASM_NOT_PAYLOAD_LEN		(FC_SOF_LEN + FC_AE_ASM_HEADER_LEN + FC_AE_HAEDER_LEN + FC_CRC_LEN + FC_EOF_LEN)
#define AE_NOT_PAYLOAD_LEN		(FC_SOF_LEN + FC_AE_HAEDER_LEN  + FC_CRC_LEN + FC_EOF_LEN)



#define ASM_CRC_OFFSET(payLoadLen)		(SELF_DEF_HEADER_LEN + FC_SOF_LEN + FC_AE_ASM_HEADER_LEN + FC_AE_HAEDER_LEN + payLoadLen)
#define AE_CRC_OFFSET(payLoadLen)		(SELF_DEF_HEADER_LEN + FC_SOF_LEN + FC_AE_HAEDER_LEN  + payLoadLen)

#define SOF_OFFSET				(SELF_DEF_HEADER_LEN)
#define AE_HEADER_OFFSET		(SOF_OFFSET + FC_SOF_LEN)
#define	ASM_HEADER_OFFSET		(AE_HEADER_OFFSET + FC_AE_HAEDER_LEN)

#define ASM_PAYLOAD_OFFSET		(ASM_HEADER_OFFSET + FC_AE_ASM_HEADER_LEN)
#define AE_PAYLOAD_OFFSET		(AE_HEADER_OFFSET + FC_AE_HAEDER_LEN)


#define IS_ASM_PKT(selfHeader) ((selfHeader[0] & 0x80) >> 7)
#define GET_RECV_PORT(selfHeader)((selfHeader[0] & 0x60) >> 7)
#define GET_RECV_CHANNEL(selfHeader)((((int)(selfHeader[1]) & 0x3f) << 3) | ((selfHeader[2] & 0xe0) >> 5))
#define GET_PAYLOAD_LEN(selfHeader) ((((int)selfHeader[2] &0x0f) << 8) | (selfHeader[3]))
#endif
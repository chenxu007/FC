#ifndef _HWA_FC_INTER_H_

#define _HWA_FC_INTER_H_





#ifdef WIN32  /* For Windows */



#ifdef _FC_INTER_EXPORT

	#ifdef __cplusplus

		#define _FC_INTER_DLL_EXPORT extern "C" __declspec(dllexport)

	#else

		#define _FC_INTER_DLL_EXPORT __declspec(dllexport)

	#endif

#else

	#ifdef __cplusplus

		#define _FC_INTER_DLL_EXPORT extern "C" __declspec(dllimport)

	#else

		#define _FC_INTER_DLL_EXPORT __declspec(dllimport)

	#endif

#endif



#else  /* For VxWorks */



#define _FC_INTER_DLL_EXPORT



#endif





typedef unsigned int THwaInterHandle;

typedef unsigned int THwaInterCmd;

typedef unsigned int THwaInterDebug;





#define  HWA_INTER_OK						0

/* Error Code */

#define	 HWA_INTER_ERR_PARA					-1001

#define	 HWA_INTER_ERR_MEM					-1002  /*apply for memory err*/

#define  HWA_INTER_ERR_SOCKET				-1003

#define  HWA_INTER_ERR_CONNECT				-1004

#define  HWA_INTER_ERR_RHEADER				-1005 /*the  recv header err*/

#define  HWA_INTER_ERR_RDATAT_TIMEDOUT		-1006  /* recv data time out*/

#define  HWA_INTER_ERR_SHEADER				-1007 /* the send header err*/

#define  HWA_INTER_ERR_SEND_DATA			-1008  /*send data err*/

#define  HWA_INTER_ERR_MSGID				-1009

#define  HWA_INTER_ERR_CMD					-1010

#define  HWA_INTER_ERR_TYPE					-1011

#define  HWA_INTER_ERR_LISTEN				-1012 /* listen err */

#define  HWA_INTER_ERR_TIMEDOUT				-1013

#define  HWA_INTER_ERR_RECV_DATA_UDP		-1014  /*recv udp data err*/

#define  HWA_INTER_ERR_HEADER				-1015

#define  HWA_INTER_ERR_BUF					-1016 /* data buf is too short*/

#define  HWA_INTER_ERR_SEND_HEADER			-1017 /*send header err*/

#define  HWA_INTER_ERR_ACCEPT				-1018  /*server accept err*/

#define  HWA_INTER_ERR_BLIND				-1019  /*blind the port err*/





#define HWA_INTER_ERR_DEBUGLEVEL_CONFLICT   -1020  /*debug level conflict*/

#define HWA_INTER_ERR_MAKEDEBUGFILE         -1021 /* make file err*/

#define HWA_INTER_ERR_RECV_DATA_TCP			-1022 /*recv tcp data err*/

#define HWA_INTER_ERR_RECV_HEAD_TCP			-1023 /*recv tcp header err*/

#define HWA_INTER_ERR_RECV_HEADER_TIMEOUT   -1024 /*recv header time out*/

#define HWA_INTER_ERR_RECV_HWADER_UDP       -1025 /*recv udp header err*/

#define HWA_INTER_ERR_SEND_DATA_LEN			-1026

#define HWA_INTER_ERR_RECV_DATA_LEN			-1027

#define HWA_INTER_ERR_SET_THREAD            -1028

#define HWA_INTER_ERR_NOT_CONNECT_THE_SERVER -1029

#define HWA_INTER_ERR_SERVER_FAILED         -1030

#define HWA_INTER_ERR_THREAD_NOT_QUIT       -1031



#define IP_LEN 32

#define ERR_MSG_LEN 128



#define TIMEOUTSEC  3

#define TIMEOUTUSEC 0





#define NMAGIC_ALL_NUMBER               0x20120205

#define NMAGIC_REQUEST_HEADER			0x20120207

#define NMAGIC_REPLY_HEADER				0x20120206



#define DEBUG_NON						0x0

#define DEBUG_FLOW						0x1

#define DEBUG_WARN						0x2

#define DEBUG_ERR						0x4

#define DEBUG_FILE						0x128







/*port type*/

typedef enum{

	eTcpCli=0,

	eTcpSrv,

	eUdpCli,

	eUdpSrv ,

	eInterTypeNum

}THwaInterType;



/*UDP recv type*/

typedef enum{

	nUdpRecvHead=0,

	nUdpRecvData,

	nUdpRecvTypeNum

}THwaUdpRecvType;



/*port information  ip and port number*/

typedef struct{

	char sIpAddr[IP_LEN];

	unsigned int nPort;

	BOOL nFlag; /*1:give port and ip

				0:anto port and ip	*/

}THwaInterInfo;





/*the handle information*/

typedef struct {

	unsigned int nMagic;

	THwaInterType nInterType;

	THwaInterInfo nLocalInfo;

	THwaInterInfo nRemoteInfo;

 	unsigned int nListenSock;

 	unsigned int nConnectSock;

	unsigned int nMsgID;

	THwaInterDebug nDebug;

	int nDebugFileFd;

	char sLastErrMsg[ERR_MSG_LEN];



	CRITICAL_SECTION  nTcpSrvLock;



	HANDLE hTthreadSer;  /*thread lock*/

}THwaInter;





/*the request information*/

typedef struct{

	unsigned int nMagic;

	THwaInterCmd nCmd;

	int nSendDataLen;

	int nMsgID;

}THwaInterRequest;





/*the reply information*/

typedef struct{

	unsigned int nMagic;

	THwaInterCmd nReplyCmd;

	int nResult;

	int nMsgID;

	int nDataLen;

}THwaInterReply;





/*the packet information*/

typedef struct{

	int nPacketLen;

	int nRecvTime;

	int nRxId;

} THwaFcPacketInfo;





/*the packet header*/

typedef struct{

	unsigned int nMagic;

	unsigned int nPktSeq;

	THwaFcPacketInfo nHeaderInfo;

}THwaFcPacketHeader;







/*API*/

 /*init the socket*/

_FC_INTER_DLL_EXPORT  int HwaEtherInterInit(THwaInterHandle *pInterHandle,THwaInterType nType, THwaInterInfo *nLocalInfo, THwaInterInfo *nRemoteInfo);



/*send message */

_FC_INTER_DLL_EXPORT  int HwaEtherInterSend(THwaInterHandle nHandle, THwaInterCmd nCmd, void *pSendData, int nSendLen);





/*recv message*/

_FC_INTER_DLL_EXPORT  int HwaEtherInterRecvData(THwaInterHandle nHandle, void *pRecvData, int nRecvLen, THwaInterCmd *pCmd,int nTimedOut); 



_FC_INTER_DLL_EXPORT  int HwaEtherInterRecvHead(THwaInterHandle nHandle, /*void *pRecvData, int nRecvLen,*/ THwaInterCmd *pCmd,int nTimedOut) ;



/*send packet*/

_FC_INTER_DLL_EXPORT  int HwaEtherInterPktSend(THwaInterHandle nHandle, void *pPacket, THwaFcPacketInfo nPacketInfo); 





/*recv packet*/

_FC_INTER_DLL_EXPORT  int HwaEtherInterPktRecv (THwaInterHandle nHandle, int nBufLen, void *pBuf, THwaFcPacketInfo *pPacketInfo, int nTimedOut, THwaUdpRecvType nType);/*1¡¢headrecv return headlen

																																										  2¡¢datarecv return datalen*/





/*close all port */

_FC_INTER_DLL_EXPORT  int HwaEtherInterDeInit(THwaInterHandle nHandle); 



/*debug message */

_FC_INTER_DLL_EXPORT  int HwaEtherInterInitDebug (THwaInterHandle nHandle, int nDebugLevel,int nDebugFile); 



/*find the err*/

_FC_INTER_DLL_EXPORT char *HwaEtherInterErr (THwaInterHandle  nHandle, int nErrCmd);







#endif
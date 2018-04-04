#ifndef _HWA_FC_REG_H_
#define _HWA_FC_REG_H_

/*FPGA REG BASE ADDRSS */
#define FPGA_REG_BASE 0x0800
#define FPGA_REG(x) (x) + FPGA_REG_BASE

//FPGA reset CMD
#define RST_FPGA			0x1

//FPGA reset success flag
#define RST_FPGA_SUCC_MASK  0x1
#define RST_FPGA_SUCC		0x1

//IRIG-B mode
#define IRIG_MASTER 0x1
#define IRIG_SLAVER	0x0

//MIB clear CMD
#define CLEAR_MIB_RM_PORT_A		0x1
#define CLEAR_MIB_RM_PORT_B		0x2
#define CLAER_MIB_PORT_A		0x4
#define CLAER_MIB_PORT_B		0x8
#define CLEAR_MIB			0xffffffff
#define CLEAR_MIB_STOP		0x0


#ifdef OLD_VER
//TX Flag config bit 
//MSG type
#define TX_FLAG_MSG_TYPE_BIT			31  
#define TX_FLAG_MSG_TYPE_MASK			0x1 
//send port
#define TX_FLAG_PORT_SEL_BIT		29
#define TX_FLAG_PORT_SEL_MASK		0x3
//send PRI
#define TX_FLAG_PRI_BIT				22
#define TX_FLAG_PRI_MASK			0x3
//send port index
#define TX_FLAG_PORT_INDEX_BIT    13
#define TX_FLAG_PORT_INDEX_MASK		0x1ff
#else
//TX Flag config bit 
//MSG type
#define TX_FLAG_MSG_TYPE_BIT			0	  
#define TX_FLAG_MSG_TYPE_MASK			0x1 
//send port
#define TX_FLAG_PORT_SEL_BIT	1
#define TX_FLAG_PORT_SEL_MASK	0x3
//send PRI
#define TX_FLAG_PRI_BIT				8
#define TX_FLAG_PRI_MASK			0x3
//send port index
#define TX_FLAG_PORT_INDEX_BIT		0xa
#define TX_FLAG_PORT_INDEX_MASK		0x1ff
#endif
//RX flag config bit
//MSG Type
#define RX_FLAG_TYPE_BIT		0x2
#define RX_FLAG_TYPE_MASK		0x1
//RX channel
#define RX_FLAG_CH_BIT			0x0
#define RX_FLAG_CH_MASK			0x3

//send overflow flag

#ifdef NEW_TX_OVER_FORMAT
#define TX_OVER_FLOW_VAILD_BIT	0x3
#define TX_OVER_FLOW_VAILD_MASK	0x1
#define TX_OVER_FLOW_BIT		0x2
#define TX_OVER_FLOW_MASK		0x1
#define PORT_LINK_BIT			0x0
#define PORT_LINK_MASK			0x3
#define PORT_A_LINK_MASK		0x1
#define PORT_B_LINK_MASK		0x2
#else
#define TX_OVER_FLOW_VAILD_BIT	0x1
#define TX_OVER_FLOW_VAILD_MASK	0x1
#define TX_OVER_FLOW_BIT		0x0
#define TX_OVER_FLOW_MASK		0x1
#endif

#define TX_NOT_OVER_FLOW		0x0
#define TX_OVER_FLOW			0x1
#define PORT_NOT_LINK			0x2



//config done CMD
#define CFG_DONE		0x1
#define CFG_NOT_DONE	0x0

//recv INTR
#define RECV_INTR_ENABLE 0x1
#define RECV_INTR_DISABLE 0x0

//port config reset CMD
#define CFG_RST				0x1
#define CFG_STOP_RST		0x0
#define CFG_RST_BIT			0x9
#define CFG_RST_PORTID_MASK	0x1ff		
#define CFG_RST_PORTID_BIT	0x0

//time manager
#define TIME_DAYS_MASK 0x1ff
#define TIME_DAYS_BIT	0x5

#define TIME_HOURS_MASK 0x1f
#define TIME_HOURS_BIT  0x0

#define TIME_MIN_MASK  0x3f
#define TIME_MIN_BIT	26

#define TIME_SEC_MASK	0x3f
#define TIME_SEC_BIT	20

#define TIME_MSEC_MASK	0x3ff
#define TIME_MSEC_BIT	10

#define TIME_USEC_MASK	0x3ff
#define TIME_USEC_BIT	0



//port status
#define	PORT_BUSY	0x1
#define PORT_FREE	0x0

//fragment mask
#define FRG_MASK	0x03ff

#define TX_PAYLOAD_DATA_CNT		0x5
#define TX_PAYLOAD_SID_BIT		0x0 /*0-23*/
#define TX_PAYLOAD_SID_MASK		0xffffff
#define TX_PAYLOAD_DID_BIT		24 /*24-47*/
#define TX_PAYLOAD_DID_MASK		0xffffff
#define TX_PAYLOAD_TYPE_BIT		(48 % 32) /*48 ~55*/
#define TX_PAYLOAD_TYPE_MASK	0xff 
#define TX_PAYLOAD_MSGID_BIT	(56 %32) /*56~87*/
#define TX_PAYLOAD_MSGID_MASK	0xffffffff
#define TX_PAYLOAD_OXID_BIT		(88 %32) /*88~103*/
#define TX_PAYLOAD_OXID_MASK	0xffff
#define TX_PAYLOAD_SEQID_BIT	(104 % 32) /*104~111*/
#define TX_PAYLOAD_SEQID_MASK	0xff
#define TX_PAYLOAD_PRIOI_BIT	(112 % 32) /*112~119*/
#define TX_PAYLOAD_PRIOI_MASK	0xff
#define TX_PAYLOAD_CH_BIT		(120 %32) /*120~121*/
#define TX_PAYLOAD_CH_MASK		0x3
#define TX_PAYLOAD_MAXFRM_LEN_BIT	0	/*0~11*/
#define TX_PAYLOAD_MAXFRM_LEN_MASK	0xfff 

#define RXANA_DATA_CNT 0x5
#define RX_ANA_ENABLE_BIT	0 /*0*/
#define RX_ANA_ENABLE_MASK 0x1
#define RX_ANA_CH_SEL_BIT	1 /*1~2*/
#define RX_ANA_CH_SEL_MASK	0x3
#define RX_ANA_FRG_OFF_BIT	3 /*3*/
#define RX_ANA_FRG_OFF_MASK 0x1
#define RX_ANA_FRG_SEQ_BIT	4 /*4*/
#define RX_ANA_FRG_SEQ_MASK 0x1
#define RX_ANA_RES_BIT		5 /*5-7*/
#define RX_ANA_RES_MASK		0x7
#define RX_ANA_SID_BIT		8 /*8~31*/
#define RX_ANA_SID_MASK		0xffffff
#define RX_ANA_DID_BIT		(32 % 32) /*32~55*/
#define RX_ANA_DID_MASK		0xffffff
#define RX_ANA_TYPE_BIT		(56 % 32)/*56~63*/
#define RX_ANA_TYPE_MASK	0xff
#define RX_ANA_MSG_ID_BIT		(64 % 32)/*64~95*/
#define RX_ANA_MSG_ID_MASK		0xffffffff
#define RX_ANA_OXID_BIT		(96 % 32) /*96~111*/
#define RX_ANA_OXID_MASK		0xffff
#define RX_ANA_SEQID_BIT		(112 % 32) /*112~119*/
#define RX_ANA_SEQID_MASK		0xff

//receive data success
#define RECV_NOT_FIN	0x0
#define RECV_FIN		0x1

#define RECV_STOP		0x0
#define RECV_FROM_A		0x1
#define RECV_FROM_B		0x2
#define RECV_RM_AUTO	0x3

#define FPGA_REG_2G	0x1
#define FPGA_REG_1G	0x0

#define FC_START 0x1

#define LINK_NORMAL	0x1
#define LINK_ERROR	0x0


/*PCIE Control Reg*/
#define PCIE_REG_DCR			0x0000
//#define PCIE_DMA_RST			0x1
//#define PCIE_DMA_NRST			0x0

#define PCIE_DMA_RST			0x3
#define PCIE_DMA_NRST			0x0
#define PCIE_DMA_SEND_RST       0x1
#define PCIE_DMA_SEND_NRST      0x0
#define PCIE_DMA_RECV_RST       0x2
#define PCIE_DMA_RECV_NRST      0x0

#define PCIE_REG_DMA_INTR_CTL	0x0004
#define DMA_INTR_DISABLE_SEND		0x00800000
#define DMA_INTR_DISABLE_RECV		0x00000080
#define DMA_INTR_ENABLE				0x00000000
#define DMA_INTR_CLR_START			0x00000000

#define DMA_INTR_DISABLE	0x00000000
#define PCIE_REG_RDTLPA_W		0x001c
#define PCIE_REG_RDTLPS_W		0x0020
#define PCIE_REG_RDTLPC_W		0x0024
#define PCIE_REG_RDTLPA_R		0x0008
#define PCIE_REG_RDTLPS_R		0x000c
#define PCIE_REG_RDTLPC_R		0x0010
#define PCIE_REG_DMA_START		0x0004
#define PCIE_REG_DMA_DONE       0x0004
#define DMA_START_SEND		(0x00010000 | (0x1 << 23))
#ifndef RECV_INT
#define DMA_START_RECV		(0x00000001 | (0x1 << 7))
#else
#define DMA_START_RECV		(0x00000001)
#endif
/*interrupt control config*/
#define PCIE_REG_INTR_CLEAR		(0x0048)
#define PCIE_REG_INTR_STAT		(0x0048)
#define PCIE_CFG_REG_INTR_TYPE	0x0048
#define PCIE_INTR_TYPE_MSI_VAL	0x00836005
#define PCIE_INTR_TYPE_LEG_VAL	0x00826005
#define PCIE_CFG_REG_INTR_LINE	0x3C
#define PCIE_INTR_LINE_UNKNOW	0xff
#define PCIE_REG_RECV_RATE		0x28
#define PCIE_REG_SEND_RATE		0x2C
#define PCIE_REG_RECV_REAL_LEN  0x50
#define PCIE_REG_DMA_RECV_DONE		0x005c
#define PCIE_REG_DMA_SEND_DONE		0x0060

#define PCIE_REG_DMA_DONE		0x0004
#define PCIE_DMA_SEND_DONE	(0x1 << 24)
#define PCIE_DMA_RECV_DONE		(0x1 << 8)

#define PCIE_REG_RECV_INTR_DISABLE 0x54
#define PCIE_ENABLE_RECV_INTR	0x0
#define PCIE_DISABLE_RECV_INTR	0x1

#define PCIE_REG_DEVICE_STAT	0x0
#define PCIE_DEVICE_STATUS		0x14021600


/*Hwa FC NIC card Config file Define*/
/*Header define*/
#define HWA_FC_NIC				"HwaFcNic"
#define HWA_FC_CREAT_TIME		"CreateTime"
/*Card Config*/
#define HWA_FC_CARD_CFG			"CardConfig"
#define HWA_CARD_WORK_MODE		"WorkMode"
#define CARD_MODE_CAP			"Capture"
#define CARD_MODE_NORMAL		"Normal"
#define	HWA_CARD_SPEED			"Speed"
#define CARD_SPEED_1G			"1G"
#define CARD_SPEED_2G			"2G"
#define HWA_CARD_IRIG_MODE		"IRIG-BMode"
#define CARD_MODE_MASTER		"Master"
#define CARD_MODE_SLAVE			"Slave"
/*Hwa FC Card Port Cfg*/
#define PORT_CFG				"PortConfig"
#define FC_PORT_HEADER			"FCPort"
#define FC_PORT_ID				"PortID"
#define FC_PORT_TYPE			"PortType"
#define PORT_TYPE_NOR			"Normal"
#define PORT_TYPE_BLOCK			"Block"
#define PORT_TYPE_SPE			"Special"
#define FC_PORT_ENABLE			"PortEnable"
#define PORT_ENABLE				"Enable"
#define PORT_DISABLE			"Disable"
#define FC_PORT_DIR				"PortDir"
#define PORT_DIR_SEND			"Send"
#define PORT_DIR_RECV			"Recv"
#define FC_PORT_SID				"S_ID"
#define FC_PORT_DID				"D_ID"
#define FC_PORT_OXID			"OX_ID"
#define FC_PORT_MSG_TYPE		"MsgType"
#define MSG_TYPE_ASM			"ASM"
#define FC_PORT_MSG_ID			"MessageID"
#define FC_PORT_SEQ_ID			"SequenceID"
#define FC_PORT_PRIO			"Prioity"
#define FC_PORT_CHANNEL			"Channel"
#define CHANNEL_A			"A"
#define CHANNEL_B			"B"
#define CHANNEL_AB			"AB"
#define FC_PORT_MAX_FRM			"MaxFrameSize"
#define FC_PORT_FRAG_SEQ		"FragSeq"
#define FC_PORT_FRAG_OFF		"FragOffset"

#define MSG_ASM 0x49



#define FPGA_REG_device_soft_reset			FPGA_REG(0x0000)
#define FPGA_REG_device_version				FPGA_REG(0x0004)
#define FPGA_REG_device_state				FPGA_REG(0x0008)	
#define FPGA_REG_work_mode					FPGA_REG(0x000C)	
#define FPGA_REG_host_time1					FPGA_REG(0x0014)	
#define FPGA_REG_host_time2					FPGA_REG(0x0018)
#define FPGA_REG_IRIG_B_set					FPGA_REG(0x001C)
#define FPGA_REG_reset_ctrl					FPGA_REG(0x0020)
#define FPGA_REG_mibs_clear					FPGA_REG(0x0024)	
#define FPGA_REG_D_ID						FPGA_REG(0x0028)	
#define FPGA_REG_TX_flag					FPGA_REG(0x0030)	
/*
Res
#define FPGA_REG_TX_port_id					FPGA_REG(0x0030)	
*/
#define FPGA_REG_TX_frame_len				FPGA_REG(0x0034)
#define FPGA_REG_Cfg_done					FPGA_REG(0x0048)				
#define FPGA_REG_port_id_clr				FPGA_REG(0x0060)	
#define FPGA_REG_busy_reset_port			FPGA_REG(0x0064)	
#define FPGA_REG_TX_buf_overflow			FPGA_REG(0x0070)	
#define FPGA_REG_TX_frame_num				FPGA_REG(0x0074)
#define FPGA_REG_loopback_en				FPGA_REG(0x0080)
#define FPGA_REG_TX_buf_overflow_port_id	FPGA_REG(0x0090)
#define FPGA_REG_TX_buf_overflow_num		FPGA_REG(0x0094)
#define FPGA_REG_RX_buf_overflow_port_id	FPGA_REG(0x0098)
#define FPGA_REG_RX_buf_overflow_num		FPGA_REG(0x009C)
#define FPGA_REG_secure_rd_addr				FPGA_REG(0x00A0)
#define FPGA_REG_secure_rd_data				FPGA_REG(0x00AC)	
#define FPGA_REG_secure_wr_addr				FPGA_REG(0x00A4)	
#define FPGA_REG_secure_wr_data				FPGA_REG(0x00A8)	
/*IO trigger config*/
#define FPGA_REG_get_in_io_addr				FPGA_REG(0x00B8)
#define FPGA_REG_set_out_io_addr			FPGA_REG(0x00BC)

#define FPGA_REG_fragment_cs_addr			FPGA_REG(0x0200)
#define FPGA_REG_fragment_cs_data			FPGA_REG(0x0204)				
#define FPGA_REG_TXpayload_cs_addr			FPGA_REG(0x0400)
#define FPGA_REG_TXpayload_cs_data(x)		FPGA_REG(0x0404 + (x) * 4)	
#define FPGA_REG_intr_RX_port_id			FPGA_REG(0x0800)
#define FPGA_REG_RX_port_id					FPGA_REG(0x0804)	
#define FPGA_REG_RX_rec_frame				FPGA_REG(0x0808)
#define FPGA_REG_intr_RX_frame_en			FPGA_REG(0x0814)
#define FPGA_REG_RX_intr_poll_state			FPGA_REG(0x0810)
#define FPGA_REG_RX_intr_poll_set			FPGA_REG(0x0810)
#define FPGA_REG_RX_check_valied			FPGA_REG(0x0830)
#define FPGA_REG_RX_rec_state				FPGA_REG(0x081C)
#define FPGA_REG_RX_flag					FPGA_REG(0x083C)
#define FPGA_REG_RX_frame_len				FPGA_REG(0x0840)
#define FPGA_REG_RX_frame_drop				FPGA_REG(0x0844)
#define FPGA_REG_RX_rec_time1				FPGA_REG(0x0848)
#define FPGA_REG_RX_rec_time2				FPGA_REG(0x084C)
#define FPGA_REG_Rx_intr_cnt                FPGA_REG(0x0868)   //0x868 intr_cnt

#define FPGA_REG_Rx_rec_Mode                FPGA_REG(0x0038)
#define FPGA_REG_RX_intr_BlockMode_DataLen  FPGA_REG(0x0828)  
#define FPGA_REG_RX_intr_BlockMode_PtkCnt   FPGA_REG(0x0824)
/*
  0x0038 add by sxy 20130416_for normal blockRecv:  normal mode recv mode is single packet(0) or block packet(1)
  0x0828 add by sxy 20130416_for normal blockRecv:  read length every normal blocked recv
  0x0824 add by sxy 20130416_for normal blockRecv:  blocked recv should recv packet num
*/

#define FPGA_REG_Rm_mode					FPGA_REG(0x1000)
#define FPGA_REG_NodeType					FPGA_REG(0x1080)
#define FPGA_REG_RX_frame_crc_err			FPGA_REG(0x1030)	
#define FPGA_REG_RX_frame_short_err			FPGA_REG(0x1034)	
#define FPGA_REG_RX_frame_long_err			FPGA_REG(0x1038)
#define FPGA_REG_RX_frame_err				FPGA_REG(0x103C)
#define FPGA_REG_FC_speed					FPGA_REG(0x1044)
#define FPGA_REG_FC_start					FPGA_REG(0x1040)	
#define FPGA_REG_FC_status					FPGA_REG(0x1048)
#define FPGA_REG_FC_tov						FPGA_REG(0x104C)	
#define FPGA_REG_FC_bb_credit				FPGA_REG(0x1050)	
#define FPGA_REG_FC_loc_sel					FPGA_REG(0x1060)
#define FPGA_REG_FC_Manage_addr				FPGA_REG(0x1064)
#define FPGA_REG_FC_Manage_datao			FPGA_REG(0x1068)
#define FPGA_REG_FC_Manage_datai			FPGA_REG(0x106C)	
#define FPGA_REG_RXanalyze_cs_addr			FPGA_REG(0x2000)
#define FPGA_REG_RXanalyze_cs_data(x)		FPGA_REG(0x2004 + (x)*4) 			
#define FPGA_REG_FCA_MIB_addr				FPGA_REG(0x2020)
#define FPGA_REG_FCA_MIB_data				FPGA_REG(0x2024)
#define FPGA_REG_FCB_MIB_addr				FPGA_REG(0x2028)
#define FPGA_REG_FCB_MIB_data				FPGA_REG(0x202C)
#define FPGA_REG_RX_MIB_addr				FPGA_REG(0x2030)
#define FPGA_REG_RX_MIB_data				FPGA_REG(0x2034)
#define FPGA_REG_TX_MIB_addr				FPGA_REG(0x2038)
#define FPGA_REG_TX_MIB_data				FPGA_REG(0x203C)


/*capture mode regs*/
#define FPGA_REG_Mon_work_mode				FPGA_REG(0x000C)
#define FPGA_REG_Mon_intr_rx_timedout		FPGA_REG(0x007c)
#define FPGA_REG_Mon_intr_rx_threshold		FPGA_REG(0x0078)
#define FPGA_REG_Mon_RX_START				FPGA_REG(0x0074)
#define MON_START	0x1	
#define MON_STOP	0x0
#define FPGA_REG_Mon_rx_rec_state			FPGA_REG(0x081C)
#define FPGA_REG_Mon_rx_flag				FPGA_REG(0x083C)
#define MON_DATA_REAL		0x1
#define MON_DATA_NOT_REAL	0x0
#define FPGA_REG_Mon_rx_total_len			FPGA_REG(0x840)



/*FC gen mode*/
#define FPGA_REG_fc_gen_tx_cycle			FPGA_REG(0x1400)
#define FPGA_REG_fc_gen_tx_times			FPGA_REG(0x1404)
#define FPGA_REG_fc_gen_tx_length			FPGA_REG(0x1408)
#define FPGA_REG_fc_gen_tx_start			FPGA_REG(0x140c)
#define FPGA_REG_fc_gen_tx_data_addr		FPGA_REG(0x1410)
#define FPGA_REG_fc_gen_tx_data				FPGA_REG(0x1414)

#endif




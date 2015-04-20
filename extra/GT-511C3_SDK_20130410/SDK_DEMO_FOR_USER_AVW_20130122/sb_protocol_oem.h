#ifndef __SB_PROTOCOL_OEM_H__
#define __SB_PROTOCOL_OEM_H__

// Header Of Cmd and Ack Packets
#define STX1				0x55	//Header1 
#define STX2				0xAA	//Header2

// Header Of Data Packet
#define STX3				0x5A	//Header1 
#define STX4				0xA5	//Header2

// Structure Of Cmd and Ack Packets 
typedef struct {		
	BYTE 	Head1;		
	BYTE 	Head2;		
	WORD	wDevId;
	int		nParam;
	WORD	wCmd;// or nAck
	WORD 	wChkSum;
} SB_OEM_PKT;			

#define SB_OEM_PKT_SIZE			12
#define SB_OEM_HEADER_SIZE		2
#define SB_OEM_DEV_ID_SIZE		2
#define SB_OEM_CHK_SUM_SIZE		2

#define PKT_ERR_START	-500
#define PKT_COMM_ERR	PKT_ERR_START+1
#define PKT_HDR_ERR		PKT_ERR_START+2
#define PKT_DEV_ID_ERR	PKT_ERR_START+3
#define PKT_CHK_SUM_ERR	PKT_ERR_START+4
#define PKT_PARAM_ERR	PKT_ERR_START+5

int oemp_CheckCmdAckPkt( WORD wDevID, SB_OEM_PKT* pPkt );
int oemp_SendCmdOrAck( WORD wDevID, WORD wCmdOrAck, int nParam );
int oemp_ReceiveCmdOrAck( WORD wDevID, WORD* pwCmdOrAck, int* pnParam );
int oemp_SendData( WORD wDevID, BYTE* pBuf, int nSize );
int oemp_ReceiveData( WORD wDevID, BYTE* pBuf, int nSize );

#endif /*__SB_PROTOCOL_OEM_H__*/

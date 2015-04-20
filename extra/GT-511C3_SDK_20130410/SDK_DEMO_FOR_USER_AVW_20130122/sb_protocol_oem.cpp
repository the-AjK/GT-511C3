///////////     Insert the code     ///////////
#include "stdafx.h"
#include "OEM.h"

///////////////////////////////////////////////
#define COMM_DEF_TIMEOUT 15000
DWORD gCommTimeOut = COMM_DEF_TIMEOUT;

WORD oemp_CalcChkSumOfCmdAckPkt( SB_OEM_PKT* pPkt )
{
	WORD wChkSum = 0;
	BYTE* pBuf = (BYTE*)pPkt;
	int i;
	
	for(i=0;i<(sizeof(SB_OEM_PKT)-SB_OEM_CHK_SUM_SIZE);i++)
		wChkSum += pBuf[i];
	return wChkSum;
}

WORD oemp_CalcChkSumOfDataPkt( BYTE* pDataPkt, int nSize )
{
	int i;
	WORD wChkSum = 0;
	BYTE* pBuf = (BYTE*)pDataPkt;
	
	for(i=0;i<nSize;i++)
		wChkSum += pBuf[i];
	return wChkSum;
}

int oemp_CheckCmdAckPkt( WORD wDevID, SB_OEM_PKT* pPkt )
{
	if( ( pPkt->Head1 != STX1 ) || 
		( pPkt->Head2 != STX2 ) )
	{
		return PKT_HDR_ERR;
	}
	
	if( pPkt->wDevId != wDevID ) 
		return PKT_DEV_ID_ERR;
	
	if( pPkt->wChkSum != oemp_CalcChkSumOfCmdAckPkt( pPkt ) ) 
		return PKT_CHK_SUM_ERR;

	return 0;
}

int oemp_SendCmdOrAck( WORD wDevID, WORD wCmdOrAck, int nParam )
{
	SB_OEM_PKT pkt;
	int nSentBytes;

	pkt.Head1 = (BYTE)STX1;
	pkt.Head2 = (BYTE)STX2;
	pkt.wDevId = wDevID;
	pkt.wCmd = wCmdOrAck;
	pkt.nParam = nParam;
	pkt.wChkSum = oemp_CalcChkSumOfCmdAckPkt( &pkt );

	nSentBytes = comm_send( (BYTE*)&pkt, SB_OEM_PKT_SIZE, gCommTimeOut );
	if( nSentBytes != SB_OEM_PKT_SIZE )
		return PKT_COMM_ERR;

	return 0;
}

int oemp_ReceiveCmdOrAck( WORD wDevID, WORD* pwCmdOrAck, int* pnParam )
{
	SB_OEM_PKT pkt;
	int nReceivedBytes;
		
	if( ( pwCmdOrAck == NULL ) || 
		( pnParam == NULL ) )
	{
		return PKT_PARAM_ERR;
	}

	nReceivedBytes = comm_recv( (BYTE*)&pkt, SB_OEM_PKT_SIZE, gCommTimeOut );
	if( nReceivedBytes != SB_OEM_PKT_SIZE )
		return PKT_COMM_ERR;

	if( ( pkt.Head1 != STX1 ) || 
		( pkt.Head2 != STX2 ) )
	{
		return PKT_HDR_ERR;
	}

	if( pkt.wDevId != wDevID ) 
		return PKT_DEV_ID_ERR;

	if( pkt.wChkSum != oemp_CalcChkSumOfCmdAckPkt( &pkt ) ) 
		return PKT_CHK_SUM_ERR;
	
	*pwCmdOrAck = pkt.wCmd;
	*pnParam = pkt.nParam;
	
	return 0;
}

int oemp_SendData( WORD wDevID, BYTE* pBuf, int nSize )
{
	WORD wChkSum = 0;
	BYTE Buf[4], *pCommBuf;
	int nSentBytes;
	
	if( pBuf == NULL )
		return PKT_PARAM_ERR;

	Buf[0] = (BYTE)STX3;
	Buf[1] = (BYTE)STX4;
	*((WORD*)(&Buf[SB_OEM_HEADER_SIZE])) = wDevID;
	
	wChkSum = oemp_CalcChkSumOfDataPkt( Buf, SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE  );
	wChkSum += oemp_CalcChkSumOfDataPkt( pBuf, nSize );
	
	//nSentBytes = comm_send( Buf, SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE, gCommTimeOut  );
	//if( nSentBytes != SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE )
	//	return PKT_COMM_ERR;
	//
	//nSentBytes = comm_send( pBuf, nSize, gCommTimeOut );
	//if( nSentBytes != nSize )
	//	return PKT_COMM_ERR;
	//
	//nSentBytes = comm_send( (BYTE*)&wChkSum, SB_OEM_CHK_SUM_SIZE, gCommTimeOut );
	//if( nSentBytes != SB_OEM_CHK_SUM_SIZE )
	//	return PKT_COMM_ERR;

	////////////// pc start ///////////////
	pCommBuf = new BYTE[nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE];
	memcpy(pCommBuf, Buf, SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE);
	memcpy(pCommBuf+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE, pBuf, nSize);
	*(WORD*)(pCommBuf+nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE) = wChkSum;

	nSentBytes = comm_send( pCommBuf, nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE, gCommTimeOut );
	if( nSentBytes != nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE )
	{
		if(pCommBuf)
			delete pCommBuf;
		return PKT_COMM_ERR;
	}

	if(pCommBuf)
		delete pCommBuf;
	////////////// pc end ///////////////

	return 0;
}

int oemp_ReceiveData( WORD wDevID, BYTE* pBuf, int nSize )
{
	WORD wReceivedChkSum, wChkSum;
	BYTE Buf[4],*pCommBuf;
	int nReceivedBytes;
	
	if( pBuf == NULL )
		return PKT_PARAM_ERR;


	/*AVW modify*/
	pCommBuf = new BYTE[nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE];
	nReceivedBytes = comm_recv( pCommBuf, nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE, gCommTimeOut );
	if( nReceivedBytes != nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE+SB_OEM_CHK_SUM_SIZE )
	{
		if(pCommBuf)
			delete pCommBuf;
		return PKT_COMM_ERR;
	}
	memcpy(Buf, pCommBuf, SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE);
	memcpy(pBuf, pCommBuf+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE, nSize);
	wReceivedChkSum = *(WORD*)(pCommBuf+nSize+SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE);
	if(pCommBuf)
		delete pCommBuf;
	////////////// pc end ///////////////

	if( ( Buf[0] != STX3 ) || 
		( Buf[1] != STX4 ) )
	{
		return PKT_HDR_ERR;
	}
	
	if( *((WORD*)(&Buf[SB_OEM_HEADER_SIZE])) != wDevID ) 
		return PKT_DEV_ID_ERR;
	
	wChkSum = oemp_CalcChkSumOfDataPkt( Buf, SB_OEM_HEADER_SIZE+SB_OEM_DEV_ID_SIZE  );
	wChkSum += oemp_CalcChkSumOfDataPkt( pBuf, nSize );
	
	if( wChkSum != wReceivedChkSum ) 
		return PKT_CHK_SUM_ERR;
	/*AVW modify*/
	return 0;
}

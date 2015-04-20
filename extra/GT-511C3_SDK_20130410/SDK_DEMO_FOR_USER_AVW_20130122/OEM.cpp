//SHW080926

#include "stdafx.h"
#include "OEM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
BYTE	gbyImg8bit[IMG8BIT_SIZE];
BYTE	gbyImgRaw[320*240];
BYTE	gbyTemplate[FP_TEMPLATE_SIZE];

WORD gwDevID = 1;
WORD gwLastAck = 0;
int  gwLastAckParam = 0;
int  gnPassedTime = 0;

devinfo gDevInfo;

//////////////////////////////////////////////////////////////////////////
int oem_CommandRun(WORD wCmd, int nCmdParam)
{
	if( oemp_SendCmdOrAck( gwDevID, wCmd, nCmdParam ) < 0 )
		return OEM_COMM_ERR;
	gnPassedTime = GetTickCount();
	if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 )
		return OEM_COMM_ERR;
	gnPassedTime = GetTickCount() - gnPassedTime;
	return 0;
}

int oem_open( void )
{
	if (oem_CommandRun( CMD_OPEN, 1 ) < 0 )
		return OEM_COMM_ERR;

	if( oemp_ReceiveData( gwDevID, (BYTE*)&gDevInfo, sizeof(devinfo)) < 0 )
		return OEM_COMM_ERR;

	return 0;
}

int oem_close( void )
{
	return oem_CommandRun( CMD_CLOSE, 0);
}

extern DWORD gCommTimeOut;
int oem_usb_internal_check( void ){
	
	DWORD prevCommTimeOut = gCommTimeOut;
	gCommTimeOut = 1000;
	
	int ret = oem_CommandRun( CMD_USB_INTERNAL_CHECK, gwDevID );
	
	gCommTimeOut = prevCommTimeOut;
	return ret;
}

int oem_change_baudrate( int nBaudrate )
{
	return oem_CommandRun( CMD_CHANGE_BAUDRATE, nBaudrate );
}

int oem_cmos_led( BOOL bOn )
{
	return oem_CommandRun( CMD_CMOS_LED, bOn ? 1 : 0 );
}

int oem_enroll_count( void )
{
	return oem_CommandRun( CMD_ENROLL_COUNT, 0 );
}

int oem_check_enrolled( int nPos )
{
	return oem_CommandRun( CMD_CHECK_ENROLLED, nPos );
}

int oem_enroll_start( int nPos )
{
	return oem_CommandRun( CMD_ENROLL_START, nPos );
}

/*AVW*/
int oem_enroll_nth( int nPos, int nTurn )
{
	if( oem_CommandRun( CMD_ENROLL_START+nTurn, 0 ) < 0 )
		return OEM_COMM_ERR;

	if( nPos == -1 && nTurn == 3)
	{
		if(gwLastAck == ACK_OK)
		{
			if( oemp_ReceiveData( gwDevID, &gbyTemplate[0], FP_TEMPLATE_SIZE ) < 0 )
				return OEM_COMM_ERR;
		}
	}

	return 0;
}

int oem_is_press_finger( void )
{
	return oem_CommandRun( CMD_IS_PRESS_FINGER, 0 );
}

int oem_delete( int nPos )
{
	return oem_CommandRun( CMD_DELETE, nPos );
}

int oem_delete_all( void )
{
	return oem_CommandRun( CMD_DELETE_ALL, 0 );
}

int oem_verify( int nPos )
{
	return oem_CommandRun( CMD_VERIFY, nPos );
}

int oem_identify( void )
{
	return oem_CommandRun( CMD_IDENTIFY, 0 );
}

int oem_verify_template( int nPos )
{
	if( oem_CommandRun( CMD_VERIFY_TEMPLATE, nPos ) < 0 )
		return OEM_COMM_ERR;
	
	if(gwLastAck == ACK_OK)
	{
		if( oemp_SendData( gwDevID, &gbyTemplate[0], FP_TEMPLATE_SIZE ) < 0 )
			return OEM_COMM_ERR;
		
		gnPassedTime = GetTickCount();
		if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 )
			return OEM_COMM_ERR;
		gnPassedTime = GetTickCount() - gnPassedTime;
	}
	
	return 0;
}

int oem_identify_template( void )
{
	if( oem_CommandRun( CMD_IDENTIFY_TEMPLATE, 0 ) < 0 )
		return OEM_COMM_ERR;
	
	if(gwLastAck == ACK_OK)
	{
		if( oemp_SendData( gwDevID, &gbyTemplate[0], FP_TEMPLATE_SIZE ) < 0 )
			return OEM_COMM_ERR;
		
		gnPassedTime = GetTickCount();
		if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 )
			return OEM_COMM_ERR;
		gnPassedTime = GetTickCount() - gnPassedTime;
	}
	
	return 0;
}

int oem_capture( BOOL bBest )
{
	return oem_CommandRun( CMD_CAPTURE, bBest );
}

/*AVW*/
static BYTE	gbyImg256_2[202*258];
static BYTE	gbyImg256_tmp[258*202];

int oem_get_image( void )
{
	if( oem_CommandRun( CMD_GET_IMAGE, 0 ) < 0 )
		return OEM_COMM_ERR;
	
	if( oemp_ReceiveData( gwDevID, gbyImg256_tmp, sizeof gbyImg256_tmp ) < 0 )
		return OEM_COMM_ERR;
	
	// image rotate
	int i, j;

	for( i=0; i<202; i++)
	{
		for( j=0; j<258; j++)
		{
			gbyImg256_2[i*258+j] = gbyImg256_tmp[j*202+i];
		}
	}
	
	memset(gbyImg8bit, 161, sizeof(gbyImg8bit));
	
	for (i=0; i<202; i++)
	{
		memcpy(&gbyImg8bit[256*(27 + i) + 0], &gbyImg256_2[i * 258 + 1], 256);
	}
	
	return 0;
}

/*AVW*/
static BYTE	gbyImgRaw2[240*320/4];

int oem_get_rawimage( void )
{
	if( oem_CommandRun( CMD_GET_RAWIMAGE, 0 ) < 0 )
		return OEM_COMM_ERR;
	
	if( oemp_ReceiveData( gwDevID, gbyImgRaw2, sizeof gbyImgRaw2 ) < 0 )
		return OEM_COMM_ERR;

	/*AVW*/
	memset(gbyImgRaw, 66, sizeof gbyImgRaw);
	int i, j;
	for (i=0; i<120; i++)
	{
		for(j=0; j< 160; j++)
		{
			gbyImgRaw[320*(2*i+0)+(2*j+0)] = gbyImgRaw2[i*160+j];
			gbyImgRaw[320*(2*i+0)+(2*j+1)] = gbyImgRaw2[i*160+j];
			gbyImgRaw[320*(2*i+1)+(2*j+0)] = gbyImgRaw2[i*160+j];
			gbyImgRaw[320*(2*i+1)+(2*j+1)] = gbyImgRaw2[i*160+j];
		}		
	}

	return 0;
}

int oem_get_template( int nPos )
{
	if( oem_CommandRun( CMD_GET_TEMPLATE, nPos ) < 0 )
		return OEM_COMM_ERR;

	if(gwLastAck == ACK_OK)
	{
		if( oemp_ReceiveData( gwDevID, &gbyTemplate[0], FP_TEMPLATE_SIZE ) < 0 )
			return OEM_COMM_ERR;
	}
	
	return 0;
}

int oem_add_template( int nPos)
{
	if( oem_CommandRun( CMD_ADD_TEMPLATE, nPos ) < 0 )
		return OEM_COMM_ERR;
	
	if(gwLastAck == ACK_OK)
	{
		if( oemp_SendData( gwDevID, &gbyTemplate[0], FP_TEMPLATE_SIZE ) < 0 )
			return OEM_COMM_ERR;
		
		if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 )
			return OEM_COMM_ERR;
	}
	
	return 0;
}

int oem_get_database_end( void ){
	return oem_CommandRun( CMD_GET_DATABASE_END, 0 );
}

int oem_get_database_start( void ){
	return oem_CommandRun( CMD_GET_DATABASE_START, 0 );
}

int oem_fw_upgrade( BYTE* pBuf, int nLen, CProgressCtrl *pProgress )
{
	if( oem_CommandRun( CMD_FW_UPDATE, nLen ) < 0 )
		return OEM_COMM_ERR;
	int nSegSize = 0;
	int cbWrote = 0;
	if(gwLastAck == ACK_OK)
	{
		nSegSize = gwLastAckParam;	// flash segment size that's returned with ack.
		if( nSegSize <= 0 ){
			return OEM_COMM_ERR;
		}

		/* send firmware */	
		int nPrevSegSize = 0;
		int cbPrevWrote = 0;
		while (cbWrote < nLen)
		{
			if (cbWrote + nSegSize > nLen) nSegSize = nLen - cbWrote;
			
			if( oemp_SendData( gwDevID, &pBuf[cbWrote], nSegSize) < 0 )
				return OEM_COMM_ERR;
			if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 ){
					return OEM_COMM_ERR;
			}
			cbWrote += nSegSize;
			if( pProgress ){
				pProgress->SetPos( cbWrote );
				ui_polling();
			}
		}
	}
	return 0;
}

int oem_iso_upgrade( BYTE* pBuf, int nLen, CProgressCtrl *pProgress )
{
	if( oem_CommandRun( CMD_ISO_UPDATE, nLen ) < 0 )
		return OEM_COMM_ERR;
	int nSegSize = 0;
	int cbWrote = 0;
	if(gwLastAck == ACK_OK)
	{
		nSegSize = gwLastAckParam;	// flash segment size that's returned with ack.
		if( nSegSize <= 0 ){
			return OEM_COMM_ERR;
		}
		
		/* send firmware */	
		int nPrevSegSize = 0;
		int cbPrevWrote = 0;
		while (cbWrote < nLen)
		{
			if (cbWrote + nSegSize > nLen) nSegSize = nLen - cbWrote;
			
			if( oemp_SendData( gwDevID, &pBuf[cbWrote], nSegSize) < 0 )
				return OEM_COMM_ERR;
			if( oemp_ReceiveCmdOrAck( gwDevID, &gwLastAck, &gwLastAckParam ) < 0 ){
				return OEM_COMM_ERR;
			}
			cbWrote += nSegSize;
			if( pProgress ){
				pProgress->SetPos( cbWrote );
				ui_polling();
			}
		}
	}
	return 0;
}

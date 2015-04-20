//SHW080926

#include "stdafx.h"
#include "OEM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
void debug_printf(char *fmt,...)
{
	static char szLine[512] = "";
	
    va_list ap;
    char szTemp[256];
    va_start(ap, fmt);
    vsprintf(szTemp, fmt, ap);
    strcat(szLine, szTemp);
	
	int len = strlen(szLine);
	if (len >= 2 && strcmp(szLine + len - 2, "\r\n") == 0)
	{
		szLine[len - 2] = 0;
		OutputDebugStringA(szLine);
		szLine[0] = 0;
	}
	
    va_end(ap);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
class CCommSerial  
{
public:
	CCommSerial();
	virtual ~CCommSerial();
	
	BOOL Open(int nPort, DWORD dwBaudrate);
	void Close();
	
	int SendData(BYTE *pBuf, int nSize, DWORD dwTimeOut);
	int RecvData(BYTE *pBuf, int nSize, DWORD dwTimeOut);
	
private:
	HANDLE m_hComm;
};

CCommSerial::CCommSerial()
{
	m_hComm = INVALID_HANDLE_VALUE;
}

CCommSerial::~CCommSerial()
{
	Close();
}

BOOL CCommSerial::Open(int nPort, DWORD dwBaudrate)
{
	Close();

	char szComName[20];
	sprintf( szComName, "COM%d", nPort );

	m_hComm = CreateFileA( szComName, GENERIC_READ | GENERIC_WRITE,0,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL );
	if(m_hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//
	DCB dcb;
	memset(&dcb, 0, sizeof(DCB));

	dcb.DCBlength = sizeof(DCB) ;
	
	dcb.BaudRate = dwBaudrate;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;

	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	dcb.fAbortOnError = TRUE;
	
	if(!SetCommState( m_hComm, &dcb ))
		return FALSE;

	//
	COMMTIMEOUTS to;
	to.ReadIntervalTimeout = 0;
	to.ReadTotalTimeoutMultiplier = 0;
	to.ReadTotalTimeoutConstant = 16;
	to.WriteTotalTimeoutMultiplier = 0;
	to.WriteTotalTimeoutConstant = 16;

	if(!SetCommTimeouts(m_hComm, &to))
		return FALSE;

	return TRUE;
}

void CCommSerial::Close()
{
	if ( m_hComm != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hComm) ;
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

int CCommSerial::SendData(BYTE *pBuf, int nSize, DWORD dwTimeOut)
{
	int nReqSize = nSize;
	int nWrittenSize = 0;
	
	DWORD dwStartTime = GetTickCount();
	while (nSize && GetTickCount() - dwStartTime < dwTimeOut)
	{
		DWORD nCurrentWrittenSize;
		BOOL bWrite = WriteFile(m_hComm, pBuf, nSize, &nCurrentWrittenSize, NULL);
				
		if (!bWrite)
			break;
		FlushFileBuffers( m_hComm );
		pBuf += nCurrentWrittenSize;
		nSize -= nCurrentWrittenSize;
		nWrittenSize += nCurrentWrittenSize;
		
		if (nCurrentWrittenSize)
			dwStartTime = GetTickCount();

		comm_percent = (double)nWrittenSize / (double)nReqSize * 100.0;
		ui_polling();
	}

	return nWrittenSize;
}


int CCommSerial::RecvData(BYTE *pBuf, int nSize, DWORD dwTimeOut)
{
	int nReqSize = nSize;
	int nReadSize = 0;

	DWORD dwStartTime = GetTickCount();
	while (nSize && GetTickCount() - dwStartTime < dwTimeOut)
	{
		DWORD nCurrentReadSize;
		BOOL bRead = ReadFile(m_hComm, pBuf, nSize, &nCurrentReadSize, NULL);

		if (!bRead)
			break;
		pBuf += nCurrentReadSize;
		nSize -= nCurrentReadSize;
		nReadSize += nCurrentReadSize;

		if (nCurrentReadSize)
			dwStartTime = GetTickCount();
	
		comm_percent = (double)nReadSize / (double)nReqSize * 100.0;
		ui_polling();
	}

	return nReadSize;
}


/************************************************************************/
/*      CCommUSB definition                                             */
/************************************************************************/
CCommUSB::CCommUSB()
{
	m_hDevice = INVALID_HANDLE_VALUE;
}

CCommUSB::~CCommUSB()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////
#define USB_DEVICE_MARK 0x55
BOOL _commex_check_usb()
{
	if( oem_usb_internal_check() < 0 )
		return FALSE;

	if(gwLastAck == NACK_INFO)
		return FALSE;
	
	return USB_DEVICE_MARK == gwLastAckParam;
}

BOOL CCommUSB::SCSI_Open()
{
	TCHAR dosDrive[8] = _T("C:");
	TCHAR ddkDrive[8] = _T("\\\\.\\C:");
	UINT DriveType;
	
// 	TCHAR szVolume[256];

	for (TCHAR Drive=_T('C');Drive<=_T('Z');Drive++) 
	{
		dosDrive[0] = Drive;
		DriveType= GetDriveType(dosDrive); 
		
		if(DriveType==DRIVE_REMOVABLE || DriveType==DRIVE_CDROM)//
		{ 
			ddkDrive[4] = Drive;
// 			if( !GetVolumeInformation(dosDrive, szVolume, 255, NULL, 
// 				NULL, NULL, NULL, 0 ) ){
// 				continue;
// 			}
			
			m_hDevice = CreateFile(ddkDrive,
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);	
			
			if(m_hDevice==INVALID_HANDLE_VALUE) continue;
			if(_commex_check_usb()) 
			{
				return TRUE;
			}
		}
	}
	
	return FALSE; 	
}

void CCommUSB::SCSI_Close()
{
	if (m_hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDevice);
	m_hDevice = INVALID_HANDLE_VALUE;
}

#define USB_BLOCK_SIZE 65536

int CCommUSB::SCSI_Operation(BYTE *pBuf, int nSize, DWORD dwTimeOut, BOOL bRead)
{
	BYTE* pDataTemp = (BYTE*)pBuf;
	int nSizeTemp = nSize, nSizeReal;
	
	while(nSizeTemp)
	{
		nSizeReal = nSizeTemp;
		if (nSizeReal > USB_BLOCK_SIZE)
			nSizeReal = USB_BLOCK_SIZE;
		
		if (!OperationInternal(pDataTemp, nSizeReal, dwTimeOut, bRead) == nSizeReal)
			break;
		
		pDataTemp += nSizeReal;
		nSizeTemp -= nSizeReal;
	}
	
	if (nSizeTemp == 0)
		return nSize;
	return 0;
}

int CCommUSB::OperationInternal(BYTE *pBuf, int nSize, DWORD dwTimeOut, BOOL bRead)
{
	if (m_hDevice == INVALID_HANDLE_VALUE) 
		return FALSE;
	
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
    ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER)); 	
	
    sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.PathId = 0;
    sptdwb.sptd.TargetId = 1;
    sptdwb.sptd.Lun = 0;
    sptdwb.sptd.CdbLength = CDB10GENERIC_LENGTH;
    sptdwb.sptd.SenseInfoLength = 0;
    sptdwb.sptd.DataIn = bRead ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT;
    sptdwb.sptd.DataTransferLength = nSize;
	
    sptdwb.sptd.TimeOutValue = dwTimeOut / 1000;
    sptdwb.sptd.DataBuffer = pBuf;
    sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,ucSenseBuf);
	
	sptdwb.sptd.Cdb[0] = 0xEF;
	sptdwb.sptd.Cdb[1] = bRead ? 0xFF : 0xFE;
	
    DWORD length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	DWORD returned = 0;
    
	if (!DeviceIoControl(m_hDevice,
		IOCTL_SCSI_PASS_THROUGH_DIRECT,
		&sptdwb,
		length,
		&sptdwb,
		length,
		&returned,
		NULL) ||
		returned < offsetof(SCSI_PASS_THROUGH_DIRECT, ScsiStatus) + sizeof (sptdwb.sptd.ScsiStatus) ||
		sptdwb.sptd.ScsiStatus != 0 /*||
		returned < offsetof(SCSI_PASS_THROUGH_DIRECT, DataTransferLength) + sizeof (sptdwb.sptd.DataTransferLength)*/)
		return 0;

	return sptdwb.sptd.DataTransferLength;
}

//////////////////////////////////////////////////////////////////////////
BOOL CCommUSB::Open()
{
	return SCSI_Open();
}

void CCommUSB::Close()
{
	SCSI_Close();
}

int CCommUSB::SendData(BYTE *pBuf, int nSize, DWORD dwTimeOut)
{
	return SCSI_Operation(pBuf, nSize, dwTimeOut, FALSE);
}

int CCommUSB::RecvData(BYTE *pBuf, int nSize, DWORD dwTimeOut)
{
	return SCSI_Operation(pBuf, nSize, dwTimeOut, TRUE);
}

/************************************************************************/
/*       global function definitions                                    */
/************************************************************************/
CCommSerial comm_serial;
CCommUSB	comm_usb;
double		comm_percent = 0.0;
int			gn_comm_type = 0;

int comm_send(BYTE* pbuf, int nsize, int ntimeout)
{
	if( gn_comm_type == COMM_MODE_SERIAL )
		return comm_serial.SendData(pbuf, nsize, ntimeout);
	else if( gn_comm_type == COMM_MODE_USB )
		return comm_usb.SendData( pbuf, nsize, ntimeout );
	return 0;
}

int comm_recv(BYTE* pbuf, int nsize, int ntimeout)
{
	if( gn_comm_type == COMM_MODE_SERIAL )
		return comm_serial.RecvData(pbuf, nsize, ntimeout);
	else if( gn_comm_type == COMM_MODE_USB )
		return comm_usb.RecvData( pbuf, nsize, ntimeout );
	return 0;
}

BOOL comm_open_serial(int nComNumber, int nComBaudRate)
{
	comm_close();
	
	gn_comm_type = COMM_MODE_SERIAL;
	if(!comm_serial.Open(nComNumber, nComBaudRate))
	{
		comm_close();
		return FALSE;
	}
	
	return TRUE;
}

BOOL comm_open_usb(){
	comm_close();
	gn_comm_type = COMM_MODE_USB;
	return comm_usb.Open();
}

void comm_close()
{
	if( gn_comm_type == COMM_MODE_SERIAL )
		comm_serial.Close();
	else if( gn_comm_type == COMM_MODE_USB )
		comm_usb.Close();
}



#pragma once

#include "CommBase.h"
#include "sb_protocol_oem.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
enum
{
	CMD_NONE				= 0x00,
	CMD_OPEN				= 0x01,
	CMD_CLOSE				= 0x02,
	CMD_USB_INTERNAL_CHECK	= 0x03,
	CMD_CHANGE_BAUDRATE		= 0x04,
	
	CMD_CMOS_LED			= 0x12,

	CMD_ENROLL_COUNT		= 0x20,
	CMD_CHECK_ENROLLED		= 0x21,
	CMD_ENROLL_START		= 0x22,
	CMD_ENROLL1				= 0x23,
	CMD_ENROLL2				= 0x24,
	CMD_ENROLL3				= 0x25,
	CMD_IS_PRESS_FINGER		= 0x26,
	
	CMD_DELETE				= 0x40,
	CMD_DELETE_ALL			= 0x41,
	
	CMD_VERIFY				= 0x50,
	CMD_IDENTIFY			= 0x51,
	CMD_VERIFY_TEMPLATE		= 0x52,
	CMD_IDENTIFY_TEMPLATE	= 0x53,
	
	CMD_CAPTURE				= 0x60,

	CMD_GET_IMAGE			= 0x62,
	CMD_GET_RAWIMAGE		= 0x63,
	
	CMD_GET_TEMPLATE		= 0x70,
	CMD_ADD_TEMPLATE		= 0x71,
	CMD_GET_DATABASE_START  = 0x72,
	CMD_GET_DATABASE_END  	= 0x73,
	
	CMD_FW_UPDATE			= 0x80,
	CMD_ISO_UPDATE			= 0x81,
	
	ACK_OK					= 0x30,
	NACK_INFO				= 0x31,
};

enum
{
		NACK_NONE				= 0x1000,
		NACK_TIMEOUT,				
		NACK_INVALID_BAUDRATE,		
		NACK_INVALID_POS,			
		NACK_IS_NOT_USED,			
		NACK_IS_ALREADY_USED,		
		NACK_COMM_ERR,				
		NACK_VERIFY_FAILED,			
		NACK_IDENTIFY_FAILED,		
		NACK_DB_IS_FULL,				
		NACK_DB_IS_EMPTY,			
		NACK_TURN_ERR,				
		NACK_BAD_FINGER,
		NACK_ENROLL_FAILED,
		NACK_IS_NOT_SUPPORTED,
		NACK_DEV_ERR,
		NACK_CAPTURE_CANCELED,
		NACK_INVALID_PARAM,
		NACK_FINGER_IS_NOT_PRESSED,
};

enum
{
		OEM_NONE					= -2000,
		OEM_COMM_ERR,				
};

typedef struct _devinfo
{
	DWORD FirmwareVersion;
	DWORD IsoAreaMaxSize;
	BYTE DeviceSerialNumber[16];
} devinfo;

//////////////////////////////////////////////////////////////////////////
#define FP_MAX_USERS		200
#define FP_TEMPLATE_SIZE	498
#define EEPROM_SIZE			16

#define IMG8BIT_SIZE	256*256

extern BYTE	gbyImg8bit[IMG8BIT_SIZE];
extern BYTE	gbyImgRaw[320*240];
extern BYTE	gbyTemplate[FP_TEMPLATE_SIZE];

extern WORD gwDevID;
extern WORD gwLastAck;
extern int  gwLastAckParam;
extern int  gnPassedTime;

extern devinfo gDevInfo;

//////////////////////////////////////////////////////////////////////////
int oem_open( void );
int oem_close( void );
int oem_usb_internal_check( void );
int oem_change_baudrate( int nBaudrate );

int oem_cmos_led( BOOL bOn );

int oem_enroll_count( void );
int oem_check_enrolled( int nPos );
int oem_enroll_start( int nPos );
/*AVW*/
int oem_enroll_nth( int nPos, int nTurn );
int oem_is_press_finger( void );

int oem_delete( int nPos );
int oem_delete_all( void );

int oem_verify( int nPos );
int oem_identify( void );
int oem_verify_template( int nPos );
int oem_identify_template( void );

int oem_capture( BOOL bBest );
int oem_get_image( void );
int oem_get_rawimage( void );

int oem_get_template( int nPos );
int oem_add_template( int nPos);
int oem_get_database_start( void );
int oem_get_database_end( void );

int oem_fw_upgrade( BYTE* pBuf, int nLen, CProgressCtrl *pProgress = NULL  );
int oem_iso_upgrade( BYTE* pBuf, int nLen, CProgressCtrl *pProgress = NULL  );

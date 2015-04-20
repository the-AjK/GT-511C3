// SDK_DEMODlg.cpp : implementation file
//

#include "stdafx.h"
#include "SDK_DEMO.h"
#include "SDK_DEMODlg.h"
#include "OEM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#pragma pack(1)
struct FP_BITMAP
{
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFO bmInfo;
	RGBQUAD bmiColors[255];
	
	FP_BITMAP(int cx, int cy)
	{
		bmfHdr.bfType = ((WORD) ('M' << 8) | 'B');  // "BM"
		bmfHdr.bfSize = sizeof(FP_BITMAP) + cx*cy;
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = sizeof(FP_BITMAP);
		
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = cx;
		bmInfo.bmiHeader.biHeight	= -cy;
		bmInfo.bmiHeader.biPlanes	= 1;
		bmInfo.bmiHeader.biBitCount = 8;
		bmInfo.bmiHeader.biCompression = 0;
		bmInfo.bmiHeader.biSizeImage = cx*cy;
		bmInfo.bmiHeader.biXPelsPerMeter = 0;
		bmInfo.bmiHeader.biYPelsPerMeter = 0;
		bmInfo.bmiHeader.biClrUsed = 0;
		bmInfo.bmiHeader.biClrImportant = 0;
		
		RGBQUAD *pals = bmInfo.bmiColors;
		for (int i = 0; i < 256; i++) {
			pals[i].rgbBlue = i;
			pals[i].rgbGreen = i;
			pals[i].rgbRed = i;
			pals[i].rgbReserved = 0;
		}
	}
};
#pragma pack()

FP_BITMAP fp_bmp256(256,256);
FP_BITMAP fp_bmpraw(320,240);

void fp_bmp_draw(HDC hdc, const FP_BITMAP* fp_bmp, void* p, int x, int y, int cx, int cy)
{
	SetStretchBltMode(hdc, COLORONCOLOR);
	
	StretchDIBits(hdc, x, y, cx, cy,
		0, 0, fp_bmp->bmInfo.bmiHeader.biWidth, abs(fp_bmp->bmInfo.bmiHeader.biHeight),
		p, &fp_bmp->bmInfo,
		DIB_RGB_COLORS, SRCCOPY);
}

BOOL fp_bmp_save(LPCTSTR szFilePath, FP_BITMAP* fp_bmp, void* p)
{
	CFile f;
	if(!f.Open(szFilePath, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))
		return FALSE;
	
	int w = fp_bmp->bmInfo.bmiHeader.biWidth;
	int h_org = fp_bmp->bmInfo.bmiHeader.biHeight;
	int h = abs(h_org);
	
	fp_bmp->bmInfo.bmiHeader.biHeight = h;
	f.Write(fp_bmp, sizeof(FP_BITMAP));
	fp_bmp->bmInfo.bmiHeader.biHeight = h_org;
	
	int i;
	for (i=h-1; i>=0; i--)
		f.Write((BYTE*)p + i*w, w);
	
	f.Close();
	
	return TRUE;
}

enum
{
	fp_bmp_none,
	fp_bmp_256,	
	fp_bmp_raw,	
};

int gImageType = fp_bmp_none;

#define TOP		50
#define TOPRAW	(TOP + (256-240)/2)
#define LEFT	20
#define LEFT256	(LEFT + (320-256)/2)

#define STD_TOP		10
#define STD_LEFT	20

/************************************************************************/
/*                                                                      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*AVW*/
BYTE gbyTemplateDB[FP_TEMPLATE_SIZE * (FP_MAX_USERS+1)+FP_MAX_USERS+1];

enum
{
	FILE_NAME_GET_TEMPLATE,
	FILE_NAME_GET_FPDB,
	FILE_NAME_GET_BMP,
	FILE_NAME_FIRMWARE,
};

CString file_name_get(int nType, BOOL bOpenSave)
{
	TCHAR* defexts[] = {_T("dat"), _T("db"), _T("bmp"), _T("bin")};
	TCHAR* deffilenames[] = {_T("00.dat"), _T("database.db"), _T("image.bmp"), _T("fingerKeyFw.BIN")};
	TCHAR* filters[] = {_T("Template Files (*.dat)|*.dat|All Files (*.*)|*.*||"),
		_T("Database Files (*.db)|*.db|All Files (*.*)|*.*||"),
		_T("Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"),
		_T("Firmware Files (*.bin)|*.bin|All Files (*.*)|*.*||")};

	CFileDialog dlg(bOpenSave, defexts[nType], deffilenames[nType], OFN_OVERWRITEPROMPT, filters[nType]);
	
	if(dlg.DoModal() == IDOK) 
	{
		return dlg.GetPathName();
	}
	
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMODlg dialog

CSDK_DEMODlg::CSDK_DEMODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSDK_DEMODlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSDK_DEMODlg)
	m_nPortNumber = 0;
	m_nBaudrate = 4;
	m_strResult = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bImageShow = TRUE;
	m_bContinue = FALSE;
	m_ing = FALSE;
}

void CSDK_DEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDK_DEMODlg)
	DDX_CBIndex(pDX, IDC_COMBO_COM, m_nPortNumber);
	DDX_CBIndex(pDX, IDC_COMBO_BAUDRATE, m_nBaudrate);
	DDX_Control(pDX, IDC_SPIN_ID, m_spinID);
	DDX_Control(pDX, IDC_PROGRESS1, m_prog);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strResult);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSDK_DEMODlg, CDialog)
	//{{AFX_MSG_MAP(CSDK_DEMODlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTT_OPEN, OnButtOpen)
	ON_BN_CLICKED(IDC_BUTT_CLOSE, OnButtClose)
	ON_BN_CLICKED(IDC_BUTT_ENROLL, OnButtEnroll)
	ON_BN_CLICKED(IDC_BUTT_VERIFY, OnButtVerify)
	ON_BN_CLICKED(IDC_BUTT_IDENTIFY, OnButtIdentify)
	ON_BN_CLICKED(IDC_BUTT_VERIFY_TEMPLATE, OnButtVerifyTemplate)
	ON_BN_CLICKED(IDC_BUTT_IDENTIFY_TEMPLATE, OnButtIdentifyTemplate)
	ON_BN_CLICKED(IDC_BUTT_ISPRESSFP, OnButtIspressfp)
	ON_BN_CLICKED(IDC_BUTT_GET_IMAGE, OnButtGetImage)
	ON_BN_CLICKED(IDC_BUTT_GET_RAWIMAGE, OnButtGetRawimage)
	ON_BN_CLICKED(IDC_BUTT_GET_USER_COUNT, OnButtGetUserCount)
	ON_BN_CLICKED(IDC_BUTT_DELETE, OnButtDelete)
	ON_BN_CLICKED(IDC_BUTT_DELETE_ALL, OnButtDeleteAll)
	ON_BN_CLICKED(IDC_BUTT_GET_TEMPLATE, OnButtGetTemplate)
	ON_BN_CLICKED(IDC_BUTT_SET_TEMPLATE, OnButtSetTemplate)
	ON_BN_CLICKED(IDC_BUTT_GET_DATABASE, OnButtGetDatabase)
	ON_BN_CLICKED(IDC_BUTT_SET_DATABASE, OnButtSetDatabase)
	ON_BN_CLICKED(IDC_BUTT_FW_UPGRADE, OnButtFwUpgrade)
	ON_BN_CLICKED(IDC_BUTT_ISO_UPGRADE, OnButtIsoUpgrade)
	ON_BN_CLICKED(IDC_BUTT_GET_LIVEIMAGE, OnButtGetLiveimage)
	ON_BN_CLICKED(IDC_BUTT_SAVE_IMAGE, OnButtSaveImage)
	ON_BN_CLICKED(IDC_BUTT_CANCEL, OnButtCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMODlg message handlers

BOOL CSDK_DEMODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	EnableBtn(FALSE);
	GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(FALSE);
	
	//m_spinID.SetBuddy(GetDlgItem(IDC_EDIT_ID));
	m_spinID.SetRange32(-1, FP_MAX_USERS - 1);
	m_spinID.SetPos32(0);

	m_prog.SetRange32(0, 10000);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSDK_DEMODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSDK_DEMODlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
// 		CDialog::OnPaint();
		
		CPaintDC dc(this); // device context for painting
		
		static int gImageType_prev = fp_bmp_raw;
		if (gImageType_prev != gImageType)
		{
			dc.FillRect(CRect(CPoint(LEFT,TOP), CSize(320,256)), &(CBrush(RGB(255,255,255))));
			gImageType_prev = gImageType;
		}

		if(gImageType == fp_bmp_256)
		{
			/*AVW*/
			fp_bmp_draw(dc.GetSafeHdc(), &fp_bmp256, gbyImg8bit, LEFT256, TOP, 256, 256);
		}
		if(gImageType == fp_bmp_raw)
		{
			fp_bmp_draw(dc.GetSafeHdc(), &fp_bmpraw, gbyImgRaw, LEFT, TOPRAW, 320, 240);
		}
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSDK_DEMODlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSDK_DEMODlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_ing)
		return;

	CDialog::OnClose();
}

void CSDK_DEMODlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (nIDEvent == 1)
	{
		m_prog.SetPos((int)(comm_percent * 100.0));
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CSDK_DEMODlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) &&
		pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSDK_DEMODlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (wParam == IDOK)
		return FALSE;

	return CDialog::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
class CSDK_DEMODlgWrap
{
	CSDK_DEMODlg* m_pDlg;
	UINT m_nLastID;
	BOOL m_bCmosLedOn;
public:
	
	CSDK_DEMODlgWrap(CSDK_DEMODlg* pDlg, UINT nLastID, BOOL bCmosLedOn = FALSE)
	{
		m_pDlg = pDlg;
		m_nLastID = nLastID;
		m_bCmosLedOn = bCmosLedOn;
		m_pDlg->m_strResult = _T(" ");
		m_pDlg->EnableBtn(FALSE, TRUE);
		m_pDlg->UpdateData();
		m_pDlg->m_prog.SetPos(0);
		//m_pDlg->m_prog.ShowWindow(SW_SHOW);

		m_pDlg->m_ing = TRUE;

		if (m_bCmosLedOn)
			oem_cmos_led(TRUE);
	}
	
	~CSDK_DEMODlgWrap()
	{
		if (m_bCmosLedOn)
			oem_cmos_led(FALSE);

		m_pDlg->m_ing = FALSE;

		m_pDlg->m_prog.ShowWindow(SW_HIDE);
		m_pDlg->UpdateData(FALSE);
		m_pDlg->EnableBtn(TRUE);
		m_pDlg->GetDlgItem(m_nLastID)->SetFocus();
	}
};

class CSDK_DEMODlgWrap_cancel
{
	CSDK_DEMODlg* m_pDlg;
public:
	
	CSDK_DEMODlgWrap_cancel(CSDK_DEMODlg* pDlg)
	{
		m_pDlg = pDlg;
		m_pDlg->m_bContinue = TRUE;
		m_pDlg->GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(TRUE);
	}
	
	~CSDK_DEMODlgWrap_cancel()
	{
		m_pDlg->GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(FALSE);
		m_pDlg->m_bContinue = FALSE;
	}
};

void CSDK_DEMODlg::EnableBtn(BOOL bEnable, BOOL bDisableAll)
{
	GetDlgItem(IDC_BUTT_OPEN)->EnableWindow(!bEnable && !bDisableAll);
    
	for (UINT id=IDC_BUTT_CLOSE; id<=IDC_BUTT_SAVE_IMAGE; id++)
	{
		try
		{
			GetDlgItem(id)->EnableWindow(bEnable && !bDisableAll);
		}
		catch (...)
		{
		}
	}
}

int CSDK_DEMODlg::Capturing(BOOL bBest/* = FALSE*/)
{
	DWORD st = GetTickCount();
	while(m_bContinue && GetTickCount() - st < 5000)
	{
		if(oem_capture(bBest) < 0)
		{
			m_strResult = _T("Communication error!");
			return -1;
		}
		else if(gwLastAck == ACK_OK)
		{
			return 0;
		}
		ui_polling();
	}

	m_strResult = _T("Timeout!");
	return -1;
}

int CSDK_DEMODlg::LoadingImage(void)
{
	int nRet = 0;
    
	m_prog.ShowWindow(SW_SHOW);
	m_prog.SetPos(0);
	SetTimer(1, 100, NULL);
	m_strResult = _T("Downloading Image...");
	UpdateData(FALSE);
	if(oem_get_image() < 0)
	{
		m_strResult = _T("Communication error!");
		nRet = -1;
	}
	else if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		nRet = -1;
	}
	else if(0)
	{
		static int gnImageNumber = 1;
		CreateDirectory(_T("C:\\_"), NULL);
		CString strFileName;
		strFileName.Format(_T("C:\\_\\Image%03d.bmp"), gnImageNumber++);
		
		fp_bmp_save(strFileName, &fp_bmp256, gbyImg8bit);
	}
	KillTimer(1);
	m_prog.ShowWindow(SW_HIDE);
	m_strResult = _T("");
	UpdateData(FALSE);
	return nRet;
}

int CSDK_DEMODlg::LoadingImageRaw(void)
{
	int nRet = 0;
	
	m_prog.ShowWindow(SW_SHOW);
	m_prog.SetPos(0);
	SetTimer(1, 100, NULL);
	m_strResult = _T("Downloading Image...");
	UpdateData(FALSE);
	if(oem_get_rawimage() < 0)
	{
		m_strResult = _T("Communication error!");
		nRet = -1;
	}
	else if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		nRet = -1;
	}
	else if(0)
	{
		static int gnImageNumber = 1;
		CreateDirectory(_T("C:\\_"), NULL);
		CString strFileName;
		strFileName.Format(_T("C:\\_\\ImageRaw%03d.bmp"), gnImageNumber++);
		
		fp_bmp_save(strFileName, &fp_bmpraw, gbyImgRaw);
	}
	KillTimer(1);
	m_prog.ShowWindow(SW_HIDE);
	m_strResult = _T("");
	UpdateData(FALSE);
	return nRet;
}

int CSDK_DEMODlg::LoadingImageLive(void)
{
	int nRet = 0;
	
	if(oem_get_rawimage() < 0)
	{
		m_strResult = _T("Communication error!");
		nRet = -1;
	}
	else if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		nRet = -1;
	}

	return nRet;
}

void CSDK_DEMODlg::DisplayErr(int nNackInfo, int nPos)
{
	if( nNackInfo > NACK_NONE)
	{
		switch(nNackInfo)
		{
		case NACK_TIMEOUT:
			m_strResult = _T("Time Out!");
			break;
		case NACK_INVALID_BAUDRATE:
			m_strResult = _T("Invalid baudrate");
			break;
		case NACK_INVALID_POS:
			m_strResult = _T("Invalid ID");
			break;
		case NACK_IS_NOT_USED:
			m_strResult.Format(_T("ID = %d: is not used!"), nPos);
			break;
		case NACK_IS_ALREADY_USED:
			m_strResult.Format(_T("ID = %d: is already used!"), nPos);
			break;
		case NACK_COMM_ERR:
			m_strResult = _T("Communication error!");
			break;
		case NACK_VERIFY_FAILED:
			m_strResult.Format(_T("ID = %d: NG!"), nPos);
			break;
		case NACK_IDENTIFY_FAILED:
			m_strResult = _T("NG!");
			break;
		case NACK_DB_IS_FULL:
			m_strResult = _T("Database is full");
			break;
		case NACK_DB_IS_EMPTY:
			m_strResult = _T("Database is empty");
			break;
		case NACK_TURN_ERR:
			m_strResult = _T("The order of enrollment is incorrect!");
			break;
		case NACK_BAD_FINGER:
			m_strResult = _T("Bad finger!");
			break;
		case NACK_ENROLL_FAILED:
			m_strResult = _T("The enrollment is failed!");
			break;
		case NACK_IS_NOT_SUPPORTED:
			m_strResult = _T("The command is not supported");
			break;
		case NACK_DEV_ERR:
			m_strResult = _T("The device error!");
			break;
		case NACK_CAPTURE_CANCELED:
			m_strResult = _T("Canceled!");
			break;
		case NACK_INVALID_PARAM:
			m_strResult = _T("Invalid Firmware Image!");
			break;
		case NACK_FINGER_IS_NOT_PRESSED:
			m_strResult = _T("Finger is not pressed!");
			break;
		default:
			m_strResult = _T("Unknown Error");
			break;
		}
	}
	else if (nNackInfo<FP_MAX_USERS)
	{
		m_strResult.Format(_T("ID = %d: is already enrolled!"), nNackInfo);
	}
}

void CSDK_DEMODlg::SetBmp(BOOL bIsRaw/* = FALSE*/)
{
	gImageType = bIsRaw ? fp_bmp_raw : fp_bmp_256;
	Invalidate(FALSE);

	ui_polling();
}

void CSDK_DEMODlg::OnButtOpen() 
{
	int nBaudrate[5] = {9600, 19200, 38400, 57600, 115200};

	UpdateData();

	if (m_nPortNumber == 0)
	{
		if( !comm_open_usb() ){
			m_strResult = _T("Device is not connected to usb port.");
			return;
		}
	}
	else
	{
		if( !comm_open_serial(m_nPortNumber, 9600) ||
			oem_change_baudrate(nBaudrate[m_nBaudrate]) < 0 ||
			!comm_open_serial(m_nPortNumber, nBaudrate[m_nBaudrate])){
			m_strResult = _T("Device is not connected to serial port.");
			return;
		}
	}
	
	if(oem_open() < 0)
	{
		m_strResult = _T("Cannot connect to the device !");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		m_strResult = _T("Cannot connect to the device !");
		return;
	}

	m_strResult.Format(_T("FirmwareVersion: %08X, IsoAreaMaxSize: %d KB\r\nDeviceSN: "), gDevInfo.FirmwareVersion, gDevInfo.IsoAreaMaxSize / 1024);

	BYTE* p = gDevInfo.DeviceSerialNumber;
	CString str;
	str.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X-%02X%02X%02X%02X%02X%02X%02X%02X"),
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7], p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
	m_strResult += str;
	
	UpdateData(FALSE);
	EnableBtn(TRUE);
	GetDlgItem(IDC_BUTT_ENROLL)->SetFocus();
}

void CSDK_DEMODlg::OnButtClose() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_strResult = _T("");
	UpdateData(FALSE);
	oem_close();
	if (gn_comm_type == COMM_MODE_SERIAL)
		oem_change_baudrate(9600);
	comm_close();
	EnableBtn(FALSE);
	GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTT_OPEN)->SetFocus();
}

void CSDK_DEMODlg::OnButtEnroll() 
{
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_ENROLL, TRUE);
	
	int nID = m_spinID.GetPos32();
	if (nID < -1)
		nID = -1;
	if (nID > FP_MAX_USERS - 1)
		nID = FP_MAX_USERS - 1;
	m_spinID.SetPos32(nID);

	if(oem_enroll_start(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}

	CSDK_DEMODlgWrap_cancel _wrap_cancel_(this);

	for(int i=1; i<4; i++)
	{
		m_strResult.Format(_T("Input finger %d !"), i);
		UpdateData(FALSE);
		if(Capturing(TRUE) < 0){
			return;
		}
		if(m_bImageShow && gn_comm_type == COMM_MODE_USB)
		{
			if(LoadingImage() < 0){
				return;
			}
			SetBmp();
		}
		
		m_strResult = _T("Processing fingerprint...");
		UpdateData(FALSE);
	
		if(oem_enroll_nth(nID, i) < 0)
		{
			m_strResult = _T("Communication error!");
			return;
		}
		if(gwLastAck == NACK_INFO)
		{
			DisplayErr(gwLastAckParam, 0);
			return;
		}
		m_strResult.Format( _T("Take off finger, please...") );
		UpdateData(FALSE);
		if( i<3 )
		while(1){
			if(  oem_is_press_finger() < 0 ){
				m_strResult = _T("Communication error!");
				return;
			}
			if( gwLastAck == NACK_INFO ){
				DisplayErr( gwLastAckParam, 0 );
				return;
			}
			if( gwLastAckParam != 0 )
				break;
		}
		m_strResult.Format(_T("Process time: %d ms"), gnPassedTime);
		UpdateData(FALSE);
	}
	if (nID == -1)
	{
		//////////////////////////////////////////////////////////////////////////
		CString m_strFilePath = file_name_get(FILE_NAME_GET_TEMPLATE, FALSE);
		if(m_strFilePath == _T(""))
		{
			m_strResult = _T("Canceled!");
			return;
		}
		
		CFile m_File;
		if( !m_File.Open( m_strFilePath,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) )
		{
			AfxMessageBox(_T("Cannot create file for write!"));
			m_strResult = _T("Save Error!");
			return;
		}
		
		m_File.Write( gbyTemplate, FP_TEMPLATE_SIZE);
		m_File.Close();
		//////////////////////////////////////////////////////////////////////////
		m_strResult.Format(_T("Enroll & Save Template OK!"));
		return;
	}
	m_spinID.SetPos32(nID+1);
	m_strResult.Format(_T("Enroll OK (ID = %d)!"), nID);
}

void CSDK_DEMODlg::OnButtVerify() 
{
	// TODO: Add your control notification handler code here
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_VERIFY, TRUE);
	
	int nID = m_spinID.GetPos32();
	if (nID < 0 || nID >= FP_MAX_USERS)
	{
		nID = 0;
		m_spinID.SetPos32(nID);
	}
	
	if( oem_check_enrolled(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}
	
	CSDK_DEMODlgWrap_cancel _wrap_cancel_(this);

	m_strResult = _T("");
	while (m_bContinue)
	{
		m_strResult += _T("; Input finger !");
		UpdateData(FALSE);
		
		if(Capturing() < 0)
			return;
		
		if(m_bImageShow && gn_comm_type == COMM_MODE_USB)
		{
			if(LoadingImage() < 0)
				return;
			SetBmp();
		}
		
		m_strResult = _T("Processing fingerprint...");
		UpdateData(FALSE);
		
		if(oem_verify(nID) < 0)
		{
			m_strResult = _T("Communication error!");
			return;
		}
		if(gwLastAck == NACK_INFO)
		{
			DisplayErr(gwLastAckParam, nID);
			UpdateData(FALSE);
			continue;
		}
		
		m_strResult.Format(_T("ID = %d      : %d ms"), nID, gnPassedTime);
		UpdateData(FALSE);
		if(m_bImageShow && gn_comm_type == COMM_MODE_USB)
		{
			if (0)
			{
				static int gnImageNumber = 1;
				CreateDirectory(_T("C:\\_"), NULL);
				CString strFileName;
				strFileName.Format(_T("C:\\_\\ImageIden%03d.bmp"), gnImageNumber++);
				
				fp_bmp_save(strFileName, &fp_bmp256, gbyImg8bit);
			}
		}
	}
	
}

void CSDK_DEMODlg::OnButtIdentify() 
{
	// TODO: Add your control notification handler code here
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_IDENTIFY, TRUE);
	
	if(oem_enroll_count() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}
	
	if (gwLastAckParam == 0)
	{
		m_strResult = _T("No user !");
		return;
	}
	
	CSDK_DEMODlgWrap_cancel _wrap_cancel_(this);

	m_strResult = _T("");
	while (m_bContinue)
	{
		m_strResult += _T("; Input finger !");
		UpdateData(FALSE);

		if(Capturing() < 0)
			return;

		if(m_bImageShow && gn_comm_type == COMM_MODE_USB)
		{
			if(LoadingImage() < 0)
				return;
			SetBmp();
		}

		m_strResult = _T("Processing fingerprint...");
		UpdateData(FALSE);

		if(oem_identify() < 0)
		{
			m_strResult = _T("Communication error!");
			return;
		}
		if(gwLastAck == NACK_INFO)
		{
			DisplayErr(gwLastAckParam, 0);
			UpdateData(FALSE);
			continue;
		}

		m_strResult.Format(_T("ID = %d      : %d ms"), gwLastAckParam, gnPassedTime);
		UpdateData(FALSE);

		if(m_bImageShow && gn_comm_type == COMM_MODE_USB)
		{
			if (0)
			{
				static int gnImageNumber = 1;
				CreateDirectory(_T("C:\\_"), NULL);
				CString strFileName;
				strFileName.Format(_T("C:\\_\\ImageIden%03d.bmp"), gnImageNumber++);
				
				fp_bmp_save(strFileName, &fp_bmp256, gbyImg8bit);
			}
		}
	}

}

void CSDK_DEMODlg::OnButtVerifyTemplate() 
{
	// TODO: Add your control notification handler code here
	
	//////////////////////////////////////////////////////////////////////////
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_VERIFY_TEMPLATE);

	int nID = m_spinID.GetPos32();
	if (nID < 0 || nID >= FP_MAX_USERS)
	{
		nID = 0;
		m_spinID.SetPos32(nID);
	}
	
	if( oem_check_enrolled(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}

	CString m_strFilePath = file_name_get(FILE_NAME_GET_TEMPLATE, TRUE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeRead | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot open file for read!"));
		return;
	}
	
	int nLength = m_File.GetLength();
	if( nLength != FP_TEMPLATE_SIZE )
	{
		AfxMessageBox(_T("Invalid size for read!"));
		return;
	}
	m_File.Read( gbyTemplate, nLength);
	m_File.Close();
	
	//////////////////////////////////////////////////////////////////////////
	
	
	if(oem_verify_template(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if( gwLastAck==NACK_INFO )
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}
	
	m_strResult.Format(_T("ID = %d      : %d ms"), nID, gnPassedTime);
	UpdateData(FALSE);
}

void CSDK_DEMODlg::OnButtIdentifyTemplate() 
{
	// TODO: Add your control notification handler code here
	//////////////////////////////////////////////////////////////////////////
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_IDENTIFY_TEMPLATE);

	if(oem_enroll_count() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}
	
	if (gwLastAckParam == 0)
	{
		m_strResult = _T("No user !");
		return;
	}
	
	CString m_strFilePath = file_name_get(FILE_NAME_GET_TEMPLATE, TRUE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeRead | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot open file for read!"));
		return;
	}
	
	int nLength = m_File.GetLength();
	if( nLength != FP_TEMPLATE_SIZE )
	{
		AfxMessageBox(_T("Invalid size for read!"));
		return;
	}
	m_File.Read( gbyTemplate, nLength);
	m_File.Close();
	
	//////////////////////////////////////////////////////////////////////////

	
	if(oem_identify_template() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}
	
	m_strResult.Format(_T("ID = %d      : %d ms"), gwLastAckParam, gnPassedTime);
	UpdateData(FALSE);

}

void CSDK_DEMODlg::OnButtIspressfp() 
{
	// TODO: Add your control notification handler code here

	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_ISPRESSFP, TRUE);
	
	if(  oem_is_press_finger() < 0 ){
		m_strResult = _T("Communication error!");
		UpdateData(FALSE);
		return;
	}
	if( gwLastAck == NACK_INFO ){
		DisplayErr( gwLastAckParam, 0 );
		UpdateData(FALSE);
		return;
	}
	if( gwLastAckParam != 0 )
	{
		m_strResult = _T("Finger is not pressed!");
		UpdateData(FALSE);
		return;
	}
	
	m_strResult = _T("Finger is pressed!");
	UpdateData(FALSE);
}

void CSDK_DEMODlg::OnButtGetImage() 
{
	// TODO: Add your control notification handler code here
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_IMAGE, TRUE);
	
	m_strResult = _T("Input finger !");
	UpdateData(FALSE);
	
	CSDK_DEMODlgWrap_cancel _wrap_cancel_(this);
	if(Capturing() < 0)
		return;
	
	if(LoadingImage() < 0)
		return;
	SetBmp();
	
	m_strResult = _T("Get Image OK !");
}

void CSDK_DEMODlg::OnButtGetRawimage() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_RAWIMAGE, TRUE);
	
	if(LoadingImageRaw() < 0)
		return;
	SetBmp(TRUE);
	
	m_strResult = _T("Get Raw Image OK !");
}

void CSDK_DEMODlg::OnButtGetUserCount() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_USER_COUNT);
	
	if(oem_enroll_count() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}
	
	m_strResult.Format(_T("Enroll count = %d !"), gwLastAckParam);
}

void CSDK_DEMODlg::OnButtDelete() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this,IDC_BUTT_DELETE);
	
	int nID = m_spinID.GetPos32();
	if (nID < 0 || nID >= FP_MAX_USERS)
	{
		nID = 0;
		m_spinID.SetPos32(nID);
	}
	
	CString str;
	str.Format(_T("%d"), m_spinID.GetPos32());
	GetDlgItem(IDC_EDIT_ID)->SetWindowText(str);
	
	if(AfxMessageBox(_T("Are you sure to delete?"), MB_YESNO) == IDNO)
	{
		m_strResult = _T("Canceled !");
		return;
	}
	
	if(oem_delete(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}

	m_strResult = _T("Delete OK !");
}

void CSDK_DEMODlg::OnButtDeleteAll() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_DELETE_ALL);
	
	if(AfxMessageBox(_T("Are you sure to delete all?"), MB_YESNO) == IDNO)
	{
		m_strResult = _T("Canceled !");
		return;
	}
	
	if(oem_delete_all() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}

	m_strResult = _T("Delete all OK !");
}

void CSDK_DEMODlg::OnButtGetTemplate() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_TEMPLATE);
	
	int nIndexBufPos = FP_TEMPLATE_SIZE * FP_MAX_USERS;
	
	int nID = m_spinID.GetPos32();
	if (nID < 0 || nID >= FP_MAX_USERS)
	{
		nID = 0;
		m_spinID.SetPos32(nID);
	}
	
	m_strResult.Format(_T("Getting of %dth template..."), nID);
	UpdateData(FALSE);
		
	if(oem_get_template(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if( gwLastAck==NACK_INFO )
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}

	
	//////////////////////////////////////////////////////////////////////////
	CString m_strFilePath = file_name_get(FILE_NAME_GET_TEMPLATE, FALSE);
	if(m_strFilePath == _T(""))
	{
		m_strResult = _T("Canceled!");
		return;
	}

	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot create file for write!"));
		m_strResult = _T("Save Error!");
		return;
	}
	
	m_File.Write( gbyTemplate, FP_TEMPLATE_SIZE);
	m_File.Close();
	//////////////////////////////////////////////////////////////////////////
	
	m_strResult.Format(_T("Get Template OK (ID = %d)!"), nID);
}

void CSDK_DEMODlg::OnButtSetTemplate() 
{
	// TODO: Add your control notification handler code here
	//////////////////////////////////////////////////////////////////////////
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_SET_TEMPLATE);

	int nID = m_spinID.GetPos32();
	if (nID < 0 || nID >= FP_MAX_USERS)
	{
		nID = 0;
		m_spinID.SetPos32(nID);
	}

	CString m_strFilePath = file_name_get(FILE_NAME_GET_TEMPLATE, TRUE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeRead | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot open file for read!"));
		return;
	}
	
	int nLength = m_File.GetLength();
	if( nLength != FP_TEMPLATE_SIZE )
	{
		AfxMessageBox(_T("Invalid size for read!"));
		return;
	}
	m_File.Read( gbyTemplate, nLength);
	m_File.Close();

	//////////////////////////////////////////////////////////////////////////
	
	
	m_strResult.Format(_T("Adding of %dth template..."), nID);
	UpdateData(FALSE);
		
	if(oem_add_template(nID) < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if( gwLastAck==NACK_INFO )
	{
		DisplayErr(gwLastAckParam, nID);
		return;
	}
	
	m_strResult.Format(_T("Set Template OK (ID = %d)!"), nID);
}

void CSDK_DEMODlg::OnButtGetDatabase() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_DATABASE);

	int nIndexBufPos = FP_TEMPLATE_SIZE * (FP_MAX_USERS+1);

	if(oem_enroll_count() < 0)
	{
		m_strResult = _T("Communication error!");
		return;
	}
	if(gwLastAck == NACK_INFO)
	{
		DisplayErr(gwLastAckParam, 0);
		return;
	}

	if (gwLastAckParam == 0)
	{
		m_strResult = _T("No user !");
		return;
	}

	if( oem_get_database_start() < 0 ){
		m_strResult = _T("Communication error!");
		return;
	}

	int nEnrollCount = 0;
	for (int i=0; i<FP_MAX_USERS; i++)
	{
		m_strResult.Format(_T("Getting of %dth template..."), i);
		UpdateData(FALSE);
		ui_polling();

		if(oem_get_template(i) < 0)
		{
			 m_strResult = _T("Communication error!");
			 oem_get_database_end();
			 return;
		}
		if( gwLastAck==NACK_INFO && gwLastAckParam!=NACK_IS_NOT_USED )
		{
			DisplayErr(gwLastAckParam, i);
			oem_get_database_end();
			return;
		}
		
		if( gwLastAck==NACK_INFO &&  gwLastAckParam==NACK_IS_NOT_USED )
		{
			memset(&gbyTemplateDB[i * FP_TEMPLATE_SIZE], 0xFF, FP_TEMPLATE_SIZE);
			gbyTemplateDB[nIndexBufPos+i] = 0;
		}
		else
		{
			memcpy(&gbyTemplateDB[i * FP_TEMPLATE_SIZE], gbyTemplate, FP_TEMPLATE_SIZE);
			gbyTemplateDB[nIndexBufPos+i] = 1;
			nEnrollCount++;
		}
	}
	oem_get_database_end();

	//////////////////////////////////////////////////////////////////////////
	CString m_strFilePath = file_name_get(FILE_NAME_GET_FPDB, FALSE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot create file for write!"));
		return;
	}

/* 
	KCN : then, database file would be the same size every time and it's not related to enroll count.
		and also database file should be contain count of templates.
		read database process should do correspond something.
*/	

	m_File.Write( &gbyTemplateDB[0], (FP_MAX_USERS+1) * FP_TEMPLATE_SIZE + (FP_MAX_USERS+1));
	m_File.Close();
	//////////////////////////////////////////////////////////////////////////
	
	m_strResult.Format(_T("Get Database OK (Enroll count = %d)!"), nEnrollCount);
}

void CSDK_DEMODlg::OnButtSetDatabase() 
{
	// TODO: Add your control notification handler code here
	
	//////////////////////////////////////////////////////////////////////////
	CString m_strFilePath = file_name_get(FILE_NAME_GET_FPDB, TRUE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeRead | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot open file for read!"));
		return;
	}
	
	int nLength = m_File.GetLength();
	if( nLength != (FP_TEMPLATE_SIZE * (FP_MAX_USERS+1) + (FP_MAX_USERS+1)) )
	{
		AfxMessageBox(_T("Invalid size for read!"));
		return;
	}
	m_File.Read( &gbyTemplateDB[0], nLength);
	m_File.Close();
	
	int nIndexBufPos = FP_TEMPLATE_SIZE * (FP_MAX_USERS+1);
	int j;
	for(j=0;j<FP_MAX_USERS;j++)
	{
		if(gbyTemplateDB[nIndexBufPos+j])
			break;
	}

	if(j==FP_MAX_USERS )
	{
		m_strResult = _T("No user !");
		UpdateData(FALSE);
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_SET_DATABASE);
	
	int nAddCount = 0;
	for (int i=0; i<FP_MAX_USERS; i++)
	{
		
		if(gbyTemplateDB[nIndexBufPos+i] == 0)
			continue;

		m_strResult.Format(_T("Adding of %dth template..."), i);
		UpdateData(FALSE);
		ui_polling();
		
		memcpy(gbyTemplate, &gbyTemplateDB[i * FP_TEMPLATE_SIZE], FP_TEMPLATE_SIZE);
		
		if(oem_add_template(i) < 0)
		{
			m_strResult = _T("Communication error!");
			return;
		}
		if( gwLastAck==NACK_INFO )
		{
			DisplayErr(gwLastAckParam, i);
			return;
		}
		nAddCount++;
	}
	
	m_strResult.Format(_T("Set Database OK (Enroll count = %d)!"), nAddCount);
}

void CSDK_DEMODlg::OnButtFwUpgrade() 
{
//	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_FW_UPGRADE);
	
	CString m_strFilePath = file_name_get(FILE_NAME_FIRMWARE, TRUE);
	if(m_strFilePath == _T(""))
		return;
	
	CFile m_File;
	if( !m_File.Open( m_strFilePath,CFile::modeRead | CFile::typeBinary) )
	{
		AfxMessageBox(_T("Cannot open file for read!"));
		return;
	}
	
	int nLength = m_File.GetLength();
	if( nLength > 0x20000 )
	{
		AfxMessageBox(_T("Invalid size for read!"));
		return;
	}

	BYTE* pBuf = new BYTE[nLength];
	memset(pBuf, 0xFF, nLength);

	m_File.Read( pBuf, nLength);
	m_File.Close();
	
	//////////////////////////////////////////////////////////////////////////
	m_prog.ShowWindow( SW_SHOW );
	m_prog.SetPos( 0 );
	m_prog.SetRange32( 0, nLength );

	EnableBtn( FALSE );
	GetDlgItem(IDC_BUTT_OPEN)->EnableWindow( FALSE );
	m_strResult = _T("Firmware Sending...  Don't power off the device.");
	UpdateData( FALSE );

	if(oem_fw_upgrade( pBuf, nLength, &m_prog ) < 0)
	{
		m_strResult = _T("Communication error!");
		if( gwLastAck==NACK_INFO )
		{
			DisplayErr( gwLastAckParam, 0 );
		}
		AfxMessageBox(_T("Firmware upgrade Error."));
	}
	else{
		m_strResult = _T("Firmware Upgrading...  Don't power off the device.");
		UpdateData( FALSE );
		Sleep( 15000 );
		m_strResult = _T("Firmware Upgrade OK.");
		AfxMessageBox(_T("Firmware upgrade OK."));
	}

	m_prog.SetRange32(0, 10000);
	m_prog.ShowWindow( SW_HIDE );
	delete[] pBuf;

	UpdateData( FALSE );
	comm_close();
	GetDlgItem(IDC_BUTT_OPEN)->EnableWindow( TRUE );
	GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(FALSE);
}

void CSDK_DEMODlg::OnButtIsoUpgrade() 
{
	// TODO: Add your control notification handler code here

	CFileDialog dlg(TRUE, _T("iso"), _T("fingerKey.iso"), OFN_OVERWRITEPROMPT, _T("ISO Image Files (*.iso)|*.iso|All Files (*.*)|*.*||"));
	
	if(dlg.DoModal() == IDOK) 
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::typeBinary | CFile::modeRead))
		{
			AfxMessageBox(_T("Cannot open ISO image file!"));
			return;
		}

		int len = f.GetLength();
		BYTE* pbyIn = new BYTE[len];
		f.Read(pbyIn, len);
		f.Close();

		if ((DWORD)len > gDevInfo.IsoAreaMaxSize)
		{
			m_strResult.Format(_T("Size: %d KB, MaxSize: %d KB\r\n"), len / 1024, gDevInfo.IsoAreaMaxSize / 1024);
			UpdateData(FALSE);
			AfxMessageBox(_T("ISO Image is too large."));
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			m_prog.ShowWindow( SW_SHOW );
			m_prog.SetPos( 0 );
			m_prog.SetRange32( 0, len );
			
			EnableBtn( FALSE );
			GetDlgItem(IDC_BUTT_OPEN)->EnableWindow( FALSE );
			m_strResult.Format(_T("Size: %d KB, MaxSize: %d KB\r\n"), len / 1024, gDevInfo.IsoAreaMaxSize / 1024);
			m_strResult += _T("ISO Image Sending...  Don't power off the device.");
			UpdateData( FALSE );
			
			if(oem_iso_upgrade( pbyIn, len, &m_prog ) < 0)
			{
				m_strResult = _T("Communication error!");
				if( gwLastAck==NACK_INFO )
				{
					DisplayErr( gwLastAckParam, 0 );
				}
				AfxMessageBox(_T("ISO Image upgrade Error."));
			}
			else{
				m_strResult = _T("ISO Image Upgrade OK.");
				AfxMessageBox(_T("ISO Image upgrade OK."));
			}
			
			m_prog.SetRange32(0, 10000);
			m_prog.ShowWindow( SW_HIDE );
			
			UpdateData( FALSE );
			comm_close();
			GetDlgItem(IDC_BUTT_OPEN)->EnableWindow( TRUE );
			GetDlgItem(IDC_BUTT_CANCEL)->EnableWindow(FALSE);
		}
		delete[] pbyIn;
	}
}

void CSDK_DEMODlg::OnButtGetLiveimage() 
{
	// TODO: Add your control notification handler code here
	
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_GET_RAWIMAGE, TRUE);
	
	CSDK_DEMODlgWrap_cancel _wrap_cancel_(this);

	m_strResult = _T("Downloading Live Image...");
	UpdateData(FALSE);

	while(m_bContinue)
	{
		if(LoadingImageLive() < 0)
		{
			return;
		}
		SetBmp(TRUE);
	}

	m_strResult = _T("");
	UpdateData(FALSE);
}

void CSDK_DEMODlg::OnButtSaveImage() 
{
	// TODO: Add your control notification handler code here
	CSDK_DEMODlgWrap _wrap_(this, IDC_BUTT_SAVE_IMAGE);
	
	if(!UpdateData(TRUE))
		return;
	
	if (gImageType == fp_bmp_none)
	{
		m_strResult = _T("No Image!");
		UpdateData(FALSE);
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	CString m_strFilePath = file_name_get(FILE_NAME_GET_BMP, FALSE);
	if(m_strFilePath == _T(""))
		return;
	
	if(gImageType == fp_bmp_256)
		fp_bmp_save(m_strFilePath, &fp_bmp256, gbyImg8bit);
	if(gImageType == fp_bmp_raw)
		fp_bmp_save(m_strFilePath, &fp_bmpraw, gbyImgRaw);
	//////////////////////////////////////////////////////////////////////////
	
	m_strResult = _T("Save Image OK!");
	UpdateData(FALSE);
}

void CSDK_DEMODlg::OnButtCancel() 
{
	// TODO: Add your control notification handler code here
	m_bContinue = FALSE;
	m_strResult = _T("");
	
}

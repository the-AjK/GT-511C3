// SDK_DEMODlg.h : header file
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMODlg dialog

#define UDM_SETPOS32            (WM_USER+113)
#define UDM_GETPOS32            (WM_USER+114)
class CMySpinButtonCtrl : public CSpinButtonCtrl
{
public:
	int GetPos32(LPBOOL lpbError = NULL) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, UDM_GETPOS32, 0, (LPARAM)lpbError); }
	int SetPos32(int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, UDM_SETPOS32, 0, (LPARAM)nPos); }
};

class CSDK_DEMODlg : public CDialog
{
// Construction
public:
	CSDK_DEMODlg(CWnd* pParent = NULL);	// standard constructor

	BOOL m_bImageShow;
	BOOL m_bContinue;
	BOOL m_ing;

	short m_nTop, m_nLeft;
	int m_nAverageLevel;
	BOOL m_bMechanicalOK, m_bGrayOK;
	BOOL m_bShowIdealFrame;
	CPen	m_penForResult;
	CBrush	m_brushForOkResult, m_brushForNoResult;

	void EnableBtn(BOOL bEnable, BOOL bDisableAll = FALSE);
	int  Capturing(BOOL bBest = FALSE);
	int  LoadingImage(void);
	int  LoadingImageRaw(void);
	int  LoadingImageLive(void);
	void DisplayErr(int nNackInfo, int nPos);
	void SetBmp(BOOL bIsRaw = FALSE);

// Dialog Data
	//{{AFX_DATA(CSDK_DEMODlg)
	enum { IDD = IDD_SDK_DEMO_DIALOG };
	int		m_nPortNumber;
	int		m_nBaudrate;
	CMySpinButtonCtrl	m_spinID;
	CProgressCtrl	m_prog;
	CString	m_strResult;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDK_DEMODlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:

// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CSDK_DEMODlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtOpen();
	afx_msg void OnButtClose();
	afx_msg void OnButtEnroll();
	afx_msg void OnButtVerify();
	afx_msg void OnButtIdentify();
	afx_msg void OnButtVerifyTemplate();
	afx_msg void OnButtIdentifyTemplate();
	afx_msg void OnButtIspressfp();
	afx_msg void OnButtGetImage();
	afx_msg void OnButtGetRawimage();
	afx_msg void OnButtGetUserCount();
	afx_msg void OnButtDelete();
	afx_msg void OnButtDeleteAll();
	afx_msg void OnButtGetTemplate();
	afx_msg void OnButtSetTemplate();
	afx_msg void OnButtGetDatabase();
	afx_msg void OnButtSetDatabase();
	afx_msg void OnButtFwUpgrade();
	afx_msg void OnButtIsoUpgrade();
	afx_msg void OnButtGetLiveimage();
	afx_msg void OnButtSaveImage();
	afx_msg void OnButtCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

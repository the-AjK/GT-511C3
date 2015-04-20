// SDK_DEMO.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SDK_DEMO.h"
#include "SDK_DEMODlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMOApp

BEGIN_MESSAGE_MAP(CSDK_DEMOApp, CWinApp)
	//{{AFX_MSG_MAP(CSDK_DEMOApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMOApp construction

CSDK_DEMOApp::CSDK_DEMOApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSDK_DEMOApp object

CSDK_DEMOApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSDK_DEMOApp initialization

BOOL CSDK_DEMOApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//////////////////////////////////////////////////////////////////////////
	if (!FirstInstance())
		return FALSE;
	if (!Copy2System())
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	WNDCLASS wc;

	// Get the info for this class.
         // #32770 is the default class name for dialogs boxes.
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);

	// Change the name of the class.
	wc.lpszClassName = _T("SDK_DEMO_CLASS_{F3A7151B-CC6B-4D4D-AB89-7838044565AC}");

	// Register this class so that MFC can use it.
	AfxRegisterClass(&wc);	
	//////////////////////////////////////////////////////////////////////////
	
	CSDK_DEMODlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CSDK_DEMOApp::FirstInstance()
{
	CWnd *PrevCWnd, *ChildCWnd;
	
	if (PrevCWnd = CWnd::FindWindow(_T("SDK_DEMO_CLASS_{F3A7151B-CC6B-4D4D-AB89-7838044565AC}"),NULL))
	{
		ChildCWnd=PrevCWnd->GetLastActivePopup();
		
		PrevCWnd->ShowWindow(SW_SHOW);
		PrevCWnd->SetForegroundWindow();

		if (PrevCWnd != ChildCWnd)
			ChildCWnd->SetForegroundWindow();
		
		return FALSE;
	}
	else
		return TRUE;  // First instance. Proceed as normal.
}

inline CString __getappdir(CString& strFileName)
{
	_TCHAR path_buffer[_MAX_PATH];
	_TCHAR drive[_MAX_DRIVE];
	_TCHAR dir[_MAX_DIR];
	_TCHAR fname[_MAX_FNAME];
	_TCHAR ext[_MAX_EXT];
	
	GetModuleFileName(NULL, path_buffer, _MAX_PATH);
	_tsplitpath( path_buffer, drive, dir, fname, ext );

	strFileName = fname;
	strFileName += ext;

	CString strDir = drive;
	strDir += dir;
	return strDir;
};

CString proc__path_combine(LPCTSTR szFolderName, LPCTSTR szFileName)
{
	CString strFullPath;
	PathCombine(strFullPath.GetBuffer(MAX_PATH), szFolderName, szFileName);
	strFullPath.ReleaseBuffer();
	return strFullPath;
}

BOOL CSDK_DEMOApp::Copy2System()
{
	CString strAppFileName;
	CString strAppdir = __getappdir(strAppFileName);

	TCHAR dosDrive[8] = _T("C:");
	dosDrive[0] = strAppdir[0];
	UINT DriveType = GetDriveType(dosDrive);

	if(DriveType==DRIVE_REMOVABLE || DriveType==DRIVE_CDROM)//maybe the executable is in his CD drive
	{
		TCHAR szPath[MAX_PATH];
		CString strSrc, strDst, strExe;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		
		if(!SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, TRUE))
			goto lError;

		strSrc = proc__path_combine(strAppdir, strAppFileName);
		strExe = strDst = proc__path_combine(szPath, strAppFileName);

		if (strSrc.CompareNoCase(strDst) == 0)
			return TRUE;
		
		SetFileAttributes(strDst, FILE_ATTRIBUTE_NORMAL);
		if (!CopyFile(strSrc, strDst, FALSE))
			goto lError;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		CreateProcess(strExe, NULL, NULL, NULL, FALSE, 0, NULL, szPath, &si, &pi);
		
 		return FALSE;
	}
	else
		return TRUE;

lError:
	AfxMessageBox(_T("Cannot open My Document folder for write!"));
	return FALSE;
}

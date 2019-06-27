// IGUIDE.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "MainFrm.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIGUIDEApp

BEGIN_MESSAGE_MAP(CIGUIDEApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIGUIDEApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CIGUIDEApp construction

CIGUIDEApp::CIGUIDEApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART_ASPECTS;

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IGUIDE.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

}

// The one and only CIGUIDEApp object
CIGUIDEApp theApp;

// CIGUIDEApp initialization

BOOL CIGUIDEApp::InitInstance()
{
	CWinAppEx::InitInstance();
	EnableTaskbarInteraction(FALSE);
	VERIFY(EnableD2DSupport());

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("AG Harmening"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIGUIDEDoc),
		RUNTIME_CLASS(CMainFrame), // custom child frame
		RUNTIME_CLASS(CIGUIDEView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

int CIGUIDEApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	return CWinAppEx::ExitInstance();
}

// CIGUIDEApp message handlers


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedAo();
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedHdf5();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_STN_CLICKED(IDB_AO, &CAboutDlg::OnStnClickedAo)
	ON_WM_SIZE()
	ON_STN_CLICKED(IDC_HDF5, &CAboutDlg::OnStnClickedHdf5)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// App command to run the dialog
void CIGUIDEApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIGUIDEApp message handlers


void CAboutDlg::OnStnClickedAo()
{
	// TODO: Add your control notification handler code here
	(32 >= (int)ShellExecute(NULL, L"open", L"http://ao.ukbonn.de", NULL, NULL, SW_SHOWNORMAL));
}



BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	wchar_t filename[130];
	GetModuleFileName(NULL, filename, 128);

	DWORD dwDummy;
	DWORD dwFVISize = GetFileVersionInfoSize(filename, &dwDummy);

	LPBYTE lpVersionInfo = new BYTE[dwFVISize];

	GetFileVersionInfo(filename, 0, dwFVISize, lpVersionInfo);

	UINT uLen;
	VS_FIXEDFILEINFO *lpFfi;

	VerQueryValue(lpVersionInfo, _T("\\"), (LPVOID *)&lpFfi, &uLen);

	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

	delete[] lpVersionInfo;

	DWORD dwLeftMost = HIWORD(dwFileVersionMS);
	DWORD dwSecondLeft = LOWORD(dwFileVersionMS);
	DWORD dwSecondRight = HIWORD(dwFileVersionLS);
	DWORD dwRightMost = LOWORD(dwFileVersionLS);

	CStringW str;

	str.Format(L"Version: %d.%d.%d.%d\n", dwLeftMost, dwSecondLeft,
		dwSecondRight, dwRightMost);

	CWnd *label = GetDlgItem(IDC_VERSION);
	label->SetWindowTextW(str);

	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT1);

	// get the initial text length
	int nLength = edit->GetWindowTextLength();
	// put the selection at the end of text
	edit->SetSel(nLength, nLength);
	// replace the selection
	str = L"THIS SOFTWARE IS PROVIDED BY AG HARMENING\r\nAND THE CONTRIBUTORS AS IS WITH NO WARRANTY\r\nOF ANY KIND, EITHER EXPRESSED OR IMPLIED.\r\nIN NO EVENT SHALL AG HARMENING OR THE CONTRIBUTORS\r\nBE LIABLE FOR ANY DAMAGES SUFFERED BY THE USERS\r\nARISING OUT OF THE USE OF THIS SOFTWARE,\r\nEVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
	edit->ReplaceSel(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDlg::OnStnClickedHdf5()
{
	// TODO: Add your control notification handler code here
	(32 >= (int)ShellExecute(NULL, L"open", L"https://support.hdfgroup.org/ftp/HDF5/documentation/doc1.8/Copyright.html", NULL, NULL, SW_SHOWNORMAL));
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_HDF5)->GetSafeHwnd()) {
			pDC->SetTextColor(RGB(0, 0, 255));
			pDC->SetBkColor(RGB(240, 240, 240));

			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

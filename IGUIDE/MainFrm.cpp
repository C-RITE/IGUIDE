
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "IGUIDE.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_MESSAGE(DOC_IS_READY, PopulateProperties)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_PROPERTIES, &CMainFrame::OnEditProperties)
	ON_COMMAND(ID_VIEW_STATUS_BAR, &CMainFrame::OnViewStatusBar)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here


	if (CSockClient::SocketInit() != 0)
	{
		AfxMessageBox(L"Unable to initialize Windows Sockets", MB_OK | MB_ICONERROR, 0);
		return;
	}

	if (!m_sock.Create() ||
		!m_sock.Connect(_T("127.0.0.1"), 1400))
	{
		TCHAR szErrorMsg[WSA_ERROR_LEN];
		CSockClient::WSAGetLastErrorMessage(szErrorMsg);

		CString error;
		error.Format(L"Socket Create/Connect failed Error:\n%s", szErrorMsg);
		AfxMessageBox(error, MB_OK | MB_ICONERROR, 0);
		//TRACE(_T("Socket Create/Connect failed Error: %s"), szErrorMsg);
		return;
	}


}

CMainFrame::~CMainFrame()
{
	
}

LRESULT CMainFrame::PopulateProperties(WPARAM w, LPARAM l) {

	m_DlgProperties.fillProperties();
	return 0L;

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_DlgProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_DlgProperties);
	
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.ShowWindow(SW_SHOW);

	return 0;

}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_DlgProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), FALSE, ID_VIEW_PROPERTIESWND, WS_CHILD | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	cs.cx = WINDOW_WIDTH;
	cs.cy = WINDOW_HEIGHT;
	cs.x = 320;
	cs.y = 180;

	return TRUE;
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	
	// TODO: Add your message handler code here
	CFrameWndEx::OnShowWindow(bShow, nStatus);
	static bool bOnce = true;

	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(L"Settings", L"WP_Main", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
		}
		delete[] lwp;
	}

}


void CMainFrame::OnClose()
{

	GetActiveView()->SendMessage(WM_CLOSE);
	// TODO: Add your message handler code here and/or call default
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"WP_Main", (LPBYTE)&wp, sizeof(wp));
	CFrameWndEx::OnClose();
	
}


void CMainFrame::OnEditProperties()
{
	if (m_DlgProperties.IsVisible())
		m_DlgProperties.ShowPane(FALSE, FALSE, TRUE);
	else
		m_DlgProperties.ShowPane(TRUE, FALSE, TRUE);
	// TODO: Add your command handler code here
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	int ret;
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case 'R':
			char msg = (char)pMsg->wParam;
			ret = m_sock.Send(&msg, 1, 0);
		}
	}
	return CFrameWndEx::PreTranslateMessage(pMsg);
}


void CMainFrame::OnViewStatusBar()
{
	m_wndStatusBar.ShowWindow(TRUE);
	// TODO: Add your command handler code here
}

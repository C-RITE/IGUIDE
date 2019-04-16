
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
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINK1, &CMainFrame::OnUpdatePage)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINK2, &CMainFrame::OnUpdatePage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_LINK1,
	ID_INDICATOR_LINK2,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
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

	CSockClient::SocketInit();

	// try to remote connect to host applications (ICANDI / AOSACA) in 1sec intervals
	SetTimer(0, 1000, NULL); 

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
			char msg;
		case 'R':
			msg = (char)pMsg->wParam;
			ret = m_pSock_ICANDI.Send(&msg, 1, 0);
			break;
		case VK_SPACE:
			msg = 'V';
			ret = m_pSock_ICANDI.Send(&msg, 1, 0);
			break;
		case VK_RETURN:
			msg = 'F';
			ret = m_pSock_AOSACA.Send(&msg, 1, 0);
			break;
		case 0x6B:
			msg = '+';
			ret = m_pSock_AOSACA.Send(&msg, 1, 0);
			break;
		case 0x6D :
			msg = '-';
			ret = m_pSock_AOSACA.Send(&msg, 1, 0);
			break;
		case VK_NUMPAD0:
			msg = '0';
			ret = m_pSock_AOSACA.Send(&msg, 1, 0);
			break;
		}
	}
	return CFrameWndEx::PreTranslateMessage(pMsg);
}


void CMainFrame::OnViewStatusBar()
{
	m_wndStatusBar.ShowWindow(TRUE);
	// TODO: Add your command handler code here
}

void CMainFrame::OnUpdatePage(CCmdUI *pCmdUI)
{
	// connection status of ICANDI/AOSACA

	switch (pCmdUI->m_nID) {

		case ID_INDICATOR_LINK1:
	
			if (m_pSock_ICANDI.IsConnected()) {
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			}
			else
			{
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(200, 0, 0), 1);
			}

		break;

		case ID_INDICATOR_LINK2:

			if (m_pSock_AOSACA.IsConnected()) {
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			}
			else
			{
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(200, 0, 0), 1);
			}

		break;

	}


}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CString IP = m_DlgProperties.getAOSACAIP();
	if (IP == L"localhost")
		IP.SetString(L"127.0.0.1");
	if (!m_pSock_AOSACA.IsConnected())
		m_pSock_AOSACA.Connect(IP,1500);


	CFrameWndEx::OnTimer(nIDEvent);
}

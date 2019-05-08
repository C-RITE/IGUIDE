
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
	ON_MESSAGE(DOC_IS_READY, OnDocumentReady)
	ON_MESSAGE(NETCOM_ERROR, ConnectionFailure)
	ON_MESSAGE(NETCOM_CLOSED, ConnectionClosed)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_PROPERTIES, &CMainFrame::OnEditProperties)
	ON_COMMAND(ID_VIEW_STATUS_BAR, &CMainFrame::OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ICANDI, &CMainFrame::OnUpdateLinkIndicators)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_AOSACA, &CMainFrame::OnUpdateLinkIndicators)
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ICANDI,
	ID_INDICATOR_AOSACA,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_pSock_AOSACA = NULL;
	
}

CMainFrame::~CMainFrame()
{
	delete m_pSock_AOSACA;
}

LRESULT CMainFrame::OnDocumentReady(WPARAM w, LPARAM l) {

	// insert all parameters into property list
	m_DlgProperties.fillProperties();

	// now that all properties are in place (i.e. IP-address)
	// we can try to establish remote control
	
	Connect2AOSACA();
	Connect2ICANDI();

	return 0L;

}

LRESULT CMainFrame::ConnectionFailure(WPARAM w, LPARAM l) {

	int errorID(w);
	CString errorDescription((LPCTSTR)l);

	CString errorMessage = L"Couldn't connect to AOSACA!\nReason: " + errorDescription + ".";

	int answer = AfxMessageBox(errorMessage, MB_RETRYCANCEL);

	if (answer == IDRETRY || answer == IDCANCEL) {
		delete m_pSock_AOSACA;
		m_pSock_AOSACA = NULL;
	}

	if (answer == IDRETRY)
		Connect2AOSACA();

	return 0L;

}

LRESULT CMainFrame::ConnectionClosed(WPARAM w, LPARAM l) {

	delete m_pSock_AOSACA;
	m_pSock_AOSACA = NULL;
	return 0;

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

			// ICANDI remote commands go here
		case 'R':
			msg = (char)pMsg->wParam;
			ret = m_pSock_ICANDI->Send(&msg, 1, 0);
			break;
		case VK_SPACE:
			msg = 'V';
			ret = m_pSock_ICANDI->Send(&msg, 1, 0);
			break;

			// AOSACA remote commands go here
		case VK_RETURN:
			msg = 'F';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case 0x6B:
			msg = '+';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case 0x6D:
			msg = '-';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case VK_NUMPAD0:
			msg = '0';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
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

void CMainFrame::OnUpdateLinkIndicators(CCmdUI *pCmdUI)
{
	// connection status of ICANDI/AOSACA
	
	switch (pCmdUI->m_nID) {

		/*case ID_INDICATOR_ICANDI:
	
			if (m_pSock_ICANDI->IsConnected()) {
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

		break;*/

		case ID_INDICATOR_AOSACA:

			if (m_pSock_AOSACA == NULL) {

				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(200, 0, 0), 1);
				return;
			}

			if	(m_pSock_AOSACA->IsConnected()) {
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			}
			
			else if (m_pSock_AOSACA->pending)
			{
				pCmdUI->Enable();
				m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(100, 100, 100), 1);
				m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(255, 188, 0), 1);
			}

		break;

	}

}


bool CMainFrame::Connect2AOSACA()
{
	// TODO: Add your implementation code here.
	/*if (WSAGetLastError() == WSAEINPROGRESS || WSAGetLastError() == WSAEALREADY || m_pSock_AOSACA.IsConnected())
		return false;*/

	if (m_pSock_AOSACA == NULL) {
		m_pSock_AOSACA = new CSockClient();
		CSockClient::SocketInit();
		m_pSock_AOSACA->Create();
		m_pSock_AOSACA->pending = true;
		m_pSock_AOSACA->setParent(this);
	}
	
	CString IP = m_DlgProperties.getAOSACAIP();
	if (!m_pSock_AOSACA->Connect(IP, 1500)) {
		delete m_pSock_AOSACA;	
		m_pSock_AOSACA = NULL;
		Connect2AOSACA();
		return false;
	}

	return true;

}


bool CMainFrame::Connect2ICANDI()
{
	// TODO: Add your implementation code here.
	return false;
}



void CMainFrame::OnParentNotify(UINT message, LPARAM lParam)
{
	// TODO: Add your message handler code here
	CFrameWndEx::OnParentNotify(message, lParam);

	if (!m_wndStatusBar)
		return;

	WORD x_coord = LOWORD(lParam);
	WORD y_coord = HIWORD(lParam);
	POINT pt{ x_coord, y_coord };
	RECT rect_AOSACA;
	RECT clientRect;
	RECT statusBarRect;

	GetClientRect(&clientRect);
	m_wndStatusBar.GetClientRect(&statusBarRect);
	pt.y -= clientRect.bottom - statusBarRect.bottom;
	

	switch (message) {
		case WM_LBUTTONDOWN:
			m_wndStatusBar.GetItemRect(2, &rect_AOSACA);
			if (PtInRect(&rect_AOSACA, pt) && m_pSock_AOSACA == NULL)
				Connect2AOSACA();
			break;

	}

}

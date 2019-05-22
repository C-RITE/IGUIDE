
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
	
}

CMainFrame::~CMainFrame()
{
	
}

LRESULT CMainFrame::OnDocumentReady(WPARAM w, LPARAM l) {

	// insert all properties into list
	m_DlgProperties.fillProperties();

	// now that all properties are in place (i.e. IP-address, etc.)
	// we can try to establish the desired remote control options
	
	RemoteControl.init(&m_DlgProperties);
	RemoteControl.connect();

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
	m_wndStatusBar.ShowWindow(TRUE);

	// create dummy remote control window
	// MFC demand to call this, else there will be no CWnd* to show messageboxes
	if (!RemoteControl.Create(_T("STATIC"), _T(""), NULL, CRect(0, 0, 0, 0), this, 1234))
	{
		TRACE0("Failed to create remote control\n");
		return -1;      // fail to create
	}
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


void CMainFrame::OnViewStatusBar()
{
	m_wndStatusBar.ShowWindow(TRUE);
	// TODO: Add your command handler code here
}

void CMainFrame::OnUpdateLinkIndicators(CCmdUI *pCmdUI)
{
	// connection status of ICANDI/AOSACA
	// 'traffic lights' paradigm

	Connection active = RemoteControl.getActiveConnections();
	Connection pending = RemoteControl.getPendingConnections();

	switch (pCmdUI->m_nID) {

	case ID_INDICATOR_ICANDI:

		if ((active == ICANDI) | (active == BOTH))
		{
			pCmdUI->Enable();
			m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
			m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			return;
		}

		if ((pending == ICANDI) | (pending == BOTH))
		{
			pCmdUI->Enable();
			m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(100, 100, 100), 1);
			m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(255, 188, 0), 1);
			return;
		}

		pCmdUI->Enable();
		m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
		m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(200, 0, 0), 1);

		break;

	case ID_INDICATOR_AOSACA:

		if ((active == AOSACA) | (active == BOTH))
		{
			pCmdUI->Enable();
			m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
			m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			return;
		}

		if ((pending == AOSACA) | (pending == BOTH))
		{
			pCmdUI->Enable();
			m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(100, 100, 100), 1);
			m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(255, 188, 0), 1);
			return;
		}

		pCmdUI->Enable();
		m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
		m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(200, 0, 0), 1);

		break;

	}

}

void CMainFrame::OnParentNotify(UINT message, LPARAM lParam)
{
	// Manual override switch for AOSACA / ICANDI connection

	// This is necessary to catch clicks in status bar, which is outside of the client window.
	// It is more of a workaround, because status bars are usually inclined to passive behaviour.

	CFrameWndEx::OnParentNotify(message, lParam);

	if (!m_wndStatusBar)
		return;

	WORD x_coord = LOWORD(lParam);
	WORD y_coord = HIWORD(lParam);
	POINT pt{ x_coord, y_coord };
	RECT rect_AOSACA;
	RECT rect_ICANDI;
	RECT clientRect;
	RECT statusBarRect;

	GetClientRect(&clientRect);
	m_wndStatusBar.GetClientRect(&statusBarRect);
	pt.y -= clientRect.bottom - statusBarRect.bottom;
	
	// trigger connection attempt upon left click on status item

	switch (message) {
		case WM_LBUTTONDOWN:
			m_wndStatusBar.GetItemRect(2, &rect_AOSACA);
			if (PtInRect(&rect_AOSACA, pt))
				RemoteControl.connect(AOSACA);
			m_wndStatusBar.GetItemRect(1, &rect_ICANDI);
			if (PtInRect(&rect_ICANDI, pt))
				RemoteControl.connect(ICANDI);
			break;

	}

}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// route keyboard input to remote control
	RemoteControl.PreTranslateMessage(pMsg);

	// then return to sender
	return CFrameWndEx::PreTranslateMessage(pMsg);

}

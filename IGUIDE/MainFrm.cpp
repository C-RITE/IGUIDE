
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_MESSAGE(DOC_IS_READY, OnDocumentReady)
	ON_MESSAGE(GAMEPAD_UPDATE, OnGamePadUpdate)
	ON_MESSAGE(MOUSE_FALLBACK, OnMouseFallback)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_VIEW_PROPERTIES, &CMainFrame::OnViewProperties)
	ON_COMMAND(ID_VIEW_REGIONS, &CMainFrame::OnViewRegions)
	ON_COMMAND(ID_VIEW_STATUS_BAR, &CMainFrame::OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ICANDI, &CMainFrame::OnUpdateLinkIndicators)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_AOSACA, &CMainFrame::OnUpdateLinkIndicators)
	ON_WM_PARENTNOTIFY()
	ON_MESSAGE(RESET_AOSACA_IP, &CMainFrame::OnResetAosacaIp)
	ON_MESSAGE(RESET_ICANDI_IP, &CMainFrame::OnResetIcandiIp)
	ON_MESSAGE(SAVE_IGUIDE_CSV, &CMainFrame::OnSaveIguideCsv)
	ON_MESSAGE(PATCH_TO_REGIONPANE, &CMainFrame::OnPatchToRegionPane)
	ON_MESSAGE(UPDATE_REGIONPANE, &CMainFrame::OnUpdateRegionPane)
	ON_MESSAGE(FINISH_PATCHJOB, &CMainFrame::OnFinishPatchjob)
	ON_MESSAGE(PATCH_SELECT, &CMainFrame::OnPatchSelect)
	ON_MESSAGE(CLEAR_REGIONPANE, &CMainFrame::OnClearRegionpane)
	ON_MESSAGE(BROWSE_PATCH, &CMainFrame::OnBrowsePatch)
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
	m_hSaveEvent = CreateEvent(NULL, FALSE, FALSE, L"SAVE_FROM_IGUIDE_VIDEOFOLDER");
}

CMainFrame::~CMainFrame()
{
	
}

LRESULT CMainFrame::OnDocumentReady(WPARAM w, LPARAM l) {
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)l;
	m_pDoc = pDoc;

	// insert all properties into list
	m_PropertyPane.fillProperties();

	// now that all properties are in place (i.e. IP-address, etc.)
	// we can try to establish the desired remote control capability

	RemoteControl.init(&m_PropertyPane, pDoc->m_pInputBuf, pDoc->m_hNetMsg);
	RemoteControl.connect();

	return 0L;

}

CIGUIDEDoc* CMainFrame::GetDoc()
{
	// Get Doc, made for other classes that need access to attributes
	// NOTE: EXCEPTIONS thrown here are most likely caused by initialization failures in OnNewDocument()

	CFrameWndEx * pFrame = (CFrameWndEx*)AfxGetApp()->GetMainWnd();
	CDocument* doc = pFrame->GetActiveDocument();
	if (doc)
		return (CIGUIDEDoc*)doc;
	else
		return NULL;

}

LRESULT CMainFrame::OnGamePadUpdate(WPARAM w, LPARAM l) {

	CIGUIDEView* pView = (CIGUIDEView*)GetActiveView();

	if (w == 1)
		pView->m_pDlgTarget->OnGamePadCalibration(); // we hit a button!
	else 
		pView->m_pDlgTarget->Invalidate();			 // move the cursor..

	return 0;

}


LRESULT CMainFrame::OnMouseFallback(WPARAM w, LPARAM l) {
	
	CIGUIDEView* pView = (CIGUIDEView*)GetActiveView();
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)pView->GetDocument();
	pDoc->m_InputController = L"Mouse";
	m_PropertyPane.fillProperties();
	pDoc->m_Controller.reset();

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
	EnableAutoHidePanes(CBRS_ALIGN_LEFT);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

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
	CString strDockablePane;

	// Create properties window
	bNameValid = strDockablePane.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_PropertyPane.Create(strDockablePane, this, CRect(0, 0, 250, 250), TRUE, ID_VIEW_PROPERTIES_WND, WS_CHILD | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	bNameValid = strDockablePane.LoadString(IDS_REGIONS_WND);
	ASSERT(bNameValid);

	if (!m_RegionPane.Create(strDockablePane, this, CRect(0, 0, 250, 250), TRUE, ID_VIEW_REGIONS_WND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	m_PropertyPane.EnableDocking(CBRS_ALIGN_LEFT);
	DockPane(&m_PropertyPane);

	m_RegionPane.EnableDocking(CBRS_ALIGN_RIGHT);
	DockPane(&m_RegionPane);

	return TRUE;

}


void CMainFrame::OnViewProperties()
{
	// TODO: Add your command handler code here
	if (m_PropertyPane.IsVisible()) {
		m_PropertyPane.ShowPane(FALSE, FALSE, TRUE);
		SetFocus();
	}
	else
		m_PropertyPane.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnViewRegions() {

	if (m_RegionPane.IsVisible()) {
		m_RegionPane.ShowPane(FALSE, FALSE, TRUE);
		SetFocus();
	}
	else
		m_RegionPane.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnViewStatusBar()
{
	// TODO: Add your command handler code here
	m_wndStatusBar.ShowWindow(TRUE);
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
	if (RemoteControl.getActiveConnections() > 0) {

		CWnd* active = GetActiveView();
		CWnd* focus = GetFocus();

		// route keyboard input to remote control
		// but only if the view has focus (not property pane, etc.)
		if (active == focus)
			RemoteControl.PreTranslateMessage(pMsg);

	}

	// call base class
	return CFrameWndEx::PreTranslateMessage(pMsg);

}


afx_msg LRESULT CMainFrame::OnResetAosacaIp(WPARAM wParam, LPARAM lParam)
{

	Connection pending = RemoteControl.getPendingConnections();
	Connection active = RemoteControl.getActiveConnections();

	if ((pending == AOSACA || pending == BOTH) || (active == AOSACA || active == BOTH)) {
		int answer = MessageBox(L"Remote connection to AOSACA is currently pending or active.\nReset connection now?", L"Attention", MB_ICONHAND | MB_YESNO);
		if (answer == IDYES) {
			RemoteControl.setIPAdress(AOSACA);
			RemoteControl.connect(AOSACA);
		}
	}

	else
		RemoteControl.setIPAdress(AOSACA);

return 0;

}


afx_msg LRESULT CMainFrame::OnResetIcandiIp(WPARAM wParam, LPARAM lParam)
{

	Connection pending = RemoteControl.getPendingConnections();
	Connection active = RemoteControl.getActiveConnections();

	if ((pending == ICANDI || pending == BOTH) || (active == ICANDI || active == BOTH )) {
		int answer = MessageBox(L"Remote connection to ICANDI is currently pending or active.\nReset connection now?", L"Attention", MB_ICONHAND | MB_YESNO);
		if (answer == IDYES) {
			RemoteControl.setIPAdress(ICANDI);
			RemoteControl.connect(ICANDI);
		}
	}

	else
		RemoteControl.setIPAdress(ICANDI);

return 0;

}

DWORD WINAPI SaveThread(LPVOID pParam) {

	CMainFrame* parent = (CMainFrame*)pParam;
	Connection active = parent->RemoteControl.getActiveConnections();
	
	if (active == ICANDI || active == BOTH) {
		WaitForSingleObject(parent->m_hSaveEvent, INFINITE);
		parent->m_pDoc->m_pCurrentOutputDir = &parent->m_pDoc->m_OutputDir_ICANDI;
		parent->m_pDoc->m_pGrid->patchlist.SaveToFile(*parent->m_pDoc->m_pCurrentOutputDir);
	}
	else {
		parent->m_pDoc->m_pCurrentOutputDir = &parent->m_pDoc->m_OutputDir;
		parent->m_pDoc->m_pGrid->patchlist.SaveToFile(*parent->m_pDoc->m_pCurrentOutputDir);
	}
	return 0;
}

afx_msg LRESULT CMainFrame::OnSaveIguideCsv(WPARAM wParam, LPARAM lParam)
{
	m_pDoc = GetDoc();
	m_hSaveThread = ::CreateThread(NULL, 0, SaveThread, this, 0, &m_thdID);
	return 0;
}


afx_msg LRESULT CMainFrame::OnPatchToRegionPane(WPARAM wParam, LPARAM lParam)
{
	int regCount = (int)lParam;
	if (regCount > m_RegionPane.getRegionSize()) {
		m_RegionPane.addRegion(regCount);
		m_RegionPane.patchItem = -1;
	}
	Patch* p = (Patch*)wParam;
	m_RegionPane.addPatch(p);
	
	return 0;

}


afx_msg LRESULT CMainFrame::OnUpdateRegionPane(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	m_RegionPane.patchItem = index;

	return 0;
}

afx_msg LRESULT CMainFrame::OnFinishPatchjob(WPARAM wParam, LPARAM lParam)
{
	int region = (int)wParam;
	m_RegionPane.finish(region);

	return 0;

}


afx_msg LRESULT CMainFrame::OnPatchSelect(WPARAM wParam, LPARAM lParam)
{
	int region = (int)wParam;
	int index = (int)lParam;

	m_pDoc->m_pGrid->selectPatch(region, index);

	m_RegionPane.patchItem = index - 1;

	return 0;

}


afx_msg LRESULT CMainFrame::OnClearRegionpane(WPARAM wParam, LPARAM lParam)
{
	m_RegionPane.clear();
	return 0;
}


afx_msg LRESULT CMainFrame::OnBrowsePatch(WPARAM wParam, LPARAM lParam)
{
	int region = (int)wParam;
	int index = (int)lParam;

	m_RegionPane.select(region, index);

	return 0;

}


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
	ON_MESSAGE(PATCH_SELECT, &CMainFrame::OnPatchSelect)
	ON_MESSAGE(CLEAR_REGIONPANE, &CMainFrame::OnClearRegionpane)
	ON_MESSAGE(UPDATE_SELECTION, &CMainFrame::OnUpdateSelection)
	ON_WM_TIMER()
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

	countDown = 0;

}

CMainFrame::~CMainFrame()
{
}

DWORD WINAPI SaveWaitThread(LPVOID pParam) {

	CMainFrame* parent = (CMainFrame*)pParam;
	Connection active = parent->RemoteControl.getActiveConnections();

	// delay writing information to logfile (and send to region pane) until netcom message receive event
	if (active == ICANDI || active == BOTH) {

		WaitForSingleObject(parent->m_pDoc->m_hSaveEvent, INFINITE);
		parent->m_pDoc->m_pCurrentOutputDir = &parent->m_pDoc->vidfolder;
		parent->StartCountDown();
	}

	// go ahead without further delay
	else {
		parent->m_pDoc->m_pCurrentOutputDir = &parent->m_pDoc->m_OutputDir;
	}

	parent->sendToRegionPane();

	parent->m_pDoc->sortPatchList();
	
	parent->m_pDoc->SaveLogFile();

	return 0;

}

void CMainFrame::GetSysTime(CString &buf) {

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y_%m_%d_%H_%M_%S", timeinfo);
	buf = buffer;

}

void CMainFrame::StartCountDown() {
	
	CIGUIDEView* pView = (CIGUIDEView*)GetActiveView();

	RemoteControl.onHold = true;
	countDown = (_ttoi(m_pDoc->m_pGrid->currentPatch->vidlength) * 1000);
	pView->SendMessage(INITIATE_COUNTDOWN, (WPARAM)&countDown, NULL);

	::SetTimer(this->m_hWnd,
		IDT_TIMER,
		countDown,
		NULL);

}

void CMainFrame::sendToRegionPane() {

	if (m_pDoc->m_pGrid->currentPatch->region > 0) 
		SendMessage(UPDATE_SELECTION,(WPARAM)&*m_pDoc->m_pGrid->currentPatch, NULL);
	else
		SendMessage(PATCH_TO_REGIONPANE,(WPARAM)&*m_pDoc->m_pGrid->currentPatch, NULL);

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

	m_pDoc->m_pActiveConnections = &active;

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

	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

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
	if (!m_PropertyPane.Create(strDockablePane, this, CRect(0, 0, 250, 250), TRUE, ID_VIEW_PROPERTIES_WND, WS_CHILD| WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBRS_LEFT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	bNameValid = strDockablePane.LoadString(IDS_REGIONS_WND);
	ASSERT(bNameValid);

	if (!m_RegionPane.Create(strDockablePane, this, CRect(0, 0, 250, 250), TRUE, ID_VIEW_REGIONS_WND, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
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

	active = RemoteControl.getActiveConnections();
	pending = RemoteControl.getPendingConnections();

	switch (pCmdUI->m_nID) {

	case ID_INDICATOR_ICANDI:

		if ((active == ICANDI) | (active == BOTH))
		{
			pCmdUI->Enable();
			m_wndStatusBar.SetPaneTextColor(pCmdUI->m_nIndex, RGB(255, 255, 255), 1);
			m_wndStatusBar.SetPaneBackgroundColor(pCmdUI->m_nIndex, RGB(0, 200, 0), 1);
			m_PropertyPane.OutputFolder->AllowEdit(false);
			m_PropertyPane.OutputFolder->SetValue(*m_pDoc->m_pCurrentOutputDir);
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

afx_msg LRESULT CMainFrame::OnSaveIguideCsv(WPARAM wParam, LPARAM lParam)
{
	m_pDoc = GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	m_hSaveWaitThread = ::CreateThread(NULL, 0, SaveWaitThread, this, 0, &m_thdID);

	return 0;
}

afx_msg LRESULT CMainFrame::OnPatchToRegionPane(WPARAM wParam, LPARAM lParam)
{
	Patch* p = (Patch*)wParam;
	
	if (p->region > m_RegionPane.getRegionSize()) {
		m_RegionPane.addRegion(p->region);
		
	}

 	m_RegionPane.addPatch(p);
	
	return 0;

}

afx_msg LRESULT CMainFrame::OnPatchSelect(WPARAM wParam, LPARAM lParam)
{
	int uID = (int)wParam;

	m_pDoc->m_pGrid->selectPatch(uID);
	m_pDoc->m_pGrid->updateCursorPatch();
	m_RegionPane.select(uID);

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	pView->m_pDlgTarget->Pinpoint(*pDoc->m_pGrid->currentPatch);

	pDoc->UpdateAllViews(NULL);

	return 0;

}

afx_msg LRESULT CMainFrame::OnClearRegionpane(WPARAM wParam, LPARAM lParam)
{
	m_RegionPane.clear();

	return 0;

}

afx_msg LRESULT CMainFrame::OnUpdateSelection(WPARAM wParam, LPARAM lParam)
{
	Patch* p = (Patch*)wParam;

	m_RegionPane.update(p);

	return 0;

}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CIGUIDEView* pView = CIGUIDEView::GetView();

	switch (nIDEvent) {

	case IDT_TIMER:

		pView->SendMessage(RESET_COUNTDOWN, NULL, NULL);
		RemoteControl.onHold = false;
		break;

	}

	CFrameWndEx::OnTimer(nIDEvent);
}
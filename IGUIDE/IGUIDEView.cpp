
// IGUIDEView.cpp : implementation of the CIGUIDEView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IGUIDE.h"
#endif
#include "MainFrm.h"
#include "IGUIDEView.h"
#include "IGUIDEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;

// CIGUIDEView

IMPLEMENT_DYNCREATE(CIGUIDEView, CView)

BEGIN_MESSAGE_MAP(CIGUIDEView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CIGUIDEView::OnDraw2d)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DISPLAYCHANGE, &CIGUIDEView::OnDisplayChange)
	ON_MESSAGE(SCREEN_SELECTED, &CIGUIDEView::ChangeTargetDisplay)
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_COMMAND(ID_FUNDUS_IMPORT, &CIGUIDEView::OnFundusImport)
	ON_WM_MOUSELEAVE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CIGUIDEView construction/destruction

CIGUIDEView::CIGUIDEView()
{

	m_pDlgTarget = NULL;
	m_pFixationTarget = NULL;
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::LightGray));
	m_bMouseTracking = false;
	m_lButtonIsDown = false;

}

CIGUIDEView::~CIGUIDEView()
{

	delete m_pDlgTarget;
	delete m_pWhiteBrush;

}

CIGUIDEView* CIGUIDEView::GetView()
{

	CFrameWndEx * pFrame = (CFrameWndEx *)(AfxGetApp()->m_pMainWnd);

	if (!pFrame)
		return NULL;

	CView * pView = pFrame->GetActiveView();

	if (!pView)
		return NULL;

	// Fail if view is of wrong kind
	// (this could occur with splitter windows, or additional
	// views on a single document

	if (!pView->IsKindOf(RUNTIME_CLASS(CIGUIDEView)))
		return NULL;

	return (CIGUIDEView*)pView;

}

// CIGUIDEView printing

BOOL CIGUIDEView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CIGUIDEView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}


void CIGUIDEView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CIGUIDEView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
		mouseStart = point;

		::SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		pDoc->m_pGrid->showCoords = false;
		pDoc->m_pGrid->showCursor = false;
		
}

void CIGUIDEView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	// calculate mouse travel distance
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	mouseDist += mousePos - mouseStart;
	pDoc->m_pGrid->showCoords = true;
	pDoc->m_pGrid->showCursor = true;
	pDoc->m_pGrid->isPanning = false;

	CView::OnRButtonUp(nFlags, point);

}


void CIGUIDEView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_lButtonIsDown)
	{
		m_lButtonIsDown = false;
		return;
	}

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	pDoc->m_pGrid->showCursor = false;
	
	if (pDoc->CheckFOV())
	{	
		// create patchjob
		if (pDoc->m_pGrid->region.size() > 0) {

			pDoc->m_pGrid->makeRegion(point);
			pDoc->m_pGrid->addRegion();
			pDoc->m_pGrid->makeRegionRects(pDoc->m_pGrid->patchlist.back().region);
			pDoc->m_pGrid->setCurrentPatch(pDoc->m_pGrid->patchlist.back().region, 1);

			m_pDlgTarget->Pinpoint(*pDoc->m_pGrid->currentPatch);
			
		}

		// add single patch
		else {
			pDoc->m_pGrid->addPatch(point);
			m_pDlgTarget->Pinpoint(*pDoc->m_pGrid->currentPatch);
			pDoc->m_pGrid->currentPatch = std::prev(pDoc->m_pGrid->patchlist.end());
		}

	}

	m_pDlgTarget->RedrawWindow();
	RedrawWindow();

	ATLTRACE(_T("patchlist: size[%d]\n"), pDoc->m_pGrid->patchlist.size());

}

void CIGUIDEView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	SetFocus();
	
	if (!m_bMouseTracking) {

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = this->m_hWnd;
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;

	}

	if (!m_pDlgTarget->calibrating)
		pDoc->m_pGrid->showCursor = true;

	mousePos = point;
	
	// pan with right mouse button
	if (nFlags & MK_RBUTTON) {

		CD2DSizeF offset(mousePos - mouseStart + mouseDist);
		offset.width *= (1 / zoom);
		offset.height *= (1 / zoom);
		translate = D2D1::Matrix3x2F::Translation(offset);

		::SetCursor(LoadCursor(NULL, IDC_SIZEALL));

		pDoc->m_pGrid->showCoords = false;
		pDoc->m_pGrid->showCursor = false;
		pDoc->m_pGrid->isPanning = true;
		
	}

	RedrawWindow();

}

void CIGUIDEView::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	pDoc->m_pGrid->showCursor = false;
	m_bMouseTracking = false;
	Invalidate();

	CView::OnMouseLeave();

}


BOOL CIGUIDEView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (nFlags & MK_SHIFT) {
		pDoc->m_pGrid->controlRegion(zDelta / WHEEL_DELTA, 0, mousePos);
	}
	else if (GetKeyState('Y') & 0x8000) {
		pDoc->m_pGrid->controlRegion(zDelta / WHEEL_DELTA, 2, mousePos);
	}
	else if (GetKeyState('X') & 0x8000) {
		pDoc->m_pGrid->controlRegion(zDelta / WHEEL_DELTA, 1, mousePos);
	}
	
	else if ((zoom + zDelta / 100) >= 1) {

		zoom += zDelta / 100;

		CD2DSizeF offset(mouseDist);
		offset.width *= (1 / zoom);
		offset.height *= (1 / zoom);

		// zoom into last mouse position
		scale = D2D1::Matrix3x2F::Scale(D2D1::SizeF(zoom, zoom), CD2DPointF(CANVAS / 2, CANVAS / 2));
		translate = D2D1::Matrix3x2F::Translation(offset);

	}

	RedrawWindow();

	return CView::OnMouseWheel(nFlags, zDelta, pt);

}

void CIGUIDEView::OnInitialUpdate()
{

	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL);
	
	ResetTransformationMatrices();

	SetFixationTarget();

	// TODO: Add your specialized code here and/or call the base class

	AfxGetMainWnd()->SendMessage(DOC_IS_READY, NULL, (LPARAM)GetDocument());

}

void CIGUIDEView::ResetTransformationMatrices() {

	CRect clientWindow;
	GetClientRect(&clientWindow);

	zoom = 2;

	CD2DSizeF size(D2D1::SizeF((clientWindow.Width() / 2) - CANVAS / 2, (clientWindow.Height() / 2) - CANVAS / 2));

	scale = D2D1::Matrix3x2F::Scale(D2D1::Size(zoom, zoom), CD2DPointF(CANVAS / 2, CANVAS / 2));
	translate = D2D1::Matrix3x2F::Translation(size.width * (1 / zoom), size.height * (1 / zoom));
	
	mouseDist.x = size.width;
	mouseDist.y = size.height;

}

LRESULT CIGUIDEView::ChangeTargetDisplay(WPARAM w, LPARAM l) {

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	CRect area = (CRect)pDoc->m_Monitors.m_pSelectedDevice->area;
	CRect wRect;
	
	m_pDlgTarget->SetWindowPos(&this->wndBottom,
		area.left - wRect.left ,
		area.top - wRect.top,
		area.Width(),
		area.Height(),
		NULL);

	m_pDlgTarget->ShowWindow(SW_SHOW);
	m_pDlgTarget->SetFixationTarget();

	return 0;

}

void CIGUIDEView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	CHwndRenderTarget* pRenderTarget = GetRenderTarget();

	CD2DPointF* FOV = new CD2DPointF[4];

	CRect clientRect;
	AfxGetMainWnd()->GetClientRect(&clientRect);

	// make edges from corners
	if (pDoc->m_raster.corner.size() == 4 && pDoc->m_raster.meanEdge == 0) {
		for (int i = 0; i < 3; i++) {
			Edge k;
			k.p = pDoc->m_raster.corner[i];
			k.q = pDoc->m_raster.corner[i + 1];
			pDoc->m_raster.perimeter.push_back(k);
		}
		for (int i = 0; i < 4; i++)
			FOV[i] = pDoc->m_raster.corner[i];
		pDoc->m_raster.mid = pDoc->compute2DPolygonCentroid(FOV, 4);
		Edge k;
		k.p = pDoc->m_raster.corner[3];
		k.q = pDoc->m_raster.corner[0];
		pDoc->m_raster.perimeter.push_back(k);
	}

	// calculate length of edges, mean edge length and angle
	if (pDoc->m_raster.corner.size() == 4 && pDoc->m_raster.meanEdge == 0) {
		pDoc->ComputeDisplacementAngles();

		if (pDoc->CheckCalibrationValidity()) {
			for (size_t i = 0; i < pDoc->m_raster.perimeter.size(); i++) {
				pDoc->m_raster.perimeter[i].length = pDoc->CalcEdgeLength(pDoc->m_raster.perimeter[i]);
				pDoc->m_raster.meanEdge += pDoc->m_raster.perimeter[i].length;
				pDoc->m_raster.meanAlpha += pDoc->m_raster.perimeter[i].alpha;
			}

			pDoc->m_raster.meanEdge /= 4;
			pDoc->m_raster.meanAlpha /= 4;
		}
		
		else {
			if (pDoc->m_InputController == L"Gamepad" && pDoc->m_Controller.m_bActive) {
				pDoc->m_Controller.m_pThread->SuspendThread();
			}
			AfxMessageBox(L"Calibration failed. Please retry!", MB_OK | MB_ICONSTOP);
			if (pDoc->m_InputController == L"Gamepad")
				pDoc->m_Controller.m_pThread->ResumeThread();
			m_pDlgTarget->restartCalibration();
			m_pDlgTarget->OnLButtonDown(0, 0);
		}
	
	}

	ASSERT_VALID(pRenderTarget);

	if (pDoc->m_pFundus->fundus) {
		pDoc->m_pFundus->picture = new CD2DBitmap(pRenderTarget, pDoc->m_pFundus->fundus, 1);
	}

	SetFixationTarget();
	
	m_pDlgTarget->SetFixationTarget();
	m_pDlgTarget->Invalidate();

	Invalidate();

	delete FOV;

}

void CIGUIDEView::SetFixationTarget() {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	if (!pDoc)
		return;

	if (m_pFixationTarget && m_pFixationTarget->IsValid())
		delete m_pFixationTarget;

	m_pFixationTarget = new CD2DBitmap(GetRenderTarget(), pDoc->m_FixationTarget, CD2DSizeU(0, 0), TRUE);

}

int CIGUIDEView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Enable D2D support for this window:
	EnableD2DSupport();

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	CHwndRenderTarget* pRenderTarget = GetRenderTarget();

	pDoc->m_pGrid->CreateD2DResources(pRenderTarget);
	pDoc->m_pGrid->CreateGridGeometry(pRenderTarget);
   
	return 0;

}

// CIGUIDEView drawing

LRESULT CIGUIDEView::OnDraw2d(WPARAM wParam, LPARAM lParam)
{

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);	

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	 if (!pDoc)	return FALSE;

	// TODO: add draw code for native data here
	if (pRenderTarget) {

		// clear background
		pRenderTarget->Clear(ColorF(ColorF::Black));

		// combine transforms
		pRenderTarget->SetTransform(translate * scale);

		// draw fundus
		pDoc->m_pFundus->Paint(pRenderTarget);

		// draw Grid
		pDoc->m_pGrid->DrawGrid(pRenderTarget);

		// draw patchjob
		pDoc->m_pGrid->DrawRegions(pRenderTarget);

		// draw extras (optic disc, crosshair, etc..)
		pDoc->m_pGrid->DrawExtras(pRenderTarget);

		// draw target zone
		pDoc->m_pGrid->DrawTargetZone(pRenderTarget);

		// draw patches
		pDoc->m_pGrid->DrawPatches(pRenderTarget);

		// disable transforms
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// draw cursor on mousepointer
		if (pDoc->m_pGrid->region.size() > 1)
			pDoc->m_pGrid->DrawRegion(pRenderTarget, mousePos, zoom);
		else
			pDoc->m_pGrid->DrawPatchCursor(pRenderTarget, mousePos, zoom);

		// draw debug info
#ifdef DEBUG
		pDoc->m_pGrid->DrawDebug(pRenderTarget);
#endif
		// draw current patch location and defocus value from AOSACA
		pDoc->m_pGrid->DrawLocation(pRenderTarget);

		// draw quickhelp
		pDoc->m_pGrid->DrawQuickHelp(pRenderTarget);
		
		// is (custom) fixation target on or off?
		if (m_pDlgTarget->m_bVisible)
			pDoc->m_pGrid->DrawTarget(pRenderTarget, m_pFixationTarget);

	}

	return 0;

}

void CIGUIDEView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	// all the drawing happens in OnDraw2D

}

BOOL CIGUIDEView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	// keyboard controls depending on drawn patches
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (pMsg->message == WM_KEYDOWN) {

		// move patch in any direction unless locked
		if ((pDoc->m_pGrid->patchlist.size() > 0) && !pDoc->m_pGrid->currentPatch->locked) {

			switch (pMsg->wParam) {

			case VK_UP:
				pDoc->m_pGrid->currentPatch->coordsDEG.y -= .1f;
				pDoc->m_pGrid->currentPatch->coordsPX.y -= .1f * PPD;
				break;

			case VK_DOWN:
				pDoc->m_pGrid->currentPatch->coordsDEG.y += .1f;
				pDoc->m_pGrid->currentPatch->coordsPX.y += .1f * PPD;
				break;

			case VK_LEFT:
				pDoc->m_pGrid->currentPatch->coordsDEG.x -= .1f;
				pDoc->m_pGrid->currentPatch->coordsPX.x -= .1f * PPD;
				break;

			case VK_RIGHT:
				pDoc->m_pGrid->currentPatch->coordsDEG.x += .1f;
				pDoc->m_pGrid->currentPatch->coordsPX.x += .1f * PPD;
				break;
			}

		}

		switch (pMsg->wParam) {

		case VK_F1:
			pDoc->OnOverlayQuickhelp();
			break;

		case VK_F2:
			pDoc->ToggleOverlay();
			break;

		case VK_F3:
			ToggleFixationTarget();
			break;

		case VK_F12:
			if (m_pDlgTarget->calibrating == false) {
				pDoc->m_pGrid->showCursor = false;
				m_pDlgTarget->restartCalibration();
			}
			break;

		case VK_SPACE:

			if (pDoc->m_pGrid->currentPatch._Ptr == NULL)
				break;

			pDoc->m_pGrid->patchlist.commit(pDoc->m_pGrid->currentPatch);

			AfxGetMainWnd()->SendMessage(SAVE_IGUIDE_CSV, NULL, NULL);
			if (pDoc->m_pGrid->currentPatch->region == 0) {
				AfxGetMainWnd()->SendMessage(PATCH_TO_REGIONPANE, (WPARAM)&*pDoc->m_pGrid->currentPatch, NULL);
				//AfxGetMainWnd()->SendMessage(UPDATE_SELECTION, (WPARAM)&*pDoc->m_pGrid->currentPatch, NULL);
			}

			else {
				AfxGetMainWnd()->SendMessage(UPDATE_SELECTION, (WPARAM)&*pDoc->m_pGrid->currentPatch, NULL);
			}

			break;

		case 'N':

			pDoc->m_pGrid->showCursor = false;
			pDoc->m_pGrid->browse(NEXT);

			m_pDlgTarget->Pinpoint(*pDoc->m_pGrid->currentPatch);
			m_pDlgTarget->Invalidate();

			break;

		case 'B':

			pDoc->m_pGrid->showCursor = false;
			pDoc->m_pGrid->browse(PREV);
			
			m_pDlgTarget->Pinpoint(*pDoc->m_pGrid->currentPatch);
			m_pDlgTarget->Invalidate();

			//ATLTRACE(_T("patchlist: size[%d]\n"), pDoc->m_pGrid->patchlist.size());
		}

	}

	this->Invalidate();

	return CView::PreTranslateMessage(pMsg);

}

BOOL CIGUIDEView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{

	// TODO: Add your specialized code here and/or call the base class

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	m_pDlgTarget = new Target();
	m_pDlgTarget->Create(IDD_TARGET, pParentWnd);

	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}

void CIGUIDEView::OnClose()
{

	// TODO: Add your message handler code here and/or call default

	m_pDlgTarget->SendMessage(WM_CLOSE);
	CView::OnClose();

}

afx_msg LRESULT CIGUIDEView::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	m_pDlgTarget->ShowWindow(SW_HIDE);
	pDoc->m_Monitors.refresh();
	pDoc->m_Monitors.selectionDialog();
	
	return 0L;

}

void CIGUIDEView::ToggleFixationTarget()
{
	m_pDlgTarget->m_bVisible ? m_pDlgTarget->m_bVisible = false : m_pDlgTarget->m_bVisible = true;

}

void CIGUIDEView::OnSize(UINT nType, int cx, int cy)
{

	CView::OnSize(nType, cx, cy);

}


void CIGUIDEView::OnFundusImport()
{
	// TODO: Add your command handler code here
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	pDoc->OnFundusImport();
	ResetTransformationMatrices();

}


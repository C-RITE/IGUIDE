
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
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DISPLAYCHANGE, &CIGUIDEView::OnDisplaychange)
	ON_MESSAGE(SCREEN_SELECTED, &CIGUIDEView::ChangeTargetDisplay)
END_MESSAGE_MAP()

// CIGUIDEView construction/destruction

CIGUIDEView::CIGUIDEView()
{
	m_pDlgTarget = NULL;
	m_pFixationTarget = NULL;
}

CIGUIDEView::~CIGUIDEView()
{
	delete m_pDlgTarget;
}

BOOL CIGUIDEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}

CIGUIDEView * CIGUIDEView::GetView()
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


// CIGUIDEView message handlers

void CIGUIDEView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	
	if (!LButtonIsDown && !m_pDlgTarget->calibrating)
		return;

	if (pDoc->CheckFOV())
	{
		pDoc->m_pGrid->StorePatch(static_cast<CD2DPointF>(point));
	}
	else
		return;

	m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
	free(pDoc->m_pMousePos);

	pDoc->m_pMousePos = NULL;
	ShowCursor(TRUE);

	m_pDlgTarget->Invalidate();
	Invalidate();

	LButtonIsDown = false;

}


void CIGUIDEView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (pDoc->m_pMousePos)
		if ((GetKeyState(VK_LBUTTON) < 0) & (pDoc->m_raster.corner.size() == 4)) {
			pDoc->m_pMousePos->SetPoint(point.x, point.y);
			RedrawWindow();
		}

}


void CIGUIDEView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (!pDoc->m_pMousePos && pDoc->CheckFOV()) {
		pDoc->m_pMousePos = (CPoint*)malloc(sizeof(CPoint));
		pDoc->m_pMousePos->SetPoint(point.x, point.y);
	}

	ShowCursor(FALSE);

	LButtonIsDown = true;

	RedrawWindow();

}


void CIGUIDEView::OnInitialUpdate()
{
	
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	AfxGetMainWnd()->SendMessage(DOC_IS_READY);
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

}

LRESULT CIGUIDEView::ChangeTargetDisplay(WPARAM w, LPARAM l) {


	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	m_pDlgTarget->ShowWindow(TRUE);

	CRect area = (CRect)pDoc->m_pSelectedScreen->area;
	CRect wRect;
	
	m_pDlgTarget->SetWindowPos(&this->wndBottom,
		area.left - wRect.left ,
		area.top - wRect.top,
		area.Width(),
		area.Height(),
		NULL);

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

	if (!pDoc->m_pFundus->filename.IsEmpty()) {
		pDoc->m_pFundus->picture = new CD2DBitmap(pRenderTarget, pDoc->m_pFundus->filename);
		pDoc->m_pFundus->picture->Create(pRenderTarget);
	}

	pDoc->m_pGrid->center.x = (float)clientRect.CenterPoint().x;
	pDoc->m_pGrid->center.y = (float)clientRect.CenterPoint().y;

	m_pDlgTarget->SetFixationTarget();
	m_pDlgTarget->Invalidate();

	SetFixationTarget();

	SetFocus();
	Invalidate();

	delete FOV;

}

void CIGUIDEView::SetFixationTarget() {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
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

	// TODO:  Add your specialized creation code here
	//Invalidate();

	return 0;

}

// CIGUIDEView drawing

afx_msg LRESULT CIGUIDEView::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	ASSERT_VALID(pRenderTarget);	

	if (!pDoc)
		return FALSE;

	// TODO: add draw code for native data here
	if (pRenderTarget) {

		pRenderTarget->Clear(ColorF(ColorF::Black));
		
		pDoc->m_pFundus->Paint(pRenderTarget);

		pDoc->m_pGrid->DrawGrid(pRenderTarget);
		pDoc->m_pGrid->DrawPatches(pRenderTarget);
		pDoc->m_pGrid->DrawOptional(pRenderTarget);
		pDoc->m_pGrid->DrawTextInfo(pRenderTarget);

	}

	return 0;

}

void CIGUIDEView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	// all the drawing happens in OnDraw2D

}

void CIGUIDEView::OnSize(UINT nType, int cx, int cy)
{
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	CView::OnSize(nType, cx, cy);
	CRect rect;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pDoc->m_raster.scale.x = (float)cx / (pMainWnd->WINDOW_WIDTH - 20);
	pDoc->m_raster.scale.y = (float)cy / (pMainWnd->WINDOW_HEIGHT - 62);
	
	if (nType == SIZE_RESTORED)
		Invalidate();

	// TODO: Add your message handler code here

}


BOOL CIGUIDEView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	// keyboard controls depending on drawn patches
	if (pDoc->m_pGrid->patchlist.size() > 0) {
		
		if (pMsg->message == WM_KEYDOWN) {
			switch (pMsg->wParam) {

				case VK_UP:
					pDoc->m_pGrid->patchlist.back().coords.y += .1f;
					m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
					break;
				case VK_DOWN:
					pDoc->m_pGrid->patchlist.back().coords.y -= .1f;
					m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
					break;
				case VK_LEFT:
					pDoc->m_pGrid->patchlist.back().coords.x -= .1f;
					m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
					break;
				case VK_RIGHT:
					pDoc->m_pGrid->patchlist.back().coords.x += .1f;
					m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
					break;
				case VK_SPACE:
					pDoc->m_pGrid->patchlist.lockIn();
					break;

			}

			if (pDoc->m_pGrid->patchlist.size() > 0)
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
			else
				m_pDlgTarget->m_POI = NULL;

			m_pDlgTarget->Invalidate();

			this->Invalidate();

		}

		// delete last patch with right mouse button

		if (pMsg->wParam == VK_RBUTTON) {
			if (pMsg->message == WM_MOUSEMOVE)
				return false;
			if (pDoc->m_pGrid->patchlist.back().locked == false) {
				pDoc->m_pGrid->DelPatch();
				pDoc->m_pGrid->patchlist.SaveToFile();
			}

			this->Invalidate();
		}

	}

	// other keyboard controls

	if (pMsg->message == WM_KEYDOWN) {
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

		case VK_F4:
			if (m_pDlgTarget->calibrating == false) {
				m_pDlgTarget->restartCalibration();
			}
			break;
		}

	}

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


afx_msg LRESULT CIGUIDEView::OnDisplaychange(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	m_pDlgTarget->ShowWindow(SW_HIDE);
	AfxMessageBox(L"Monitor configuration change detected.\n\nSelect a screen in Properties/Target View now!", MB_ICONWARNING);
	pDoc->getScreens();
	AfxGetMainWnd()->SendMessage(DOC_IS_READY);
	return 0;

}


void CIGUIDEView::ToggleFixationTarget()
{
	m_pDlgTarget->m_bVisible ? m_pDlgTarget->m_bVisible = false : m_pDlgTarget->m_bVisible = true;
}

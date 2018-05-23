
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

	if (pDoc->CheckFOV()) {
		pDoc->m_pGrid->StoreClick(static_cast<CD2DPointF>(point));
	}
	else
		return;

	m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
	free(pDoc->mousePos);
	pDoc->mousePos = NULL;
	ShowCursor(TRUE);

	m_pDlgTarget->Invalidate();
	Invalidate();
}


void CIGUIDEView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (pDoc->mousePos)
		if ((GetKeyState(VK_LBUTTON) < 0) & (pDoc->raster.corner.size() == 4)) {
			pDoc->mousePos->SetPoint(point.x, point.y);
			RedrawWindow();
		}

}


void CIGUIDEView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (!pDoc->mousePos && pDoc->CheckFOV()) {
		pDoc->mousePos = (CPoint*)malloc(sizeof(CPoint));
		pDoc->mousePos->SetPoint(point.x, point.y);
	}

	ShowCursor(FALSE);

	RedrawWindow();
}


void CIGUIDEView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	

	// TODO: Add your specialized code here and/or call the base class
	AfxGetMainWnd()->SendMessage(DOC_IS_READY);
	
}

LRESULT CIGUIDEView::ChangeTargetDisplay(WPARAM w, LPARAM l) {

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	CRect area = (CRect)pDoc->m_selectedScreen->area;
	CRect wRect;

	m_pDlgTarget->GetClientRect(wRect);
	m_pDlgTarget->ClientToScreen(wRect);
	
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

	// deduce edges from corners
	if (pDoc->raster.corner.size() == 4 && pDoc->raster.meanEdge == 0) {
		for (int i = 0; i < 3; i++) {
			Edge k;
			k.p = pDoc->raster.corner[i];
			k.q = pDoc->raster.corner[i + 1];
			pDoc->raster.perimeter.push_back(k);
		}
		for (int i = 0; i < 4; i++)
			FOV[i] = pDoc->raster.corner[i];
		pDoc->raster.mid = pDoc->compute2DPolygonCentroid(FOV, 4);
		Edge k;
		k.p = pDoc->raster.corner[3];
		k.q = pDoc->raster.corner[0];
		pDoc->raster.perimeter.push_back(k);
	}

	// calculate length of edges, mean edge length and angle
	if (pDoc->raster.corner.size() == 4 && pDoc->raster.meanEdge == 0) {
		pDoc->ComputeDisplacementAngles();
		if (pDoc->CheckCalibrationValidity()) {
			for (size_t i = 0; i < pDoc->raster.perimeter.size(); i++) {
				pDoc->raster.perimeter[i].length = pDoc->CalcEdgeLength(pDoc->raster.perimeter[i]);
				pDoc->raster.meanEdge += pDoc->raster.perimeter[i].length;
				pDoc->raster.meanAlpha += pDoc->raster.perimeter[i].alpha;
			}

			pDoc->raster.meanEdge /= 4;
			pDoc->raster.meanAlpha /= 4;
		}
		else {
			AfxMessageBox(L"Calibration failed. Please retry!", MB_OK | MB_ICONSTOP);
			m_pDlgTarget->OnLButtonDown(NULL, NULL);
		}
	
	}

	ASSERT_VALID(pRenderTarget);

	if (!pDoc->m_pFundus->calibration) {
		if (nullptr != pDoc->m_pFundus->picture)
		{
			pDoc->m_pFundus->picture->Destroy();
		}
		if (pDoc->m_pFundus->filename) {
			pDoc->m_pFundus->picture = new CD2DBitmap(pRenderTarget, *pDoc->m_pFundus->filename);
			pDoc->m_pFundus->picture->Create(pRenderTarget);
		}
		
	}

	pDoc->m_pGrid->center.x = (float)clientRect.CenterPoint().x;
	pDoc->m_pGrid->center.y = (float)clientRect.CenterPoint().y;

	m_pDlgTarget->pDoc = pDoc;
	m_pDlgTarget->calcFieldSize();
	pDoc->raster.corner.size()==0? m_pDlgTarget->setCross():0;
	m_pDlgTarget->getFixationTarget();	

	Invalidate();
	delete FOV;
}

int CIGUIDEView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Enable D2D support for this window:
	EnableD2DSupport(D2D1_FACTORY_TYPE_MULTI_THREADED);

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
		pDoc->m_pGrid->Paint(pRenderTarget);
		pDoc->m_pGrid->Mark(pRenderTarget);
		pDoc->m_pGrid->DrawOverlay(pRenderTarget);

		if (pDoc->m_pGrid->overlay & TRACEINFO) {
			CD2DSizeF sizeTarget = pRenderTarget->GetSize();
			CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
			CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
				_T("Consolas"),								// font family name
				sizeDpi.height / 8);						// font size

			CString traceText = pDoc->getTraceInfo();
			// construct a CD2DTextLayout object which represents a block of formatted text 
			CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
				traceText,									// text to be drawn
				textFormat,									// text format
				sizeTarget);								// size of the layout box

			pRenderTarget->DrawTextLayout(CD2DPointF(5, 5),	// top-left corner of the text 
				&textLayout,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::LightGreen)));

		}
		
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
	pDoc->raster.scale.x = (float)cx / (pMainWnd->WINDOW_WIDTH - 20);
	pDoc->raster.scale.y = (float)cy / (pMainWnd->WINDOW_HEIGHT - 62);
	
	if (nType == SIZE_RESTORED)
		Invalidate();

	// TODO: Add your message handler code here


}


BOOL CIGUIDEView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

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
		}

		if (pMsg->wParam == VK_RBUTTON) {
			if (pMsg->message == WM_MOUSEMOVE)
				return false;
			pDoc->m_pGrid->DelPatch();
			pDoc->m_pGrid->patchlist.SaveToFile();
		}

		if (pDoc->m_pGrid->patchlist.size() > 0)
			m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
		else
			m_pDlgTarget->m_POI = NULL;

		m_pDlgTarget->Invalidate();

		this->Invalidate();
		
	}

		return CView::PreTranslateMessage(pMsg);

}

BOOL CIGUIDEView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{

	m_pDlgTarget = new Target();
	m_pDlgTarget->Create(IDD_TARGET, pParentWnd);
	m_pDlgTarget->ShowWindow(TRUE);
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	// TODO: Add your specialized code here and/or call the base class

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
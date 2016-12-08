
// GridTargetsView.cpp : implementation of the CGridTargetsView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GridTargets.h"
#endif

#include "GridTargetsDoc.h"
#include "GridTargetsView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;


// CGridTargetsView

IMPLEMENT_DYNCREATE(CGridTargetsView, CView)

BEGIN_MESSAGE_MAP(CGridTargetsView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CGridTargetsView::OnDraw2d)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CGridTargetsView construction/destruction

CGridTargetsView::CGridTargetsView()
{
	// Draw the Target View as a new Target Dialog
	m_pDlgTarget = new Target();
	m_pDlgTarget->Create(IDD_TARGET, m_pDlgTarget->GetTopLevelParent());
	m_pDlgTarget->ShowWindow(SW_SHOW);
	showTrace = TRUE;

}

CGridTargetsView::~CGridTargetsView()
{
	delete m_pDlgTarget;

}

BOOL CGridTargetsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}


// CGridTargetsView printing

BOOL CGridTargetsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGridTargetsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGridTargetsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGridTargetsView diagnostics

#ifdef _DEBUG
void CGridTargetsView::AssertValid() const
{
	CView::AssertValid();
}

void CGridTargetsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGridTargetsDoc* CGridTargetsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGridTargetsDoc)));
	return (CGridTargetsDoc*)m_pDocument;
}
#endif //_DEBUG


// CGridTargetsView message handlers


void CGridTargetsView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CGridTargetsDoc* pDoc = GetDocument();

	if (pDoc->CheckFOV()) {
		pDoc->m_pGrid->StoreClick(static_cast<CD2DPointF>(point));
	}
	else
		return;

	m_pDlgTarget->Pinpoint(pDoc->m_pGrid->centerOffset.x, pDoc->m_pGrid->centerOffset.y);
	free(pDoc->mousePos);
	pDoc->mousePos = NULL;
	ShowCursor(TRUE);

	m_pDlgTarget->Invalidate();
	Invalidate();
}


void CGridTargetsView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CGridTargetsDoc* pDoc = GetDocument();

	if (pDoc->mousePos)
		if ((GetKeyState(VK_LBUTTON) < 0) & (pDoc->raster.corner.size() == 4)) {
			pDoc->mousePos->SetPoint(point.x, point.y);
		RedrawWindow();

		}

}


void CGridTargetsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CGridTargetsDoc* pDoc = GetDocument();

	if (!pDoc->mousePos && pDoc->CheckFOV()) {
		pDoc->mousePos = (CPoint*)malloc(sizeof(CPoint));
		pDoc->mousePos->SetPoint(point.x, point.y);
	}
	ShowCursor(FALSE);

	RedrawWindow();
}

void CGridTargetsView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CGridTargetsDoc* pDoc = GetDocument();	
	CHwndRenderTarget* pRenderTarget = GetRenderTarget();
	CD2DPointF* FOV = new CD2DPointF[4];
	
	CRect clientRect;
	GetClientRect(&clientRect);

	// derive edges from corners
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
		for (int i = 0; i < pDoc->raster.perimeter.size(); i++) {
			pDoc->raster.perimeter[i].length = pDoc->CalcEdgeLength(pDoc->raster.perimeter[i]);
			pDoc->raster.meanEdge += pDoc->raster.perimeter[i].length;
			pDoc->raster.meanAlpha += pDoc->raster.perimeter[i].alpha;
		}

		pDoc->raster.meanEdge /= 4;
		pDoc->raster.meanAlpha /= 4;
	
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

	pDoc->m_pGrid->center.x = clientRect.CenterPoint().x;
	pDoc->m_pGrid->center.y = clientRect.CenterPoint().y;

	Invalidate();
	delete FOV;
}

int CGridTargetsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Enable D2D support for this window:
	EnableD2DSupport();

	// TODO:  Add your specialized creation code here

	return 0;
	Invalidate();
}

// CGridTargetsView drawing

afx_msg LRESULT CGridTargetsView::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);
	
	CGridTargetsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return FALSE;

	// TODO: add draw code for native data here
	if (pRenderTarget) {

		pRenderTarget->Clear(ColorF(ColorF::Black));
		pDoc->m_pFundus->Paint(pRenderTarget);
		pDoc->m_pGrid->Paint(pRenderTarget);
		pDoc->m_pGrid->Tag(pRenderTarget);
		pDoc->m_pGrid->DrawOverlay(pRenderTarget);

		if (showTrace) {
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

void CGridTargetsView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	
	// all the drawing happens in OnDraw2D

}


void CGridTargetsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	CGridTargetsDoc* pDoc = GetDocument();
	CRect rect;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pDoc->raster.scale.x = (float)cx / (pMainWnd->WINDOW_WIDTH - 20);
	pDoc->raster.scale.y = (float)cy / (pMainWnd->WINDOW_HEIGHT - 62);
	
	if (nType == SIZE_RESTORED)
		Invalidate();

	// TODO: Add your message handler code here


}


BOOL CGridTargetsView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	
	CGridTargetsDoc* pDoc = GetDocument();

	if (pDoc->m_pGrid->taglist.size() > 0) {
		if (pMsg->message == WM_KEYDOWN) {
			switch (pMsg->wParam) {
			case VK_UP:
				pDoc->m_pGrid->taglist.back().y += .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->taglist.back().x, pDoc->m_pGrid->taglist.back().y);
				break;
			case VK_DOWN:
				pDoc->m_pGrid->taglist.back().y -= .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->taglist.back().x, pDoc->m_pGrid->taglist.back().y);
				break;
			case VK_LEFT:
				pDoc->m_pGrid->taglist.back().x -= .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->taglist.back().x, pDoc->m_pGrid->taglist.back().y);
				break;
			case VK_RIGHT:
				pDoc->m_pGrid->taglist.back().x += .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->taglist.back().x, pDoc->m_pGrid->taglist.back().y);
				break;
			}

		}
		if (pMsg->wParam == VK_RBUTTON) {
			if (pMsg->message == WM_MOUSEMOVE)
				return false;
			pDoc->m_pGrid->DelTag();
		}

		if (pDoc->m_pGrid->taglist.size() > 0)
			m_pDlgTarget->Pinpoint(pDoc->m_pGrid->taglist.back().x, pDoc->m_pGrid->taglist.back().y);
		else
			m_pDlgTarget->m_POI = NULL;
		m_pDlgTarget->Invalidate();
		this->Invalidate();

	}

		return CView::PreTranslateMessage(pMsg);
}


void CGridTargetsView::OnDestroy()
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_pDlgTarget->OnClose();
}

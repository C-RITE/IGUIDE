
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
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CGridTargetsView construction/destruction

CGridTargetsView::CGridTargetsView()
{
	// Draw the Target View as a new Target Dialog
	m_pDlgTarget = new Target();
	m_pDlgTarget->Create(IDD_TARGET, m_pDlgTarget->GetTopLevelParent());
	m_pDlgTarget->ShowWindow(SW_SHOW);

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

	m_pDlgTarget->Invalidate();
	RedrawWindow();

}



void CGridTargetsView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CGridTargetsDoc* pDoc = GetDocument();

	pDoc->m_pGrid->DelTag();
	RedrawWindow();

}


void CGridTargetsView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CGridTargetsDoc* pDoc = GetDocument();

	if (pDoc->mousePos)
		if ((GetKeyState(VK_LBUTTON) & 0x100) != 0 & (pDoc->raster.boundary.size() > 3)) {
			pDoc->mousePos->SetPoint(point.x, point.y);
		}

	RedrawWindow();

}


void CGridTargetsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CGridTargetsDoc* pDoc = GetDocument();

	if (!pDoc->mousePos && pDoc->CheckFOV()) {
		pDoc->mousePos = (CPoint*)malloc(sizeof(CPoint));
		pDoc->mousePos->SetPoint(point.x, point.y);
	}

	RedrawWindow();

}


void CGridTargetsView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CGridTargetsDoc* pDoc = GetDocument();	
	CHwndRenderTarget* pRenderTarget = this->GetRenderTarget();
	
	CRect clientRect;
	GetClientRect(&clientRect);

	pDoc->CalcMeanEdge();

	ASSERT_VALID(pRenderTarget);

	if (nullptr != pDoc->m_pGrid->m_pFundus)
	{
		pDoc->m_pGrid->m_pFundus->Destroy();
	}

	if (!pDoc->strFile.IsEmpty())
	{
		pDoc->m_pGrid->m_pFundus = new CD2DBitmap(pRenderTarget, pDoc->strFile);
		pDoc->m_pGrid->m_pFundus->Create(pRenderTarget);

		if (pDoc->m_pGrid->m_pFundus->IsValid())
		{
			CD2DSizeF size = pDoc->m_pGrid->m_pFundus->GetSize();
			//sizeImage.SetSize(static_cast<int>(size.width), static_cast<int>(size.height));
		}
	}

	if (!pDoc->m_pGrid->m_pGrid_mark->IsValid())
	pDoc->m_pGrid->m_pGrid_mark->Create(pRenderTarget);

	pDoc->m_pGrid->center.x = clientRect.Width() / 2;
	pDoc->m_pGrid->center.y = clientRect.Height() / 2;

	Invalidate();

}

int CGridTargetsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Enable D2D support for this window:
	EnableD2DSupport();

	// TODO:  Add your specialized creation code here

	return 0;

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
	if (pRenderTarget->IsValid()) {

		pDoc->m_pGrid->Paint(pRenderTarget);
		pDoc->m_pGrid->Tag(pRenderTarget);
	}

	return TRUE;

}

void CGridTargetsView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	
	// all the drawing happens in OnDraw2D

}

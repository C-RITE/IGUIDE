
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
END_MESSAGE_MAP()

// CGridTargetsView construction/destruction

CGridTargetsView::CGridTargetsView()
{
	// TODO: add construction code here
	
	// Initialize Direct2D
	EnableD2DSupport();
	m_pBrshWhite = new CD2DSolidColorBrush(GetRenderTarget(), ColorF(ColorF::White));

}

CGridTargetsView::~CGridTargetsView()
{
}

BOOL CGridTargetsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}

// CGridTargetsView drawing

void CGridTargetsView::OnDraw(CDC* /*pDC*/)
{
	CGridTargetsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

afx_msg LRESULT CGridTargetsView::OnDraw2d(WPARAM wParam, LPARAM lParam)
{

	CGridTargetsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return false;

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

	pDoc->m_pGrid->paint(pRenderTarget);
	Invalidate();
	pDoc->m_pDlgTarget->Invalidate();

	return true;
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
	CHwndRenderTarget* pRenderTarget = GetRenderTarget();

	pRenderTarget->AssertValid();
	pDoc->DrawGrid(pRenderTarget);
	

	//CView::OnLButtonUp(nFlags, point);
}

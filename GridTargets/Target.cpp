// Target.cpp : implementation file
//

#include "stdafx.h"
#include "GridTargets.h"
#include "GridTargetsDoc.h"
#include "Target.h"
#include "afxdialogex.h"

using namespace D2D1;

// Target dialog

IMPLEMENT_DYNAMIC(Target, CDialog);

Target::Target(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TARGET, pParent)
{
	m_pBrushWhite = new CD2DSolidColorBrush(GetRenderTarget(), ColorF(ColorF::White));
	m_POI = NULL;
	EnableD2DSupport();
}

Target::~Target()
{
	delete m_pBrushWhite;
	delete m_POI;
}

void Target::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Target, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &Target::OnDraw2d)
END_MESSAGE_MAP()


// Target message handlers

afx_msg LRESULT Target::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

	pRenderTarget->Clear(ColorF(ColorF::Black));

	if (m_POI)
		pRenderTarget->DrawEllipse(*m_POI, m_pBrushWhite, 1, NULL);

	else {

		// draw white crosses to user define FOV
		for (size_t i = 0; i < pDoc->raster.boundary.size(); i++) {
			pRenderTarget->DrawLine(CD2DPointF(pDoc->raster.boundary[i].x - 7, pDoc->raster.boundary[i].y - 7),
				CD2DPointF(pDoc->raster.boundary[i].x + 7, pDoc->raster.boundary[i].y + 7),
				m_pBrushWhite,
				1,
				NULL);
			pRenderTarget->DrawLine(CD2DPointF(pDoc->raster.boundary[i].x - 7, pDoc->raster.boundary[i].y + 7),
				CD2DPointF(pDoc->raster.boundary[i].x + 7, pDoc->raster.boundary[i].y - 7),
				m_pBrushWhite,
				1,
				NULL);
		}

	}
	return true;
}

void Target::Pinpoint(float centerOffset_x, float centerOffset_y)
{
	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	// create coordinates for fixation target
	
	CD2DRectF FOV = {
		pDoc->raster.boundary[0].x,
		pDoc->raster.boundary[1].y,
		pDoc->raster.boundary[2].x,
		pDoc->raster.boundary[3].y 
	};
	
	if (!m_POI)
		m_POI = (CD2DRectF*)malloc(sizeof(CD2DRectF));

	CD2DPointF mid = { (FOV.left + FOV.right) / 2, (FOV.bottom + FOV.top) / 2 };
	
	*m_POI = { CD2DRectF(
		mid.x - centerOffset_x -2,
		mid.y - centerOffset_y -2,
		mid.x - centerOffset_x + 2,
		mid.y - centerOffset_y + 2)
	};
}


void Target::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CD2DPointF d2dpoint;
	d2dpoint = static_cast<CD2DPointF>(point);
	
	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	if (pDoc->raster.boundary.size() < 4)
		pDoc->raster.boundary.push_back(d2dpoint);

	else {
		free(m_POI);
		m_POI = NULL;
		pDoc->raster.boundary.clear();
		pDoc->m_pGrid->ClearTaglist();
	}

	pDoc->UpdateAllViews(NULL);
	RedrawWindow();

	// call default
	// CDialog::OnLButtonUp(nFlags, point);
}
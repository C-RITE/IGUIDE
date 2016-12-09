// Target.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"
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

void Target::Pinpoint(float centerOffset_x, float centerOffset_y)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	// transform coordinates for fixation target (rotate and scale)

	if (!m_POI)
		m_POI = (CD2DRectF*)malloc(sizeof(CD2DRectF));

	float alpha, beta, gamma, theta;
	float pi = atan(1) * 4;
	float a, b, c, x, y;
	ppd_client = (1 / pDoc->raster.size) * pDoc->raster.meanEdge;

	Edge k;
	k.q.x = centerOffset_x;
	k.q.y = centerOffset_y;

	alpha = pDoc->raster.meanAlpha;
	beta = 360 - pDoc->ComputeOrientationAngle(k);
	gamma = alpha + beta;

	a = centerOffset_x;
	b = centerOffset_y;
	c = sqrt(pow(a, 2) + pow(b, 2));

	y = sin(gamma * pi / 180) * c * ppd_client; // calc. x shift and scale to client ppd
	x = cos(gamma * pi / 180) * -c * ppd_client; // calc. y shift and scale to client ppd

	*m_POI = { CD2DRectF(
		((pDoc->raster.mid.x + x) - 4),
		((pDoc->raster.mid.y + y) - 4),
		((pDoc->raster.mid.x + x) + 4),
		((pDoc->raster.mid.y + y) + 4))
	};

	/*ATLTRACE(_T("alpha is %f\n"), alpha);
	ATLTRACE(_T("beta is %f\n"), beta);
	ATLTRACE(_T("gamma is %f\n"), gamma);
	ATLTRACE(_T("x \t\t%f\n"), x);
	ATLTRACE(_T("y \t\t%f\n"), y);
	ATLTRACE(_T("c \t\t%f\n"), c);
	ATLTRACE(_T("center \tx=%f, y=%f\n"), pDoc->m_pGrid->center.x, pDoc->m_pGrid->center.y);
	ATLTRACE(_T("offset \tx=%f, y=%f\n"), centerOffset_x, centerOffset_y);
	ATLTRACE(_T("POI \t %f\n\n"), *m_POI);
	*/
}

void Target::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Target, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &Target::OnDraw2d)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Target message handlers

afx_msg LRESULT Target::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

	pRenderTarget->Clear(ColorF(ColorF::Black));

	if (m_POI) {
		pRenderTarget->DrawEllipse(*m_POI, m_pBrushWhite, 1, NULL);
		pRenderTarget->DrawLine(CD2DPointF(m_POI->left - 4, m_POI->top - 4),
								CD2DPointF(m_POI->right + 4, m_POI->bottom + 4),
								m_pBrushWhite);
		pRenderTarget->DrawLine(CD2DPointF(m_POI->left - 4, m_POI->bottom + 4),
								CD2DPointF(m_POI->right + 4, m_POI->top - 4),
								m_pBrushWhite);
	}

	else {

		// draw white crosses to user define FOV
		for (size_t i = 0; i < pDoc->raster.corner.size(); i++) {
			pRenderTarget->DrawLine(CD2DPointF(pDoc->raster.corner[i].x - 7, pDoc->raster.corner[i].y - 7),
				CD2DPointF(pDoc->raster.corner[i].x + 7, pDoc->raster.corner[i].y + 7),
				m_pBrushWhite,
				1,
				NULL);
			pRenderTarget->DrawLine(CD2DPointF(pDoc->raster.corner[i].x - 7, pDoc->raster.corner[i].y + 7),
				CD2DPointF(pDoc->raster.corner[i].x + 7, pDoc->raster.corner[i].y - 7),
				m_pBrushWhite,
				1,
				NULL);
		}

	}
	return true;
}

void Target::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CD2DPointF d2dpoint;
	d2dpoint = static_cast<CD2DPointF>(point);
	
	CIGUIDEDoc* pDoc;
	pDoc = CIGUIDEDoc::GetDoc();

	if (pDoc->raster.corner.size() < 4)
		pDoc->raster.corner.push_back(d2dpoint);

	else {
		free(m_POI);
		m_POI = NULL;
		pDoc->raster.meanAlpha = 0;
		pDoc->raster.meanEdge = 0;
		pDoc->raster.corner.clear();
		pDoc->raster.perimeter.clear();
		pDoc->m_pGrid->ClearTaglist();
	}

	pDoc->UpdateAllViews(NULL);
	RedrawWindow();

	// call default
	// CDialog::OnLButtonUp(nFlags, point);
}


void Target::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"IGUIDE", L"WP_Target", (LPBYTE)&wp, sizeof(wp));
}


void Target::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
	static bool bOnce = true;

	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(L"IGUIDE", L"WP_Target", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}

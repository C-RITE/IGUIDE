// Target.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "Target.h"
#include "MainFrm.h"


using namespace D2D1;
// Target dialog

IMPLEMENT_DYNAMIC(Target, CDialogEx);
CCriticalSection m_CritSection;

Target::Target(CIGUIDEView* pParent /*=NULL*/)
	: CDialogEx(IDD_TARGET, pParent)
{

	EnableD2DSupport();
	m_pBrushWhite = new CD2DSolidColorBrush(GetRenderTarget(), ColorF(ColorF::White));
	m_POI = NULL;
	m_pFixationTarget = NULL;
	fieldsize = 0;
	pDoc = NULL;
	m_bVisible = true;
	show_cross = false;
	discretion = 10;

}

Target::~Target()
{

	delete m_POI;
}

BEGIN_MESSAGE_MAP(Target, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &Target::OnDraw2d)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void Target::calcFieldSize() {

	double fs;
	double pi = atan(1) * 4;			// just pi
	double pixelpitch = 0.13725;		// pixel pitch of screen in mm
	double d = 650;						// distance between beam splitter and screen in mm
	fs = 2 * d * tan((pDoc->raster.size / 2) * (pi / 180));
	fs /= pixelpitch;
	fieldsize = (int)fs;

}

void Target::setCross() {
	
	if (pDoc->m_Screens.size() > 0) {
		CRect cRect = (CRect)pDoc->m_selectedScreen->area;
		
		// place first cross on top-left corner of the raster
		xbox_cross = CD2DPointF((float)(cRect.Width() / 2 - fieldsize / 2), (float)(cRect.Height() / 2 - fieldsize / 2));
		
		// add discretion
		xbox_cross.x -= discretion;
		xbox_cross.y -= discretion;

	}
	
	show_cross = true;
	
}

void Target::getFixationTarget() {

	if (m_pFixationTarget && m_pFixationTarget->IsValid())
		delete m_pFixationTarget;

	m_pFixationTarget = new CD2DBitmap(GetRenderTarget(), pDoc->m_FixationTarget, CD2DSizeU(0,	0), TRUE);

}

void Target::Pinpoint(float centerOffset_x, float centerOffset_y)
{

	// transform coordinates for fixation target (rotate and scale)
	// current calculation assumes that target view is counter-rotating
	// flip signs of Edge 'k' to customize

	if (!m_POI)
		m_POI = (CD2DRectF*)malloc(sizeof(CD2DRectF));

	double alpha, beta, gamma;
	double pi = atan(1) * 4;
	double a, b, c, x, y;
	ppd_client = (1 / pDoc->raster.size) * pDoc->raster.meanEdge;

	Edge k;
	k.q.x = -centerOffset_x;
	if (pDoc->m_FlipVertical) {
		k.q.y = -centerOffset_y;
	}
	else {
		k.q.y = centerOffset_y;
		
	}
	alpha = pDoc->raster.meanAlpha;
	beta = 360 - pDoc->ComputeOrientationAngle(k);
	gamma = beta - alpha;

	a = centerOffset_x;
	b = centerOffset_y;
	c = sqrt(pow(a, 2) + pow(b, 2));

	y = sin(gamma * pi / 180) * c * ppd_client; // calc. x shift and scale to client ppd
	x = cos(gamma * pi / 180) * c * ppd_client; // calc. y shift and scale to client ppd

	*m_POI = { CD2DRectF(
		(pDoc->raster.mid.x + (float)x - 10),
		(pDoc->raster.mid.y + (float)y - 10),
		(pDoc->raster.mid.x + (float)x + 10),
		(pDoc->raster.mid.y + (float)y + 10))
	};

}

void Target::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// Target message handlers

afx_msg LRESULT Target::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	ControlState state = pDoc->m_Controller.state;

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;

	if (!pRenderTarget->IsValid())
		return -1;

	pRenderTarget->Clear(ColorF(ColorF::Black));

	if (m_bVisible) {

		float scalingFactor;
		if (pDoc)
			scalingFactor = (float)pDoc->m_FixationTargetSize / 100;

		// custom fixation target
		if (m_POI && m_pFixationTarget->IsValid()) {
			CD2DSizeF size = m_pFixationTarget->GetSize();
			CD2DPointF center{ (m_POI->left + m_POI->right) / 2,
				(m_POI->bottom + m_POI->top) / 2,
			};
			pRenderTarget->DrawBitmap(m_pFixationTarget, CD2DRectF(
				center.x - (size.width / 2 * scalingFactor),
				center.y - (size.height / 2 * scalingFactor),
				center.x + (size.width / 2 * scalingFactor),
				center.y + (size.height / 2 * scalingFactor)),
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
			);
		}

		// default fixation target
		else if (m_POI) {
			pRenderTarget->DrawEllipse(*m_POI, m_pBrushWhite, 1, NULL);
			pRenderTarget->DrawLine(CD2DPointF(m_POI->left - 4, m_POI->top - 4),
				CD2DPointF(m_POI->right + 4, m_POI->bottom + 4),
				m_pBrushWhite);
			pRenderTarget->DrawLine(CD2DPointF(m_POI->left - 4, m_POI->bottom + 4),
				CD2DPointF(m_POI->right + 4, m_POI->top - 4),
				m_pBrushWhite);
		}

		else if (pDoc) {

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

	}

	// draw cross while moving around with pov hat

	if (show_cross) {
		// points outlining the cross
		CD2DPointF a(xbox_cross.x - 7, xbox_cross.y - 7);
		CD2DPointF b(xbox_cross.x + 7, xbox_cross.y + 7);
		CD2DPointF c(xbox_cross.x + 7, xbox_cross.y - 7);
		CD2DPointF d(xbox_cross.x - 7, xbox_cross.y + 7);

		pRenderTarget->DrawLine(CD2DPointF(a.x + state.LX, a.y + state.LY),
			CD2DPointF(b.x + state.LX, b.y + state.LY),
			m_pBrushWhite, 1, NULL);

		pRenderTarget->DrawLine(CD2DPointF(c.x + state.LX, c.y + state.LY),
			CD2DPointF(d.x + state.LX, d.y + state.LY),
			m_pBrushWhite, 1, NULL);
	}

	return 0;

}

void Target::restartCalibration() {

	free(m_POI);
	m_POI = NULL;
	pDoc->raster.meanAlpha = 0;
	pDoc->raster.meanEdge = 0;
	pDoc->raster.corner.clear();
	pDoc->raster.perimeter.clear();
	pDoc->m_pGrid->ClearPatchlist();
	
}

void Target::finishCalibration() {

	CRect mainWnd;
	CPoint center;
	CIGUIDEView* pView = CIGUIDEView::GetView();
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	pView->OnLButtonUp(0, center);
	pView->SetFocus();

}

void Target::OnGamePadCalibration() {

	// hijack left-mouse-button click handler to store calibration coordinates
	// move cursor from one raster corner to the next with each click

	ControlState state = pDoc->m_Controller.state;

	switch (state.fired % 5) {

	case 0:
		// reset it all
		setCross();
		OnLButtonDown(0x00FF, CPoint(0, 0));
		break;

	case 1:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY)
		);
		xbox_cross.x += fieldsize + discretion * 2;
		break;

	case 2:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));
		
		xbox_cross.y += fieldsize + discretion * 2;

		break;

	case 3:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));
		xbox_cross.x -= fieldsize + discretion * 2;
		break;

	case 4:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));
		show_cross = false;
		break;

	}

	RedrawWindow();

}

void Target::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CD2DPointF d2dpoint;
	d2dpoint = static_cast<CD2DPointF>(point);

	if (nFlags == 0x00FF || pDoc->raster.corner.size() > 3) {
		restartCalibration();
		return;
	}

	if (pDoc->raster.corner.size() == 3) {
		pDoc->raster.corner.push_back(d2dpoint);
		finishCalibration();
	}
	else
		pDoc->raster.corner.push_back(d2dpoint);

	pDoc->UpdateAllViews(NULL);
	RedrawWindow();

}

BOOL Target::PreTranslateMessage(MSG* pMsg)
{

	// TODO: Add your specialized code here and/or call the base class

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		if (pDoc->m_Controller.m_bActive == true)
			return true;
	}

	return __super::PreTranslateMessage(pMsg);

}


void Target::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here

}

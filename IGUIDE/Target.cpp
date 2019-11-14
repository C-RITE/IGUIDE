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
	pDoc = NULL;
	fieldsize = 0;
	m_bVisible = true;
	show_cross = false;
	calibrating = false;
	discretion = 20;

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


void Target::setCross() {

	if (pDoc->m_Screens.size() > 0) {

		CRect cRect = (CRect)pDoc->m_pSelectedScreen->area;

		// place first cross on top-left corner of the raster
		xbox_cross = CD2DPointF(
			(float)(cRect.Width() / 2 - ((fieldsize / 2) * pDoc->m_Controller.flipSign.x)),
			(float)(cRect.Height() / 2 - ((fieldsize / 2) * pDoc->m_Controller.flipSign.y))
		);

		// add discretion
		xbox_cross.x -= discretion * pDoc->m_Controller.flipSign.x;
		xbox_cross.y -= discretion * pDoc->m_Controller.flipSign.y;

	}

	show_cross = true;

}

void Target::getFixationTarget() {

	if (m_pFixationTarget && m_pFixationTarget->IsValid())
		delete m_pFixationTarget;

	m_pFixationTarget = new CD2DBitmap(GetRenderTarget(), pDoc->m_FixationTarget, CD2DSizeU(0, 0), TRUE);

}

void Target::Pinpoint(float centerOffset_x, float centerOffset_y)
{

	// transform coordinates for fixation target (rotate and scale) using subject calibration

	if (!m_POI)
		m_POI = (CD2DRectF*)malloc(sizeof(CD2DRectF));

	double alpha, beta, gamma;
	double pi = atan(1) * 4;
	double a, b, c, x, y;

	ppd_client = ((float)pDoc->m_raster.videodim / (float)pDoc->m_raster.size) * pDoc->m_raster.meanEdge;
	
	Edge k;

	if (pDoc->m_FlipHorizontal == L"True") {
		k.q.x = centerOffset_x;
	}
	
	else
		k.q.x = -centerOffset_x;

	if (pDoc->m_FlipVertical == L"True") {
		k.q.y = centerOffset_y;
	}

	else {
		k.q.y = -centerOffset_y;
	}

	alpha = pDoc->m_raster.meanAlpha;
	beta = 360 - pDoc->ComputeOrientationAngle(k);
	gamma = beta - alpha;

	a = centerOffset_x;
	b = centerOffset_y;
	c = sqrt(pow(a, 2) + pow(b, 2));

	y = sin(gamma * pi / 180) * c * ppd_client; // calc. x shift and scale to client ppd
	x = cos(gamma * pi / 180) * c * ppd_client; // calc. y shift and scale to client ppd

	*m_POI = { CD2DRectF(
		(pDoc->m_raster.mid.x + (float)x - 10),
		(pDoc->m_raster.mid.y + (float)y - 10),
		(pDoc->m_raster.mid.x + (float)x + 10),
		(pDoc->m_raster.mid.y + (float)y + 10))
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
			CD2DPointF center{
				(m_POI->left + m_POI->right) / 2,
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
			for (size_t i = 0; i < pDoc->m_raster.corner.size(); i++) {

				pRenderTarget->DrawLine
					(CD2DPointF(
						pDoc->m_raster.corner[i].x - 7, pDoc->m_raster.corner[i].y - 7),
					CD2DPointF(
						pDoc->m_raster.corner[i].x + 7, pDoc->m_raster.corner[i].y + 7),
					m_pBrushWhite,
					1,
					NULL);
				
				pRenderTarget->DrawLine(
					CD2DPointF(
						pDoc->m_raster.corner[i].x - 7, pDoc->m_raster.corner[i].y + 7),
					CD2DPointF(
						pDoc->m_raster.corner[i].x + 7, pDoc->m_raster.corner[i].y - 7),
					m_pBrushWhite,
					1,
					NULL);

			}

		}

	}
	

	// draw cross for calibration while moving around with D-pad

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

	fieldsize = (512.f / (float)pDoc->m_raster.size) * 66.6f;

	free(m_POI);
	m_POI = NULL;
	pDoc->m_raster.mid = { 0, 0 };
	pDoc->m_raster.meanAlpha = 0;
	pDoc->m_raster.meanEdge = 0;
	pDoc->m_raster.corner.clear();
	pDoc->m_raster.perimeter.clear();
	pDoc->m_pGrid->ClearPatchlist();
	pDoc->m_Controller.state.fired = 0;

	calibrating = true;

	if (pDoc->m_InputController == L"Gamepad")
		setCross();

}

void Target::finishCalibration() {

	show_cross = false;
	CRect mainWnd;
	CPoint center;
	CIGUIDEView* pView = CIGUIDEView::GetView();
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	pView->OnLButtonUp(0, center);
	pView->SetFocus();

	calibrating = false;
	
}

void Target::OnGamePadCalibration() {

	// hijack left-mouse-button click handler to store calibration coordinates
	// move cursor from one raster corner to the next with each click

	!calibrating ? restartCalibration() : 0;

	ControlState state = pDoc->m_Controller.state;

	switch (state.fired % 5) {

	case 0:
		// reset it all
		OnLButtonDown(0, CPoint(0, 0));

		break;

	case 1:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));

		xbox_cross.x += (fieldsize + discretion) * pDoc->m_Controller.flipSign.x;

		break;

	case 2:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));

		xbox_cross.y += distance.getLength() * pDoc->m_Controller.flipSign.y;

		break;

	case 3:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));

		xbox_cross.x -= distance.getLength() * pDoc->m_Controller.flipSign.x;

		break;

	case 4:
		OnLButtonDown(0, CPoint(
			(int)xbox_cross.x + state.LX,
			(int)xbox_cross.y + state.LY));

		break;

	}

	RedrawWindow();

}

void Target::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	// parameter point == {0,0}: reset subject calibration by gamepad
	CD2DPointF d2dpoint;
	d2dpoint = static_cast<CD2DPointF>(point);

	if (pDoc->m_raster.corner.size() < 4 && point != CD2DPointF{ 0,0 }) {
			pDoc->m_raster.corner.push_back(d2dpoint);
			switch (pDoc->m_raster.corner.size()) {
				case 1:
					distance.p = point;
					break;
				case 2:
					distance.q = point;
					break;

			}
			pDoc->UpdateAllViews(NULL);
			RedrawWindow();

		if (pDoc->m_raster.corner.size() == 4)
			finishCalibration();
		}

	else
		restartCalibration();

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

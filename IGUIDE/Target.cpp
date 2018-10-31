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

XboxControlState Target::xbox_state;
bool Target::m_bPushed = false;
bool Target::show_cross = false;
bool Target::m_bFireDown = false;
bool Target::m_bFireUp = true;

IMPLEMENT_DYNAMIC(Target, CDialog);
CCriticalSection m_CritSection;

Target::Target(CIGUIDEView* pParent /*=NULL*/)
	: CDialog(IDD_TARGET, pParent)
{

	EnableD2DSupport(D2D1_FACTORY_TYPE_MULTI_THREADED);
	m_pBrushWhite = new CD2DSolidColorBrush(GetRenderTarget(), ColorF(ColorF::White));
	m_POI = NULL;
	m_pFixationTarget = NULL;
	m_fired = 0;
	m_bRunning = false;
	fieldsize = 0;
	pDoc = NULL;
	m_flip = 0;

}

Target::~Target()
{
	delete m_POI;
}

BEGIN_MESSAGE_MAP(Target, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &Target::OnDraw2d)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
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
		CRect cRect = (CRect)pDoc->m_Screens[1].area;;
		xbox_cross = CD2DPointF((float)(cRect.Width() / 2 - fieldsize / 2), (float)(cRect.Height() / 2 - fieldsize / 2));
	}
	

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

// Target message handlers

afx_msg LRESULT Target::OnDraw2d(WPARAM wParam, LPARAM lParam)
{
	CSingleLock singleLock(&m_CritSection);
	singleLock.Lock();
	ThreadDraw(this);
	singleLock.Unlock();
	return (LRESULT)TRUE;

}

UINT ThreadDraw(PVOID pParam) {

	Target* pTarget = (Target*)pParam;

	CHwndRenderTarget* pRenderTarget = NULL;
	pRenderTarget = pTarget->GetRenderTarget();

	if (!pRenderTarget->IsValid())
		return -1;

	pRenderTarget->Clear(ColorF(ColorF::Black));


	float scalingFactor;
	if (pTarget->pDoc)
		scalingFactor = (float)pTarget->pDoc->m_FixationTargetSize / 100;

	// custom fixation target
	if (pTarget->m_POI && pTarget->m_pFixationTarget->IsValid()) {
		CD2DSizeF size = pTarget->m_pFixationTarget->GetSize();
		CD2DPointF center{ (pTarget->m_POI->left + pTarget->m_POI->right) / 2,
			(pTarget->m_POI->bottom + pTarget->m_POI->top) / 2,
		};
		pRenderTarget->DrawBitmap(pTarget->m_pFixationTarget, CD2DRectF(
			center.x - (size.width / 2 * scalingFactor),
			center.y - (size.height / 2 * scalingFactor),
			center.x + (size.width / 2 * scalingFactor),
			center.y + (size.height / 2 * scalingFactor)),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
			);
	}

	// default fixation target
	else if (pTarget->m_POI) {
		pRenderTarget->DrawEllipse(*pTarget->m_POI, pTarget->m_pBrushWhite, 1, NULL);
		pRenderTarget->DrawLine(CD2DPointF(pTarget->m_POI->left - 4, pTarget->m_POI->top - 4),
			CD2DPointF(pTarget->m_POI->right + 4, pTarget->m_POI->bottom + 4),
			pTarget->m_pBrushWhite);
		pRenderTarget->DrawLine(CD2DPointF(pTarget->m_POI->left - 4, pTarget->m_POI->bottom + 4),
			CD2DPointF(pTarget->m_POI->right + 4, pTarget->m_POI->top - 4),
			pTarget->m_pBrushWhite);
	}

	else if (pTarget->pDoc) {

		// draw white crosses to user define FOV
		for (size_t i = 0; i < pTarget->pDoc->raster.corner.size(); i++) {
			pRenderTarget->DrawLine(CD2DPointF(pTarget->pDoc->raster.corner[i].x - 7, pTarget->pDoc->raster.corner[i].y - 7),
				CD2DPointF(pTarget->pDoc->raster.corner[i].x + 7, pTarget->pDoc->raster.corner[i].y + 7),
				pTarget->m_pBrushWhite,
				1,
				NULL);
			pRenderTarget->DrawLine(CD2DPointF(pTarget->pDoc->raster.corner[i].x - 7, pTarget->pDoc->raster.corner[i].y + 7),
				CD2DPointF(pTarget->pDoc->raster.corner[i].x + 7, pTarget->pDoc->raster.corner[i].y - 7),
				pTarget->m_pBrushWhite,
				1,
				NULL);
		}


	}

	// catch pushing A button

	if (pTarget->m_bFireUp && pTarget->m_bFireDown) {

		pTarget->m_bFireDown = false;

		switch (pTarget->m_fired % 5) {

		case 0:
			pTarget->pDoc->raster.corner.size() == 4 ? pTarget->OnLButtonDown(0, CPoint(0, 0)) : 0;
			pTarget->m_fired++;
			break;
		case 1:
			pTarget->OnLButtonDown(0, CPoint(
				(int)pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				(int)pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->xbox_cross.x += pTarget->fieldsize;
			pTarget->m_fired++;
			break;
		case 2:
			pTarget->OnLButtonDown(0, CPoint(
				(int)pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				(int)pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			if (pTarget->pDoc->m_FlipVertical) {
				pTarget->xbox_cross.y += pTarget->fieldsize;
			}
			else {
				pTarget->xbox_cross.y -= pTarget->fieldsize;
			}

			pTarget->m_fired++;
			break;
		case 3:
			pTarget->OnLButtonDown(0, CPoint(
				(int)pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				(int)pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->xbox_cross.x -= pTarget->fieldsize;
			pTarget->m_fired++;
			break;
		case 4:
			pTarget->OnLButtonDown(0, CPoint(
				(int)pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				(int)pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->show_cross = false;
			pTarget->finishCalibration();
			pTarget->setCross();
			pTarget->m_fired++;
			
		break;

		}

	}


	// draw cross while moving around with pov hat

	if (pTarget->show_cross) {
		// points outlining the cross
		CD2DPointF a(pTarget->xbox_cross.x - 7, pTarget->xbox_cross.y - 7);
		CD2DPointF b(pTarget->xbox_cross.x + 7, pTarget->xbox_cross.y + 7);
		CD2DPointF c(pTarget->xbox_cross.x + 7, pTarget->xbox_cross.y - 7);
		CD2DPointF d(pTarget->xbox_cross.x - 7, pTarget->xbox_cross.y + 7);

		pRenderTarget->DrawLine(CD2DPointF(a.x + pTarget->xbox_state.LX, a.y + pTarget->xbox_state.LY),
			CD2DPointF(b.x + pTarget->xbox_state.LX, b.y + pTarget->xbox_state.LY),
			pTarget->m_pBrushWhite, 1, NULL);

		pRenderTarget->DrawLine(CD2DPointF(c.x + pTarget->xbox_state.LX, c.y + pTarget->xbox_state.LY),
			CD2DPointF(d.x + pTarget->xbox_state.LX, d.y + pTarget->xbox_state.LY),
			pTarget->m_pBrushWhite, 1, NULL);
	}

	return 0;

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

void Target::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CD2DPointF d2dpoint;
	d2dpoint = static_cast<CD2DPointF>(point);

	if (pDoc->raster.corner.size() < 4)
		pDoc->raster.corner.push_back(d2dpoint);

	else {
		free(m_POI);
		m_POI = NULL;
		pDoc->raster.meanAlpha = 0;
		pDoc->raster.meanEdge = 0;
		pDoc->raster.corner.clear();
		pDoc->raster.perimeter.clear();
		pDoc->m_pGrid->ClearPatchlist();
	}

	pDoc->IsModified();
	pDoc->UpdateAllViews(NULL);
	
	CSingleLock singleLock(&m_CritSection);
	singleLock.Lock();
	RedrawWindow();
	singleLock.Unlock();


	// call default
	// CDialog::OnLButtonUp(nFlags, point);
}


void Target::OnClose()
{

	// TODO: Add your message handler code here and/or call default

	m_bRunning = false;
	WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	delete m_pThread;

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"WP_Target", (LPBYTE)&wp, sizeof(wp));

}


void Target::OnShowWindow(BOOL bShow, UINT nStatus)
{

	// TODO: Add your message handler code here
	static bool bOnce = true;
	
	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(L"Settings", L"WP_Target", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
		}
	
		delete[] lwp;
	}

	if (!m_bRunning) {
		
		m_pThread = AfxBeginThread(InputControllerThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_pThread->m_bAutoDelete = FALSE;
		m_bRunning = true;
		m_pThread->ResumeThread();

	}

}

UINT Target::InputControllerThread(LPVOID pParam)
{
	CXBOXController* Player1 = new CXBOXController(1);
	Target* pTarget = (Target*)pParam;
	int flip= pTarget->m_flip;
	while (pTarget->m_bRunning){
		
		if (Player1->GetState().Gamepad.wButtons == 0) {

			m_bPushed = false;
			m_bFireUp = true;

		}

		else

			show_cross = true;

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)

		{

			m_bFireUp = false;
			m_bFireDown = true;

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {

			if (!m_bPushed) {
				xbox_state.LY -= flip*2;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LY -= flip*2;

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			if (!m_bPushed) {
				xbox_state.LY += flip*2;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LY += flip*2;

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			if (!m_bPushed) {
				xbox_state.LX -= 2;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LX -= 2;
		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			if (!m_bPushed) {
				xbox_state.LX += 2;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LX += 2;
		}

		Sleep(50);
		pTarget->Invalidate();

	}
	
	delete Player1;

	return 0;

}


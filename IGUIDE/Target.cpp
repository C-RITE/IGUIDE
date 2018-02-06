// Target.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "Target.h"


using namespace D2D1;
// Target dialog

XboxControlState Target::xbox_state;
bool Target::m_bPushed = false;
bool Target::show_cross = false;
bool Target::m_bFireDown = false;
bool Target::m_bFireUp = true;

IMPLEMENT_DYNAMIC(Target, CDialog);
CCriticalSection m_CritSection;

Target::Target(CWnd* pParent /*=NULL*/)
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
	m_animCtrl.SetRelatedWnd(this);
	m_animCtrl.EnableAnimationTimerEventHandler();

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

	LPRECT wRect = new CRect;
	CMonitor secondary = mons.GetMonitor(0);
	secondary.GetMonitorRect(wRect);
	CRect cRect = (CRect)wRect;
	xbox_cross = CD2DPointF(cRect.Width() / 2 - fieldsize / 2, cRect.Height() / 2 - fieldsize / 2);
	delete wRect;

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

	float alpha, beta, gamma;
	float pi = atan(1) * 4;
	float a, b, c, x, y;
	ppd_client = (1 / pDoc->raster.size) * pDoc->raster.meanEdge;

	Edge k;
	k.q.x = -centerOffset_x;
	k.q.y = centerOffset_y;

	alpha = pDoc->raster.meanAlpha;
	beta = 360 - pDoc->ComputeOrientationAngle(k);
	gamma = beta - alpha;


	a = centerOffset_x;
	b = centerOffset_y;
	c = sqrt(pow(a, 2) + pow(b, 2));

	y = sin(gamma * pi / 180) * c * ppd_client; // calc. x shift and scale to client ppd
	x = cos(gamma * pi / 180) * c * ppd_client; // calc. y shift and scale to client ppd

	*m_POI = { CD2DRectF(
		((pDoc->raster.mid.x + x) - 10),
		((pDoc->raster.mid.y + y) - 10),
		((pDoc->raster.mid.x + x) + 10),
		((pDoc->raster.mid.y + y) + 10))
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

	float scalingFactor = (float)pTarget->pDoc->m_FixationTargetSize / 100;

	// custom fixation target
	if (pTarget->m_POI && pTarget->m_pFixationTarget->IsValid()) {
		CD2DSizeF size = pTarget->m_pFixationTarget->GetSize();
		pRenderTarget->DrawBitmap(pTarget->m_pFixationTarget, CD2DRectF(
			pTarget->m_POI->left - (size.width / 2) * scalingFactor,
			pTarget->m_POI->top - (size.height / 2) * scalingFactor,
			pTarget->m_POI->right + (size.width / 2) * scalingFactor,
			pTarget->m_POI->bottom + (size.height / 2) * scalingFactor)
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

	else {

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
				pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->xbox_cross.x += pTarget->fieldsize;
			pTarget->m_fired++;
			break;
		case 2:
			pTarget->OnLButtonDown(0, CPoint(
				pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->xbox_cross.y -= pTarget->fieldsize;
			pTarget->m_fired++;
			break;
		case 3:
			pTarget->OnLButtonDown(0, CPoint(
				pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->xbox_cross.x -= pTarget->fieldsize;
			pTarget->m_fired++;
			break;
		case 4:
			pTarget->OnLButtonDown(0, CPoint(
				pTarget->xbox_cross.x +
				pTarget->xbox_state.LX,
				pTarget->xbox_cross.y +
				pTarget->xbox_state.LY));
			pTarget->show_cross = false;
			//pTarget->zoomIn();
			pTarget->finish();
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

	CRect animRect;

	pTarget->animationRect.GetValue(animRect);
	pRenderTarget->DrawRectangle(animRect, pTarget->m_pBrushWhite);

	return 0;

}

void Target::finish() {

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	CMDIChildWnd *pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CIGUIDEView *pView = (CIGUIDEView*)pChild->GetActiveView();
	CRect mainWnd;
	CPoint center;
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	pView->OnLButtonUp(0, center);

}

void Target::zoomIn() {

	int x = 0, y = 0;
	for (auto it = pDoc->raster.corner.begin(); it != pDoc->raster.corner.end(); it++) {
		x += it._Ptr->x;
		y += it._Ptr->y;
	}

	x /= 4;
	y /= 4;

	//CD2DPathGeometry pGeo(m_pRenderTarget, TRUE);
	//CD2DGeometrySink gSink(pGeo);
	//gSink.BeginFigure(CD2DPointF(pDoc->raster.corner[0].x, pDoc->raster.corner[0].y), D2D1_FIGURE_BEGIN_FILLED);
	//D2D1_POINT_2F points[3] = {
	//	D2D1::Point2F(),
	//	D2D1::Point2F(236, 192),
	//	D2D1::Point2F(212, 160),
	//	D2D1::Point2F(156, 255),
	//	D2D1::Point2F(346, 255),
	//};
	//
	//m_pRenderTarget->DrawGeometry();
	
	animationRect = CRect(x,y,x,y);
	animationRect.AddTransition(new CAccelerateDecelerateTransition(2, pDoc->raster.corner[0].x),
		new CAccelerateDecelerateTransition(2, pDoc->raster.corner[0].y),
		new CAccelerateDecelerateTransition(2, pDoc->raster.corner[1].x),
		new CAccelerateDecelerateTransition(2, pDoc->raster.corner[2].y));

	m_animCtrl.AddAnimationObject(&animationRect);
	m_animCtrl.AnimateGroup(0);

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
	m_pThread->Delete();
	delete m_pThread;

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"WP_Target", (LPBYTE)&wp, sizeof(wp));
	CDialog::OnClose();

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
				xbox_state.LY -= 1;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LY -= 1;

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			if (!m_bPushed) {
				xbox_state.LY += 1;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LY += 1;

		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			if (!m_bPushed) {
				xbox_state.LX -= 1;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LX -= 1;
		}

		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			if (!m_bPushed) {
				xbox_state.LX += 1;
				m_bPushed = true;
				Sleep(100);
			}

			else
				xbox_state.LX += 1;
		}

		Sleep(50);
		pTarget->Invalidate();

	}
	
	delete Player1;

	return 0;

}

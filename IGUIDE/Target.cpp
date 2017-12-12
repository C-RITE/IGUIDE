// Target.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
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
	EnableD2DSupport();
	m_pBrushWhite = new CD2DSolidColorBrush(GetRenderTarget(), ColorF(ColorF::White));
	m_POI = NULL;
	m_pFixationTarget = NULL;
	Player1 = new CXBOXController(1);
	THREADSTRUCT *_param = new THREADSTRUCT;
	_param->_this = this;
	xboxThread = AfxBeginThread(StartThread, _param);

}

Target::~Target()
{
	delete m_pBrushWhite;
	delete m_pFixationTarget;
	delete m_POI;
	delete Player1;

}

void Target::getFixationTarget() {
	
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CString file = pDoc->m_FixationTarget;
	if (m_pFixationTarget && m_pFixationTarget->IsValid())
		m_pFixationTarget->~CD2DBitmap();
	
	m_pFixationTarget = new CD2DBitmap(GetRenderTarget(), file);

}

void Target::Pinpoint(float centerOffset_x, float centerOffset_y)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
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

	float scalingFactor = (float)pDoc->m_FixationTargetSize / 100;

	// custom fixation target
	if (m_POI && m_pFixationTarget) {
		CD2DSizeF size = m_pFixationTarget->GetSize();
		pRenderTarget->DrawBitmap(m_pFixationTarget, CD2DRectF(
			m_POI->left - (size.width/2) * scalingFactor,
			m_POI->top - (size.height/2) * scalingFactor,
			m_POI->left + (size.width/2) * scalingFactor,
			m_POI->top + (size.height/2) * scalingFactor)
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

	if (xbox_state.LX !=0 || xbox_state.LY !=0) {

		pRenderTarget->DrawLine(CD2DPointF(25, 25),
			CD2DPointF(50, 50),
			m_pBrushWhite,
			1,
			NULL);
		pRenderTarget->DrawLine(CD2DPointF(50, 25),
			CD2DPointF(25, 50),
			m_pBrushWhite,
			1,
			NULL);

		CString traceText;
		CD2DSolidColorBrush WhiteBrush(NULL, ColorF(ColorF::White));

		CD2DSizeF sizeTarget = pRenderTarget->GetSize();
		CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

		CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
			_T("Consolas"),								// font family name
			sizeDpi.height / 9);						// font size

		traceText.Format(L"%.1f : %.1f", xbox_state.LX, xbox_state.LY);
		CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
			traceText,									// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		pRenderTarget->DrawTextLayout(
			CD2DPointF(0, 0),
			&textLayout,
			&WhiteBrush);
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
		pDoc->m_pGrid->ClearPatchlist();
	}

	pDoc->UpdateAllViews(NULL);
	RedrawWindow();

	// call default
	// CDialog::OnLButtonUp(nFlags, point);
}


void Target::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	xboxThread->SuspendThread();
	CDialog::OnClose();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"WP_Target", (LPBYTE)&wp, sizeof(wp));
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

		if (AfxGetApp()->GetProfileBinary(L"Settings", L"WP_Target", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
	
	getFixationTarget();

}

UINT Target::StartThread(LPVOID param)
{

	// XBOX Gamepad Controller Thread

	THREADSTRUCT* ts = (THREADSTRUCT*)param;
	
	while(true)
	{
		
			if (ts->_this->Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				ts->_this->Player1->Vibrate(65535, 0);
			}
			if (ts->_this->Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				ts->_this->Player1->Vibrate(0,0);
			}
			
			float LX = ts->_this->Player1->GetState().Gamepad.sThumbLX;
			float LY = ts->_this->Player1->GetState().Gamepad.sThumbLY;

			//determine how far the controller is pushed
			float magnitude = sqrt(LX*LX + LY*LY);

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767) magnitude = 32767;

				//adjust magnitude relative to the end of the dead zone
				magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
			}

			//determine the direction the controller is pushed
			float normalizedLX = LX / normalizedMagnitude;
			float normalizedLY = LY / normalizedMagnitude;

			ts->_this->xbox_state.LX = normalizedLX;
 			ts->_this->xbox_state.LY = normalizedLY;
			
			if(ts)ts->_this->RedrawWindow();
	}


	//you can also call AfxEndThread() here
	
	return 1;

}
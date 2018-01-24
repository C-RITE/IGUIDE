// Calibration.cpp : implementation file
//

#include "stdafx.h"
#include "Calibration.h"
#include "afxdialogex.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"
#include "Resource.h"
using namespace D2D1;

// D2DStatic

IMPLEMENT_DYNAMIC(D2DStatic, CStatic)


void D2DStatic::PreSubclassWindow()
{
	EnableD2DSupport(D2D1_FACTORY_TYPE_MULTI_THREADED);
	CIGUIDEDoc* pDoc = GetDoc();
	fundus = new CD2DBitmap(GetRenderTarget(), *pDoc->m_pFundus->filename);
	fundus->Create(GetRenderTarget());
}

D2DStatic::D2DStatic(Calibration* Dialog) :
	m_pBrushWhite(new CD2DSolidColorBrush(NULL, ColorF(ColorF::White))),
	m_pDlg(Dialog),
	m_clicked(0)
{
	
}

D2DStatic::~D2DStatic(){

	delete m_pBrushWhite;

}


BEGIN_MESSAGE_MAP(D2DStatic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &D2DStatic::OnDraw2d)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// D2DStatic message handlers

afx_msg LRESULT D2DStatic::OnDraw2d(WPARAM wParam, LPARAM lParam) {

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

		CD2DSizeF size = fundus->GetSize();
		CRect rect;
		GetWindowRect(&rect);
		//float r = (float)rect.Height() / (float)rect.Width();

		pRenderTarget->DrawBitmap(fundus, CD2DRectF(0, 0, rect.Width(), rect.Height()));

		switch (m_clicked){
			case 2:
				pRenderTarget->DrawLine(
					k.p,
					CD2DPointF(m_curMousePos.x, m_curMousePos.y),
					m_pBrushWhite,
					1,
					NULL);
				break;
			case 3:
				pRenderTarget->DrawLine(
					k.p,
					k.q,
					m_pBrushWhite,
					1,
					NULL);
				break;
		}

		m_pBrushWhite->Detach();

	return (LRESULT)TRUE;

}

void D2DStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	CIGUIDEDoc* pDoc = GetDoc();
	CRect rect;
	CD2DSizeF size = fundus->GetSize();
	GetWindowRect(&rect);
	float r = (float)rect.Height() / (float)rect.Width();
	if (!m_clicked)
		m_clicked++;

	switch (m_clicked) {
		case 1:
			k.p = { (float)point.x , (float)point.y };
			m_clicked++;
			break;
		case 2:
			k.q = { (float)point.x , (float)point.y };
			m_clicked++;
			break;
	}

}

void D2DStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
			
	Edge k;
	m_clicked = 0;
	RedrawWindow();
			
}

void D2DStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	m_curMousePos = static_cast<CD2DPointF>(point);
	RedrawWindow();

}

BOOL D2DStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	::SetCursor(LoadCursor(NULL, IDC_CROSS));
	return TRUE;
}



// Calibration dialog

IMPLEMENT_DYNAMIC(Calibration, CDialog)

Calibration::Calibration(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CALIBRATION, pParent),
	Shrink2Fit(.9125),
	m_D2DStatic(this),
	m_sFactor(1)
{
}

Calibration::~Calibration()
{
}

void Calibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAL, m_D2DStatic );
}

void Calibration::ClientResize(int nWidth, int nHeight)
{
	CIGUIDEDoc* pDoc;
	pDoc = GetDoc();
	CD2DSizeF size = pDoc->m_pFundus->picture->GetSize();
	RECT rcClient, rcWind;
	POINT ptDiff;
	GetClientRect(&rcClient);
	GetWindowRect(&rcWind);
	ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
	ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
	MoveWindow(m_WorkArea.right / 2 - size.width / 2,
		(m_WorkArea.bottom - 75) / 2 - size.height / 2,
		nWidth + ptDiff.x,
		nHeight + ptDiff.y,
		TRUE);
}

BEGIN_MESSAGE_MAP(Calibration, CDialog)
	ON_BN_CLICKED(IDOK, &Calibration::OnBnClickedOk)
END_MESSAGE_MAP()

void Calibration::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CIGUIDEDoc* pDoc = GetDoc();
	switch (m_D2DStatic.m_clicked) {
		case (3) :
			pDoc->m_pFundus->calibration = TRUE;
			m_D2DStatic.m_clicked = 0;
			m_D2DStatic.k.length = pDoc->CalcEdgeLength(m_D2DStatic.k);
			m_D2DStatic.k.alpha = pDoc->ComputeDisplacementAngle(m_D2DStatic.k);
			CDialog::OnOK();
			break;
		default:
			AfxMessageBox(_T("Calibration incomplete!\n\nPlease select two distinct points in image to proceed."), MB_OK | MB_ICONSTOP);
	}
}

BOOL Calibration::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add your specialized code here and/or call the base class
	CIGUIDEDoc* pDoc = GetDoc();

	// Get the current work area
	SystemParametersInfo(SPI_GETWORKAREA, 0, &m_WorkArea, 0);

	// Get the OK button dimensions
	CWnd* OK = GetDlgItem(IDOK);
	CRect OKRect;
	OK->GetClientRect(&OKRect);

	// TODO:  Add extra initialization here)
	CD2DSizeF size = pDoc->m_pFundus->picture->GetSize();

	ry = size.height / m_WorkArea.bottom;
	rx = size.width / m_WorkArea.right;

	if ((size.width + 20 > m_WorkArea.right) | (size.height + 50 > m_WorkArea.bottom)) {

		m_D2DStatic.MoveWindow(10,
			10,
			size.width / max(rx, ry) * Shrink2Fit,
			size.height / max(rx, ry) * Shrink2Fit);

		m_D2DStatic.GetClientRect(m_ClientRect);
		m_sFactor = size.width / m_ClientRect.Width();

		MoveWindow(m_WorkArea.right / 2 - (m_ClientRect.Width() + 40) /2,
			m_WorkArea.bottom / 2 - 60 - m_ClientRect.Height() / 2,
			m_ClientRect.Width() + 40,
			m_ClientRect.Height() + 120);
		
		OK->MoveWindow(m_ClientRect.Width() / 2 - OKRect.Width() / 2,
			m_ClientRect.Height() + 37,
			OKRect.Width(),
			OKRect.Height());
	}

	else {

		ClientResize(size.width + 20, size.height + 60);
		GetClientRect(m_ClientRect);

		m_D2DStatic.MoveWindow(10,
			10,
			size.width,
			size.height);

		OK->MoveWindow(m_ClientRect.Width() / 2 - OKRect.Width() / 2,
			size.height + 23,
			OKRect.Width(),
			OKRect.Height());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

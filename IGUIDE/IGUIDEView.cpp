
// IGUIDEView.cpp : implementation of the CIGUIDEView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IGUIDE.h"
#endif
#include "MainFrm.h"
#include "IGUIDEView.h"
#include "IGUIDEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;

// CIGUIDEView

IMPLEMENT_DYNCREATE(CIGUIDEView, CView)

BEGIN_MESSAGE_MAP(CIGUIDEView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CIGUIDEView::OnDraw2d)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DISPLAYCHANGE, &CIGUIDEView::OnDisplayChange)
	ON_MESSAGE(SCREEN_SELECTED, &CIGUIDEView::ChangeTargetDisplay)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CIGUIDEView construction/destruction

CIGUIDEView::CIGUIDEView()
{
	m_pDlgTarget = NULL;
	m_pFixationTarget = NULL;
}

CIGUIDEView::~CIGUIDEView()
{
	delete m_pDlgTarget;
}

CIGUIDEView * CIGUIDEView::GetView()
{
	CFrameWndEx * pFrame = (CFrameWndEx *)(AfxGetApp()->m_pMainWnd);

	CView * pView = pFrame->GetActiveView();

	if (!pView)
		return NULL;

	// Fail if view is of wrong kind
	// (this could occur with splitter windows, or additional
	// views on a single document

	if (!pView->IsKindOf(RUNTIME_CLASS(CIGUIDEView)))
		return NULL;

	return (CIGUIDEView*)pView;

}


// CIGUIDEView printing

BOOL CIGUIDEView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIGUIDEView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIGUIDEView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CIGUIDEView message handlers

void CIGUIDEView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (pDoc->CheckFOV()) {
		pDoc->m_pGrid->StorePatch(static_cast<CD2DPointF>(point));
	}
	else
		return;

	m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
	free(pDoc->mousePos);
	pDoc->mousePos = NULL;
	ShowCursor(TRUE);

	m_pDlgTarget->Invalidate();
	Invalidate();

}


void CIGUIDEView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (pDoc->mousePos)
		if ((GetKeyState(VK_LBUTTON) < 0) & (pDoc->raster.corner.size() == 4)) {
			pDoc->mousePos->SetPoint(point.x, point.y);
			RedrawWindow();
		}

}


void CIGUIDEView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	if (!pDoc->mousePos && pDoc->CheckFOV()) {
		pDoc->mousePos = (CPoint*)malloc(sizeof(CPoint));
		pDoc->mousePos->SetPoint(point.x, point.y);
	}

	ShowCursor(FALSE);

	RedrawWindow();

}


void CIGUIDEView::OnInitialUpdate()
{
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL);

	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	AfxGetMainWnd()->SendMessage(DOC_IS_READY);
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	pDoc->m_Controller.reset();

	
}

LRESULT CIGUIDEView::ChangeTargetDisplay(WPARAM w, LPARAM l) {

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	m_pDlgTarget->ShowWindow(TRUE);
	
	CRect area = (CRect)pDoc->m_selectedScreen->area;
	CRect wRect;
	
	m_pDlgTarget->SetWindowPos(&this->wndBottom,
		area.left - wRect.left ,
		area.top - wRect.top,
		area.Width(),
		area.Height(),
		NULL);
	
	return 0;

}


void CIGUIDEView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{

	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	CHwndRenderTarget* pRenderTarget = GetRenderTarget();

	CD2DPointF* FOV = new CD2DPointF[4];

	CRect clientRect;
	AfxGetMainWnd()->GetClientRect(&clientRect);

	// make edges from corners
	if (pDoc->raster.corner.size() == 4 && pDoc->raster.meanEdge == 0) {
		for (int i = 0; i < 3; i++) {
			Edge k;
			k.p = pDoc->raster.corner[i];
			k.q = pDoc->raster.corner[i + 1];
			pDoc->raster.perimeter.push_back(k);
		}
		for (int i = 0; i < 4; i++)
			FOV[i] = pDoc->raster.corner[i];
		pDoc->raster.mid = pDoc->compute2DPolygonCentroid(FOV, 4);
		Edge k;
		k.p = pDoc->raster.corner[3];
		k.q = pDoc->raster.corner[0];
		pDoc->raster.perimeter.push_back(k);
	}

	// calculate length of edges, mean edge length and angle
	if (pDoc->raster.corner.size() == 4 && pDoc->raster.meanEdge == 0) {
		pDoc->ComputeDisplacementAngles();
		if (pDoc->CheckCalibrationValidity()) {
			for (size_t i = 0; i < pDoc->raster.perimeter.size(); i++) {
				pDoc->raster.perimeter[i].length = pDoc->CalcEdgeLength(pDoc->raster.perimeter[i]);
				pDoc->raster.meanEdge += pDoc->raster.perimeter[i].length;
				pDoc->raster.meanAlpha += pDoc->raster.perimeter[i].alpha;
			}

			pDoc->raster.meanEdge /= 4;
			pDoc->raster.meanAlpha /= 4;
		}
		else {
			AfxMessageBox(L"Calibration failed. Please retry!", MB_OK | MB_ICONSTOP);
			m_pDlgTarget->OnLButtonDown(NULL, NULL);
		}
	
	}

	ASSERT_VALID(pRenderTarget);

	if (!pDoc->m_pFundus->filename.IsEmpty()) {
		pDoc->m_pFundus->picture = new CD2DBitmap(pRenderTarget, pDoc->m_pFundus->filename);
		pDoc->m_pFundus->picture->Create(pRenderTarget);
	}

	pDoc->m_pGrid->center.x = (float)clientRect.CenterPoint().x;
	pDoc->m_pGrid->center.y = (float)clientRect.CenterPoint().y;

	m_pDlgTarget->pDoc = pDoc;
	m_pDlgTarget->getFixationTarget();	

	if (m_pFixationTarget && m_pFixationTarget->IsValid())
		delete m_pFixationTarget;

	m_pFixationTarget = new CD2DBitmap(GetRenderTarget(), pDoc->m_FixationTarget, CD2DSizeU(0, 0), TRUE);

	SetFocus();
	Invalidate();

	delete FOV;

}

int CIGUIDEView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Enable D2D support for this window:
	EnableD2DSupport();

	// TODO:  Add your specialized creation code here
	//Invalidate();

	return 0;

}

// CIGUIDEView drawing

afx_msg LRESULT CIGUIDEView::OnDraw2d(WPARAM wParam, LPARAM lParam)
{

	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	ASSERT_VALID(pRenderTarget);	

	if (!pDoc)
		return FALSE;

	// TODO: add draw code for native data here
	if (pRenderTarget) {

		pRenderTarget->Clear(ColorF(ColorF::Black));
		pDoc->m_pFundus->Paint(pRenderTarget);
		pDoc->m_pGrid->Paint(pRenderTarget);
		pDoc->m_pGrid->Mark(pRenderTarget);
		pDoc->m_pGrid->DrawOverlay(pRenderTarget);

		CD2DSizeF sizeTarget = pRenderTarget->GetSize();
		CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
		CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
			_T("Consolas"),								// font family name
			sizeDpi.height / 8);							// font size
		CD2DTextFormat textFormat2(pRenderTarget,		// pointer to the render target
			_T("Consolas"),								// font family name
			sizeDpi.height / 4);

		if (pDoc->m_pGrid->overlay & TRACEINFO) {

			CString traceText = pDoc->getTraceInfo();
			// construct a CD2DTextLayout object which represents a block of formatted text 
			CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
				traceText,									// text to be drawn
				textFormat,									// text format
				sizeTarget);								// size of the layout box

			pRenderTarget->DrawTextLayout(CD2DPointF(sizeTarget.width - 210, 5),
															// place on top-right corner
				&textLayout,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::LightGreen)));

		}

		if (pDoc->m_pGrid->overlay & DEFOCUS) {

			CString defocus = L"DEFOCUS: ";
			defocus.Append(pDoc->getCurrentDefocus());

			CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
				defocus,									// text to be drawn
				textFormat2,									// text format
				sizeTarget);								// size of the layout box

			pRenderTarget->DrawTextLayout(CD2DPointF(5),
				// place on top-right corner
				&textLayout,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::White)));

		}


		if (pDoc->m_pGrid->overlay & QUICKHELP) {

			vector<CString> help = pDoc->getQuickHelp();

			CD2DPointF down_middle{ sizeTarget.width / 2 - 100, sizeTarget.height - 200 };
			CD2DPointF down_left{ down_middle.x - 250, sizeTarget.height - 200 };
			CD2DPointF down_right{ down_middle.x + 250, sizeTarget.height - 200 };

			CD2DSolidColorBrush BlackBrush{ pRenderTarget, D2D1::ColorF(D2D1::ColorF::Black, 0.5f) };
			CD2DSolidColorBrush YellowGreenBrush{ pRenderTarget, D2D1::ColorF(D2D1::ColorF::PaleGoldenrod) };

			CD2DRectF black_box{ down_left.x - 5, down_left.y - 5, down_right.x + 215, down_right.y + 120};
			pRenderTarget->FillRectangle(black_box, &BlackBrush);
			pRenderTarget->DrawRectangle(black_box, &YellowGreenBrush);

			CD2DTextLayout AOSACA_help(pRenderTarget,		// pointer to the render target 
				help[0],									// text to be drawn
				textFormat,									// text format
				sizeTarget);								// size of the layout box

			CD2DTextLayout IGUIDE_help(pRenderTarget,		// pointer to the render target 
				help[1],									// text to be drawn
				textFormat,									// text format
				sizeTarget);								// size of the layout box

			CD2DTextLayout ICANDI_help(pRenderTarget,		// pointer to the render target 
				help[2],									// text to be drawn
				textFormat,									// text format
				sizeTarget);								// size of the layout box


			pRenderTarget->DrawTextLayout(down_left,		// top-left corner of the text 
				&AOSACA_help,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::PaleGoldenrod)));

			pRenderTarget->DrawTextLayout(down_middle,		// top-left corner of the text 
				&IGUIDE_help,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::PaleGoldenrod)));

			pRenderTarget->DrawTextLayout(down_right,		// top-left corner of the text 
				&ICANDI_help,								// text layout object
				&CD2DSolidColorBrush						// brush used for text
				(pRenderTarget,
					D2D1::ColorF(D2D1::ColorF::PaleGoldenrod)));

		}

		// is (custom) fixation target on or off?
		if (m_pDlgTarget->m_bVisible) {

			CD2DBrushProperties prop{ .5f };
			CD2DSolidColorBrush brush{ pRenderTarget, D2D1::ColorF(D2D1::ColorF::Beige), &prop };
			CD2DRectF upperRight{sizeTarget.width - 100,
								50, sizeTarget.width - 50,
								100 };
			pRenderTarget->DrawRectangle(CD2DRectF(
				upperRight.left-1,
				upperRight.top-1,
				upperRight.right+1,
				upperRight.bottom+1),
				&brush);

			if (m_pFixationTarget && m_pFixationTarget->IsValid()) {
				CD2DSizeF size = m_pFixationTarget->GetSize();
				pRenderTarget->DrawBitmap(
					m_pFixationTarget,
					upperRight,
					0.25f,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
			}

			else {

				CD2DRectF frame(upperRight);
				frame.left += 15;
				frame.right -= 15;
				frame.top += 15;
				frame.bottom -= 15;
				pRenderTarget->DrawEllipse(frame, &brush);
				pRenderTarget->DrawLine(
					CD2DPointF(frame.left, frame.top), 
					CD2DPointF(frame.right, frame.bottom),
					&brush);
				pRenderTarget->DrawLine(
					CD2DPointF(frame.right, frame.top),
					CD2DPointF(frame.left, frame.bottom),
					&brush);

			}

		}
			   
	}

	return 0;

}

void CIGUIDEView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	// all the drawing happens in OnDraw2D

}

BOOL CIGUIDEView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	// keyboard controls depending on drawn patches
	if (pDoc->m_pGrid->patchlist.size() > 0) {
		
		if (pMsg->message == WM_KEYDOWN) {
			switch (pMsg->wParam) {
			case VK_UP:
				pDoc->m_pGrid->patchlist.back().coords.y += .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
				break;
			case VK_DOWN:
				pDoc->m_pGrid->patchlist.back().coords.y -= .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
				break;
			case VK_LEFT:
				pDoc->m_pGrid->patchlist.back().coords.x -= .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
				break;
			case VK_RIGHT:
				pDoc->m_pGrid->patchlist.back().coords.x += .1f;
				m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
				break;
			case VK_SPACE:
				pDoc->m_pGrid->patchlist.lockIn();
				break;

			}

		}

		// delete last patch with right mouse button

		if (pMsg->wParam == VK_RBUTTON) {
			if (pMsg->message == WM_MOUSEMOVE)
				return false;
			pDoc->m_pGrid->DelPatch();
			pDoc->m_pGrid->patchlist.SaveToFile();
		}

		if (pDoc->m_pGrid->patchlist.size() > 0)
			m_pDlgTarget->Pinpoint(pDoc->m_pGrid->patchlist.back().coords.x, pDoc->m_pGrid->patchlist.back().coords.y);
		else
			m_pDlgTarget->m_POI = NULL;

		m_pDlgTarget->Invalidate();

		this->Invalidate();
		
	}

	// other keyboard controls

	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case VK_F1:
			pDoc->OnOverlayQuickhelp();
			break;

		case VK_F2:
			pDoc->ToggleOverlay();
			break;

		case VK_F3:
			ToggleFixationTarget();
			break;
		}

	}

	return CView::PreTranslateMessage(pMsg);

}

BOOL CIGUIDEView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{

	// TODO: Add your specialized code here and/or call the base class
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();
	m_pDlgTarget = new Target();
	m_pDlgTarget->Create(IDD_TARGET, pParentWnd);

	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}



void CIGUIDEView::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	m_pDlgTarget->SendMessage(WM_CLOSE);
	CView::OnClose();

}


afx_msg LRESULT CIGUIDEView::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	m_pDlgTarget->ShowWindow(SW_HIDE);
	AfxMessageBox(L"Monitor configuration change detected.\n\nSelect a screen in Properties/Target View now!", MB_ICONWARNING);
	pDoc->getScreens();
	
	return 0L;

}


void CIGUIDEView::ToggleFixationTarget()
{
	m_pDlgTarget->m_bVisible ? m_pDlgTarget->m_bVisible = false : m_pDlgTarget->m_bVisible = true;
}


BOOL CIGUIDEView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CIGUIDEDoc* pDoc = (CIGUIDEDoc*)GetDocument();

	//CRect rect;
	//CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	//pDoc->raster.scale.x = (float)zDelta / (pMainWnd->WINDOW_WIDTH - 20);
	//pDoc->raster.scale.y = (float)zDelta / (pMainWnd->WINDOW_HEIGHT - 62);

	//RedrawWindow();

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

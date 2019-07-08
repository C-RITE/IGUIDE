#include "stdafx.h"
#include "Grid.h"
#include "Target.h"
#include "MainFrm.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "resource.h"
#include "Patches.h"

using namespace D2D1;

Grid::Grid()
{
	m_pBrushProp = new CD2DBrushProperties(.5f);
	m_pPatchBrush = new CD2DSolidColorBrush(NULL, NULL);
	m_pDarkRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::White));
	m_pGrayBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkGray), m_pBrushProp);
	m_pDarkGreenBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkGreen));
	m_pMagentaBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Magenta));

	lpHi = { D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		.4f,
		NULL,
		D2D1_LAYER_OPTIONS_NONE };
	pLayer = new CD2DLayer(NULL);
	
}

Grid::~Grid() {

	delete m_pDarkRedBrush;
	delete m_pRedBrush;
	delete m_pWhiteBrush;
	delete m_pBlueBrush;
	delete m_pDarkGreenBrush;
	delete m_pMagentaBrush;;
	delete m_pPatchBrush;
	delete m_pGrayBrush;
	delete m_pBrushProp;
	delete pLayer;
}

void Grid::DelPatch() {

	if (patchlist.size() > 0)
		patchlist.pop_back();
}

void Grid::ClearPatchlist() {

	patchlist.clear();

}

void Grid::StorePatch(CD2DPointF loc) {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	Patch patch;

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	patch.coords.x = (center.x - loc.x)*-1 * (float)dpp;
	patch.coords.y = (center.y - loc.y)* (float)dpp;
	patch.color = pDoc->raster.color;
	patch.rastersize = pDoc->raster.size;
	patch.locked = false;
	patch.defocus = pDoc->getCurrentDefocus();
	patchlist.push_back(patch);

}

void Grid::Paint(CHwndRenderTarget* pRenderTarget) {

	pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	CMainFrame* pMainWnd = (CMainFrame*) AfxGetMainWnd();
	pMainWnd->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();

	dpp = (m_pDeltaFOD + m_pRadNerve) / pMainWnd->WINDOW_WIDTH;
	double ppd = 1 / dpp;	//pixel per degree

	// draw a grid background around the center
	if (overlay & GRID) {
		for (float x = - ppd / 2 ; x < mainWnd.Width(); x += ppd)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, - ppd / 2),
				CD2DPointF(x, (float)mainWnd.Height()),
				m_pGrayBrush,
				1
				);
		}

		
		for (float y = - ppd / 2; y < mainWnd.Height(); y += ppd)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(- ppd / 2, y),
				CD2DPointF((float)mainWnd.Width(), y),
				m_pGrayBrush,
				1
				);
		}

	}

	pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

}

void Grid::DrawOverlay(CHwndRenderTarget* pRenderTarget) {

	CMainFrame* mainWnd = (CMainFrame*)AfxGetMainWnd();

	//draw circles around the center
	if (overlay & DEGRAD) {
		for (float x = 0; x < 16; x++) {
			CD2DRectF e = { center.x - 1 / (float)dpp * x,
				center.y - 1 / (float)dpp * x,
				center.x + 1 / (float)dpp * x,
				center.y + 1 / (float)dpp * x };
			pRenderTarget->DrawEllipse(e, m_pGrayBrush, 1);
		}
	}

	// draw circles around foveola and fovea
	if (overlay & FOVEOLA) {
		CD2DRectF e1 = { center.x - 1 / (float)dpp * 1.2f,
		center.y - 1 / (float)dpp * 1.2f,
		center.x + 1 / (float)dpp * 1.2f,
		center.y + 1 / (float)dpp * 1.2f };
		pRenderTarget->DrawEllipse(e1, m_pRedBrush, .5f);
	}

	if (overlay & FOVEA){
		CD2DRectF e2 = { center.x - 1 / (float)dpp * 6.2f,
		center.y - 1 / (float)dpp * 6.2f,
		center.x + 1 / (float)dpp * 6.2f,
		center.y + 1 / (float)dpp * 6.2f };
	pRenderTarget->DrawEllipse(e2, m_pRedBrush, .3f);
	}

	// draw optic nerve as blue circle
	if (overlay & OPTICDISC) {
		nerve = { float(center.x + mainWnd->WINDOW_WIDTH / 2 - 5 / dpp),
		(float)((center.y - 2.5 / dpp) - 86),
		(float)(center.x + mainWnd->WINDOW_WIDTH / 2),
		(float)((center.y + 2.5 / dpp) - 86) };
		pRenderTarget->DrawEllipse(nerve, m_pBlueBrush, .3f);
	}

	// draw cross in fovea
	if (overlay & CROSSHAIR) {
		CD2DRectF fovea(center.x - 4,
			center.y - 4,
			center.x + 4,
			center.y + 4);

		pRenderTarget->DrawEllipse(fovea, m_pWhiteBrush);
		pRenderTarget->DrawLine(CD2DPointF(center.x - 8, center.y),
			CD2DPointF(center.x + 8, center.y),
			m_pWhiteBrush);
	pRenderTarget->DrawLine(CD2DPointF(center.x, center.y - 8),
		CD2DPointF(center.x, center.y + 8),
		m_pWhiteBrush);
	}

}

void Grid::Mark(CHwndRenderTarget* pRenderTarget) {

	// draw all marked locations (i.e. list of patches) in operator view

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	CD2DRectF rect1;
	CRect intersect;
	float rsdeg; // raster size in degree visual angle
	

	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {
		
		rsdeg = (float)pDoc->raster.videodim / it._Ptr->_Myval.rastersize; 

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + it._Ptr->_Myval.coords.x * (float)(1 / dpp) + (float)dpp * pDoc->raster.scale.x - (float)(rsdeg / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp - (float)(rsdeg / 2 / dpp),
			center.x + it._Ptr->_Myval.coords.x * 1 / (float)dpp + (float)dpp * pDoc->raster.scale.x + (float)(rsdeg / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp + (float)(rsdeg / 2 / dpp)
		};

		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

	}

	if (patchlist.size() > 0) {

		rsdeg = (float)pDoc->raster.videodim / patchlist.back().rastersize; // raster size in degree visual angle

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + patchlist.back().coords.x * (float)(1 / dpp) + (float)dpp * pDoc->raster.scale.x - (float)(rsdeg / 2 / dpp),
			center.y - patchlist.back().coords.y * 1 / (float)dpp - (float)(rsdeg / 2 / dpp),
			center.x + patchlist.back().coords.x * 1 / (float)dpp + (float)dpp * pDoc->raster.scale.x + (float)(rsdeg / 2 / dpp),
			center.y - patchlist.back().coords.y * 1 / (float)dpp + (float)(rsdeg / 2 / dpp)
		};
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, 2);
		pRenderTarget->PopLayer();

		CD2DEllipse center(&rect1);
		center.radiusX = center.radiusY = .5;
		
		pRenderTarget->DrawEllipse(center, m_pWhiteBrush, .5);

		ShowCoordinates(pRenderTarget, patchlist.back().coords.x, patchlist.back().coords.y, rect1);	
		
		
		int number = 1;
		for (auto it = pDoc->m_pGrid->patchlist.begin(); it != pDoc->m_pGrid->patchlist.end(); it++) {
			
			rsdeg = (double)pDoc->raster.videodim / (double)it._Ptr->_Myval.rastersize; // raster size in degree visual angle

			if (it._Ptr->_Myval.locked == true) {
				ShowVidNumber(pRenderTarget, it._Ptr->_Myval.coords.x, it._Ptr->_Myval.coords.y, rsdeg, number);
				number++;
			}
		}

	}


	if (pDoc && pDoc->mousePos) {
		
		rsdeg = (float)pDoc->raster.videodim / pDoc->raster.size;

		pRenderTarget->DrawRectangle(CD2DRectF(
			pDoc->mousePos->x - (float)(rsdeg / 2 / dpp),
			pDoc->mousePos->y - (float)(rsdeg / 2 / dpp),
			pDoc->mousePos->x + (float)(rsdeg / 2 / dpp),
			pDoc->mousePos->y + (float)(rsdeg / 2 / dpp)),
			m_pWhiteBrush,
			.5f,
			NULL);
	}

}





void Grid::ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, CRect rect)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	CString xCoords, yCoords;
	CD2DSizeF sizeTarget(rect.Width(), 10);
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
	CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
		_T("Consolas"),								// font family name
		sizeDpi.height /7);							// font size

	xCoords.Format(L"%.1f", xPos);
	CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
		xCoords,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	CSize marginX;
	CSize marginY;

	if (xPos >= 10.0f)
		marginX = { 28, 0 };
	else if (xPos >= 0.f)
		marginX = { 20, 0 };
	else if (xPos <= -10.f)
		marginX = { 35, 0 };
	else if (xPos < 0.f)
		marginX = { 28, 0 };

	if (yPos > 0)
		marginY = { 25, 4 };
	else
		marginY = { 33, 4 };
	
	pRenderTarget->DrawTextLayout(rect.BottomRight() - marginX, &textLayout, m_pWhiteBrush);

	yCoords.Format(L"%.1f", yPos);
	CD2DTextLayout textLayout2(pRenderTarget,		// pointer to the render target 
		yCoords,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	pRenderTarget->DrawTextLayout(rect.TopLeft() - marginY, &textLayout2, m_pWhiteBrush);

}

void Grid::ShowVidNumber(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, float rastersize, int number) {
	
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CString vidText;

	CD2DSizeF sizeTarget = pRenderTarget->GetSize();
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
	CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
		_T("Consolas"),								// font family name
		sizeDpi.height / 9);						// font size

	vidText.Format(L"%d", number);
	CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
		vidText,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	pRenderTarget->DrawTextLayout(
		CD2DPointF((xPos * 1 / (float)dpp + center.x) - (rastersize / 2 * 1 / (float)dpp), (yPos * -1 / (float)dpp + center.y) - (rastersize / 2 * 1 / (float)dpp)),
		&textLayout,
		m_pWhiteBrush);

}
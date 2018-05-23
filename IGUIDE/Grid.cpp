#include "stdafx.h"
#include "Grid.h"
#include "Target.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "resource.h"
#include "Patches.h"

using namespace D2D1;

Grid::Grid()
{
	m_pPatchBrush = new CD2DSolidColorBrush(NULL, NULL);
	m_pDarkRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::White));
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
	overlay = AfxGetApp()->GetProfileInt(L"Settings", L"Overlays", GRID | CROSSHAIR | FUNDUS);
	
}

Grid::~Grid() {

	delete m_pDarkRedBrush;
	delete m_pRedBrush;
	delete m_pBlueBrush;
	delete m_pWhiteBrush;
	delete m_pPatchBrush;
	delete m_pDarkGreenBrush;
	delete m_pMagentaBrush;;
	delete pLayer;
}

void Grid::DelPatch() {

	if (patchlist.size() > 0)
		patchlist.pop_back();
}

void Grid::ClearPatchlist() {

	patchlist.clear();

}

void Grid::StoreClick(CD2DPointF loc) {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	Patch patch;

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	patch.coords.x = (center.x - loc.x)*-1 * (float)dpp;
	patch.coords.y = (center.y - loc.y)* (float)dpp;
	patch.color = pDoc->raster.color;
	patch.rastersize = pDoc->raster.size;
	patch.locked = false;
	patchlist.push_back(patch);

}

void Grid::Paint(CHwndRenderTarget* pRenderTarget) {

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	dpp = (m_pDeltaFOD + m_pRadNerve) / (mainWnd.Width() / 2);

	// draw a grid background around the center
	if (overlay & GRID) {
		for (float x = center.x; x > 0; x -= 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 0.0f),
				CD2DPointF(x, (float)mainWnd.Height()),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float x = center.x + 1 / (float)dpp; x < mainWnd.Width(); x += 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 0.0f),
				CD2DPointF(x, (float)mainWnd.Height()),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float y = center.y; y > 0; y -= 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.0f, y),
				CD2DPointF((float)mainWnd.Width(), y),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float y = center.y + 1 / (float)dpp; y < mainWnd.Height(); y += 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.0f, y),
				CD2DPointF((float)mainWnd.Width(), y),
				m_pWhiteBrush,
				0.1f
				);
		}
	}

}

void Grid::DrawOverlay(CHwndRenderTarget* pRenderTarget) {

	//draw circles around the center
	if (overlay & DEGRAD) {
		for (float x = 0; x < 16; x++) {
			CD2DRectF e = { center.x - 1 / (float)dpp * x,
				center.y - 1 / (float)dpp * x,
				center.x + 1 / (float)dpp * x,
				center.y + 1 / (float)dpp * x };
			pRenderTarget->DrawEllipse(e, m_pWhiteBrush, .1f);
		}
	}

	// draw circles around foveola and fovea
	if (overlay & FOVEOLA) {
		CD2DRectF e1 = { center.x - 1 / (float)dpp * 1.2f,
		center.y - 1 / (float)dpp * 1.2f,
		center.x + 1 / (float)dpp * 1.2f,
		center.y + 1 / (float)dpp * 1.2f };
		pRenderTarget->DrawEllipse(e1, m_pRedBrush, .3f);
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
		nerve = { float(center.x + mainWnd.Width() / 2 - 5 / dpp),
		(float)((center.y - 2.5 / dpp) - 86),
		(float)(center.x + mainWnd.Width() / 2),
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


	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	CD2DRectF rect1;
	CRect intersect;

	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + it._Ptr->_Myval.coords.x * (float)(1 / dpp) + (float)dpp * pDoc->raster.scale.x - (float)(it._Ptr->_Myval.rastersize / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp - (float)(it._Ptr->_Myval.rastersize / 2 / dpp),
			center.x + it._Ptr->_Myval.coords.x * 1 / (float)dpp + (float)dpp * pDoc->raster.scale.x + (float)(it._Ptr->_Myval.rastersize / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp + (float)(it._Ptr->_Myval.rastersize / 2 / dpp)
		};
		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

	}

	if (patchlist.size() > 0) {

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + patchlist.back().coords.x * (float)(1 / dpp) + (float)dpp * pDoc->raster.scale.x - (float)(patchlist.back().rastersize / 2 / dpp),
			center.y - patchlist.back().coords.y * 1 / (float)dpp - (float)(patchlist.back().rastersize / 2 / dpp),
			center.x + patchlist.back().coords.x * 1 / (float)dpp + (float)dpp * pDoc->raster.scale.x + (float)(patchlist.back().rastersize / 2 / dpp),
			center.y - patchlist.back().coords.y * 1 / (float)dpp + (float)(patchlist.back().rastersize / 2 / dpp)
		};
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, 1);
		ShowCoordinates(pRenderTarget, patchlist.back().coords.x, patchlist.back().coords.y, (float)patchlist.back().rastersize);
		
		int number = 1;
		for (auto it = pDoc->m_pGrid->patchlist.begin(); it != pDoc->m_pGrid->patchlist.end(); it++) {
			if (it._Ptr->_Myval.locked == true) {
				ShowVidNumber(pRenderTarget, it._Ptr->_Myval.coords.x, it._Ptr->_Myval.coords.y, (float)it._Ptr->_Myval.rastersize, number);
				number++;
			}
		}

	}

	if (pDoc && pDoc->mousePos) {

		pRenderTarget->DrawRectangle(CD2DRectF(
			pDoc->mousePos->x - (float)(pDoc->raster.size / 2 / dpp),
			pDoc->mousePos->y - (float)(pDoc->raster.size / 2 / dpp),
			pDoc->mousePos->x + (float)(pDoc->raster.size / 2 / dpp),
			pDoc->mousePos->y + (float)(pDoc->raster.size / 2 / dpp)),
			m_pWhiteBrush,
			.5f,
			NULL);
		ShowCoordinates(pRenderTarget, (float)-(center.x - pDoc->mousePos->x) * (float)dpp,
			(float)(center.y - pDoc->mousePos->y) * (float)dpp, (float)pDoc->raster.size);
	}

}





void Grid::ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, float rastersize)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	CString traceText;
	CD2DSizeF sizeTarget = pRenderTarget->GetSize();
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
	CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
		_T("Consolas"),								// font family name
		sizeDpi.height / 9);						// font size

	traceText.Format(L"%.1f,%.1f", xPos, yPos);
	CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
		traceText,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	pRenderTarget->DrawTextLayout(
		CD2DPointF((xPos * 1 / (float)dpp + center.x ) - (rastersize / 2 * 1 / (float)dpp), (yPos * - 1 / (float)dpp + center.y) - (rastersize / 2 * 1 / (float)dpp + 12)),
		&textLayout,
		m_pWhiteBrush);

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
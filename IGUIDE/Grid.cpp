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

	m_pLayer1 = new CD2DLayer(NULL);	// opacity layer parameters

	m_pGridGeom = NULL;

	// layer for patch-opacity
	lpHi = {
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_ALIASED,
		D2D1::IdentityMatrix(),
		1.0,
		NULL,
		D2D1_LAYER_OPTIONS_NONE
	};


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
	delete m_pLayer1;
}

void Grid::DelPatch() {

	if (patchlist.size() > 0)
		patchlist.pop_back();
}

void Grid::ClearPatchlist() {

	patchlist.clear();

}

void Grid::StorePatch(CPoint loc, float zoom) {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CIGUIDEView* view = CIGUIDEView::GetView();
	CD2DPointF center = view->GetRelativeCenter();;

	Patch patch;
	patch.coords.x = ((center.x - loc.x) * DPP) / zoom;
	patch.coords.y = ((center.y - loc.y) * DPP) / zoom;
	patch.color = pDoc->raster.color;
	patch.rastersize = pDoc->raster.size;
	patch.locked = false;
	patch.defocus = pDoc->getCurrentDefocus();
	patchlist.push_back(patch);

}

void Grid::CreateGridGeometry(CHwndRenderTarget* pRenderTarget) {

	// grid geometry creation

	CD2DPointF offset = CMainFrame::GetCenterOffset();

	m_pGridGeom = new CD2DPathGeometry(pRenderTarget,1);
	m_pGridGeom->Create(pRenderTarget);
	CD2DGeometrySink GridGeomSink(*m_pGridGeom);

	CD2DPointF point;

	// x-plot
	for (int i = 1; i < LANES; i++) {
		GridGeomSink.BeginFigure(
			CD2DPointF(offset.x + PPD * i, offset.y), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);
			
		point.x = offset.x + PPD * i;
		point.y = offset.y + PPD * LANES;

		GridGeomSink.AddLine(point);
		
		GridGeomSink.EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);

	}

	// y-plot
	for (int i = 1; i < LANES; i++) {
		GridGeomSink.BeginFigure(
			CD2DPointF(offset.x, offset.y + PPD * i), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);

		point.x = offset.x + PPD * LANES;
		point.y = offset.y + PPD * i;

		GridGeomSink.AddLine(point);

		GridGeomSink.EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);

	}
	
}

void Grid::DrawOverlay(CHwndRenderTarget* pRenderTarget) {

	CIGUIDEView* view = CIGUIDEView::GetView();
	CD2DPointF offset = CMainFrame::GetCenterOffset();
	CD2DPointF gridcenter{ offset.x + CENTER, offset.y + CENTER };
	CD2DPointF a, b;
	CD2DRectF r;

	//draw circles around the center
	if (overlay & DEGRAD) {
		const int RANGE = CENTER * DPP;
		CD2DPointF a[RANGE];
		CD2DPointF b[RANGE];
		for (int f = 0; f < RANGE; f++) {
			a[f].x = offset.x + f * PPD;
			a[f].y = offset.y + f * PPD;
			b[f].x = offset.x + CANVAS - f * PPD;
			b[f].y = offset.y + CANVAS - f * PPD;
		}

		for (int f = 0; f < RANGE; f++) {
			CD2DRectF r{ a[f].x, a[f].y, b[f].x, b[f].y };
				pRenderTarget->DrawEllipse(r, m_pGrayBrush, .1f);
		}
	}

	// draw circles around foveola and fovea
	if (overlay & FOVEOLA) {
		a.x = gridcenter.x - _FOVEOLA * PPD;
		a.y = gridcenter.y - _FOVEOLA * PPD;
		b.x = gridcenter.x + _FOVEOLA * PPD;
		b.y = gridcenter.y + _FOVEOLA * PPD;
		r = { a.x, a.y, b.x, b.y };
		pRenderTarget->DrawEllipse(r, m_pRedBrush, .2f);
	}

	if (overlay & FOVEA){
		a.x = gridcenter.x - _FOVEA * PPD;
		a.y = gridcenter.y - _FOVEA * PPD;
		b.x = gridcenter.x + _FOVEA * PPD;
		b.y = gridcenter.y + _FOVEA * PPD;
		r = { a.x, a.y, b.x, b.y };
		pRenderTarget->DrawEllipse(r, m_pRedBrush, .2f);
	}

	// draw optic nerve as blue circle
	if (overlay & OPTICDISC) {
		a.x = gridcenter.x + _DELTA_D * PPD - _OPTIC_DISC / DPP;
		a.y = gridcenter.y - _OPTIC_DISC / DPP - _DELTA_DY / DPP;
		b.x = gridcenter.x + _DELTA_D * PPD + _OPTIC_DISC / DPP;
		b.y = gridcenter.y + _OPTIC_DISC / DPP - _DELTA_DY / DPP;
		r = { a.x, a.y, b.x, b.y };
		pRenderTarget->DrawEllipse(r, m_pBlueBrush, .2f);
	}

	// draw cross in center of the fovea
	if (overlay & CROSSHAIR) {
		a.x = gridcenter.x - 5;
		a.y = gridcenter.y - 5;
		b.x = gridcenter.x + 5;
		b.y = gridcenter.y + 5;
		r = { a.x, a.y, b.x, b.y };
		pRenderTarget->DrawEllipse(r, m_pWhiteBrush, .1f);

		pRenderTarget->DrawLine(CD2DPointF(gridcenter.x - 8, gridcenter.y),
			CD2DPointF(gridcenter.x + 8, gridcenter.y),
			m_pWhiteBrush, .1f);

		pRenderTarget->DrawLine(CD2DPointF(gridcenter.x, gridcenter.y - 8),
			CD2DPointF(gridcenter.x, gridcenter.y + 8),
			m_pWhiteBrush, .1f);
	}

}

void Grid::Mark(CHwndRenderTarget* pRenderTarget) {

	// draw all marked locations (i.e. list of patches) in operator view

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CIGUIDEView* view = CIGUIDEView::GetView();
	CD2DPointF center = view->GetRelativeCenter();

	CD2DRectF rect1;
	CRect intersect;
	float rsdeg; // raster size in degree visual angle


	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {
		
		if (!m_pLayer1) m_pLayer1->Create(pRenderTarget);
		pRenderTarget->PushLayer(lpHi, *m_pLayer1);

		rsdeg = (float)pDoc->raster.videodim / it._Ptr->_Myval.rastersize; 

		rect1 = { 
			(float)(center.x - it._Ptr->_Myval.coords.x * PPD - rsdeg / 2 * PPD),
			(float)(center.y - it._Ptr->_Myval.coords.y * PPD - rsdeg / 2 * PPD),
			(float)(center.x - it._Ptr->_Myval.coords.x * PPD + rsdeg / 2 * PPD),
			(float)(center.y - it._Ptr->_Myval.coords.y * PPD + rsdeg / 2 * PPD)
		};

		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

	}

	if (patchlist.size() > 0) {

		rsdeg = (float)pDoc->raster.videodim / patchlist.back().rastersize;

		rect1 = { 
			(float)(center.x - patchlist.back().coords.x * PPD - rsdeg / 2 * PPD),
			(float)(center.y - patchlist.back().coords.y * PPD - rsdeg / 2 * PPD),
			(float)(center.x - patchlist.back().coords.x * PPD + rsdeg / 2 * PPD),
			(float)(center.y - patchlist.back().coords.y * PPD + rsdeg / 2 * PPD)
		};
		
		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, 1);

		CD2DEllipse center(&rect1);
		center.radiusX = center.radiusY = .5;
		
		pRenderTarget->DrawEllipse(center, m_pWhiteBrush, .5);

		ShowCoordinates(pRenderTarget, patchlist.back().coords.x, patchlist.back().coords.y, rect1);	
				
		int number = 1;

		for (auto it = pDoc->m_pGrid->patchlist.begin(); it != pDoc->m_pGrid->patchlist.end(); it++) {
			
			rsdeg = (double)pDoc->raster.videodim / (double)it._Ptr->_Myval.rastersize;

			if (it._Ptr->_Myval.locked == true) {
				ShowVidNumber(pRenderTarget, it._Ptr->_Myval.coords.x, it._Ptr->_Myval.coords.y, rsdeg, number);
				number++;
			}

		}

	}


	//if (pDoc && pDoc->mousePos) {
	//	
	//	rsdeg = (float)pDoc->raster.videodim / pDoc->raster.size;

	//	pRenderTarget->DrawRectangle(CD2DRectF(
	//		pDoc->mousePos->x - (float)(rsdeg / 2 / DPP),
	//		pDoc->mousePos->y - (float)(rsdeg / 2 / DPP),
	//		pDoc->mousePos->x + (float)(rsdeg / 2 / DPP),
	//		pDoc->mousePos->y + (float)(rsdeg / 2 / DPP)),
	//		m_pWhiteBrush,
	//		.5f,
	//		NULL);
	//}

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
	
	CD2DPointF center = CMainFrame::GetCenter();

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
		CD2DPointF((xPos * 1 / (float)DPP + center.x) - (rastersize / 2 * 1 / (float)DPP), (yPos * -1 / (float)DPP + center.y) - (rastersize / 2 * 1 / (float)DPP)),
		&textLayout,
		m_pWhiteBrush);

}
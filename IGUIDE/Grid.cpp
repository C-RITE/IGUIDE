#include "stdafx.h"
#include "MainFrm.h"
#include "Grid.h"
#include "Target.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "resource.h"
#include "Patches.h"

using namespace D2D1;

Grid::Grid()
{
	// create own pallette
	m_pPatchBrush = new CD2DSolidColorBrush(NULL, NULL);
	m_pDarkRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::White));
	m_pGrayBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkGray));
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
	delete m_pLayer1;

}

void Grid::DelPatch() {

	if (patchlist.size() > 0)
		patchlist.pop_back();

}

void Grid::ClearPatchlist() {

	patchlist.clear();

}

void Grid::StorePatch(CPoint loc) {

	// store patches in degrees from fovea

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	float zoom = pView->getZoomFactor();
	CD2DPointF mouseDist = pView->getMouseDist();
	CD2DPointF transLoc;
	
	transLoc.x = ((loc.x - mouseDist.x) - CANVAS / 2) * DPP;
	transLoc.y = ((loc.y - mouseDist.y) - CANVAS / 2) * DPP;

	transLoc.x *= zoom;
	transLoc.y *= zoom;

	Patch patch;

	patch.coords.x = transLoc.x;
	patch.coords.y = transLoc.y;
	patch.color = pDoc->m_raster.color;
	patch.rastersize = pDoc->m_raster.size;
	patch.locked = false;
	patch.defocus = pDoc->getCurrentDefocus();
	patchlist.push_back(patch);

}

void Grid::CreateGridGeometry(CHwndRenderTarget* pRenderTarget) {

	// grid geometry creation

	m_pGridGeom = new CD2DPathGeometry(pRenderTarget, 1);
	m_pGridGeom->Create(pRenderTarget);

	CD2DGeometrySink GridGeomSink(*m_pGridGeom);

	CD2DPointF point;

	// x-plot
	for (int i = 1; i < LANES; i++) {
		GridGeomSink.BeginFigure(
			CD2DPointF(PPD * i, 0), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);

		point.x = PPD * i;
		point.y = PPD * LANES;

		GridGeomSink.AddLine(point);
		GridGeomSink.EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);

	}

	// y-plot
	for (int i = 1; i < LANES; i++) {
		GridGeomSink.BeginFigure(
			CD2DPointF(0, PPD * i), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);

		point.x = PPD * LANES;
		point.y = PPD * i;

		GridGeomSink.AddLine(point);
		GridGeomSink.EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);

	}

}

void Grid::DrawCircles(CHwndRenderTarget* pRenderTarget) {

	//draw circles around the center
	const int RANGE = LANES / 2;
		
	CD2DPointF a[RANGE];
	CD2DPointF b[RANGE];

	for (int f = 0; f < RANGE; f++) {
		a[f].x = f * PPD;
		a[f].y = f * PPD;
		b[f].x = CANVAS - f * PPD;
		b[f].y = CANVAS - f * PPD;

	}

	for (int f = 0; f < RANGE; f++) {
		CD2DRectF r{ a[f].x, a[f].y, b[f].x, b[f].y };
		CD2DEllipse e(r);
		pRenderTarget->DrawEllipse(e, m_pGrayBrush, .1f, NULL);
		
	}

}

void Grid::DrawOverlay(CHwndRenderTarget* pRenderTarget) {

	CD2DPointF a, b;
	CD2DRectF r;

	// draw optic nerve as blue circle
	if (overlay & OPTICDISC) {
		a.x = CANVAS / 2 + _DELTA_D * PPD - _OPTIC_DISC / DPP;
		a.y = CANVAS / 2 - _OPTIC_DISC / DPP - _DELTA_DY / DPP;
		b.x = CANVAS / 2 + _DELTA_D * PPD + _OPTIC_DISC / DPP;
		b.y = CANVAS / 2 - _OPTIC_DISC / DPP - _DELTA_DY / DPP;

		r = { a.x, a.y, b.x, b.y };

		pRenderTarget->DrawEllipse(r, m_pBlueBrush, .2f);

	}

	// draw cross in center of the fovea
	if (overlay & CROSSHAIR) {

		a.x = CANVAS / 2 - 5;
		a.y = CANVAS / 2 - 5;
		b.x = CANVAS / 2 + 5;
		b.y = CANVAS / 2 + 5;

		r = { a.x, a.y, b.x, b.y };

		pRenderTarget->DrawEllipse(r, m_pWhiteBrush, .1f);

		pRenderTarget->DrawLine(CD2DPointF(CANVAS / 2 - 8, CANVAS / 2),
			CD2DPointF(CANVAS / 2 + 8, CANVAS / 2),
			m_pWhiteBrush, .1f);

		pRenderTarget->DrawLine(CD2DPointF(CANVAS / 2, CANVAS / 2 - 8),
			CD2DPointF(CANVAS / 2, CANVAS / 2 + 8),
			m_pWhiteBrush, .1f);

	}

}

void Grid::DrawPatches(CHwndRenderTarget* pRenderTarget) {

	// draw all marked locations (i.e. list of patches) in operator view
	CIGUIDEView* pView = CIGUIDEView::GetView();

	if (!(overlay & PATCHES))
		return;

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	CD2DRectF rect1;
	CRect intersect;
	CD2DSizeF zoom = pView->getZoomFactor();


	float rsdeg; // raster size in degree visual angle

	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {

		if (it == patchlist.end())
			break;
		
		rsdeg = (float)pDoc->m_raster.videodim / it._Ptr->_Myval.rastersize;

		rect1 = {

			(float)(it._Ptr->_Myval.coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.x * PPD + rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.y * PPD + rsdeg / 2 * PPD) + CANVAS / 2

		};

		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);

	}

	if (patchlist.size() > 0) {

		rsdeg = (float)pDoc->m_raster.videodim / patchlist.back().rastersize;

		rect1 = {

			(float)(patchlist.back().coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.x * PPD + rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.y * PPD + rsdeg / 2 * PPD) + CANVAS / 2

		};
		
		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, 1);

		CD2DEllipse center(&rect1);
		center.radiusX = center.radiusY = .5;
		
		pRenderTarget->DrawEllipse(center, m_pWhiteBrush, .5);

		ShowCoordinates(pRenderTarget, patchlist.back().coords.x, patchlist.back().coords.y, rect1);	
				
		int number = 1;

		for (auto it = pDoc->m_pGrid->patchlist.begin(); it != pDoc->m_pGrid->patchlist.end(); it++) {
			

			rsdeg = (double)pDoc->m_raster.videodim / (double)it._Ptr->_Myval.rastersize; // raster size in degree visual angle

			if (it._Ptr->_Myval.locked == true) {
				ShowVidNumber(pRenderTarget, it._Ptr->_Myval.coords.x, it._Ptr->_Myval.coords.y, rsdeg, number);
				number++;
			}

		}

	}


	// while left button pressed: hover over grid with raster outline
	if (pView->mouseHovering() & pDoc->calibrationComplete) {
		
		rsdeg = (float)pDoc->m_raster.videodim / pDoc->m_raster.size;
		CD2DPointF mousePos = pView->getMousePos();

		pRenderTarget->DrawRectangle(CD2DRectF(
			mousePos.x - (float)(rsdeg / 2 / DPP),
			mousePos.y - (float)(rsdeg / 2 / DPP),
			mousePos.x + (float)(rsdeg / 2 / DPP),
			mousePos.y + (float)(rsdeg / 2 / DPP)),
			m_pWhiteBrush,
			.5f,
			NULL);

	}

}

void Grid::DrawTextInfo(CHwndRenderTarget* pRenderTarget) {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	if (!pDoc)
		return;

	CD2DSizeF sizeTarget = pRenderTarget->GetSize();
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

	CD2DSolidColorBrush BlackBrush{ pRenderTarget, D2D1::ColorF(BLACK, 0.5f) };
	CD2DSolidColorBrush YellowBrush{ pRenderTarget, D2D1::ColorF(YELLOW) };

	CD2DPointF upperLeft{ 100, 50 };

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
			(pRenderTarget, D2D1::ColorF(GREEN)));

	}

	if (pDoc->m_pGrid->overlay & DEFOCUS) {

		CD2DRectF black_box{ upperLeft.x - 5, upperLeft.y - 5, upperLeft.x + 215, upperLeft.y + 120 };
		CString defocus = L"DEFOCUS: ";
		defocus.Append(pDoc->getCurrentDefocus());

		CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
			defocus,									// text to be drawn
			textFormat2,								// text format
			sizeTarget);								// size of the layout box

		pRenderTarget->DrawTextLayout(CD2DPointF(5),
			// place on top-right corner
			&textLayout,								// text layout object
			&CD2DSolidColorBrush						// brush used for text
			(pRenderTarget,
				D2D1::ColorF(YELLOW)));

	}

	if (pDoc->m_pGrid->overlay & QUICKHELP) {

		vector<CString> help = pDoc->getQuickHelp();

		CD2DPointF down_middle{ sizeTarget.width / 2 - 100, sizeTarget.height - 200 };
		CD2DPointF down_left{ down_middle.x - 250, sizeTarget.height - 200 };
		CD2DPointF down_right{ down_middle.x + 250, sizeTarget.height - 200 };

		CD2DRectF black_box2{ down_left.x - 5, down_left.y - 5, down_right.x + 215, down_right.y + 120 };
		pRenderTarget->FillRectangle(black_box2, &BlackBrush);
		pRenderTarget->DrawRectangle(black_box2, &YellowBrush);

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
				D2D1::ColorF(YELLOW)));

		pRenderTarget->DrawTextLayout(down_middle,		// top-left corner of the text 
			&IGUIDE_help,								// text layout object
			&CD2DSolidColorBrush						// brush used for text
			(pRenderTarget,
				D2D1::ColorF(YELLOW)));

		pRenderTarget->DrawTextLayout(down_right,		// top-left corner of the text 
			&ICANDI_help,								// text layout object
			&CD2DSolidColorBrush						// brush used for text
			(pRenderTarget,
				D2D1::ColorF(YELLOW)));

	}

}


void Grid::ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, CRect rect)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	CString xCoords, yCoords;
	CD2DSizeF sizeTarget(50,50);
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
	CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
		_T("Consolas"),								// font family name
		sizeDpi.height /7);							// font size

	CSize marginX;
	CSize marginY;

	float x_rnd = roundf(xPos * 10) / 10;
	if (x_rnd == -0.0)
		x_rnd = 0.0;

	if (x_rnd <= -10)
		marginX = { 35, 0 };
	else if ((x_rnd < 0) | (x_rnd >= 10))
		marginX = { 28, 0 };
	else if (x_rnd >= 0)
		marginX = { 20, 0 };

	float y_rnd = roundf(yPos * 10) / 10;
	if (y_rnd == -0.0)
		y_rnd = 0.0;
	
	if (y_rnd <= -10)
		marginY = { 40, 4 };
	else if ((y_rnd < 0) | (y_rnd >= 10))
		marginY = { 33, 4 };
	else if (y_rnd >= 0)
		marginY = { 25, 4 };

	xCoords.Format(L"%.1f", x_rnd);
	CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
		xCoords,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	pRenderTarget->DrawTextLayout(rect.BottomRight() - marginX, &textLayout, m_pWhiteBrush);

	yCoords.Format(L"%.1f", y_rnd);
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
		CD2DPointF((xPos * 1 / (float)DPP + CANVAS / 2) - (rastersize / 2 * 1 / (float)DPP), (yPos * -1 / (float)DPP + CANVAS / 2) - (rastersize / 2 * 1 / (float)DPP)),
		&textLayout,
		m_pWhiteBrush);

}
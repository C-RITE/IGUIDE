#include "stdafx.h"
#include "MainFrm.h"
#include "Grid.h"
#include "Target.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "resource.h"
#include "Patches.h"

using namespace D2D1;

Grid::Grid(){

	isPanning = false;
}

Grid::~Grid() {
}

void Grid::DelPatch() {

	if (patchlist.size() > 0)
		patchlist.pop_back();

}

void Grid::ClearPatchlist() {

	patchlist.clear();

}

CD2DPointF Grid::PixelToDegree(CPoint point) {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	float zoom = pView->getZoomFactor();

	CD2DPointF mouseDist = pView->getMouseDist();
	CD2DPointF transLoc;

	transLoc.x = ((point.x - mouseDist.x) - CANVAS / 2) * DPP;
	transLoc.y = ((point.y - mouseDist.y) - CANVAS / 2) * DPP;

	transLoc.x *= zoom;
	transLoc.y *= zoom;

	return transLoc;

}

void Grid::StorePatch(CPoint loc) {

	// store patches as degrees from fovea
	// along with color, rastersize and defocus

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

	CD2DPointF transLoc = PixelToDegree(loc);
	
	Patch patch;

	patch.coords.x = transLoc.x;
	patch.coords.y = transLoc.y;
	patch.color = pDoc->m_raster.color;
	patch.rastersize = pDoc->m_raster.size;
	patch.locked = false;
	patch.defocus = pDoc->getCurrentDefocus();
	patchlist.push_back(patch);

}

void Grid::CreateD2DResources(CHwndRenderTarget* pRenderTarget) {

	// create own pallette
	m_pPatchBrush = new CD2DSolidColorBrush(pRenderTarget, NULL);
	m_pDarkRedBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::LightGray));
	m_pGrayBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::DarkGray));
	m_pDarkGreenBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::DarkGreen));
	m_pMagentaBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::Magenta));
	m_pBlackBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::Black, 0.5f));
	m_pYellowBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::Yellow));
	m_pGoldenBrush = new CD2DSolidColorBrush(pRenderTarget, ColorF(ColorF::PaleGoldenrod));

	m_pLayer1 = new CD2DLayer(pRenderTarget);	// opacity layer parameters
	m_pGridGeom = NULL;

	showCoords = true;
	showCursor = true;

	// layer for high opacity
	lpHi = {
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		.4f,
		NULL,
		D2D1_LAYER_OPTIONS_NONE
	};

	// layer for low opacity
	lpLo = {

		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		1,
		NULL,
		D2D1_LAYER_OPTIONS_NONE

	};
}


void Grid::CreateGridGeometry(CHwndRenderTarget* pRenderTarget) {

	// grid geometry creation

	m_pGridGeom = new CD2DPathGeometry(pRenderTarget);
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

void Grid::DrawGrid(CHwndRenderTarget* pRenderTarget) {

	if (overlay & GRID) {
		pRenderTarget->DrawGeometry(
			m_pGridGeom,
			m_pWhiteBrush,
			.1f);

		DrawCircles(pRenderTarget);

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
		pRenderTarget->DrawEllipse(e, m_pWhiteBrush, .1f, NULL);
		
	}

}

void Grid::DrawTargetZone(CHwndRenderTarget* pRenderTarget) {

	if (overlay & TARGETZONE) {

		CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

		CD2DPointF a, b;
		CD2DRectF r;

		CD2DRectF zone = pDoc->ComputeTargetZone();

		a.x = CANVAS / 2 - zone.left * 10;
		a.y = CANVAS / 2 - zone.top * 10;
		b.x = CANVAS / 2 + zone.right * 10;
		b.y = CANVAS / 2 + zone.bottom * 10;

		r = { a.x, a.y, b.x, b.y };

		pRenderTarget->DrawRectangle(r, m_pWhiteBrush, .1f);

	}

}

void Grid::DrawExtras(CHwndRenderTarget* pRenderTarget) {

	CD2DPointF a, b;
	CD2DRectF r;

	// draw optic nerve as blue circle
	if (overlay & OPTICDISC) {
		a.x = CANVAS / 2 + _DELTA_D * PPD - _OPTIC_DISC / DPP;
		a.y = CANVAS / 2 - _DELTA_DY / DPP - _OPTIC_DISC / DPP;
		b.x = CANVAS / 2 + _DELTA_D * PPD + _OPTIC_DISC / DPP;
		b.y = CANVAS / 2 - _DELTA_DY / DPP + _OPTIC_DISC / DPP;

		r = { a.x, a.y, b.x, b.y };

		pRenderTarget->DrawEllipse(r, m_pBlueBrush, .3f);

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

	if (!(overlay & PATCHES))
		return;

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	CD2DRectF rect1;
	CD2DRectF lastPatch;
	CRect intersect;

	float rsdeg; // raster size in degree visual angle

	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {

		if (it == patchlist.end())
			break;

		rsdeg = (float)pDoc->m_raster.videodim / it._Ptr->_Myval.rastersize;

		pRenderTarget->PushLayer(lpHi, *m_pLayer1);

		rect1 = {

			(float)(it._Ptr->_Myval.coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.x * PPD + rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(it._Ptr->_Myval.coords.y * PPD + rsdeg / 2 * PPD) + CANVAS / 2

		};

		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

	}

	if (patchlist.size() > 0) {

		rsdeg = (float)pDoc->m_raster.videodim / patchlist.back().rastersize;

		pRenderTarget->PushLayer(lpHi, *m_pLayer1);

		lastPatch = {

			(float)(patchlist.back().coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.x * PPD + rsdeg / 2 * PPD) + CANVAS / 2,
			(float)(patchlist.back().coords.y * PPD + rsdeg / 2 * PPD) + CANVAS / 2

		};

		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, .2f);

		CD2DEllipse center(&rect1);
		center.radiusX = center.radiusY = .5;

		pRenderTarget->DrawEllipse(center, m_pWhiteBrush, .1f);
		pRenderTarget->PopLayer();

		// calculate position of text in real pixelspace

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		float zoom = 1 / pView->getZoomFactor();
		CD2DSizeF mouseDist = pView->getMouseDist();
		mouseDist.width *= 1 / zoom;
		mouseDist.height *= 1 / zoom;

		// show coords in real pixelspace

		if (showCoords) {

			pRenderTarget->PushLayer(lpLo, *m_pLayer1);

			lastPatch = {

				(float)(zoom * (mouseDist.width + patchlist.back().coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2),
				(float)(zoom * (mouseDist.height + patchlist.back().coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2),
				(float)(zoom * (mouseDist.width + patchlist.back().coords.x * PPD + rsdeg / 2 * PPD) + CANVAS / 2),
				(float)(zoom * (mouseDist.height + patchlist.back().coords.y * PPD + rsdeg / 2 * PPD) + CANVAS / 2)

			};

			if (patchlist.back().locked == false)
				DrawCoordinates(
					pRenderTarget,
					patchlist.back().coords,
					lastPatch);

			pRenderTarget->PopLayer();

		}

		// show vid number in real pixelspace

		if (!isPanning) {

			int number = 1;

				CD2DPointF pos;

				for (auto it = pDoc->m_pGrid->patchlist.begin(); it != pDoc->m_pGrid->patchlist.end(); it++) {

					rsdeg = (double)pDoc->m_raster.videodim / (double)it._Ptr->_Myval.rastersize; // raster size in degree visual angle

						if (it._Ptr->_Myval.locked == true) {

							pos.x = (float)(zoom * (mouseDist.width + it._Ptr->_Myval.coords.x * PPD - rsdeg / 2 * PPD) + CANVAS / 2);
							pos.y = (float)(zoom * (mouseDist.height + it._Ptr->_Myval.coords.y * PPD - rsdeg / 2 * PPD) + CANVAS / 2);

							DrawVidNumber(pRenderTarget,
								pos,
								number);

							number++;

						}

				}

		}

	}

}

void Grid::DrawPatchCursor(CHwndRenderTarget* pRenderTarget, CD2DPointF loc) {

	// show cursor around the mouse pointer
	if (!showCursor)
		return;

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	if (pDoc->calibrationComplete) {

		float rsdeg = (float)pDoc->m_raster.videodim / (float)pDoc->m_raster.size;
		float zoom = 1 / pView->getZoomFactor();
		
		cursor = {
			loc.x - (float)(zoom * (rsdeg / 2 / DPP)),
			loc.y - (float)(zoom * (rsdeg / 2 / DPP)),
			loc.x + (float)(zoom * (rsdeg / 2 / DPP)),
			loc.y + (float)(zoom * (rsdeg / 2 / DPP))
		};

		pRenderTarget->DrawRectangle(cursor, m_pWhiteBrush, .5f, NULL);

		DrawCoordinates(pRenderTarget, PixelToDegree(loc), cursor);

	}

}

void Grid::DrawVidNumber(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, int number) {
	

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
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
		pos,
		&textLayout,
		m_pWhiteBrush);

}

void Grid::DrawDebug(CHwndRenderTarget* pRenderTarget) {

	// for debug purposes only
	if (overlay & TRACEINFO) {

		CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

		CD2DSizeF sizeTarget = pRenderTarget->GetSize();
		CD2DSizeF sizeDpi = pRenderTarget->GetDpi();
		
		CD2DTextFormat textFormat(pRenderTarget,			// pointer to the render target
			_T("Consolas"),									// font family name
			sizeDpi.height / 8);							// font size

		CString traceText = pDoc->getTraceInfo();

		// construct a CD2DTextLayout object which represents a block of formatted text 
		CD2DTextLayout textLayout(pRenderTarget,		// pointer to the render target 
			traceText,									// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		pRenderTarget->DrawTextLayout(
			CD2DPointF(sizeTarget.width - 210, 5),		// place on top-right corner
			&textLayout,								// text layout object
			&CD2DSolidColorBrush						// brush used for text
			(pRenderTarget,
				D2D1::ColorF(D2D1::ColorF::LightGreen)));

	}

}

void Grid::DrawLocation(CHwndRenderTarget* pRenderTarget) {

	// draw additional text box in overlay for display of coordinates and defocus value from AOSACA

	if (overlay & LOCATION) {

		CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

		CD2DSizeF sizeTarget = pRenderTarget->GetSize();
		CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

		CD2DPointF upper_middle{ sizeTarget.width / 2, 25 };
		CD2DPointF upper_left{ upper_middle.x - 180, 25 };
		CD2DPointF upper_right{ upper_middle.x + 180, 25 };

		CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
			_T("Consolas"),								// font family name
			sizeDpi.height / 5);

		CD2DRectF black_box{ upper_left.x, upper_left.y, upper_right.x, upper_right.y + 40 };
		pRenderTarget->FillRectangle(black_box, m_pBlackBrush);
		pRenderTarget->DrawRectangle(black_box, m_pGoldenBrush);

		CString coordsCaption(L" LOC(deg):");
		CString coordsX, coordsY, defocus;

		coordsX.Format(L"x=%.1f", currentPos.x);
		coordsY.Format(L"y=%.1f", currentPos.y);
		defocus.Format(L"z=%s", pDoc->getCurrentDefocus());
	
		CD2DTextLayout textLayout1(pRenderTarget,		// pointer to the render target 
			coordsCaption,								// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		CD2DTextLayout textLayout2(pRenderTarget,		// pointer to the render target 
			coordsX,									// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		CD2DTextLayout textLayout3(pRenderTarget,		// pointer to the render target 
			coordsY,									// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		CD2DTextLayout textLayout4(pRenderTarget,		// pointer to the render target 
			defocus,									// text to be drawn
			textFormat,									// text format
			sizeTarget);								// size of the layout box

		pRenderTarget->DrawTextLayout(
			CD2DPointF(upper_left.x, upper_left.y + 8),
			&textLayout1,								// text layout object
			m_pGoldenBrush);							// brush used for text

		pRenderTarget->DrawTextLayout(
			CD2DPointF(upper_left.x + 110, upper_left.y + 8),
			&textLayout2,								// text layout object
			m_pGoldenBrush);							// brush used for text

		pRenderTarget->DrawTextLayout(
			CD2DPointF(upper_left.x + 190, upper_left.y + 8),
			&textLayout3,								// text layout object
			m_pGoldenBrush);							// brush used for text

		pRenderTarget->DrawTextLayout(
			CD2DPointF(upper_left.x + 270, upper_left.y + 8),
			&textLayout4,								// text layout object
			m_pGoldenBrush);							// brush used for text
			
	}

}

void Grid::DrawQuickHelp(CHwndRenderTarget* pRenderTarget) {

	if (overlay & QUICKHELP) {

		CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

		CD2DSizeF sizeTarget = pRenderTarget->GetSize();
		CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

		vector<CString> help = pDoc->getQuickHelp();

		CD2DPointF down_middle{ sizeTarget.width / 2 - 100, sizeTarget.height - 200 };
		CD2DPointF down_left{ down_middle.x - 250, sizeTarget.height - 200 };
		CD2DPointF down_right{ down_middle.x + 250, sizeTarget.height - 200 };

		CD2DTextFormat textFormat(pRenderTarget,			// pointer to the render target
			_T("Consolas"),									// font family name
			sizeDpi.height / 8);							// font size

		CD2DRectF black_box{ down_left.x - 5, down_left.y - 5, down_right.x + 215, down_right.y + 120 };
		
		pRenderTarget->FillRectangle(black_box, m_pBlackBrush);
		pRenderTarget->DrawRectangle(black_box, m_pGoldenBrush);

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
			m_pGoldenBrush);							// brush used for text

		pRenderTarget->DrawTextLayout(down_middle,		// top-left corner of the text 
			&IGUIDE_help,								// text layout object
			m_pGoldenBrush);							// brush used for text
			
		pRenderTarget->DrawTextLayout(down_right,		// top-left corner of the text 
			&ICANDI_help,								// text layout object
			m_pGoldenBrush);							// brush used for text
			
	}

}

void Grid::DrawTarget(CHwndRenderTarget* pRenderTarget, CD2DBitmap* pFixationTarget) {

	CD2DSizeF sizeTarget = pRenderTarget->GetSize();
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

	CD2DBrushProperties prop{ .5f };
	CD2DSolidColorBrush brush{ pRenderTarget, D2D1::ColorF(D2D1::ColorF::Beige), &prop };
	CD2DRectF upperRight{ sizeTarget.width - 100,
						50, sizeTarget.width - 50,
						100 };
	pRenderTarget->DrawRectangle(CD2DRectF(
		upperRight.left - 1,
		upperRight.top - 1,
		upperRight.right + 1,
		upperRight.bottom + 1),
		&brush);

	if (pFixationTarget && pFixationTarget->IsValid()) {
		CD2DSizeF size = pFixationTarget->GetSize();
		pRenderTarget->DrawBitmap(
			pFixationTarget,
			upperRight,
			0.25f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
	}

	else {
		// use default fixation target

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


void Grid::DrawCoordinates(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, CD2DRectF loc) {

	// draw coordinates around mouse cursor

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	currentPos = pos;
	CString xCoords, yCoords;

	CD2DSizeF sizeTarget(40, 10);
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

	CD2DTextFormat textFormat(pRenderTarget,		// pointer to the render target
		_T("Consolas"),								// font family name
		sizeDpi.height / 7);						// font size

	CD2DPointF marginX;
	CD2DPointF marginY;

	float x_rnd = roundf(pos.x * 10) / 10;
	if (x_rnd == -0.0)
		x_rnd = 0.0;

	if (x_rnd <= -10)
		marginX = { 35, 0 };
	else if ((x_rnd < 0) | (x_rnd >= 10))
		marginX = { 28, 0 };
	else if (x_rnd >= 0)
		marginX = { 20, 0 };

	float y_rnd = roundf(pos.y * 10) / 10;
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

	pRenderTarget->DrawTextLayout(
		CD2DPointF(loc.right - marginX.x, loc.bottom - marginX.y),
		&textLayout,
		m_pWhiteBrush);

	yCoords.Format(L"%.1f", y_rnd);
	CD2DTextLayout textLayout2(pRenderTarget,		// pointer to the render target 
		yCoords,									// text to be drawn
		textFormat,									// text format
		sizeTarget);								// size of the layout box

	pRenderTarget->DrawTextLayout(
		CD2DPointF(loc.left - marginY.x, loc.top - marginY.y),
		&textLayout2,
		m_pWhiteBrush);

}

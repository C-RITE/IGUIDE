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
	patch.color = pDoc->m_raster.color;
	patch.rastersize = pDoc->m_raster.size;
	patch.locked = false;
	patch.defocus = pDoc->getCurrentDefocus();
	patchlist.push_back(patch);

}

void Grid::DrawGrid(CHwndRenderTarget* pRenderTarget) {

	pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	dpp = (m_pDeltaFOD + m_pRadNerve) / (mainWnd.Width() / 2);

	// draw a grid background around the center
	if (overlay & GRID) {
		for (float x = center.x; x > 0; x -= 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 1),
				CD2DPointF(x, (float)mainWnd.Height()),
				m_pGrayBrush,
				1
				);
		}

		for (float x = center.x + 1 / (float)dpp; x < mainWnd.Width(); x += 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 0.1f),
				CD2DPointF(x, (float)mainWnd.Height()),
				m_pGrayBrush,
				1
				);
		}

		for (float y = center.y; y > 0; y -= 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.1f, y),
				CD2DPointF((float)mainWnd.Width(), y),
				m_pGrayBrush,
				1
				);
		}

		for (float y = center.y + 1 / (float)dpp; y < mainWnd.Height(); y += 1 / (float)dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.1f, y),
				CD2DPointF((float)mainWnd.Width(), y),
				m_pGrayBrush,
				1
				);
		}

		//draw circles around the center
		for (float x = 0; x < 16; x++) {
			CD2DRectF e = {
				center.x - 1 / (float)dpp * x,
				center.y - 1 / (float)dpp * x,
				center.x + 1 / (float)dpp * x,
				center.y + 1 / (float)dpp * x };
			pRenderTarget->DrawEllipse(e, m_pGrayBrush, 1);
		}

	}

	pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

}

void Grid::DrawOptional(CHwndRenderTarget* pRenderTarget) {

	CIGUIDEView *pView = CIGUIDEView::GetView();
	CD2DSizeF sizeTarget = pRenderTarget->GetSize();
	CD2DSizeF sizeDpi = pRenderTarget->GetDpi();

	CD2DSolidColorBrush BlackBrush{ pRenderTarget, D2D1::ColorF(BLACK, 0.5f) };
	CD2DSolidColorBrush YellowBrush{ pRenderTarget, D2D1::ColorF(YELLOW) };

	// draw optic nerve as blue circle
	if (overlay & OPTICDISC) {
		nerve = { 
			float(center.x + mainWnd.Width() / 2 - 5 / dpp),
			(float)((center.y - 2.5 / dpp) - 86),
			(float)(center.x + mainWnd.Width() / 2),
			(float)((center.y + 2.5 / dpp) - 86) };
		pRenderTarget->DrawEllipse(nerve, m_pBlueBrush, .3f);
	}

	// draw cross in fovea
	if (overlay & CROSSHAIR) {
		CD2DRectF fovea(
			center.x - 4,
			center.y - 4,
			center.x + 4,
			center.y + 4);

		pRenderTarget->DrawEllipse(fovea, m_pWhiteBrush);
		pRenderTarget->DrawLine(
			CD2DPointF(center.x - 8, center.y),
			CD2DPointF(center.x + 8, center.y),
			m_pWhiteBrush);
		pRenderTarget->DrawLine(
			CD2DPointF(center.x, center.y - 8),
			CD2DPointF(center.x, center.y + 8),
			m_pWhiteBrush);
	}

	// is (custom) fixation target on or off?
	if (pView && pView->m_pDlgTarget->m_bVisible) {

		CD2DBrushProperties prop{ .5f };
		CD2DSolidColorBrush brush{ pRenderTarget, D2D1::ColorF(YELLOW), &prop };

		CD2DRectF upperRight{
					sizeTarget.width - 100,
					50,
					sizeTarget.width - 50,
					100 };

		pRenderTarget->DrawRectangle(CD2DRectF(
				upperRight.left - 1,
				upperRight.top - 1,
				upperRight.right + 1,
				upperRight.bottom + 1),
				&brush);

		pRenderTarget->FillRectangle(upperRight, &BlackBrush);

		if (pView->m_pFixationTarget && pView->m_pFixationTarget->IsValid()) {
			CD2DSizeF size = pView->m_pFixationTarget->GetSize();
			pRenderTarget->DrawBitmap(
				pView->m_pFixationTarget,
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

void Grid::DrawPatches(CHwndRenderTarget* pRenderTarget) {

	// draw all marked locations (i.e. list of patches) in operator view

	if (!(overlay & PATCHES))
		return;

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	CD2DRectF rect1;
	CRect intersect;
	float rsdeg; // raster size in degree visual angle
	
	for (auto it = patchlist.begin(); it != patchlist.end(); it++) {
		
		rsdeg = (float)pDoc->m_raster.videodim / it._Ptr->_Myval.rastersize;

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { 
			center.x + it._Ptr->_Myval.coords.x * (float)(1 / dpp) + (float)dpp * pDoc->m_raster.scale.x - (float)(rsdeg / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp - (float)(rsdeg / 2 / dpp),
			center.x + it._Ptr->_Myval.coords.x * 1 / (float)dpp + (float)dpp * pDoc->m_raster.scale.x + (float)(rsdeg / 2 / dpp),
			center.y - it._Ptr->_Myval.coords.y * 1 / (float)dpp + (float)(rsdeg / 2 / dpp)
		};

		m_pPatchBrush->SetColor(it._Ptr->_Myval.color);
		pRenderTarget->FillRectangle(rect1, m_pPatchBrush);
		pRenderTarget->PopLayer();

	}

	if (patchlist.size() > 0) {

		rsdeg = (float)pDoc->m_raster.videodim / patchlist.back().rastersize; // raster size in degree visual angle

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + patchlist.back().coords.x * (float)(1 / dpp) + (float)dpp * pDoc->m_raster.scale.x - (float)(rsdeg / 2 / dpp),
			center.y - patchlist.back().coords.y * 1 / (float)dpp - (float)(rsdeg / 2 / dpp),
			center.x + patchlist.back().coords.x * 1 / (float)dpp + (float)dpp * pDoc->m_raster.scale.x + (float)(rsdeg / 2 / dpp),
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
			
			rsdeg = (double)pDoc->m_raster.videodim / (double)it._Ptr->_Myval.rastersize; // raster size in degree visual angle

			if (it._Ptr->_Myval.locked == true) {
				ShowVidNumber(pRenderTarget, it._Ptr->_Myval.coords.x, it._Ptr->_Myval.coords.y, rsdeg, number);
				number++;
			}
		}

	}

	if (pDoc && pDoc->m_pMousePos) {
		
		rsdeg = (float)pDoc->m_raster.videodim / pDoc->m_raster.size;

		pRenderTarget->DrawRectangle(CD2DRectF(
			pDoc->m_pMousePos->x - (float)(rsdeg / 2 / dpp),
			pDoc->m_pMousePos->y - (float)(rsdeg / 2 / dpp),
			pDoc->m_pMousePos->x + (float)(rsdeg / 2 / dpp),
			pDoc->m_pMousePos->y + (float)(rsdeg / 2 / dpp)),
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
		CD2DPointF((xPos * 1 / (float)dpp + center.x) - (rastersize / 2 * 1 / (float)dpp), (yPos * -1 / (float)dpp + center.y) - (rastersize / 2 * 1 / (float)dpp)),
		&textLayout,
		m_pWhiteBrush);

}
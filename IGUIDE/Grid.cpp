#include "stdafx.h"
#include "Grid.h"
#include <sstream>
#include "Target.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "resource.h"
#include "Tags.h"

using namespace std;
using namespace D2D1;

Grid::Grid()
{
	m_pTagBrush = new CD2DSolidColorBrush(NULL, NULL);
	m_pDarkRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::White));
	m_pDarkGreenBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkGreen));
	m_pMagentaBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Magenta));
	
	locked = 0;
	lpHi = { D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		.4f,
		NULL,
		D2D1_LAYER_OPTIONS_NONE };
	pLayer = new CD2DLayer(NULL);
	overlay = AfxGetApp()->GetProfileInt(L"Settings", L"Overlays", GRID | CROSSHAIR | FUNDUS);
	taglist;
}
	

Grid::~Grid() {

	delete m_pDarkRedBrush;
	delete m_pRedBrush;
	delete m_pBlueBrush;
	delete m_pWhiteBrush;
	delete m_pTagBrush;
	delete m_pDarkGreenBrush;
	delete m_pMagentaBrush;;
	delete pLayer;
}

void Grid::DelTag() {

	if (taglist.size() > 0)
		taglist.pop_back();
}

void Grid::ClearTaglist() {
	taglist.clear();
}

void Grid::StoreClick(CD2DPointF loc) {

	CIGUIDEDoc* pDoc;
	pDoc = CIGUIDEDoc::GetDoc();
	Tags tag;

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	tag.coords.x = (center.x - loc.x)*-1 * dpp;
	tag.coords.y = (center.y - loc.y)* dpp;
	tag.color = pDoc->raster.color;
	tag.rastersize = pDoc->raster.size;
	taglist.push_back(tag);

}

void Grid::Paint(CHwndRenderTarget* pRenderTarget) {

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	dpp = (m_pDeltaFOD + m_pRadNerve) / (mainWnd.Width() / 2);

	// draw a grid background around the center
	if (overlay & GRID) {
		for (float x = center.x; x > 0; x -= 1 / dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 0.0f),
				CD2DPointF(x, mainWnd.Height()),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float x = center.x + 1 / dpp; x < mainWnd.Width(); x += 1 / dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(x, 0.0f),
				CD2DPointF(x, mainWnd.Height()),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float y = center.y; y > 0; y -= 1 / dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.0f, y),
				CD2DPointF(mainWnd.Width(), y),
				m_pWhiteBrush,
				0.1f
				);
		}

		for (float y = center.y + 1 / dpp; y < mainWnd.Height(); y += 1 / dpp)
		{
			pRenderTarget->DrawLine(
				CD2DPointF(0.0f, y),
				CD2DPointF(mainWnd.Width(), y),
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
			CD2DRectF e = { center.x - 1 / dpp * x,
				center.y - 1 / dpp * x,
				center.x + 1 / dpp * x,
				center.y + 1 / dpp * x };
			pRenderTarget->DrawEllipse(e, m_pWhiteBrush, .1f);
		}
	}

	// draw circles around foveola and fovea
	if (overlay & FOVEOLA) {
		CD2DRectF e1 = { center.x - 1 / dpp * 1.2f,
		center.y - 1 / dpp * 1.2f,
		center.x + 1 / dpp * 1.2f,
		center.y + 1 / dpp * 1.2f };
		pRenderTarget->DrawEllipse(e1, m_pRedBrush, .3f);
	}

	if (overlay & FOVEA){
		CD2DRectF e2 = { center.x - 1 / dpp * 6.2f,
		center.y - 1 / dpp * 6.2f,
		center.x + 1 / dpp * 6.2f,
		center.y + 1 / dpp * 6.2f };
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

void Grid::Tag(CHwndRenderTarget* pRenderTarget) {


	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	CD2DRectF rect1;
	CRect intersect;
	
	for (size_t i = 0; i < taglist.size(); i++) {

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + taglist[i].coords.x * 1 / dpp + dpp * pDoc->raster.scale.x - taglist[i].rastersize / 2 / dpp,
			center.y - taglist[i].coords.y * 1 / dpp - taglist[i].rastersize / 2 / dpp,
			center.x + taglist[i].coords.x * 1 / dpp + dpp * pDoc->raster.scale.x + taglist[i].rastersize / 2 / dpp,
			center.y - taglist[i].coords.y * 1 / dpp + taglist[i].rastersize / 2 / dpp };
		m_pTagBrush->SetColor(taglist[i].color);
		pRenderTarget->FillRectangle(rect1, m_pTagBrush);

		pRenderTarget->PopLayer();

	}

	if (taglist.size() > 0) {

		pRenderTarget->PushLayer(lpHi, *pLayer);

		rect1 = { center.x + taglist.back().coords.x * 1 / dpp + dpp * pDoc->raster.scale.x - taglist.back().rastersize / 2 / dpp,
			center.y - taglist.back().coords.y * 1 / dpp - taglist.back().rastersize / 2 / dpp,
			center.x + taglist.back().coords.x * 1 / dpp + dpp * pDoc->raster.scale.x + taglist.back().rastersize / 2 / dpp,
			center.y - taglist.back().coords.y * 1 / dpp + taglist.back().rastersize / 2 / dpp };
		pRenderTarget->FillRectangle(rect1, m_pTagBrush);
		pRenderTarget->PopLayer();

		pRenderTarget->DrawRectangle(rect1, m_pWhiteBrush, 1);
		ShowCoordinates(pRenderTarget, taglist.back().coords.x, taglist.back().coords.y, taglist.back().rastersize);

	}

	if (pDoc->mousePos) {

		pRenderTarget->DrawRectangle(CD2DRectF(
			pDoc->mousePos->x - pDoc->raster.size / 2 / dpp,
			pDoc->mousePos->y - pDoc->raster.size / 2 / dpp,
			pDoc->mousePos->x + pDoc->raster.size / 2 / dpp,
			pDoc->mousePos->y + pDoc->raster.size / 2 / dpp),
			m_pWhiteBrush,
			.5f,
			NULL);
		ShowCoordinates(pRenderTarget, (center.x - pDoc->mousePos->x) *-1 * dpp,
			(center.y - pDoc->mousePos->y) * dpp, pDoc->raster.size );
	}

}

bool Grid::SaveToFile() {

	wstringstream sstream;

	for (size_t i = 0; i < taglist.size(); ++i)
	{
		if (i != 0)
			sstream << "\n";
		sstream
			<< center.x - taglist[i].coords.x
			<< ";"
			<< center.y - taglist[i].coords.y;

	}

	CFileDialog FileDlg(FALSE, L"csv", L"targets", OFN_OVERWRITEPROMPT, NULL, NULL, NULL, 1);

	if (FileDlg.DoModal() == IDOK) {
		CString pathName = FileDlg.GetPathName();
		CStdioFile outputFile(pathName, CFile::modeWrite | CFile::modeCreate | CFile::typeUnicode);
		outputFile.WriteString((sstream.str().c_str()));
		outputFile.Close();
		return TRUE;
	}

	return FALSE;

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
		CD2DPointF((xPos * 1 / dpp + center.x ) - (rastersize / 2 * 1 / dpp), (yPos * - 1 / dpp + center.y) - (rastersize / 2 * 1 / dpp + 12)),
		&textLayout,
		m_pWhiteBrush);

}

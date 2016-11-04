#include "stdafx.h"
#include "Grid.h"
#include <sstream>
#include "Target.h"
#include "GridTargetsDoc.h"
#include "resource.h"

using namespace std;
using namespace D2D1;

Grid::Grid()
{
	
	m_pDarkRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::DarkRed));
	m_pRedBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::Red));
	m_pBlueBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::RoyalBlue));
	m_pWhiteBrush = new CD2DSolidColorBrush(NULL, ColorF(ColorF::White));

}

Grid::~Grid() {

	delete m_pDarkRedBrush;
	delete m_pRedBrush;
	delete m_pBlueBrush;
	delete m_pWhiteBrush;

}

void Grid::DelTag() {

	if (taglist.size() > 0)
		taglist.pop_back();
}

void Grid::ClearTaglist() {
	taglist.clear();
}

void Grid::StoreClick(CD2DPointF loc) {

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();

	centerOffset.x = (center.x - loc.x)*-1 * dpp;
	centerOffset.y = (center.y - loc.y)* dpp;
	taglist.push_back(centerOffset);

}


void Grid::Paint(CHwndRenderTarget* pRenderTarget) {

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	dpp = (m_pDeltaFOD + m_pRadNerve) / (mainWnd.Width() / 2);

	// draw a grid background around the center
	for (float x = center.x; x > 0; x -= 1/dpp)
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

	for (float y = center.y; y > 0; y -= 1/dpp)
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

	// //draw circles around the center
	//for (float x = 0; x < 16; x++) {
	//	CD2DRectF e = { center.x - 1 / dpp * x,
	//		center.y - 1 / dpp * x,
	//		center.x + 1 / dpp * x,
	//		center.y + 1 / dpp * x};
	//	pRenderTarget->DrawEllipse(e, m_pWhiteBrush, .5f);
	//}
	//
	
	// draw circles around foveola and fovea
	CD2DRectF e1 = { center.x - 1 / dpp * 1.2f,
		center.y - 1 / dpp * 1.2f,
		center.x + 1 / dpp * 1.2f,
		center.y + 1 / dpp * 1.2f};
	CD2DRectF e2 = { center.x - 1 / dpp * 6.2f,
		center.y - 1 / dpp * 6.2f,
		center.x + 1 / dpp * 6.2f,
		center.y + 1 / dpp * 6.2f };
	pRenderTarget->DrawEllipse(e1, m_pRedBrush, .5f);
	pRenderTarget->DrawEllipse(e2, m_pRedBrush, .5f);

	// draw red cross in fovea
	CD2DRectF fovea(center.x - 4,
		center.y - 4,
		center.x + 4,
		center.y + 4);
	
	pRenderTarget->DrawEllipse(fovea, m_pRedBrush);
	pRenderTarget->DrawLine(CD2DPointF(center.x - 8, center.y), 
							CD2DPointF(center.x + 8, center.y),
							m_pRedBrush);

	pRenderTarget->DrawLine(CD2DPointF(center.x, center.y - 8),
							CD2DPointF(center.x, center.y + 8),
							m_pRedBrush);

	// draw optic nerve as blue circle
	nerve = { float(center.x + mainWnd.Width() / 2 - 5 / dpp),
		(float)((center.y - 2.5 / dpp)-86),
		(float)(center.x + mainWnd.Width() / 2),
		(float)((center.y + 2.5 / dpp)-86) };

	pRenderTarget->DrawEllipse(nerve, m_pBlueBrush);
	
}

void Grid::Tag(CHwndRenderTarget* pRenderTarget) {

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();
	CD2DRectF rect1, rect2;
	CRect intersect;

	for (size_t i = 0; i < taglist.size(); i++) {
		rect1 = { center.x + taglist[i].x * 1 / dpp + dpp * pDoc->raster.scale.x - pDoc->raster.size / 2 / dpp,
			center.y - taglist[i].y * 1 / dpp - pDoc->raster.size / 2 / dpp,
			center.x + taglist[i].x * 1 / dpp + dpp * pDoc->raster.scale.x + pDoc->raster.size / 2 / dpp,
			center.y - taglist[i].y * 1 / dpp + pDoc->raster.size / 2 / dpp };
		pRenderTarget->FillRectangle(rect1, m_pDarkRedBrush);

		for (size_t j = 0; j < taglist.size(); j++) {
			rect2 = { center.x + taglist[j].x * 1 / dpp + dpp * pDoc->raster.scale.x - pDoc->raster.size / 2 / dpp,
				center.y - taglist[j].y * 1 / dpp - pDoc->raster.size / 2 / dpp,
				center.x + taglist[j].x * 1 / dpp + dpp * pDoc->raster.scale.x + pDoc->raster.size / 2 / dpp,
				center.y - taglist[j].y * 1 / dpp + pDoc->raster.size / 2 / dpp };
			if (i != j && IntersectRect(&intersect, (CRect)rect1, (CRect)rect2))
				pRenderTarget->FillRectangle((CD2DRectF)intersect, m_pRedBrush);
		}
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
	}

}

bool Grid::SaveToFile() {

	wstringstream sstream;

	for (size_t i = 0; i < taglist.size(); ++i)
	{
		if (i != 0)
			sstream << "\n";
		sstream
			<< center.x - taglist[i].x
			<< ";"
			<< center.y - taglist[i].y;

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


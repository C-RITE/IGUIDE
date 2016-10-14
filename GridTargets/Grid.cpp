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

	if (taglist.size() > 0) {
		taglist.pop_back();
	}

}

void Grid::ClearTaglist() {
	taglist.clear();
}

void Grid::StoreClick(CD2DPointF loc) {

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();

	int halfMean = pDoc->raster.meanEdge / 2;

	centerOffset.x = (center.x - loc.x)*-1;
	centerOffset.y = center.y - loc.y;
	CD2DRectF tag = { loc.x - halfMean, loc.y - halfMean, loc.x + halfMean, loc.y + halfMean };
	taglist.push_back(tag);

}

void Grid::transformTags(int cx, int cy) {

	// transform and scale

	for (size_t i = 0; i < taglist.size(); i++) {
			taglist[i].top = 1 / taglist[i].top * cy * 1 / dpp;
			taglist[i].left = 1 / taglist[i].left * cx * 1 / dpp;
			taglist[i].bottom = 1 / taglist[i].bottom * cy * 1 / dpp ;
			taglist[i].right = 1 / taglist[i].right * cx * 1 / dpp;
	}

}


void Grid::paint(CHwndRenderTarget* pRenderTarget) {

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
			0.5f
			);
	}
	
	for (float x = center.x + 1 / dpp; x < mainWnd.Width(); x += 1 / dpp)
	{
		pRenderTarget->DrawLine(
			CD2DPointF(x, 0.0f),
			CD2DPointF(x, mainWnd.Height()),
			m_pWhiteBrush,
			0.5f
			);
	}

	for (float y = center.y; y > 0; y -= 1/dpp)
	{
		pRenderTarget->DrawLine(
			CD2DPointF(0.0f, y),
			CD2DPointF(mainWnd.Width(), y),
			m_pWhiteBrush,
			0.5f
			);
	}

	for (float y = center.y + 1 / dpp; y < mainWnd.Height(); y += 1 / dpp)
	{
		pRenderTarget->DrawLine(
			CD2DPointF(0.0f, y),
			CD2DPointF(mainWnd.Width(), y),
			m_pWhiteBrush,
			0.5f
			);
	}

	/* draw circles around the center
	for (float x = 0; x < 16; x++) {
		CD2DRectF e = { center.x - 1 / dpp * x,
			center.y - 1 / dpp * x,
			center.x + 1 / dpp * x,
			center.y + 1 / dpp * x};
		pRenderTarget->DrawEllipse(e, m_pWhiteBrush, .5f);
	}
	*/
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

void Grid::tag(CHwndRenderTarget* pRenderTarget) {

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();
	
	AfxGetMainWnd()->GetClientRect(mainWnd);
	center = mainWnd.CenterPoint();

	CRect intersect;

	for (size_t i = 0; i < taglist.size(); i++) {
			pRenderTarget->FillRectangle(taglist[i], m_pDarkRedBrush);
		for (size_t j = 0; j < taglist.size(); j++) 
			if (i != j && IntersectRect(&intersect, (CRect)taglist[i], (CRect)taglist[j])) 
				pRenderTarget->FillRectangle((CD2DRectF)intersect, m_pRedBrush);
	}

	if (pDoc->mousePos) {
		pRenderTarget->DrawRectangle(CD2DRectF(
			pDoc->mousePos->x - pDoc->raster.meanEdge/2,
			pDoc->mousePos->y - pDoc->raster.meanEdge/2,
			pDoc->mousePos->x + pDoc->raster.meanEdge/2,
			pDoc->mousePos->y + pDoc->raster.meanEdge/2),
			m_pWhiteBrush,
			1,
			NULL);
	}

}

bool Grid::saveToFile() {

	wstringstream sstream;

	for (size_t i = 0; i < taglist.size(); ++i)
	{
		if (i != 0)
			sstream << "\n";
		sstream
			<< center.x - (taglist[i].right - taglist[i].left) / 2
			<< ";"
			<< center.y - (taglist[i].bottom - taglist[i].top) / 2;

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


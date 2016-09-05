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
	
	m_pGrid_mark = new CD2DBitmap(NULL, IDB_GRIDMARK, L"PNG");

	m_pFundus = NULL;

}

Grid::~Grid() {

	delete m_pDarkRedBrush;
	delete m_pRedBrush;
	delete m_pBlueBrush;
	delete m_pWhiteBrush;
	delete m_pGrid_mark;

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

	int halfMean = pDoc->raster.meanEdge / 2;

	centerOffset.x = center.x - loc.x;
	centerOffset.y = center.y - loc.y;
	CD2DRectF tag = { loc.x - halfMean, loc.y - halfMean, loc.x + halfMean, loc.y + halfMean };
	taglist.push_back(tag);

}

void Grid::reposition_tags() {
	for (size_t i = 0; i < taglist.size(); i++) {

	}
}


void Grid::Paint(CHwndRenderTarget* pRenderTarget) {

	// paint grid

	pRenderTarget->Clear(ColorF(ColorF::Black));

	m_pLayer_A = new CD2DLayer(pRenderTarget, 1);

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();

	CRect mainWnd;
	AfxGetMainWnd()->GetClientRect(mainWnd);

		if (m_pFundus)
		{
			CD2DSizeF size = m_pFundus->GetSize();
			CPoint point = (size.width /2, size.height /2);
			D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation((float)-point.x, (float)-point.y);
			D2D1_MATRIX_3X2_F identity = D2D1::IdentityMatrix();
			pRenderTarget->SetTransform(matrix);
			pRenderTarget->DrawBitmap(m_pFundus, CD2DRectF(0, 0, size.width/2, size.height/2));
			pRenderTarget->SetTransform(identity);
		}

	pRenderTarget->PushLayer(D2D1::LayerParameters(
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_ALIASED,
		D2D1::IdentityMatrix(),
		1.0,
		NULL,
		D2D1_LAYER_OPTIONS_NONE),
		*m_pLayer_A);

	if (m_pGrid_mark->IsValid()) {

		CD2DSizeF size = m_pGrid_mark->GetPixelSize();
		pRenderTarget->DrawBitmap(m_pGrid_mark, CD2DRectF(
			mainWnd.CenterPoint().x - size.width / 2, 
			mainWnd.CenterPoint().y - size.height / 2, 
			mainWnd.CenterPoint().x + size.width / 2,
			mainWnd.CenterPoint().y + size.height / 2)
);
	}
	
	pRenderTarget->PopLayer();

}

void Grid::Tag(CHwndRenderTarget* pRenderTarget) {

	CGridTargetsDoc* pDoc;
	pDoc = CGridTargetsDoc::GetDoc();
	
	CRect intersect;

	pRenderTarget->PushLayer(D2D1::LayerParameters(
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_ALIASED,
		D2D1::IdentityMatrix(),
		0.5,
		NULL,
		D2D1_LAYER_OPTIONS_NONE),
		*m_pLayer_A);

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

	pRenderTarget->PopLayer();


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


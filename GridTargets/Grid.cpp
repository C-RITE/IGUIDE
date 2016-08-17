#include "stdafx.h"
#include "Grid.h"
#include <sstream>
#include "Target.h"
#include "resource.h"

using namespace std;
using namespace D2D1;

Grid::Grid()

{
	taglist.reserve(100);
	hAxis.reserve(30);
	vAxis.reserve(30);

}

Grid::~Grid() {
}

void Grid::del_tag(){

	if (taglist.size() > 0){
		taglist.pop_back();
	}
}

void Grid::clear_taglist(){
	taglist.clear();
}

void Grid::store_click(CPoint loc){

	centerOffset.x = center.x - loc.x;
	centerOffset.y = center.y - loc.y;
	CRect tag = { loc.x - 36, loc.y - 36, loc.x + 36, loc.y + 36 };
	taglist.push_back(tag);

}

void Grid::reposition_tags(){
	for (size_t i = 0; i < taglist.size(); i++){

	}
}


void Grid::paint(CHwndRenderTarget* renderTarget){
	
	// paint grid

	CD2DSolidColorBrush BlueBrush(renderTarget, ColorF(ColorF::RoyalBlue));
	CD2DSolidColorBrush WhiteBrush(renderTarget, ColorF(ColorF::White));
	CD2DLayer Layer(renderTarget, 1);
	CD2DBitmap Grid_mark(renderTarget, IDB_GRIDMARK, L"PNG");

	CRect rect;
	GetClientRect(renderTarget->GetHwnd(), &rect);		// get client area size
	center = rect.CenterPoint();
	
		renderTarget->PushLayer(D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			NULL,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			1.0,
			NULL,
			D2D1_LAYER_OPTIONS_NONE), Layer);
	
	/*if (theTarget->boundary.size() > 3){
		CD2DRectF gridTarget;
		CD2DRectF targetRect = { theTarget->boundary[0].x, theTarget->boundary[1].y, 
								 theTarget->boundary[2].x, theTarget->boundary[3].y };
		gridTarget.left = center.x - ((targetRect.right - targetRect.left /2) - targetRect.left);
		gridTarget.top = center.y - ((targetRect.bottom - targetRect.top /2) - targetRect.top);
		gridTarget.right = center.x + (targetRect.right - (targetRect.right - targetRect.left /2));
		gridTarget.bottom = center.y + (targetRect.bottom - (targetRect.bottom - targetRect.top /2));

		renderTarget->FillRectangle(&gridTarget, &BlueBrush);

	}*/

	renderTarget->DrawBitmap(&Grid_mark, CD2DRectF(30, 30, 750, 750));
	renderTarget->PopLayer();

}

void Grid::tag(CHwndRenderTarget* renderTarget){
	
	for (size_t i = 0; i < taglist.size(); i++){
		CD2DSolidColorBrush BlueBrush(renderTarget, ColorF(ColorF::RoyalBlue));
		renderTarget->FillRectangle(taglist[i], &BlueBrush);
	}
	
}

bool Grid::saveToFile(){

	wstringstream sstream;

	for (size_t i = 0; i < taglist.size(); ++i)
	{
		if (i != 0)
			sstream << "\n";
		sstream << center.x - (taglist[i].right - taglist[i].left)/2 << ";" << center.y - (taglist[i].bottom -  taglist[i].top)/2;

	}

	CFileDialog FileDlg(FALSE, L"csv", L"targets", OFN_OVERWRITEPROMPT, NULL, NULL, NULL, 1);
	
	if (FileDlg.DoModal() == IDOK){
		CString pathName = FileDlg.GetPathName();
		CStdioFile outputFile(pathName, CFile::modeWrite | CFile::modeCreate | CFile::typeUnicode);
		outputFile.WriteString((sstream.str().c_str()));
		outputFile.Close();
		return TRUE;
	}

	return FALSE;
	
}
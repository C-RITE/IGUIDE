#pragma once
#include "stdafx.h"
#include <vector>

class Grid{

private:

	std::vector<CD2DRectF>taglist;
	std::vector<CD2DRectF>hAxis;
	std::vector<CD2DRectF>vAxis;

	CD2DBitmap*				m_pGrid_clean;
	CD2DBitmap*				m_pGrid_mark;

private:

	CD2DPointF				center;

public:

	Grid();
	~Grid();

	CPoint centerOffset;
	float square;						//square size
	int size;

	void del_tag();
	void clear_taglist();
	void reposition_tags();
	void store_click(CPoint point);
	void paint(CHwndRenderTarget* renderTarget);			//paint the grid
	void tag(CHwndRenderTarget* renderTarget);				//paint scanfield
	bool saveToFile();
	
};

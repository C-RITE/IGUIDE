#pragma once
#include "stdafx.h"
#include <vector>

class Grid{

private:

	CD2DSolidColorBrush*	m_pDarkRedBrush;
	CD2DSolidColorBrush*	m_pRedBrush;
	CD2DSolidColorBrush*	m_pWhiteBrush;
	CD2DSolidColorBrush*	m_pBlueBrush;

	CD2DLayer*				m_pLayer_A;

public:

	Grid();
	~Grid();

	CD2DBitmap*				m_pFundus;						// fundus picture
	std::vector<CD2DRectF>	taglist;						// storage for all scan rasters
	CPoint					centerOffset;					// offset from grid's center

	CD2DPointF				center;							// center of grid
	CD2DBitmap*				m_pGrid_clean;					// grid without markings
	CD2DBitmap*				m_pGrid_mark;					// grid with markings

	void DelTag();
	void ClearTaglist();
	void reposition_tags();
	void StoreClick(CD2DPointF point);
	void Paint(CHwndRenderTarget* pRenderTarget);			// paint the grid
	void Tag(CHwndRenderTarget* pRenderTarget);				// user tag the grid
	bool saveToFile();
	
};
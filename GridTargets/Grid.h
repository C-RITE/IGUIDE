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
	CD2DRectF				nerve;							// optic disc
	CD2DPointF				center;
	CRect					mainWnd;
	CPoint					centerOffset;					// offset from grid's center
	CD2DBitmap*				m_pGrid_clean;					// grid without markings
	CD2DBitmap*				m_pGrid_mark;					// grid with markings

	const int				m_pDeltaFOD = 15;				/* distance between foveal and 
															   optic disc center in degrees */
	const float				m_pRadNerve = 2.5;				// radius of optic nerve head
	float					dpp;							// degree per pixel

	void DelTag();
	void ClearTaglist();
	void reposition_tags();
	void StoreClick(CD2DPointF point);
	void Paint(CHwndRenderTarget* pRenderTarget);			// paint the grid
	void Tag(CHwndRenderTarget* pRenderTarget);				// user tag the grid
	bool saveToFile();
	
};
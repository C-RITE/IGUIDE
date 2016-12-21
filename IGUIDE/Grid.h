#pragma once
#include "stdafx.h"
#include <vector>
#include "Tags.h"

enum Overlay
{
	GRID = 1,
	DEGRAD = 2,
	FOVEA = 4,
	FOVEOLA = 8,
	OPTICDISC = 16,
	CROSSHAIR = 32,
	TRACEINFO = 64,
	FUNDUS = 128
};
	

class Grid{

private:

	CD2DSolidColorBrush 
		*m_pDarkRedBrush,
		*m_pRedBrush,
		*m_pWhiteBrush,
		*m_pBlueBrush,
		*m_pDarkGreenBrush,
		*m_pMagentaBrush,
		*m_pTagBrush;

	CD2DLayer*				pLayer;

public:


	Grid();
	~Grid();

	DWORD					overlay;						// for different view styles
	Tags					taglist;						// storage for all rasters
	int						locked;							// number of locked tags
	CD2DRectF				nerve;							// optic disc
	CD2DPointF				center;
	CRect					mainWnd;
	
	D2D1_LAYER_PARAMETERS	lpHi;

	const int				m_pDeltaFOD = 15;				/* distance between foveal and 
															   optic disc center in degrees */
	const float				m_pRadNerve = 2.5;				// radius of optic nerve head
	float					dpp;							// degree per pixel

	void DelTag();
	void ClearTaglist();
	void StoreClick(CD2DPointF point);
	void Paint(CHwndRenderTarget* pRenderTarget);			// paint the grid
	void DrawOverlay(CHwndRenderTarget* pRenderTarget);		// draw information overlay
	void Tag(CHwndRenderTarget* pRenderTarget);				// user tag the grid
	bool SaveToFile();
	
	void ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, float rastersize);
};
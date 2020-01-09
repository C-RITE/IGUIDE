#pragma once
#include "stdafx.h"
#include <vector>
#include "Patches.h"

enum Overlay
{
	GRID = 1,
	RESERVED = 2,
	PATCHES = 4,
	RESERVED2 = 8,
	OPTICDISC = 16,
	CROSSHAIR = 32,
	TRACEINFO = 64,
	FUNDUS = 128,
	QUICKHELP = 256,
	DEFOCUS = 512
};
	

class Grid{

private:

	CD2DSolidColorBrush
		*m_pDarkRedBrush,
		*m_pRedBrush,
		*m_pWhiteBrush,
		*m_pBlueBrush,
		*m_pGrayBrush,
		*m_pDarkGreenBrush,
		*m_pMagentaBrush,
		*m_pPatchBrush;

	CD2DBrushProperties*	m_pBrushProp;
	CD2DLayer*				pLayer;

public:

	Grid();
	~Grid();

	DWORD					overlay;						// for different view styles
	Patches					patchlist;						// storage for all rasters
	CD2DRectF				nerve;							// optic disc
	CD2DPointF				center;
	CRect					mainWnd;
	
	D2D1_LAYER_PARAMETERS	lpHi;

	const int				m_pDeltaFOD = 15;				/* distance between foveal and 
															   optic disc center in degrees */
	const double			m_pRadNerve = 2.5;				// radius of optic nerve head
	double					dpp;							// degree per pixel

	void DelPatch();
	void ClearPatchlist();
	void StorePatch(CD2DPointF point);						// store patch upon click of mouse button

	void DrawGrid(CHwndRenderTarget* pRenderTarget);		// paint the grid
	void DrawOptional(CHwndRenderTarget* pRenderTarget);	// draw information overlay
	void DrawPatches(CHwndRenderTarget* pRenderTarget);		// draw patches
	void DrawTextInfo(CHwndRenderTarget* pRenderTarget);	// draw text
	
	void ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, CRect rect);
	void ShowVidNumber(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, float rastersize, int number);

};
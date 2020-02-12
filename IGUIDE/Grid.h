#pragma once
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
	CD2DLayer*				m_pLayer1;
	D2D1_LAYER_PARAMETERS	lpHi;

public:

	Grid();
	~Grid();

	DWORD					overlay;						// for different view styles
	Patches					patchlist;						// storage for all rasters
	CD2DRectF				nerve;							// optic disc
	CD2DPathGeometry*		m_pGridGeom;					// the grid

	void DelPatch();
	void ClearPatchlist();
	void StorePatch(CPoint point);											// store patch upon click of mouse button
	void DrawOverlay(CHwndRenderTarget* pRenderTarget);						// draw information overlay
	void DrawPatches(CHwndRenderTarget* pRenderTarget);						// draw patches
	void DrawTextInfo(CHwndRenderTarget* pRenderTarget);					// draw text
	void CreateGridGeometry(CHwndRenderTarget* pRenderTarget);				// construct the grid
	void DrawCircles(CHwndRenderTarget* pRenderTarget);						// draw circles around center
	void DrawPatchField(CHwndRenderTarget* pRenderTarget, CD2DPointF loc);	// draw patch outline around mouse cursor
	void ShowCoordinates(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, CRect rect);
	void ShowVidNumber(CHwndRenderTarget* pRenderTarget, float xPos, float yPos, float rastersize, int number);

};
#pragma once
#include "Patches.h"
#include <queue>

enum Overlay
{
	GRID = 1,
	LOCATION = 2,
	PATCHES = 4,
	TARGETZONE = 8,
	OPTICDISC = 16,
	CROSSHAIR = 32,
	TRACEINFO = 64,
	FUNDUS = 128,
	QUICKHELP = 256,
	RESERVED2 = 512
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
		*m_pPatchBrush,
		*m_pBlackBrush,
		*m_pYellowBrush,
		*m_pGoldenBrush;

	CD2DBrushProperties*	m_pBrushProp;
	CD2DLayer*				m_pLayer1;
	D2D1_LAYER_PARAMETERS	lpHi;
	D2D1_LAYER_PARAMETERS	lpLo;

public:

	Grid();
	~Grid();

	bool					showCursor;						// toggle cursor visibility
	bool					showCoords;						// toggle coords visibility
	bool					isPanning;						// toggle vidnumber visibility
	CD2DSizeF				POISize;						// size of patch matrix
	CD2DPointF				currentPos;						// current cursor position
	DWORD					overlay;						// for different overlays
	Patches					patchlist;						// storage for all patches
	Patches					POI;							// storage matrix for patches spanning a POI
	std::queue<Patch>		patchjob;						// storage queue for patch matrices
	CD2DRectF				nerve;							// optic disc
	CD2DRectF				cursor;							// current cursor
	CD2DPathGeometry*		m_pGridGeom;					// the grid
	
	CD2DPointF				PixelToDegree(CPoint point);	// calculate degrees from fovea from pixel coordinates

	void DelPatch();
	void ClearPatchlist();
	void StorePatch(Patch p);												// store a patch
	void makePOI(CPoint point, CD2DSizeF size);								// create a patchlist around mousepoint
	void fillPatchJob();													// fill patch queue to process a POI
	Patch* doPatchJob();													// process patch queue

	void CreateD2DResources(CHwndRenderTarget* pRenderTarget);				// something to paint with
	void CreateGridGeometry(CHwndRenderTarget* pRenderTarget);				// construct the grid

	void DrawExtras(CHwndRenderTarget* pRenderTarget);						// draw information overlay
	void DrawGrid(CHwndRenderTarget* pRenderTarget);						// draw grid overlay
	void DrawCircles(CHwndRenderTarget* pRenderTarget);						// draw circles around center
	void DrawPatches(CHwndRenderTarget* pRenderTarget);						// draw patches
	void DrawDebug(CHwndRenderTarget* pRenderTarget);						// draw debug info
	void DrawLocation(CHwndRenderTarget* pRenderTarget);					// draw coordinates and defocus of cursor
	void DrawPOI(CHwndRenderTarget* pRenderTarget, CPoint mousePos);		// draw point of interest
	void DrawQuickHelp(CHwndRenderTarget* pRenderTarget);					// draw quick help
	void DrawTarget(CHwndRenderTarget* pRenderTarget, CD2DBitmap* fTarget);	// draw target
	void DrawPatchCursor(CHwndRenderTarget* pRenderTarget, CD2DPointF loc);	// draw patch outline around mouse pointer
	void DrawTargetZone(CHwndRenderTarget* pRenderTarget);					// show borders of target visibility
	void DrawCoordinates(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, CD2DRectF loc);	// show coordinates of cursor and last patch
	void DrawVidNumber(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, int number);

};
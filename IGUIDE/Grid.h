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
	REGIONS = 512
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
		*m_pLightGreenBrush,
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

	bool					showCursor;						// toggle mouse cursor visibility
	bool					showCoords;						// toggle coords visibility
	bool					isPanning;						// toggle vidnumber/coords visibility

public:

	CD2DSizeF				regionSize;						// region dimensions w/patch overlap
	CD2DPointF				currentPos;						// current cursor position
	DWORD					overlay;						// for different overlays
	Patches					patchlist;						// storage for all patches
	Patches					region;							// storage for patches spanning a region
	std::vector<CD2DRectF>	regRects;
	Patches::iterator		currentPatch;					// current patchlist element
	Patch*					cursorPatch;					// the cursor patch
	CD2DRectF				nerve;							// optic disc
	CD2DRectF				cursor;							// current cursor
	CD2DRectF				regionRect;						// current patchjob outline
	CD2DPathGeometry*		m_pGridGeom;					// the grid
	CD2DPathGeometry*		m_pRegionGeom;					// the patchjob geometry
	
	CD2DPointF				PixelToDegree(CPoint point);	// calculate to degrees from fovea from pixel coordinates
	CD2DPointF				calcMargin(	CD2DPointF &marginX,// calculate margin for displaying coordinates around patch
										CD2DPointF &marginY,// or position
										CD2DPointF pos);

	void clearPatchlist();
	void addPatch(CPoint point);							// store single patch in patchlist
	void addRegion();										// add region to patchlist and pane
	void makeRegion(CPoint point, SIZE wheel);				// make a region with given size
	void makeRegionRects(int region);						// set borders of region for operator view
	void calcRegionSize(float zoom);						// calculate region size depending on zoom
	void browse(Element e);
	void setCurrentPatch(int region, int index);			// set current patch
	void commitPatch();										// commit current patch
	void updateCursorPatch();								// update cursor patch
	void selectPatch(int uID);								// select current patch from region pane

	void CreateD2DResources(CHwndRenderTarget* pRenderTarget);								// something to paint with
	void CreateGridGeometry(CHwndRenderTarget* pRenderTarget);								// construct the grid
	void DrawExtras(CHwndRenderTarget* pRenderTarget);										// draw information overlay
	void DrawGrid(CHwndRenderTarget* pRenderTarget);										// draw grid overlay
	void DrawCircles(CHwndRenderTarget* pRenderTarget);										// draw circles around center
	void DrawPatches(CHwndRenderTarget* pRenderTarget);										// draw patches
	void DrawRegions(CHwndRenderTarget* pRenderTarget);										// draw region matrix
	void DrawDebug(CHwndRenderTarget* pRenderTarget);										// draw debug info
	void DrawLocation(CHwndRenderTarget* pRenderTarget);									// draw coordinates and defocus of cursor
	void DrawRegion(CHwndRenderTarget* pRenderTarget, CPoint mousePos, float zoom);			// draw region
	void DrawQuickHelp(CHwndRenderTarget* pRenderTarget);									// draw quick help
	void DrawTarget(CHwndRenderTarget* pRenderTarget, CD2DBitmap* fTarget);					// draw target
	void DrawPatchCursor(CHwndRenderTarget* pRenderTarget, float zoom, CD2DSizeF mouseDist);// draw patch outline of last click
	void DrawVidIndex(CHwndRenderTarget* pRenderTarget, float zoom, CD2DSizeF mouseDist);	// draw index in patch
	void DrawMouseCursor(CHwndRenderTarget* pRenderTarget, CD2DPointF loc, float zoom);		// draw patch outline around mouse pointer
	void DrawTargetZone(CHwndRenderTarget* pRenderTarget);									// show borders of target visibility
	void DrawCoordinates(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, CD2DRectF loc);	// show coordinates around mouse cursor

};
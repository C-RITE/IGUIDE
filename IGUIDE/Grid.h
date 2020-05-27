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

	bool					showCursor;						// toggle cursor visibility
	bool					showCoords;						// toggle coords visibility
	bool					isPanning;						// toggle vidnumber visibility

public:

	CD2DSizeF				POISize;						// actual POI dimensions with chosen patch overlap
	SIZE					wheelNotch;						// current wheel location in x and y axis
	CD2DPointF				currentPos;						// current cursor position
	DWORD					overlay;						// for different overlays
	Patches					patchlist;						// storage for all patches
	Patches::iterator		currentPatch;					// current focus on patch
	Patches					POI;							// storage matrix for patches spanning a POI
	Patches					patchjob;						// storage queue for patch matrices
	CD2DRectF				nerve;							// optic disc
	CD2DRectF				cursor;							// current cursor
	CD2DPathGeometry*		m_pGridGeom;					// the grid
	CD2DPathGeometry*		m_pPatchJobGeom;				// the patchjob geometry

	int						regCount;						// count the regions
	
	CD2DPointF				PixelToDegree(CPoint point);	// calculate to degrees from fovea from pixel coordinates

	void clearPatchlist();
	void addPatch(CPoint point);							// store single patch in patchlist
	void controlPOI(int notch, int dim, CPoint point);		// set POI dimensions
	void makePOI(CPoint point);								// create a patchlist around mousepointer
	void fillPatchJob(CHwndRenderTarget* pRenderTarget);	// fill patch queue to process a POI
	void calcPOIsize(float zoom);							// calculate real POI size
	Patch* doPatchJob(Element e);							// process patch queue
	Patch getPatch(int index);								// get patch by index
	int getCurrentPatchJobPos();							// get current position in patchjob

	void CreateD2DResources(CHwndRenderTarget* pRenderTarget);								// something to paint with
	void CreateGridGeometry(CHwndRenderTarget* pRenderTarget);								// construct the grid
	void DrawExtras(CHwndRenderTarget* pRenderTarget);										// draw information overlay
	void DrawGrid(CHwndRenderTarget* pRenderTarget);										// draw grid overlay
	void DrawCircles(CHwndRenderTarget* pRenderTarget);										// draw circles around center
	void DrawPatches(CHwndRenderTarget* pRenderTarget);										// draw patches
	void DrawPatchJob(CHwndRenderTarget* pRenderTarget);									// draw POI matrix
	void DrawDebug(CHwndRenderTarget* pRenderTarget);										// draw debug info
	void DrawLocation(CHwndRenderTarget* pRenderTarget);									// draw coordinates and defocus of cursor
	void DrawPOI(CHwndRenderTarget* pRenderTarget, CPoint mousePos, float zoom);			// draw point of interest
	void DrawQuickHelp(CHwndRenderTarget* pRenderTarget);									// draw quick help
	void DrawTarget(CHwndRenderTarget* pRenderTarget, CD2DBitmap* fTarget);					// draw target
	void DrawPatchCursor(CHwndRenderTarget* pRenderTarget, CD2DPointF loc, float zoom);		// draw patch outline around mouse pointer
	void DrawTargetZone(CHwndRenderTarget* pRenderTarget);									// show borders of target visibility
	void DrawCoordinates(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, CD2DRectF loc);	// show coordinates of cursor and last patch
	void DrawVidIndex(CHwndRenderTarget* pRenderTarget, CD2DPointF pos, int number);

};

// GridTargetsDoc.h : interface of the CGridTargetsDoc class
//

#pragma once
#include "Target.h"
#include "Grid.h"
#include "Fundus.h"
#include "afxrendertarget.h"
#include <vector>

using namespace std;

class CGridTargetsDoc : public CDocument
{
protected: // create from serialization only
	CGridTargetsDoc();
	DECLARE_DYNCREATE(CGridTargetsDoc)

// Attributes
public:

	Grid*					m_pGrid;							// grid class
	Fundus*					m_pFundus;							// fundus class

	struct Edge
	{
		CD2DPointF p, q;
		float length;
		float alpha;
	};

	struct Raster
	{
		vector<CD2DPointF>	corner;								// raster corners as clicked by subject
		vector<Edge>		perimeter;							// raster perimeter
		float				meanEdge;							// average of 4 raster edges
		float				meanAlpha;							// displacement angle
		CD2DPointF			mid;								// triangulate mid point of raster
	};

	Raster					raster;
		
	CPoint					center;								// center of main window
	CPoint*					mousePos;							// current mouse position

// Operations
public:
	BOOL CheckFOV();
	static CGridTargetsDoc* GetDoc();
	float CalcEdgeLength(Edge k);
	CD2DPointF compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount);
	void computeDisplacementAngles();
	float computeDisplacementAngle(Edge k);
	CString getTraceInfo();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CGridTargetsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	afx_msg void OnFileImport();

};

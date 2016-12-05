
// GridTargetsDoc.h : interface of the CGridTargetsDoc class
//

#pragma once
#include "Target.h"
#include "Grid.h"
#include "Fundus.h"
#include "Raster.h"
#include "Properties.h"
#include "afxrendertarget.h"

class CGridTargetsDoc : public CDocument
{
protected: // create from serialization only
	CGridTargetsDoc();
	DECLARE_DYNCREATE(CGridTargetsDoc)

	// Attributes
public:

	Grid*					m_pGrid;							// grid class
	Fundus*					m_pFundus;							// fundus class
	Calibration*			m_pDlgCalibration;					// calibration class
	Properties*				m_pDlgProperties;					// properties class
	Raster					raster;
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
	afx_msg void OnEditProperties();
	afx_msg void OnOverlayGrid();
	afx_msg void OnUpdateOverlayGrid(CCmdUI *pCmdUI);
	afx_msg void OnOverlayRadius();
	afx_msg void OnUpdateOverlayRadius(CCmdUI *pCmdUI);
	afx_msg void OnOverlayFovea();
	afx_msg void OnUpdateOverlayFovea(CCmdUI *pCmdUI);
	afx_msg void OnOverlayOpticdisc();
	afx_msg void OnUpdateOverlayOpticdisc(CCmdUI *pCmdUI);
	afx_msg void OnOverlayCrosshair();
	afx_msg void OnUpdateOverlayCrosshair(CCmdUI *pCmdUI);
	afx_msg void OnOverlayFundus();
	afx_msg void OnUpdateOverlayFundus(CCmdUI *pCmdUI);
	afx_msg void OnOverlayTraceinfo();
	afx_msg void OnUpdateOverlayTraceinfo(CCmdUI *pCmdUI);
};

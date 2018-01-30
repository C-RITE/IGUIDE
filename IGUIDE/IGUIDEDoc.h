
// IGUIDEDoc.h : interface of the CIGUIDEDoc class

#pragma once
#include "Target.h"
#include "Grid.h"
#include "Fundus.h"
#include "Raster.h"
#include "Properties.h"

CIGUIDEDoc* GetDoc();

class CIGUIDEDoc : public CDocument
{
protected: // create from serialization only
	CIGUIDEDoc();
	DECLARE_DYNCREATE(CIGUIDEDoc)

	// Attributes
public:

	Grid*					m_pGrid;							// grid class
	Fundus*					m_pFundus;							// fundus class
	Calibration*			m_pDlgCalibration;					// calibration class
	Properties*				m_pDlgProperties;					// properties dialog
	CString					m_FixationTarget;					// fixation target filename
	int						m_FixationTargetSize;				// fixation target size in percent
	Raster					raster;
	CPoint*					mousePos;							// current mouse position

// Operations
public:
	bool CheckFOV();
	float CalcEdgeLength(Edge k);
	CD2DPointF compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount);
	void ComputeDisplacementAngles();
	float ComputeDisplacementAngle(Edge k);
	float ComputeOrientationAngle(Edge k);
	bool CheckCalibrationValidity();
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
	virtual ~CIGUIDEDoc();
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
	virtual void OnCloseDocument();
};

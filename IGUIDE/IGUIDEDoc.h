
// IGUIDEDoc.h : interface of the CIGUIDEDoc class

#pragma once
#include "Target.h"
#include "Grid.h"
#include "Fundus.h"
#include "Raster.h"
#include "Properties.h"
#include "Monitors.h"


class CIGUIDEDoc : public CDocument
{
protected: // create from serialization only
	CIGUIDEDoc();
	DECLARE_DYNCREATE(CIGUIDEDoc)

	// Attributes

public:
	vector<Screen>			m_Screens;							// all connected monitors
	Screen*					m_selectedScreen;					// target monitor
	int						m_ScreenPixelPitch;					// pixel pitch of target monitor
	int						m_ScreenDistance;					// distance between eye and screen
	Grid*					m_pGrid;							// grid class
	Fundus*					m_pFundus;							// fundus class
	Calibration*			m_pDlgCalibration;					// calibration class
	CString					m_FixationTarget;					// fixation target filename
	CString					m_OutputDir;						// .csv output directory
	int						m_FixationTargetSize;				// fixation target size in percent
	CString					m_AOSACA_IP;						// AOSACA IP Address
	CString					m_ICANDI_IP;						// AOSACA IP Address
	int						m_FlipVertical;						// flip target screen
	Raster					raster;
	CPoint*					mousePos;							// current mouse position
	CString					m_RemoteCtrl;						// remote control subsystem settings

private:
	DWORD					overlaySettings;

// Operations
public:
	static CIGUIDEDoc * GetDoc();
	bool CheckFOV();
	float CalcEdgeLength(Edge k);
	CD2DPointF compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount);
	void ComputeDisplacementAngles();
	double ComputeDisplacementAngle(Edge k);
	double ComputeOrientationAngle(Edge k);
	bool CheckCalibrationValidity();
	bool getScreens();
	CString getTraceInfo();					// for debug purposes only
	vector<CString> getQuickHelp();			// show remote control hotkeys
	CString getCurDefocus();				// show current defocus value from AOSACA

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
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	afx_msg void OnFileImport();
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
	afx_msg void OnOverlayQuickhelp();
	afx_msg void OnUpdateOverlayQuickhelp(CCmdUI *pCmdUI);
	afx_msg void OnOverlayDefocus();
	afx_msg void OnUpdateOverlayDefocus(CCmdUI *pCmdUI);
	void ToggleOverlay();

};

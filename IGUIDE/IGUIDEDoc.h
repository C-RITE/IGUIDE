
// IGUIDEDoc.h : interface of the CIGUIDEDoc class

#pragma once
#include "Target.h"
#include "Grid.h"
#include "Fundus.h"
#include "raster.h"
#include "Properties.h"
#include "Monitors.h"
#include "Controller.h"

struct NetMsg {

	CString property;
	CString value;

};

class CIGUIDEDoc : public CDocument
{
protected: // create from serialization only
	CIGUIDEDoc();
	DECLARE_DYNCREATE(CIGUIDEDoc)

	// Attributes

public:

	Monitors				m_Monitors;							// output device for target view
	Controller				m_Controller;						// controller used for subject calibration
	Grid*					m_pGrid;							// grid class
	Fundus*					m_pFundus;							// fundus class
	Raster					m_raster;							// raster class
	Calibration*			m_pDlgCalibration;					// fundus calibration dialog class
	CString					m_FixationTarget;					// fixation target filename
	CString					m_OutputDir;						// .csv output directory
	CString					m_AOSACA_IP;						// AOSACA IP Address
	CString					m_ICANDI_IP;						// ICANDI IP Address
	CString					m_InputController;					// for subject calibration procedure
	CString					m_FlipVertical;						// flip target screen in x
	CString					m_FlipHorizontal;					// flip target screen in y
	float					m_Overlap;							// patch overlap value in deg
	int						m_FixationTargetSize;				// fixation target size in percent
	CPoint*					m_pMousePos;						// current mouse position
	CString					m_RemoteCtrl;						// remote control subsystem settings
	bool					calibrationComplete;				// true if calibration is valid

	CString*				m_pInputBuf;						// input buffer for incoming messages
	HANDLE*					m_hNetMsg;							// handle for netcom message events
	std::queue<NetMsg>		m_NetMsgQueue;						// net message queue for further processing


private:

	DWORD					overlaySettings;					// used as buffer for toggle options
	bool					overlayVisible;						// visibility status of overlays
	CString					defocus;							// AOSACA defocus

	void					createNetComThread();				// for processing incoming remote messages
	HANDLE					m_hNetComThread;					// handle for incoming message thread
	DWORD					m_thdID;
		
	static	DWORD WINAPI	ThreadNetMsgProc(LPVOID pParam);

// Operations
public:
	bool					CheckFOV();
	float					CalcEdgeLength(Edge k);
	CD2DPointF				compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount);
	void					ComputeDisplacementAngles();
	double					ComputeDisplacementAngle(Edge k);
	double					ComputeOrientationAngle(Edge k);
	CD2DRectF				ComputeTargetZone();
	bool					CheckCalibrationValidity();

	CString					getCurrentDefocus() { return defocus; };
	void					setDefocus(CString def) { defocus = def; };
	void					setVideoDir(CString dir) { m_OutputDir = dir; };

	CString					getTraceInfo();								// for debug purposes only
	vector<CString>			getQuickHelp();								// show remote control hotkeys
	CString					getOutputDir();								// where to store IGUIDE data

	void					OnFundusImport();

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
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

	void ToggleOverlay();
	afx_msg void OnOverlayGrid();
	afx_msg void OnUpdateOverlayGrid(CCmdUI *pCmdUI);
	afx_msg void OnOverlayPatches();
	afx_msg void OnUpdateOverlayPatches(CCmdUI *pCmdUI);
	afx_msg void OnOverlayOpticdisc();
	afx_msg void OnUpdateOverlayOpticdisc(CCmdUI *pCmdUI);
	afx_msg void OnOverlayCrosshair();
	afx_msg void OnUpdateOverlayCrosshair(CCmdUI *pCmdUI);
	afx_msg void OnOverlayFundus();
	afx_msg void OnUpdateOverlayFundus(CCmdUI *pCmdUI);
	afx_msg void OnOverlayTraceinfo();
	afx_msg void OnUpdateOverlayTraceinfo(CCmdUI *pCmdUI);
	afx_msg void OnOverlayQuickhelp();
	afx_msg void OnUpdateOverlayQuickhelp(CCmdUI *pCmdUI);
	afx_msg void OnOverlayLocation();
	afx_msg void OnUpdateOverlayLocation(CCmdUI *pCmdUI);
	afx_msg void OnOverlayTargetzone();
	afx_msg void OnUpdateOverlayTargetzone(CCmdUI* pCmdUI);

};

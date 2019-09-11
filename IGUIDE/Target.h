#pragma once
#include "IGUIDEView.h"

class Edge;
class CIGUIDEDoc;

// Target dialog

struct cursorposition {
	float x;
	float y;
};

class Target : public CDialogEx
{
	DECLARE_DYNAMIC(Target)

// Attributes

	CIGUIDEDoc*				pDoc;				// pointer to corresponding doc
	CD2DSolidColorBrush*	m_pBrushWhite;		// white brush
	CD2DRectF*				m_POI;				// fixation target area
	CD2DBitmap*				m_pFixationTarget;	// custom target
	CD2DPointF				xbox_cross;			// controller's current cursor location
	bool					m_bVisible;			// for toggling the fixation target
	bool					show_cross;			// cross visibility
	bool					m_bMouseEnable;		// toggle mouse operation
	double					ppd_client;			// pixel per degree on client screen
	int						fieldsize;			// fieldsize in pixel on client screen
	int						discretion;			// distance in pixel to raster corners during calibration

// Operations

public:
	Target(CIGUIDEView* pParent = NULL);   // standard constructor
	~Target();
	void Pinpoint(float x, float y);
	void getFixationTarget();
	void calcFieldSize();
	void setCross();
	void OnGamePadCalibration();
	void restartCalibration();
	void finishCalibration();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TARGET };
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

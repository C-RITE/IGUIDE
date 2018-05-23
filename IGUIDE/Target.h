#pragma once
#include "CXBoxController.h"
#include "IGUIDEView.h"

class Edge;
class CIGUIDEDoc;

// Target dialog

struct XboxControlState {
	int LX;	// DPAD x pos
	int LY;	// DPAD y pos
};

struct cursorposition {
	float x;
	float y;
};

UINT ThreadDraw(PVOID pParam);

class Target : public CDialog
{
	DECLARE_DYNAMIC(Target)

// Attributes

	CIGUIDEDoc*				pDoc;				// pointer to corresponding doc
	CD2DSolidColorBrush*	m_pBrushWhite;		// white brush
	CD2DRectF*				m_POI;				// fixation target area
	CD2DBitmap*				m_pFixationTarget;	// custom target
	
	static XboxControlState	xbox_state;			// store controller states
	CD2DPointF				xbox_cross;			// controller's cursor location
	static bool				show_cross;			// cross visibility
	static bool				m_bPushed;			// for input delay
	static bool				m_bFireDown;		// for fire signal
	static bool				m_bFireUp;			// for fire signal
	int						m_fired;			// times hit
	double					ppd_client;			// pixel per degree on client screen
	int						fieldsize;			// fieldsize in pixel on client screen

private:
	bool					m_bRunning;
	CWinThread*				m_pThread;

// Operations
public:
	Target(CIGUIDEView* pParent = NULL);   // standard constructor
	~Target();
	void Pinpoint(float x, float y);
	void getFixationTarget();
	void calcFieldSize();
	void setCross();
	void finishCalibration();
	static UINT InputControllerThread(LPVOID pParam);
	
	

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TARGET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

};

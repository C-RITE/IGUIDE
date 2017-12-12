#pragma once
#include "CXBOXController.h"
class Edge;

// Target dialog

struct xboxcontrolstate {
	float LX;	// left thumbstick x pos
	float LY;	// left thumbstick y pos
};

struct cursorposition {
	float x;
	float y;
};

class Target : public CDialog
{
	DECLARE_DYNAMIC(Target)

// Attributes

	CD2DSolidColorBrush*	m_pBrushWhite;		// white brush
	CD2DRectF*				m_POI;				// fixation target area
	CD2DBitmap*				m_pFixationTarget;	// custom target
	
	CXBOXController*		Player1;			// for target calibration process
	CWinThread*				xboxThread;			// controller's very own thread
	xboxcontrolstate		xbox_state;			// store controller states
	
	float					ppd_client;			// pixel per degree on client screen
	



//structure for passing to the controlling function
typedef struct THREADSTRUCT
{
	Target*    _this;
	//you can add here other parameters you might be interested on
} THREADSTRUCT;

// Operations
public:
	Target(CWnd* pParent = NULL);   // standard constructor
	virtual ~Target();
	void Pinpoint(float x, float y);
	void getFixationTarget();

private:

static UINT StartThread(LPVOID param);

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

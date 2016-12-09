#pragma once
class Edge;

// Target dialog

class Target : public CDialog
{
	DECLARE_DYNAMIC(Target)

// Attributes

	CD2DSolidColorBrush*	m_pBrushWhite;		// white brush
	CD2DRectF*				m_POI;				// fixation target
	float					ppd_client;			// pixel per degree on client screen


// Operations
public:
	Target(CWnd* pParent = NULL);   // standard constructor
	virtual ~Target();
	void Pinpoint(float x, float y);

private:

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


// IGUIDEView.h : interface of the CIGUIDEView class

#pragma once
#include "afxcview.h"
class Target;

class CIGUIDEView : public CView
{

protected: // create from serialization only
	CIGUIDEView();
	DECLARE_DYNCREATE(CIGUIDEView)

// Overrides
protected:

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

// Implementation
public:

	CPoint getMousePos() { 
		return mousePos;
	}

	CPoint getGridCenter(){
		return CPoint (mouseDist.width + CANVAS / 2, mouseDist.height + CANVAS / 2);
	}

	CPoint getMouseDist() {
		return mouseDist;
	}

	CPoint getMouseStart() {
		return mouseStart;
	}

	SIZE getWheel() {
		return wheelNotch;
	}

	float getZoomFactor() {
		return 1 / zoom;
	}

	virtual					~CIGUIDEView();
	void					ToggleFixationTarget();

	CD2DSolidColorBrush*	m_pWhiteBrush;

	static CIGUIDEView*		GetView();
	Target*					m_pDlgTarget;							// target dialog


// Attributes
private:

	CD2DBitmap*				m_pFixationTarget;					// need 2 instances of these (in view and grid class)
	D2D1_MATRIX_3X2_F		scale;
	D2D1_MATRIX_3X2_F		translate;
	CPoint					mousePos;							// current mouse location
	SIZE					wheelNotch;							// current wheel location in x and y axis
	CPoint					mouseStart;							// starting point
	CD2DSizeF				mouseDist;							// distance travelled
	float					zoom;								// zoom factor
																
	bool					m_bMouseTracking;					// track mouse movements outside operator view
	int						countdown;							// countdown video length
	bool					onHold;								// true as long as ICANDI is busy recording videos

// Generated message map functions

	DECLARE_MESSAGE_MAP()
	
public:

	bool					m_lButtonIsDown;					// workaround for not accidently clicking 
	void					ResetTransformationMatrices();
	void					controlWheel(int notch, int dim, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);

private:
	void SetFixationTarget();

protected:
	
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ChangeTargetDisplay(WPARAM w, LPARAM l);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnDraw(CDC* /*pDC*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();

public:
	afx_msg void OnFundusImport();
	afx_msg void OnMouseLeave();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
	afx_msg LRESULT OnInitiateCountdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetCountdown(WPARAM wParam, LPARAM lParam);
};

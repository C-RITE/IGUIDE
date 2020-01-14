
// IGUIDEView.h : interface of the CIGUIDEView class

#pragma once
class Target;

class CIGUIDEView : public CView
{

protected: // create from serialization only
	CIGUIDEView();
	DECLARE_DYNCREATE(CIGUIDEView)

// Attributes
public:
	static CIGUIDEView* GetView();
	Target*				m_pDlgTarget;						// target dialog
	CD2DBitmap*			m_pFixationTarget;
	static CIGUIDEView* GetView();

// Overrides

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

// Implementation
public:
	virtual ~CIGUIDEView();

	void		ToggleFixationTarget();
	CD2DPointF	GetRelativeCenter();

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);

private:

	CD2DBitmap*				m_pFixationTarget;
	CD2DPointF				center;
	CD2DPointF				origin;
	D2D1_MATRIX_3X2_F		scale;
	CPoint					mousePos;							// current mouse location
	CPoint					mouseDist;							// distance travelled
	CPoint					mouseZoom;							// zoom origin
	bool					mouseTraveled;						// travel discriminator
	float					zoom;								// zoom factor
	bool		LButtonIsDown;	// workaround for not accidently clicking 
								// into operator view after openfiledialog

	void CIGUIDEView::SetFixationTarget();


public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnDraw(CDC* /*pDC*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnClose();

protected:
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ChangeTargetDisplay(WPARAM w, LPARAM l);
};

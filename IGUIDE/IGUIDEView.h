
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
	Target*				m_pDlgTarget;						// target dialog
	static CIGUIDEView * GetView();
	
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

// Implementation
public:
	virtual ~CIGUIDEView();
	void ToggleFixationTarget();

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);

private:
	CD2DBitmap* m_pFixationTarget;

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnClose();
protected:
	afx_msg LRESULT OnDisplaychange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ChangeTargetDisplay(WPARAM w, LPARAM l);
};

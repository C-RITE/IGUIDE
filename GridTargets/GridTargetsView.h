
// GridTargetsView.h : interface of the CGridTargetsView class
//

#pragma once


class CGridTargetsView : public CView
{
protected: // create from serialization only
	CGridTargetsView();
	DECLARE_DYNCREATE(CGridTargetsView)

// Attributes
public:
	CGridTargetsDoc*	GetDocument() const;
	Target*				m_pDlgTarget;

private:

	bool showTrace;
	
// Operations
public:

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
	virtual ~CGridTargetsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);

private:
	// brushes, etc..

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in GridTargetsView.cpp
inline CGridTargetsDoc* CGridTargetsView::GetDocument() const
   { return reinterpret_cast<CGridTargetsDoc*>(m_pDocument); }
#endif


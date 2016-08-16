
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
	CGridTargetsDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

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
};

#ifndef _DEBUG  // debug version in GridTargetsView.cpp
inline CGridTargetsDoc* CGridTargetsView::GetDocument() const
   { return reinterpret_cast<CGridTargetsDoc*>(m_pDocument); }
#endif


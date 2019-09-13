
// MainFrm.h : interface of the CMainFrame class

#pragma once
#include "Remote.h"

class Properties;

class CMainFrame : public CFrameWndEx
{

protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	const int WINDOW_WIDTH = 1200;
	const double ASPECT_RATIO = (double)16 / 9;
	const int WINDOW_HEIGHT = WINDOW_WIDTH * (1 / (int)ASPECT_RATIO);

// Operations
public:

// Overrides

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCStatusBar   m_wndStatusBar;
	Properties		m_DlgProperties;

private:

	// for communication with host apps (ICANDI / AOSACA)
	Remote RemoteControl;

// Generated message map functions
protected:
	BOOL CreateDockingWindows();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDocumentReady(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGamePadUpdate(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMouseFallback(WPARAM w, LPARAM l);

public:
	afx_msg void OnEditProperties();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateLinkIndicators(CCmdUI *pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};



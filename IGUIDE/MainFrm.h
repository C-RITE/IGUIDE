
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Properties.h"
#include "netcomm\sockclient.h"

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
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

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

	// for communication with host apps

	CSockClient*				m_pSock_ICANDI;	
	CSockClient*				m_pSock_AOSACA;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL CreateDockingWindows();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg LRESULT PopulateProperties(WPARAM w, LPARAM l);

public:
	afx_msg void OnEditProperties();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdatePage(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};



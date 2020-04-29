
// MainFrm.h : interface of the CMainFrame class

#pragma once
#include "Remote.h"
#include "AreaPane.h"

class CMainFrame : public CFrameWndEx
{
	   
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Implementation
public:
	virtual ~CMainFrame();
	static	CIGUIDEDoc *			GetDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	Remote			RemoteControl;	// for communication with host apps (ICANDI / AOSACA)

protected:  // control bar embedded members
	
	CMFCStatusBar   m_wndStatusBar;
	Properties		m_PropertyPane;
	AreaPane		m_AreaPane;

	BOOL			CreateDockingWindows();


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDocumentReady(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGamePadUpdate(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMouseFallback(WPARAM w, LPARAM l);

public:
	afx_msg void OnViewProperties();
	afx_msg void OnViewAreas();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateLinkIndicators(CCmdUI *pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg LRESULT OnResetAosacaIp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetIcandiIp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPatchToAreapane(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitAreapane(WPARAM wParam, LPARAM lParam);
};
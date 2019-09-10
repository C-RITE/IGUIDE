
// MainFrm.h : interface of the CMainFrame class

#pragma once
#include "Remote.h"

class Properties;

class CMainFrame : public CFrameWndEx
{

public:
	   
	static
	CPoint CMainFrame::GetCenterOffset()
	{
		CRect rect;
		if (!AfxGetMainWnd())
			return CPoint(0, 0);
		AfxGetMainWnd()->GetClientRect(&rect);
		CPoint clientCenter = rect.CenterPoint();
		return CPoint(-CENTER + clientCenter.x,
			-CENTER + clientCenter.y);
	}

	static
	CPoint CMainFrame::GetCenter()
	{
		return CPoint{ CENTER, CENTER };
	}
	
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

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
	DECLARE_MESSAGE_MAP()
	BOOL CreateDockingWindows();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDocumentReady(WPARAM w, LPARAM l);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGamePadUpdate(WPARAM w, LPARAM l);

public:
	afx_msg void OnEditProperties();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateLinkIndicators(CCmdUI *pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};



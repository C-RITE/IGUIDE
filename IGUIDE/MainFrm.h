
// MainFrm.h : interface of the CMainFrame class

#pragma once
#include "Remote.h"
#include "RegionPane.h"

class CMainFrame : public CFrameWndEx
{
	   
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Implementation
public:
	virtual ~CMainFrame();
	static	CIGUIDEDoc *GetDoc();

	void			sendToRegionPane();				// send patch information to region pane
	static void		GetSysTime(CString &buf);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	Remote			RemoteControl;					// for communication with ICANDI and AOSACA

	CIGUIDEDoc*		m_pDoc;

	RegionPane		m_RegionPane;
	Connection		active;
	Connection		pending;

	void			StartCountDown();
	int				countDown;

protected:  // control bar embedded members
	
	CMFCStatusBar   m_wndStatusBar;
	Properties		m_PropertyPane;

	BOOL			CreateDockingWindows();
	
	HANDLE			m_hSaveWaitThread;
	DWORD			m_thdID;
	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewProperties();
	afx_msg void OnViewRegions();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateLinkIndicators(CCmdUI *pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg LRESULT OnDocumentReady(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGamePadUpdate(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMouseFallback(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSaveIguideCsv(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetAosacaIp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetIcandiIp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPatchToRegionPane(WPARAM /*patch*/, LPARAM /*region*/);
	afx_msg LRESULT OnPatchSelect(WPARAM wParam /*index*/, LPARAM lParam /*region*/);
	afx_msg LRESULT OnClearRegionpane(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateSelection(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
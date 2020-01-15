#pragma once

class Properties : public CDockablePane
{
	// Override PropertyGrid base class for some customization

	class MyCMFCPropertyGridCtrl : public CMFCPropertyGridCtrl {
	public:

		// we want the left column a bit smaller than default

		virtual void AdjustLayout()
		{
			m_nLeftColumnWidth = 100;
			CMFCPropertyGridCtrl::AdjustLayout();

		}

		// we also want the window focus to go back to main frame after hitting return

		virtual BOOL MyCMFCPropertyGridCtrl::PreTranslateMessage(MSG* pMsg)
		{
			if (pMsg->message == WM_KEYDOWN)
			{
				if (pMsg->wParam == VK_RETURN) {
					GetMainFrame()->PostMessageW(WM_SETFOCUS);
				}
			}
			return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
		}

		CFrameWndEx* MyCMFCPropertyGridCtrl::GetMainFrame()
		{
			CFrameWndEx* pFrame = (CFrameWndEx *)(AfxGetApp()->m_pMainWnd);

			if (!pFrame)
				return NULL;

			// Fail if view is of wrong kind
			// (this could occur with splitter windows, or additional
			// views on a single document

			if (!pFrame->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
				return NULL;

			return (CFrameWndEx*)pFrame;
		}
	
	};

public:
	Properties();   // standard constructor
	~Properties();

	void AdjustLayout();
	void fillProperties();
	void InitPropList();

	CMFCPropertyGridProperty*		Patch;
	CMFCPropertyGridProperty*		RasterSize;
	CMFCPropertyGridProperty*		PhysParam;
	CMFCPropertyGridProperty*		FixationTargetSize;
	CMFCPropertyGridProperty*		FixationTargetScreen;
	CMFCPropertyGridColorProperty*	Color;
	CMFCPropertyGridProperty*		ICANDI;
	CMFCPropertyGridProperty*		TargetView;
	CMFCPropertyGridFileProperty*	VideoFolder;
	CMFCPropertyGridFileProperty*	FixationFile;
	CMFCPropertyGridProperty*		SubjectCalibration;
	CMFCPropertyGridProperty*		InputController;
	CMFCPropertyGridProperty*		RemoteControl;
	CMFCPropertyGridProperty*		RemoteCapability;
	CMFCPropertyGridProperty*		AOSACA_IP;
	CMFCPropertyGridProperty*		ICANDI_IP;
	CMFCPropertyGridProperty*		FlipVertical;
	CMFCPropertyGridProperty*		FlipHorizontal;

private:
	_variant_t FixationTargetValue;
	_variant_t RasterSizeValue;
	_variant_t ScreenValue;
	_variant_t InputControl;
	_variant_t AOSACA_IPValue;
	_variant_t ICANDI_IPValue;
	_variant_t RemoteValue;
	_variant_t FlipVerticalValue;
	_variant_t FlipHorizontalValue;

	CFont					m_fntPropList;
	MyCMFCPropertyGridCtrl	m_wndPropList;


protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);

DECLARE_DYNAMIC(Properties)
DECLARE_MESSAGE_MAP()


public:
	CString getRemoteCapability();
	CString getAOSACA_IP();
	CString getICANDI_IP();

};
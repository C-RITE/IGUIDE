#pragma once
// Custom Property Pane implementation

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWndEx* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWndEx*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }

};

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

		// we also want the window focus to go back to main frame 
		// after hitting return and not having changed anything

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
	void createPropertyList();
	void setPropertyValues();

	CMFCPropertyGridProperty*		Patch;
	CMFCPropertyGridProperty*		RasterSize;
	CMFCPropertyGridProperty*		PhysParam;
	CMFCPropertyGridFileProperty*	FixationFile;
	CMFCPropertyGridProperty*		FixationSize;
	CMFCPropertyGridProperty*		FixationScreen;
	CMFCPropertyGridColorProperty*	Color;
	CMFCPropertyGridProperty*		ICANDI;
	CMFCPropertyGridProperty*		TargetView;
	CMFCPropertyGridFileProperty*	VideoFolder;
	CMFCPropertyGridProperty*		SubjectCalibration;
	CMFCPropertyGridProperty*		InputController;
	CMFCPropertyGridProperty*		RemoteControl;
	CMFCPropertyGridProperty*		RemoteCapability;
	CMFCPropertyGridProperty*		AOSACA_IP;
	CMFCPropertyGridProperty*		ICANDI_IP;
	CMFCPropertyGridProperty*		FlipVertical;

private:
	_variant_t FixationTargetSizeValue;
	_variant_t FixationTargetFilenameValue;
	_variant_t RasterSizeValue;
	_variant_t VideoFolderValue;
	_variant_t FixationScreenValue;
	_variant_t InputControllerValue;
	_variant_t AOSACA_IP_Value;
	_variant_t ICANDI_IP_Value;
	_variant_t RemoteControlValue;
	_variant_t FlipVerticalValue;
	_variant_t PatchColorValue;
	
	bool isValidIpAddress(_variant_t ipAddr);

protected:
	CFont m_fntPropList;
	MyCMFCPropertyGridCtrl m_wndPropList;
	void InitPropList();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
DECLARE_MESSAGE_MAP()


public:
	CString getRemoteCapability();
	CString getAOSACA_IP();
	CString getICANDI_IP();

};
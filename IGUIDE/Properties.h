#pragma once
// Properties dialog

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
			m_nLeftColumnWidth = 85;
			CMFCPropertyGridCtrl::AdjustLayout();
		}

		// we also want to focus back on the main window after hitting return

		virtual BOOL MyCMFCPropertyGridCtrl::PreTranslateMessage(MSG* pMsg)
		{
			if (pMsg->message == WM_KEYDOWN)
			{
				if (GetKeyState(VK_RETURN))
					AfxGetMainWnd()->PostMessageW(WM_SETFOCUS);
			}
			return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
		}
	
	};

public:
	Properties();   // standard constructor
	~Properties();

	void AdjustLayout();
	void fillProperties();

	CMFCPropertyGridProperty*		Raster;
	CMFCPropertyGridProperty*		RasterSize;
	CMFCPropertyGridProperty*		FixationTargetSize;
	CMFCPropertyGridProperty*		FixationTargetScreen;
	CMFCPropertyGridProperty*		ScreenDistance;
	CMFCPropertyGridProperty*		PixelDensity;
	CMFCPropertyGridColorProperty*	Color;
	CMFCPropertyGridProperty*		ICANDI;
	CMFCPropertyGridProperty*		TargetView;
	CMFCPropertyGridFileProperty*	VideoFolder;
	CMFCPropertyGridFileProperty*	FixationFile;
	CMFCPropertyGridProperty*		RigProperties;
	CMFCPropertyGridProperty*		RemoteControl;
	CMFCPropertyGridProperty*		RemoteCapability;
	CMFCPropertyGridProperty*		AOSACA_IP;
	CMFCPropertyGridProperty*		ICANDI_IP;
	CMFCPropertyGridProperty*		FlipVertical;

private:
	_variant_t * m_pFixationTarget;
	_variant_t * m_pRasterSize;
	_variant_t * m_pScreen;
	_variant_t * m_pPixelPitch;
	_variant_t * m_pDistance;
	_variant_t * m_pAOSACA_IP;
	_variant_t * m_pICANDI_IP;
	_variant_t * m_pRemote;
	_variant_t * m_pFlipVertical;

protected:
	CFont m_fntPropList;
	MyCMFCPropertyGridCtrl m_wndPropList;

	void InitPropList();

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
DECLARE_MESSAGE_MAP()


public:
	CString getRemoteCapability();
	CString getAOSACA_IP();
	CString getICANDI_IP();

};
#pragma once
//#include "afxpropertygridctrl.h"


// Properties dialog

class Properties : public CDialogEx
{
	DECLARE_DYNAMIC(Properties)

	class MyCMFCPropertyGridCtrl : public CMFCPropertyGridCtrl{
	public:
		void setLabelWidth(int width) {
			m_nLeftColumnWidth = width;
			AdjustLayout();
		}
	};

public:
	Properties();   // standard constructor
	virtual ~Properties();

	MyCMFCPropertyGridCtrl			GridCtrl;
	CMFCPropertyGridProperty*		Raster;
	CMFCPropertyGridProperty*		RasterSize;
	CMFCPropertyGridProperty*		FixationTargetSize;
	CMFCPropertyGridColorProperty*	Color;
	CMFCPropertyGridProperty*		ICANDI;
	CMFCPropertyGridProperty*		FixationTarget;
	CMFCPropertyGridFileProperty	VideoFolder;
	CMFCPropertyGridFileProperty	FixationFile;

	_variant_t*						m_pRasterSize;
	_variant_t*						m_pFixationTargetSize;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTIES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

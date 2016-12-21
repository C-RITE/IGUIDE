#pragma once
#include "afxpropertygridctrl.h"


// Properties dialog

class Properties : public CDialogEx
{
	DECLARE_DYNAMIC(Properties)

public:
	Properties();   // standard constructor
	virtual ~Properties();
	CMFCPropertyGridCtrl			GridCtrl;
	CMFCPropertyGridProperty*		Raster;
	CMFCPropertyGridProperty*		Size;
	CMFCPropertyGridColorProperty*	Color;

	_variant_t*					m_RasterSize;

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

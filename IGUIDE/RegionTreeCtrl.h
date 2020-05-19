#pragma once

// RegionTreeCtrl

class RegionTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(RegionTreeCtrl)

public:
	RegionTreeCtrl();
	virtual ~RegionTreeCtrl();

protected:
	COLORREF crText, crBkgrnd;
	HBRUSH bkGnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCstDrw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

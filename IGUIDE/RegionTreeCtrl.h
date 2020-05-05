#pragma once

// RegionTreeCtrl

class RegionTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(RegionTreeCtrl)

public:
	RegionTreeCtrl();
	virtual ~RegionTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
};



#pragma once

// AreaTreeCtrl

class AreaTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(AreaTreeCtrl)

public:
	AreaTreeCtrl();
	virtual ~AreaTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
};



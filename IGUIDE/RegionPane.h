#pragma once
#include "Patches.h"
#include <vector>
#include "RegionTreeCtrl.h"

class RegionPane : public CDockablePane
{

	DECLARE_DYNAMIC(RegionPane)

public:
	RegionPane();
	virtual ~RegionPane();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:

	RegionTreeCtrl			m_wndTree;
	std::vector<HTREEITEM>	areaNodes;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void init();
	void update(Patches* p);
	void add(Patch* p);

};



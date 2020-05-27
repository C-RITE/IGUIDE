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
	int getRegionSize() { return regionNodes.size(); };
	int patchindex;											// where to put patch during patchjob

private:

	RegionTreeCtrl			m_wndTree;
	std::vector<HTREEITEM>	regionNodes;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void addRegion(int regCount);
	void addPatch(Patch* p);

};



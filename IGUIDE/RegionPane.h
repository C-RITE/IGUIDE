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

	int					getRegionSize()							{ return regionNodes.size(); };
	std::vector<int>	getPatchOffset()						{ return patchOffset; }
	void				setPatchOffset(std::vector<int> offset)	{ patchOffset = offset; };

	int patchItem;												// where to put patch during patchjob
																// of one patch in a particular region

private:

	RegionTreeCtrl			m_wndTree;
	std::vector<HTREEITEM>	regionNodes;
	std::vector<int>		patchOffset;							// item offset resulting in multiple commits

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void addRegion(int regCount);
	void addPatch(Patch* p);
	void remove(int region);
	void select(int region, int index);
	void clear();

};



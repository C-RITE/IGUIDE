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

private:

	struct mapIndex {
		HTREEITEM h;
		int index;
	};

	RegionTreeCtrl			m_wndTree;
	std::vector<HTREEITEM>	regionNodes;
	std::vector<mapIndex>	indexmap;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void addRegion(int regCount);
	void addPatch(Patch* p);
	void update(Patch* p);
	void remove(int region);
	void select(int index);
	void browse(Element e);
	void clear();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

};



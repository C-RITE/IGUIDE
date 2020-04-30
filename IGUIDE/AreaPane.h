#pragma once
#include "Patches.h"
#include <vector>
#include "AreaTreeCtrl.h"

class AreaPane : public CDockablePane
{

	DECLARE_DYNAMIC(AreaPane)

public:
	AreaPane();
	virtual ~AreaPane();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:

	AreaTreeCtrl			m_wndTree;
	std::vector<HTREEITEM>	areaNodes;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void init();
	void update(Patches* p);
	void add(Patch* p);

};



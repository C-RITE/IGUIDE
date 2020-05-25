// Area.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "RegionPane.h"
#include "resource.h"


// Area Pane

IMPLEMENT_DYNAMIC(RegionPane, CDockablePane)

RegionPane::RegionPane()
{
}

RegionPane::~RegionPane()
{
}


BEGIN_MESSAGE_MAP(RegionPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Area message handlers


int RegionPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    DWORD style = TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT |
        WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
    CRect dump(0, 0, 0, 0);
    if (!m_wndTree.Create(style, dump, this, ID_REGION_TREE))
        return -1;

    return 0;

}


void RegionPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_wndTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

}

void RegionPane::add(Patch* p) {
    
    CString patchname;
    patchname.Format(L"P%d: %.2f, %.2f, %s [v%.3d]", p->index, p->coordsDEG.x, p->coordsDEG.y, p->defocus, p->index);
    int region = p->region;

    if (region == 0) {
        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = (LPTSTR)(LPCTSTR)patchname;
        m_wndTree.InsertItem(&tvInsert);
    }
    
    else {
        m_wndTree.InsertItem(patchname, regionNodes[region - 1], TVI_LAST);
        m_wndTree.Expand(regionNodes[region - 1], TVE_EXPAND);
    }

    m_wndTree.UpdateData(TRUE);

}

void RegionPane::update(int regCount)
{
    // TODO: Add your implementation code here.
    
    TVINSERTSTRUCT tvInsert;    

    // grow node tree with patchlist information
    CString strRegion;

    strRegion.Format(L"Region: %d", regCount);
    tvInsert.hParent = NULL;
    tvInsert.hInsertAfter = NULL;
    tvInsert.item.mask = TVIF_TEXT;
    tvInsert.item.pszText = (LPTSTR)(LPCTSTR)strRegion;

    HTREEITEM regionNode = m_wndTree.InsertItem(&tvInsert);
    regionNodes.push_back(regionNode);

    /*
    for (auto it = p->begin(); it != p->end(); it++) {  
 

            
    }

    // populate nodes
    CString patchname;

    for (auto it = p->begin(); it != p->end(); it++) {
        patchname.Format(L"Patch: %d", it->index);
        m_wndTree.InsertItem(patchname, regionNodes[it->region], TVI_LAST);

    }
    */
}

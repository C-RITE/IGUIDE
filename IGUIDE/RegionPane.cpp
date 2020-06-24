// Area.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "RegionPane.h"
#include "resource.h"


// Area Pane

IMPLEMENT_DYNAMIC(RegionPane, CDockablePane)

RegionPane::RegionPane(): patchItem(-1)
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

    m_wndTree.SetTextColor(RGB(255, 255, 255));

    return 0;

}


void RegionPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_wndTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

}

void RegionPane::clear() {

    m_wndTree.DeleteAllItems();
    patchOffset.clear();
    regionNodes.clear();
    patchItem = -1;

}

void RegionPane::addPatch(Patch* p) {
    
    CString patchname;

    // add upcoming patchjob

    if (p->index == -1) {
        patchname.Format(L"P?: %.1f, %.1f, %ws [v?]", p->coordsDEG.x, p->coordsDEG.y, p->defocus);
    }

    // add single patch
    else
        patchname.Format(L"P%d: %.1f, %.1f, %ws [v%.3d]", p->index, p->coordsDEG.x, p->coordsDEG.y, p->defocus, p->index);
    
    int region = p->region;

    // if single patch, insert into root of tree
    TVINSERTSTRUCT tvInsert;

    if (region == 0) {
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = (LPTSTR)(LPCTSTR)patchname;
        HTREEITEM insert = m_wndTree.InsertItem(&tvInsert);
        m_wndTree.SetItemColor(insert, RGB(255, 255, 255));
     }
    
    // else insert into corresponding branch of tree
    else if (patchItem >= 0){
        HTREEITEM regNode = regionNodes[region - 1];
        HTREEITEM elem = m_wndTree.GetChildItem(regNode);
        for (int i = 0; i < (patchItem + patchOffset[region - 1]); i++)
            elem = m_wndTree.GetNextSiblingItem(elem);

        // if the element was already comitted (i.e. green), append
        HTREEITEM insert;

        if (m_wndTree.GetItemColor(elem) == RGB(255, 0, 0)) {
            insert = m_wndTree.InsertItem(patchname, regNode, elem);
            m_wndTree.DeleteItem(elem);
        }
        else {
            insert = m_wndTree.InsertItem(patchname, regNode, elem);
            patchOffset[region - 1]++;
        }

        m_wndTree.SetItemColor(insert, RGB(0, 200, 0));

    }

    else {
        HTREEITEM insert = m_wndTree.InsertItem(patchname, regionNodes[region - 1], TVI_LAST);
        m_wndTree.SetItemColor(insert, RGB(255, 0, 0));
        m_wndTree.Expand(regionNodes[region - 1], TVE_EXPAND);
    }

    patchItem = -1;

    m_wndTree.UpdateData(TRUE);

}

void RegionPane::remove(int region)
{
    if (patchItem == -1)
        return;
    
    HTREEITEM regNode = regionNodes[region - 1];
    HTREEITEM hItemChild = m_wndTree.GetChildItem(regNode);

    // save all commited (i.e. green) patches in root before erasing region
    while (hItemChild != NULL)
    {
        HTREEITEM hItemNextChild = m_wndTree.GetNextSiblingItem(hItemChild);
        CString patchname = m_wndTree.GetItemText(hItemChild);
        COLORREF col = m_wndTree.GetItemColor(hItemChild);
        COLORREF green = RGB(0, 200, 0);

        if (col == green) {
            m_wndTree.InsertItem(patchname, TVI_ROOT, TVI_LAST);
            m_wndTree.SetItemColor(hItemChild, RGB(255, 255, 255));
        }

        hItemChild = hItemNextChild;
    }

    // erase and rewind
    regionNodes.pop_back();
    m_wndTree.DeleteItem(regNode);
    patchItem = -1;

}

void RegionPane::finish(int region)
{
    HTREEITEM regNode = regionNodes[region - 1];
    HTREEITEM child = m_wndTree.GetChildItem(regNode);

    while (child != NULL)
    {
        m_wndTree.SetItemColor(child, RGB(255, 255, 255));
        child = m_wndTree.GetNextSiblingItem(child);
    }

    patchItem = -1;
    patchOffset[region - 1] = 0;

}

void RegionPane::addRegion(int regCount)
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
    m_wndTree.SetItemColor(regionNode, RGB(255, 255, 255));
    regionNodes.push_back(regionNode);

    patchOffset.push_back(0);

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
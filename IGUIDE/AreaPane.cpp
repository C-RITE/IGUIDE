// Area.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "AreaPane.h"
#include "resource.h"


// Area Pane

IMPLEMENT_DYNAMIC(AreaPane, CDockablePane)

AreaPane::AreaPane()
{
}

AreaPane::~AreaPane()
{
}


BEGIN_MESSAGE_MAP(AreaPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Area message handlers


int AreaPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    DWORD style = TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT |
        WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
    CRect dump(0, 0, 0, 0);
    if (!m_wndTree.Create(style, dump, this, ID_AREA_TREE))
        return -1;
    
    return 0;

}


void AreaPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_wndTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

}

void AreaPane::add(Patch* p) {
    
    CString patchname;
    patchname.Format(L"Patch: %d", p->index);
    int area = p->area;
    m_wndTree.InsertItem(patchname, areaNodes[area-1], TVI_LAST);
    m_wndTree.Expand(areaNodes[area - 1], TVE_EXPAND);
    m_wndTree.UpdateData(TRUE);

}

void AreaPane::init() {

    CString strArea;
    TVINSERTSTRUCT tvInsert;
    strArea.Format(L"Area: 1");
    tvInsert.hParent = NULL;
    tvInsert.hInsertAfter = NULL;
    tvInsert.item.mask = TVIF_TEXT;
    tvInsert.item.pszText = (LPTSTR)(LPCTSTR)strArea;
    HTREEITEM areaNode = m_wndTree.InsertItem(&tvInsert);
    areaNodes.push_back(areaNode);

}


void AreaPane::update(Patches* p)
{
    // TODO: Add your implementation code here.
    m_wndTree.DeleteAllItems();

    TVINSERTSTRUCT tvInsert;    

    // grow node tree with patchlist information
    CString strArea;
    int area = 0;
    
    for (auto it = p->begin(); it != p->end(); it++) {  
 
        strArea.Format(L"Area: %d", it->area);
        tvInsert.hParent = NULL;
        tvInsert.hInsertAfter = NULL;
        tvInsert.item.mask = TVIF_TEXT;
        tvInsert.item.pszText = (LPTSTR)(LPCTSTR)strArea;
        if (it->area != area) {
            
            area++;
        }
            
    }

    // populate nodes
    CString patchname;

    for (auto it = p->begin(); it != p->end(); it++) {
        patchname.Format(L"Patch: %d", it->index);
        area = it->area;
        m_wndTree.InsertItem(patchname, areaNodes[area-1], TVI_LAST);

    }

}

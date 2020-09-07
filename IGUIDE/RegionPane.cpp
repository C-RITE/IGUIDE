// Area.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "IGUIDEView.h"
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
        WS_CHILD | WS_VISIBLE | TVS_FULLROWSELECT;
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
    regionNodes.clear();

}

void RegionPane::addPatch(Patch* p) {
    
    CString patchname;
	HTREEITEM insert;

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
        insert = m_wndTree.InsertItem(&tvInsert);
        m_wndTree.SetItemColor(insert, RGB(255, 255, 255));
		indexmap.push_back(mapIndex{ insert, p->index });
     }

	else {

		// else insert into subtree

		HTREEITEM regNode = regionNodes[region - 1];
		HTREEITEM elem = m_wndTree.GetChildItem(regNode);
		//for (int i = 0; i < (patchOffset[region - 1]); i++)
		//	elem = m_wndTree.GetNextSiblingItem(elem);

		insert = m_wndTree.InsertItem(patchname, regionNodes[region - 1], TVI_LAST);
		m_wndTree.SetItemColor(insert, RGB(255, 0, 0));
		m_wndTree.Expand(regionNodes[region - 1], TVE_EXPAND);
		indexmap.push_back(mapIndex{ insert, p->index });

		// also select first element in subtree
		if (insert == m_wndTree.GetChildItem(regNode)) {
			m_wndTree.selected = insert;
			m_wndTree.SelectItem(insert);
		}

	}

    m_wndTree.UpdateData(TRUE);

}

void RegionPane::update(Patch* p) {
	
	CString patchname;
	HTREEITEM insert;
	
	patchname.Format(L"P%d: %.1f, %.1f, %ws [v%.3d]", p->index, p->coordsDEG.x, p->coordsDEG.y, p->defocus, p->index);
	
	// if the element wasn't yet comitted, update info and paint it green

	if (m_wndTree.GetItemColor(m_wndTree.selected) == RGB(255, 0, 0)) {
		insert = m_wndTree.InsertItem(patchname, m_wndTree.GetParentItem(m_wndTree.selected), m_wndTree.selected);
		m_wndTree.DeleteItem(m_wndTree.selected);
		m_wndTree.SetItemColor(insert, RGB(0, 200, 0));
		m_wndTree.selected = insert;
	}

	// if the element was already comitted (i.e. green), append

}

void RegionPane::select(int index) {

	for (auto it = indexmap.begin(); it != indexmap.end(); it++)
		if (it->index == index)
			m_wndTree.selected = it->h;

	m_wndTree.SelectItem(m_wndTree.selected);

}

void RegionPane::browse(Element e) {

	HTREEITEM temp = m_wndTree.selected;
		
	switch (e) {

		case NEXT:
			m_wndTree.selected = m_wndTree.GetNextSiblingItem(m_wndTree.selected);
			break;

		case PREV:
			m_wndTree.selected = m_wndTree.GetPrevSiblingItem(m_wndTree.selected);
			break;

	}

	if (m_wndTree.selected != NULL)
		m_wndTree.SelectItem(m_wndTree.selected);
	else
		m_wndTree.selected = temp;

}

void RegionPane::remove(int region)
{
    
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

}


BOOL RegionPane::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN) {

		CIGUIDEView* pView = CIGUIDEView::GetView();

		switch (pMsg->wParam) {

		case VK_SPACE:
			pView->PostMessage(WM_KEYDOWN, VK_SPACE, NULL);
			break;

		case 'N':
			pView->PostMessage(WM_KEYDOWN, 'N', NULL);
			PostMessage(WM_KEYDOWN, VK_DOWN, NULL);
			break;

		case 'B':
			pView->PostMessage(WM_KEYDOWN, 'B', NULL);
			PostMessage(WM_KEYDOWN, VK_UP, NULL);
			break;
		}

	}

	return CDockablePane::PreTranslateMessage(pMsg);

}

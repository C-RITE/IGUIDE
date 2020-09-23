// RegionTreeCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "RegionTreeCtrl.h"
#include "PatchInfo.h"
#include "MainFrm.h"


// RegionTreeCtrl

IMPLEMENT_DYNAMIC(RegionTreeCtrl, CTreeCtrl)

RegionTreeCtrl::RegionTreeCtrl()
{
    crTextWhite = RGB(255, 255, 255);
    crBkgrnd = RGB(50, 50, 50);
    brBkGnd = CreateSolidBrush(RGB(50, 50, 50));
	click = false;
}

RegionTreeCtrl::~RegionTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(RegionTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &RegionTreeCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &RegionTreeCtrl::OnNMCstDrw)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_PAINT()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, &RegionTreeCtrl::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, &RegionTreeCtrl::OnNMClick)
END_MESSAGE_MAP()

void RegionTreeCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

    if (ItemHasChildren(selected))
        return;

	CString WindowText;
	CString text = GetItemText(selected);

	PatchInfo patchinfo;
	Patch p = *pDoc->m_pGrid->currentPatch;

	if (p.index == -1)
		return;

	patchinfo.directory = *pDoc->m_pCurrentOutputDir;
    patchinfo.filename = p.vidfilename;
    patchinfo.defocus = p.defocus;
    patchinfo.x = p.coordsDEG.x;
    patchinfo.y = p.coordsDEG.y;
    patchinfo.timestamp = p.timestamp;
	patchinfo.wavelength = p.wavelength;
	patchinfo.videolength = p.vidlength;
	patchinfo.subject = pDoc->prefix;
	patchinfo.system = pDoc->system;
	
	WindowText.Format(L"Details about Patch %d", p.index);
	patchinfo.windowTitle = WindowText;
	patchinfo.DoModal();

}

void RegionTreeCtrl::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{

	if (!click)
		return;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	selected = GetSelectedItem();
	if (ItemHasChildren(selected))
		return;

	UINT uID = -1;

	for (auto it = indexTable.begin(); it != indexTable.end(); it++) {
		if (selected == it->h)
			uID = it->uID;
	}
		
	ATLTRACE(_T("uid: [%d] found for treeitem: [%d]\n"), uID, selected);

	AfxGetMainWnd()->SendMessage(PATCH_SELECT, (WPARAM)uID);

	click = false;

}

void RegionTreeCtrl::select(UINT uID) {

	for (auto it = indexTable.begin(); it != indexTable.end(); it++) {
		if (uID == it->uID)
			selected = it->h;
	}

}

void RegionTreeCtrl::OnNMCstDrw(NMHDR* pNMHDR, LRESULT* pResult)
{

    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    // Take the default processing unless we 
    // set this to something else below.
    *pResult = 0;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.

    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        // This is the prepaint stage for an item. Here's where we set the
        // item's text color. Our return value will tell Windows to draw the
        // item itself, but it will use the new color we set here.

        // Store the color back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crTextWhite;
        pLVCD->clrTextBk = crBkgrnd;

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
    }
}

BOOL RegionTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    CRect client;
    HBRUSH brush = CreateSolidBrush(RGB(50, 50, 50));
    GetClientRect(&client);
    FillRect(*pDC, client, brush);

    return TRUE;
}

HBRUSH RegionTreeCtrl::CtlColor(CDC* pDC, UINT uCtlColor)
{
    HBRUSH brush = NULL;

    switch (uCtlColor) {
        case COLOR_BACKGROUND:
            brush = brBkGnd;
            break;
    }

  return brush;

}

void RegionTreeCtrl::SetItemFont(HTREEITEM hItem, LOGFONT& logfont)
{
    Color_Font cf;
    if (!m_mapColorFont.Lookup(hItem, cf))
        cf.color = (COLORREF)-1;
    cf.logfont = logfont;
    m_mapColorFont[hItem] = cf;
}

void RegionTreeCtrl::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
    SetItemState(hItem, bBold ? TVIS_BOLD : 0, TVIS_BOLD);
}

void RegionTreeCtrl::SetItemColor(HTREEITEM hItem, COLORREF color)
{
    Color_Font cf;
    if (!m_mapColorFont.Lookup(hItem, cf))
        cf.logfont.lfFaceName[0] = '\0';
    cf.color = color;
    m_mapColorFont[hItem] = cf;
}

BOOL RegionTreeCtrl::GetItemFont(HTREEITEM hItem, LOGFONT* plogfont)
{
    Color_Font cf;
    if (!m_mapColorFont.Lookup(hItem, cf))
        return FALSE;
    if (cf.logfont.lfFaceName[0] == '\0')
        return FALSE;
    *plogfont = cf.logfont;
    return TRUE;

}

BOOL RegionTreeCtrl::GetItemBold(HTREEITEM hItem)
{
    return GetItemState(hItem, TVIS_BOLD) & TVIS_BOLD;
}

COLORREF RegionTreeCtrl::GetItemColor(HTREEITEM hItem)
{
    // Returns (COLORREF)-1 if color was not set
    Color_Font cf;
    if (!m_mapColorFont.Lookup(hItem, cf))
        return (COLORREF)-1;
    return cf.color;

}

void RegionTreeCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CTreeCtrl::OnPaint() for painting messages

    // Create a memory DC compatible with the paint DC
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CRect rcClip, rcClient;
    dc.GetClipBox(&rcClip);
    GetClientRect(&rcClient);

    // Select a compatible bitmap into the memory DC
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    memDC.SelectObject(&bitmap);

    // Set clip region to be same as that in paint DC
    CRgn rgn;
    rgn.CreateRectRgnIndirect(&rcClip);
    memDC.SelectClipRgn(&rgn);
    rgn.DeleteObject();



    // First let the control do its default drawing.
    CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);


    HTREEITEM hItem = GetFirstVisibleItem();

    int n = GetVisibleCount() + 1;
    while (hItem && n--)
    {
        CRect rect;

        // Do not meddle with selected items or drop highlighted items
        UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
        Color_Font cf;

        if (!(GetItemState(hItem, selflag) & selflag)
            && m_mapColorFont.Lookup(hItem, cf))
        {
            CFont* pFontDC;
            CFont fontDC;
            LOGFONT logfont;

            if (cf.logfont.lfFaceName[0] != '\0')
            {
                logfont = cf.logfont;
            }
            else
            {
                // No font specified, so use window font
                CFont* pFont = GetFont();
                pFont->GetLogFont(&logfont);
            }

            if (GetItemBold(hItem))
                logfont.lfWeight = 700;
            fontDC.CreateFontIndirect(&logfont);
            pFontDC = memDC.SelectObject(&fontDC);

            if (cf.color != (COLORREF)-1)
                memDC.SetTextColor(cf.color);

            CString sItem = GetItemText(hItem);

            GetItemRect(hItem, &rect, TRUE);
            //memDC.SetBkColor(GetSysColor(COLOR_WINDOW));
            memDC.SetBkColor(RGB(50,50,50));
            memDC.TextOut(rect.left + 2, rect.top + 1, sItem);

            memDC.SelectObject(pFontDC);
        }
        hItem = GetNextVisibleItem(hItem);
    }


    dc.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC,
        rcClip.left, rcClip.top, SRCCOPY);

}


void RegionTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	click = TRUE;
}

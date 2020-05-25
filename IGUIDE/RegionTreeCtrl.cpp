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
    crText = RGB(255, 255, 255);
    crBkgrnd = RGB(50, 50, 50);
    bkGnd = CreateSolidBrush(RGB(50, 50, 50));
}

RegionTreeCtrl::~RegionTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(RegionTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &RegionTreeCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &RegionTreeCtrl::OnNMCstDrw)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


void RegionTreeCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

	HTREEITEM selected = GetSelectedItem();
	CString text = GetItemText(selected);
	int index = 1;
	PatchInfo patchinfo;
	Patch p = pDoc->m_pGrid->getPatch(index);
	patchinfo.directory = *pDoc->m_pCurrentOutputDir;
    patchinfo.filename = p.vidfilename;
    patchinfo.defocus = p.defocus;
    patchinfo.x = p.coordsDEG.x;
    patchinfo.y = p.coordsDEG.y;
    patchinfo.timestamp = p.timestamp;
	patchinfo.DoModal();

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
        pLVCD->clrText = crText;
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

  pDC->SetBkColor(RGB(50, 50, 50));
  return bkGnd;

}

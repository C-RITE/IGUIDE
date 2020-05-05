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

}

RegionTreeCtrl::~RegionTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(RegionTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &RegionTreeCtrl::OnNMDblclk)
END_MESSAGE_MAP()



// RegionTreeCtrl message handlers




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
	patchinfo.DoModal();

}

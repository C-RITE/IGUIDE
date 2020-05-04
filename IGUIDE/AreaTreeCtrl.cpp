// AreaTreeCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "IGUIDE.h"
#include "AreaTreeCtrl.h"
#include "PatchInfo.h"
#include "MainFrm.h"


// AreaTreeCtrl

IMPLEMENT_DYNAMIC(AreaTreeCtrl, CTreeCtrl)

AreaTreeCtrl::AreaTreeCtrl()
{

}

AreaTreeCtrl::~AreaTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(AreaTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &AreaTreeCtrl::OnNMDblclk)
END_MESSAGE_MAP()



// AreaTreeCtrl message handlers




void AreaTreeCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
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

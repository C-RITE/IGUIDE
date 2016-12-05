// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "Properties.h"
#include "GridTargetsDoc.h"
#include "afxdialogex.h"

// Properties dialog

IMPLEMENT_DYNAMIC(Properties, CDialogEx)

Properties::Properties(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROPERTIES, pParent)
{
}

Properties::~Properties()
{
}

void Properties::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_ctrlGrid);
}

BEGIN_MESSAGE_MAP(Properties, CDialogEx)
END_MESSAGE_MAP()

// Properties message handlers


BOOL Properties::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CGridTargetsDoc* pDoc = CGridTargetsDoc::GetDoc();

	HSECTION hs = m_ctrlGrid.AddSection(L"Raster");
	m_ctrlGrid.AddDoubleItem(hs, L"Field Size", pDoc->raster.size, L"%g deg");
	m_ctrlGrid.AddIntegerItem(hs, L"Formatted Integer", 8, L"%d inches");
	m_ctrlGrid.AddColorItem(hs, L"Color", RGB(255, 156, 12));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
// PatchInfo.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "PatchInfo.h"
#include "afxdialogex.h"


// PatchInfo dialog

IMPLEMENT_DYNAMIC(PatchInfo, CDialogEx)

PatchInfo::PatchInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PATCHINFO, pParent)
{

}

PatchInfo::~PatchInfo()
{
}

void PatchInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILENAME_EDIT, filename);
	DDX_Text(pDX, IDC_DIRECTORY_EDIT, directory);
	DDX_Text(pDX, IDC_DEFOCUS_EDIT, defocus);
	DDX_Text(pDX, IDC_TIMESTAMP_EDIT, timestamp);
	DDX_Text(pDX, IDC_VIDLENGTH_EDIT, videolength);
	DDX_Text(pDX, IDC_WAVELENGTH_EDIT, wavelength);
	DDX_Text(pDX, IDC_SYSTEM_EDIT, system);
	DDX_Text(pDX, IDC_SUBJECT_EDIT, subject);
	DDX_Text(pDX, IDC_X_COORDINATE_EDIT, x);
	DDX_Text(pDX, IDC_Y_COORDINATE_EDIT, y);
}


BEGIN_MESSAGE_MAP(PatchInfo, CDialogEx)
END_MESSAGE_MAP()


BOOL PatchInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowTextW(windowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

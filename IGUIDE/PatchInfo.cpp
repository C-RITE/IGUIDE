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
	, filename(_T(""))
	, directory(_T(""))
	, defocus(_T(""))
	, timestamp(_T(""))
	, vidlength(0)
	, x(0)
	, y(0)
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
	DDX_Text(pDX, IDC_VIDLENGTH_EDIT, vidlength);
	DDX_Text(pDX, IDC_X_COORDINATE_EDIT, x);
	DDX_Text(pDX, IDC_Y_COORDINATE_EDIT, y);
}


BEGIN_MESSAGE_MAP(PatchInfo, CDialogEx)
END_MESSAGE_MAP()


// PatchInfo message handlers

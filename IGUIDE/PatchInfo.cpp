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
{

}

PatchInfo::~PatchInfo()
{
}

void PatchInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILENAME_EDIT, filename);
	DDV_MaxChars(pDX, filename, 255);
	DDX_Text(pDX, IDC_DIRECTORY_EDIT, directory);
	DDV_MaxChars(pDX, directory, 255);
}


BEGIN_MESSAGE_MAP(PatchInfo, CDialogEx)
END_MESSAGE_MAP()


// PatchInfo message handlers

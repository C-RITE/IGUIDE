// Target.cpp : implementation file
//

#include "stdafx.h"
#include "GridTargets.h"
#include "Target.h"
#include "afxdialogex.h"


// Target dialog

IMPLEMENT_DYNAMIC(Target, CDialog)

Target::Target(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TARGET, pParent)
{
	// Initialize Direct2D
	EnableD2DSupport();
}

Target::~Target()
{
}

void Target::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Target, CDialog)
END_MESSAGE_MAP()


// Target message handlers

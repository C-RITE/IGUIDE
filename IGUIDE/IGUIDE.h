
// IGUIDE.h : main header file for the IGUIDE application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CIGUIDEApp:
// See IGUIDE.cpp for the implementation of this class
//

class CIGUIDEApp : public CWinAppEx
{
public:
	CIGUIDEApp();
	static CStringW GetFileVersion();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL StoreWindowPlacement(
		const CRect& rectNormalPosition,
		int nFlags,
		int nShowCmd);

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIGUIDEApp theApp;

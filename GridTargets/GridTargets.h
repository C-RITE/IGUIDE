
// GridTargets.h : main header file for the GridTargets application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGridTargetsApp:
// See GridTargets.cpp for the implementation of this class
//

class CGridTargetsApp : public CWinApp
{
public:
	CGridTargetsApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGridTargetsApp theApp;

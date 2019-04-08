
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define DOC_IS_READY (WM_USER + 100)
#define SCREEN_SELECTED (WM_USER + 101)

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afx.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <afxdlgs.h>

// stuff for Application interoperability
#include <Winsock2.h>
#include "../NetCom/WinSock2Async.h"

//#ifdef _DEBUG
//#ifdef ATLTRACE 
//#undef ATLTRACE
//#undef ATLTRACE2
//
//#define ATLTRACE CustomTrace
//#define ATLTRACE2 ATLTRACE
//#endif // ATLTRACE
//#endif // _DEBUG
//
//void CustomTrace(const wchar_t* format, ...);
//void CustomTrace(int dwCategory, int line, const wchar_t* format, ...);

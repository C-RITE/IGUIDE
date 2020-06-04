
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define DOC_IS_READY (WM_USER + 100)
#define SCREEN_SELECTED (WM_USER + 101)
#define NETCOM_ERROR (WM_USER + 102)
#define NETCOM_CLOSED (WM_USER + 103)
#define GAMEPAD_UPDATE (WM_USER + 105)
#define MOUSE_FALLBACK (WM_USER + 106)
#define RESET_AOSACA_IP (WM_USER + 107)
#define RESET_ICANDI_IP (WM_USER + 108)
#define SAVE_IGUIDE_CSV (WM_USER + 109)
#define	PATCH_TO_REGIONPANE (WM_USER + 110)
#define UPDATE_REGIONPANE (WM_USER + 111)
#define FINISH_PATCHJOB (WM_USER + 112)
#define PATCH_SELECT (WM_USER + 113)

#define NETMSG_RESPONSE_TIMEOUT 200
#define SNAP_INTERVAL	0.1F							// Snap mouse cursor to grid in degree

#define CANVAS			360								// Visual Area
#define CENTER			CANVAS / 2						// Visual Area Center
#define DPP				0.1F							// 10 pixels = 1 degree
#define PPD				1 / DPP							// reciprocal of DPP
#define LANES			(unsigned int)(CANVAS * DPP)	// helper for grid drawing
#define LANE			CANVAS / LANES					// helper for snap to grid

#define MIN_ZOOM		0.01F //not yet integrated
#define MAX_ZOOM		50F

// definition source: https://de.wikipedia.org/wiki/Fovea_centralis#/media/Datei:Macula.svg
#define _DELTA_D		15				// Assumed distance between foveal center and optic disc
#define _DELTA_DY		2.1				// Assumed Y-shift of optic disc
#define _OPTIC_DISC		2.77			// Assumed radius of optic disc
#define _FOVEOLA		1.1				// Assumed diameter of foveola
#define _FOVEA			1.4				// Assumed diameter of fovea
#define _PARAFOVEA		5				// Assumed diameter of parafovea
#define _PERIFOVEA		8.2				// Assumed diameter of perifovea
#define _MACULA			18.2			// Assumed diameter of macula

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

// stuff for Application interoperability (AOSACA / ICANDI)
#include <Winsock2.h>
#include "../NetCom/WinSock2Async.h"
#include <afxwin.h>

#ifdef _DEBUG
#ifdef ATLTRACE 
#undef ATLTRACE
#undef ATLTRACE2

#define ATLTRACE CustomTrace
#define ATLTRACE2 ATLTRACE
#endif // ATLTRACE
#endif // _DEBUG

void CustomTrace(const wchar_t* format, ...);
void CustomTrace(int dwCategory, int line, const wchar_t* format, ...);

#define GREEN	D2D1::ColorF::DarkSeaGreen
#define BLUE	D2D1::ColorF::Aqua
#define YELLOW	D2D1::ColorF::PaleGoldenrod
#define BLACK	D2D1::ColorF::Black
#define RED		D2D1::ColorF::Red
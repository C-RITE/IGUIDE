// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "Properties.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "afxdialogex.h"
#include <Shlobj.h>


Properties::Properties()
{

	m_pRasterSize = new _variant_t;
	m_pFixationTarget = new _variant_t;
	m_pScreen = new _variant_t;
	m_pPixelPitch = new _variant_t;
	m_pDistance = new _variant_t;
	m_pAOSACAIP = new _variant_t;
	m_pFlipVertical = new _variant_t;
}

Properties::~Properties()
{

	delete m_pFixationTarget;
	delete m_pRasterSize;
	delete m_pScreen;
	delete m_pPixelPitch;
	delete m_pDistance;
	delete m_pAOSACAIP;
	delete m_pFlipVertical;

}


BEGIN_MESSAGE_MAP(Properties, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

// Properties message handler

LRESULT Properties::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CMFCPropertyGridCtrl* gridctrl = (CMFCPropertyGridCtrl*)wParam;
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty*)lParam;
	_variant_t vt(prop->GetValue());
	CString propName = prop->GetName();
		if (propName == L"Size") {
			if (prop->GetParent() == Raster)
				pDoc->raster.size = vt;
			if (prop->GetParent() == TargetView)
				pDoc->m_FixationTargetSize = vt;
		}
		if (propName == L"Color") {
			COLORREF ref = vt;
			pDoc->raster.color = D2D1_COLOR_F(m_pRenderTarget->COLORREF_TO_D2DCOLOR(ref));
		}
		if (propName == L"File") {
			CString ftfile = vt.bstrVal;
			pDoc->m_FixationTarget = ftfile;
		}
		if (propName == L"Video Folder") {
			CString folder = vt.bstrVal;
			pDoc->m_OutputDir = folder;
		}
		if (propName == L"Screen") {
			CString ref = vt.bstrVal;
			int index = ref.ReverseFind(_T(' '));
			ref.Truncate(index);
			for (auto it = pDoc->m_Screens.begin(); it != pDoc->m_Screens.end(); it++) {
				if (it->name == ref) {
					pDoc->m_selectedScreen = it._Ptr;
					CIGUIDEView* pView = CIGUIDEView::GetView();
					pView->SendMessage(SCREEN_SELECTED);
				}
			}
		}
		if (propName == L"AOSACA IP") {
			CString aoip = vt.bstrVal;
			pDoc->m_AOSACAIP = aoip;
		}
		if (propName == L"Flip Vertical") {
			pDoc->m_FlipVertical = vt;
		}

		pDoc->UpdateAllViews(NULL);

	return 0;
}

void Properties::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void Properties::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient, rectCombo;
	GetClientRect(rectClient);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void Properties::InitPropList()
{
	
	
	m_wndPropList.SetCustomColors(	RGB(50, 50, 50),		// Background
									RGB(255, 255, 255),	// Text
									RGB(50, 50, 50),	// GroupBackground
									RGB(255, 255, 255), // GroupText
									RGB(100, 100, 100),	// DescriptionBackground
									RGB(200, 200, 200),	// DescriptionText
									RGB(150, 150, 150));// Line

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.MarkModifiedProperties();

	VideoFolder = new CMFCPropertyGridFileProperty(L"Video Folder", L"D:\\", NULL, _T("Choose output directory of captured video files"));
	FixationFile = new CMFCPropertyGridFileProperty(L"File", true, NULL, NULL, NULL, NULL, _T("Choose your custom fixation target from file"));
	Raster = new CMFCPropertyGridProperty(L"Raster");
	RasterSize = new CMFCPropertyGridProperty(L"Size", m_pRasterSize, _T("Choose the raster size in degrees"), NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), col, NULL, _T("Choose the desired raster color"));
	Color->EnableOtherButton(L"Other..");
	ICANDI = new CMFCPropertyGridProperty(L"ICANDI");
	TargetView = new CMFCPropertyGridProperty(L"Target View");
	PixelDensity = new CMFCPropertyGridProperty(L"Pixel Pitch", m_pPixelPitch, _T("Distance between pixel centers. Check for specifications in monitor manual!"), NULL, NULL, NULL);
	ScreenDistance = new CMFCPropertyGridProperty(L"Distance", m_pDistance, _T("Distance between subject's pupil and screen surface in mm"), NULL, NULL, NULL);
	FixationTargetScreen = new CMFCPropertyGridProperty(L"Screen", m_pScreen, _T("Pick the display for target screen. If empty, setup and connect another monitor to your computer."), NULL, NULL, NULL);
	FixationTargetSize = new CMFCPropertyGridProperty(L"Size", m_pFixationTarget , _T("Scale the size of the custom fixation target in percent (%)"), NULL, NULL, NULL);
	RigProperties = new CMFCPropertyGridProperty(L"Rig Properties");
	AOSACAIP = new CMFCPropertyGridProperty(L"AOSACA IP", m_pAOSACAIP, _T("IP Address of computer running AOSACA, port 1500"), NULL, NULL, NULL);
	FlipVertical = new CMFCPropertyGridProperty(L"Flip Vertical", m_pFlipVertical, _T("Flips vertical orientation of Target Screen"), NULL, NULL, NULL);

	RECT Rect;
	GetClientRect(&Rect);
	MapWindowPoints(this, &Rect);
	m_wndPropList.AddProperty(Raster);
	Raster->AddSubItem(RasterSize);
	Raster->AddSubItem(Color);
	m_wndPropList.AddProperty(ICANDI);
	ICANDI->AddSubItem(VideoFolder);
	m_wndPropList.AddProperty(TargetView);
	TargetView->AddSubItem(FixationTargetScreen);
	TargetView->AddSubItem(ScreenDistance);
	TargetView->AddSubItem(PixelDensity);
	TargetView->AddSubItem(FixationFile);
	TargetView->AddSubItem(FixationTargetSize);
	m_wndPropList.AddProperty(RigProperties);
	RigProperties->AddSubItem(AOSACAIP);
	RigProperties->AddSubItem(FlipVertical);
	
}

int Properties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();


	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();
	AdjustLayout();

	return 0;

}

void Properties::fillProperties() {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	_variant_t rs(pDoc->raster.size);
	_variant_t fts(pDoc->m_FixationTargetSize);
	_variant_t ft(pDoc->m_FixationTarget);
	_variant_t od(pDoc->m_OutputDir);
	_variant_t scr(pDoc->m_Screens[pDoc->m_FixationTargetScreen - 1].name);
	_variant_t dens(pDoc->m_ScreenPixelPitch);
	_variant_t dist(pDoc->m_ScreenDistance);
	_variant_t aoip(pDoc->m_AOSACAIP);
	_variant_t fv(pDoc->m_FlipVertical);
	
	VideoFolder->SetValue(od);
	RasterSize->SetValue(rs);
	FixationTargetSize->SetValue(fts);
	FixationFile->SetValue(ft);
	FixationTargetScreen->SetValue(scr);
	PixelDensity->SetValue(dens);
	ScreenDistance->SetValue(dist);
	AOSACAIP->SetValue(aoip);
	FlipVertical->SetValue(fv);


	COLORREF col = RGB(
		(int)(pDoc->raster.color.r / (1 / 255.0)),
		(int)(pDoc->raster.color.g / (1 / 255.0)),
		(int)(pDoc->raster.color.b / (1 / 255.0))
	);
	
	Color->SetColor(col);
	 
	CString option;

	FixationTargetScreen->RemoveAllOptions(); // need for removal if fillProperties() is called more than once

	for (auto& it : pDoc->m_Screens) {
		option.Format(L"%s (%ix%i)", it.name, it.resolution.x, it.resolution.y);
		FixationTargetScreen->AddOption(option);
	}

	CRect rc;
	GetWindowRect(&rc);

}

CString Properties::getAOSACAIP() {
	return AOSACAIP->GetValue();
}
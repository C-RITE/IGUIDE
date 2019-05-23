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
		
	// override default font for something bigger
	m_fntPropList.CreateFont(14, 0, 0, 0, FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		0,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH
		FF_DONTCARE,
		L"Arial");

}

Properties::~Properties()
{

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

	if (propName == "RasterSize") {
		pDoc->raster.size = vt;
	}
	if (propName == L"Size") {
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
		pDoc->m_AOSACA_IP = aoip;
	}

	if (propName == L"ICANDI IP") {
		CString icip = vt.bstrVal;
		pDoc->m_ICANDI_IP = icip;
	}

	if (propName == L"Flip Vertical") {
		pDoc->m_FlipVertical = vt;
	}

	if (propName == L"Capability") {
		pDoc->m_RemoteCtrl = vt;
	}

	pDoc->UpdateAllViews(NULL);

	return S_OK;

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
	m_wndPropList.SetFont(&m_fntPropList);

}

void Properties::InitPropList()
{
	
	
	m_wndPropList.SetCustomColors(	RGB(50, 50, 50),	// Background
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
	Patch = new CMFCPropertyGridProperty(L"Patch");
	RasterSize = new CMFCPropertyGridProperty(L"Raster Size", RasterSizeValue, _T("Choose the raster size in degrees"), NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), col, NULL, _T("Choose the desired raster color"));
	Color->EnableOtherButton(L"Other..");
	ICANDI = new CMFCPropertyGridProperty(L"ICANDI");
	TargetView = new CMFCPropertyGridProperty(L"Target View");
	PhysParam = new CMFCPropertyGridProperty(L"Physical Parameters");
	FixationTargetScreen = new CMFCPropertyGridProperty(L"Screen", ScreenValue, _T("Pick the display for fixation target presentation. If empty, setup and connect another monitor to your computer."), NULL, NULL, NULL);
	FixationTargetSize = new CMFCPropertyGridProperty(L"Scale", FixationTargetValue, _T("Scale the size of the custom fixation target in percent (%)"), NULL, NULL, NULL);
	RigProperties = new CMFCPropertyGridProperty(L"Rig Properties");
	RemoteControl = new CMFCPropertyGridProperty(L"Remote Control");
	RemoteCapability = new CMFCPropertyGridProperty(L"Capability", RemoteValue, _T("Enable or Disable Remote Control Function"), NULL, NULL, NULL);
	AOSACA_IP = new CMFCPropertyGridProperty(L"AOSACA IP", AOSACA_IPValue, _T("IP Address of computer running AOSACA, port 1500"), NULL, NULL, NULL);
	ICANDI_IP = new CMFCPropertyGridProperty(L"ICANDI IP", ICANDI_IPValue, _T("IP Address of computer running ICANDI, port 1400"), NULL, NULL, NULL);
	FlipVertical = new CMFCPropertyGridProperty(L"Flip Vertical", FlipVerticalValue, _T("Flips vertical orientation of Target Screen"), NULL, NULL, NULL);

	RECT Rect;
	GetClientRect(&Rect);
	MapWindowPoints(this, &Rect);
	m_wndPropList.AddProperty(PhysParam);
	PhysParam->AddSubItem(RasterSize);
	m_wndPropList.AddProperty(Patch);
	Patch->AddSubItem(Color);
	m_wndPropList.AddProperty(ICANDI);
	ICANDI->AddSubItem(VideoFolder);
	m_wndPropList.AddProperty(TargetView);
	TargetView->AddSubItem(FixationTargetScreen);
	TargetView->AddSubItem(FixationFile);
	TargetView->AddSubItem(FixationTargetSize);
	m_wndPropList.AddProperty(RigProperties);
	RigProperties->AddSubItem(FlipVertical);
	m_wndPropList.AddProperty(RemoteControl);
	RemoteControl->AddSubItem(RemoteCapability);
	RemoteControl->AddSubItem(AOSACA_IP);
	RemoteControl->AddSubItem(ICANDI_IP);
	
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
	_variant_t aoip(pDoc->m_AOSACA_IP);
	_variant_t icip(pDoc->m_ICANDI_IP);
	_variant_t fv(pDoc->m_FlipVertical);
	_variant_t rem(pDoc->m_RemoteCtrl);
	
	VideoFolder->SetValue(od);
	RasterSize->SetValue(rs);
	FixationTargetSize->SetValue(fts);
	FixationFile->SetValue(ft);
	AOSACA_IP->SetValue(aoip);
	ICANDI_IP->SetValue(icip);
	FlipVertical->SetValue(fv);
	RemoteCapability->SetValue(rem);

	COLORREF col = RGB(
		(int)(pDoc->raster.color.r / (1 / 255.0)),
		(int)(pDoc->raster.color.g / (1 / 255.0)),
		(int)(pDoc->raster.color.b / (1 / 255.0))
	);
	
	Color->SetColor(col);
	 
	CString option;

	// fill monitor property
	for (auto& screen : pDoc->m_Screens) {
		// never parse primary monitor, because it is dedicated to operator view
		if (screen.number == 1)
			FixationTargetScreen->SetValue(L"NONE");
		else if (screen.number == pDoc->m_selectedScreen->number) {
			option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
			FixationTargetScreen->SetValue(option);
			CIGUIDEView* pView = CIGUIDEView::GetView();
			pView->SendMessage(SCREEN_SELECTED);
		}
	}
	
	FixationTargetScreen->RemoveAllOptions(); // need for removal if fillProperties() is called more than once

	for (auto& screen : pDoc->m_Screens) {
		// never parse primary monitor, because it is dedicated to operator view
		if (screen.number == 1)	continue;
		// all others: put in list
		option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
		FixationTargetScreen->AddOption(option);
	}
	
	CString capability[4]{ L"NONE", L"AOSACA", L"ICANDI", L"FULL"};
	for (int i = 0; i < 4; i++)
		RemoteCapability->AddOption(capability[i]);

	CRect rc;
	GetWindowRect(&rc);

}

CString Properties::getAOSACA_IP() {
	return AOSACA_IP->GetValue();
}

CString Properties::getICANDI_IP() {
	return ICANDI_IP->GetValue();
}

CString Properties::getRemoteCapability()
{
	// TODO: Add your implementation code here.
	return RemoteCapability->GetValue();

}

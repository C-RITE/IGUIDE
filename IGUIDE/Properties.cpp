// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "Properties.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "afxdialogex.h"
#include "MainFrm.h"
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

IMPLEMENT_DYNAMIC(Properties, CDockablePane)

// Properties message handler
BEGIN_MESSAGE_MAP(Properties, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()


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
	
	m_wndPropList.SetFont(&m_fntPropList);
	InitPropList();
	AdjustLayout();

	return 0;

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

LRESULT Properties::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CIGUIDEView* pView = CIGUIDEView::GetView();

	CMFCPropertyGridCtrl* gridctrl = (CMFCPropertyGridCtrl*)wParam;
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty*)lParam;
	_variant_t vt(prop->GetValue());

	CString propName = prop->GetName();

	if (propName == L"Raster Size") {
		pDoc->m_raster.size = vt;
	}

	if (propName == L"Overlap") {
		pDoc->m_Overlap = vt;
		pDoc->m_pGrid->makeRegion(pView->getMousePos(), pView->getWheel());
	}

	if (propName == L"Scale") {
		pDoc->m_FixationTargetSize = vt;
	}

	if (propName == L"Color") {
		COLORREF ref = vt;
		pDoc->m_raster.color = D2D1_COLOR_F(m_pRenderTarget->COLORREF_TO_D2DCOLOR(ref));
	}

	if (propName == L"File") {
		CString ftfile = vt.bstrVal;
		pDoc->m_FixationTarget = ftfile;
	}

	if (propName == L"Output Folder") {
		CString folder = vt.bstrVal;
		folder.Format(folder + "\\");
		pDoc->m_OutputDir = folder;

	}

	if (propName == L"Screen") {
		CString ref = vt.bstrVal;
		int index = ref.ReverseFind(_T(' '));
		ref.Truncate(index);
		for (auto it = pDoc->m_Monitors.m_Devices.begin(); it != pDoc->m_Monitors.m_Devices.end(); it++) {
			if (it->name == ref) {
				pDoc->m_Monitors.m_pSelectedDevice = it._Ptr;
				pView->SendMessage(SCREEN_SELECTED);
			}
		}
	}

	if (propName == L"Input Controller") {
		CString inpcon = vt.bstrVal;
		pDoc->m_InputController = inpcon;
	}

	if (propName == L"AOSACA IP") {
		CString aoip = vt.bstrVal;
		pDoc->m_AOSACA_IP = aoip;
		AfxGetMainWnd()->SendMessage(RESET_AOSACA_IP);
	}

	if (propName == L"ICANDI IP") {
		CString icip = vt.bstrVal;
		pDoc->m_ICANDI_IP = icip;
		AfxGetMainWnd()->SendMessage(RESET_ICANDI_IP);
	}

	if (propName == L"Flip Vertical") {
		pDoc->m_FlipVertical = vt.bstrVal;
		pDoc->m_Controller.setFlip();
	}

	if (propName == L"Flip Horizontal") {
		pDoc->m_FlipHorizontal = vt.bstrVal;
		pDoc->m_Controller.setFlip();
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

	OutputFolder = new CMFCPropertyGridFileProperty(L"Output Folder", L"", NULL, _T("Choose output directory of data log (.csv-file)"));
	FixationFile = new MyCMFCPropertyGridFileProperty(L"File", true, NULL, NULL, NULL, NULL, _T("Choose your custom fixation target from file"));
	Patch = new CMFCPropertyGridProperty(L"Patch");
	RasterSize = new CMFCPropertyGridProperty(L"Raster Size", RasterSizeValue, _T("Set the raster size in degrees"), NULL, NULL, NULL);
	Overlap = new CMFCPropertyGridProperty(L"Overlap", OverlapValue, _T("Set the the overlap value of neighboring patches in percent"), NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), col, NULL, _T("Choose the desired raster color"));
	Color->EnableOtherButton(L"Other..");
	DataLog = new CMFCPropertyGridProperty(L"Data Log");
	TargetView = new CMFCPropertyGridProperty(L"Target View");
	PhysParam = new CMFCPropertyGridProperty(L"Physical Parameters");
	FixationTargetScreen = new CMFCPropertyGridProperty(L"Screen", ScreenValue, _T("Pick the display for fixation target presentation. If empty, setup and connect another monitor to your computer."), NULL, NULL, NULL);
	FixationTargetSize = new CMFCPropertyGridProperty(L"Scale", FixationTargetValue, _T("Scale the size of the custom fixation target in percent (%)"), NULL, NULL, NULL);
	SubjectCalibration = new CMFCPropertyGridProperty(L"Subject Calibration");
	RemoteControl = new CMFCPropertyGridProperty(L"Remote Control");
	RemoteCapability = new CMFCPropertyGridProperty(L"Capability", RemoteValue, _T("Change Remote Control Function"), NULL, NULL, NULL);
	AOSACA_IP = new CMFCPropertyGridProperty(L"AOSACA IP", AOSACA_IPValue, _T("IP Address of computer running AOSACA, port 1500"), NULL, NULL, NULL);
	ICANDI_IP = new CMFCPropertyGridProperty(L"ICANDI IP", ICANDI_IPValue, _T("IP Address of computer running ICANDI, port 1400"), NULL, NULL, NULL);
	FlipVertical = new CMFCPropertyGridProperty(L"Flip Vertical", FlipVerticalValue, _T("Flips vertical orientation of Target Screen"), NULL, NULL, NULL);
	FlipHorizontal = new CMFCPropertyGridProperty(L"Flip Horizontal", FlipHorizontalValue, _T("Flips horizontal orientation of Target Screen"), NULL, NULL, NULL);
	InputController = new CMFCPropertyGridProperty(L"Input Controller", InputControl, _T("Select Mouse or Gamepad Controller for Subject Calibration procedure"), NULL, NULL, NULL);

	RECT Rect;
	GetClientRect(&Rect);
	MapWindowPoints(this, &Rect);
	m_wndPropList.AddProperty(PhysParam);
	PhysParam->AddSubItem(RasterSize);
	m_wndPropList.AddProperty(Patch);
	Patch->AddSubItem(Color);
	Patch->AddSubItem(Overlap);
	m_wndPropList.AddProperty(DataLog);
	DataLog->AddSubItem(OutputFolder);
	m_wndPropList.AddProperty(TargetView);
	TargetView->AddSubItem(FixationTargetScreen);
	TargetView->AddSubItem(FlipVertical);
	TargetView->AddSubItem(FlipHorizontal);
	TargetView->AddSubItem(FixationFile);
	TargetView->AddSubItem(FixationTargetSize);
	m_wndPropList.AddProperty(SubjectCalibration);
	SubjectCalibration->AddSubItem(InputController);
	m_wndPropList.AddProperty(RemoteControl);
	RemoteControl->AddSubItem(RemoteCapability);
	RemoteControl->AddSubItem(AOSACA_IP);
	RemoteControl->AddSubItem(ICANDI_IP);
	
}

void Properties::fillProperties() {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

	_variant_t rs(pDoc->m_raster.size);
	_variant_t ol(pDoc->m_Overlap);
	_variant_t fts(pDoc->m_FixationTargetSize);
	_variant_t ft(pDoc->m_FixationTarget);
	_variant_t od(pDoc->m_OutputDir);
	_variant_t aoip(pDoc->m_AOSACA_IP);
	_variant_t icip(pDoc->m_ICANDI_IP);
	_variant_t fv(pDoc->m_FlipVertical);
	_variant_t fh(pDoc->m_FlipHorizontal);
	_variant_t rem(pDoc->m_RemoteCtrl);
	_variant_t inpcon(pDoc->m_InputController);
	
	OutputFolder->SetValue(od);
	RasterSize->SetValue(rs);
	Overlap->SetValue(ol);
	FixationTargetSize->SetValue(fts);
	FixationFile->SetValue(ft);
	AOSACA_IP->SetValue(aoip);
	ICANDI_IP->SetValue(icip);
	FlipVertical->SetValue(fv);
	FlipHorizontal->SetValue(fh);
	InputController->SetValue(inpcon);
	RemoteCapability->SetValue(rem);

	InputController->AllowEdit(false);
	RemoteCapability->AllowEdit(false);
	FixationTargetScreen->AllowEdit(false);
	FlipVertical->AllowEdit(false);
	FlipHorizontal->AllowEdit(false);

	COLORREF col = RGB(
		(int)(pDoc->m_raster.color.r / (1 / 255.0)),
		(int)(pDoc->m_raster.color.g / (1 / 255.0)),
		(int)(pDoc->m_raster.color.b / (1 / 255.0))
	);
	
	Color->SetColor(col);
	 
	CString option;

	if (!pDoc->m_Monitors.m_pSelectedDevice)
		FixationTargetScreen->SetValue(L"NONE");

	else

		// fill monitor property
		for (auto& screen : pDoc->m_Monitors.m_Devices) {

			if (screen.number == pDoc->m_Monitors.m_pSelectedDevice->number) {
				option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
				FixationTargetScreen->SetValue(option);
				CIGUIDEView* pView = CIGUIDEView::GetView();
				pView->SendMessage(SCREEN_SELECTED);
			}

		}

	FixationTargetScreen->RemoveAllOptions();	// need for removal if fillProperties() is called more than once

	for (auto& screen : pDoc->m_Monitors.m_Devices) {
		// never parse primary monitor, because it is dedicated to operator view
		if (screen.number == 1)	continue;
		// all others: put in list
		option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
		FixationTargetScreen->AddOption(option);
	}
	
	RemoteCapability->RemoveAllOptions();	// need for removal if fillProperties() is called more than once
	CString capability[4]{ L"NONE", L"AOSACA", L"ICANDI", L"BOTH"};
	for (int i = 0; i < 4; i++)
		RemoteCapability->AddOption(capability[i]);

	InputController->RemoveAllOptions(); // same as before
	CString control[2]{ L"Mouse", L"Gamepad" };
	for (int i = 0; i < 2; i++)
		InputController->AddOption(control[i]);

	FlipVertical->RemoveAllOptions(); // same as before
	FlipHorizontal->RemoveAllOptions(); 
	CString vert[2]{ L"True", L"False" };
	for (int i = 0; i < 2; i++) {
		FlipVertical->AddOption(vert[i]);
		FlipHorizontal->AddOption(vert[i]);
	}

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

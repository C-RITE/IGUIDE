// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "Properties.h"
#include "IGUIDEDoc.h"
#include "IGUIDEView.h"
#include "afxdialogex.h"
#include "ws2tcpip.h"
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
		DEFAULT_PITCH,
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

	if (propName == "Raster Size") {
		pDoc->raster.size = vt;
	}

	if (propName == L"Scale") {
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

		VideoFolder->SetOriginalValue(pDoc->m_OutputDir);
		CString folder = vt.bstrVal;

		if (pDoc->m_pGrid->patchlist.isFileTouched()) {

			CString message;
			message.Format(L"Data has already been written in into %s.\nChange folder anyway?\n", pDoc->m_OutputDir);
			
			switch (AfxMessageBox(message, MB_YESNO)) {
			
			case IDYES:
				pDoc->m_OutputDir = folder;
				pDoc->m_pGrid->patchlist.untouch();
				break;

			case IDNO:
				VideoFolder->ResetOriginalValue();
				break;

			}
		}
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

	if (propName == L"Input Controller") {
		CString inpcon = vt.bstrVal;
		pDoc->m_InputController = inpcon;
		pDoc->m_Controller.reset();
	}

	if (propName == L"AOSACA IP") {

		AOSACA_IP->SetOriginalValue(pDoc->m_AOSACA_IP);
		
		if (isValidIpAddress(vt)) {
			pDoc->m_AOSACA_IP = vt.bstrVal;
		}

		else
			AOSACA_IP->ResetOriginalValue();

	}

	if (propName == L"ICANDI IP") {
		
		ICANDI_IP->SetOriginalValue(pDoc->m_ICANDI_IP);

		if (isValidIpAddress(vt))
			pDoc->m_ICANDI_IP = vt.bstrVal;

		else

			ICANDI_IP->ResetOriginalValue();
	
	}

	if (propName == L"Flip Vertical") {
		if (vt.bstrVal == (CString)L"Yes")
			pDoc->m_FlipVertical = -1;
		else
			pDoc->m_FlipVertical = 1;
		pDoc->m_Controller.reset();

	}

	if (propName == L"Capability") {
		pDoc->m_RemoteCtrl = vt;
	}

	prop->OnEndEdit();
	pDoc->UpdateAllViews(NULL);

	return S_OK;

}

bool Properties::isValidIpAddress(_variant_t ipAddress)
{
	int len = wcslen(ipAddress.bstrVal);
	char* str = new char[len + 1];
	wcstombs(str, ipAddress.bstrVal, len + 1);

	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, str, &(sa.sin_addr));
	delete str;

	if (!result)
		MessageBox(L"Not a valid IP address!");

	return result != 0;

}


void Properties::OnSize(UINT nType, int cx, int cy)
{
	AdjustLayout();
	CDockablePane::OnSize(nType, cx, cy);
}

void Properties::AdjustLayout()
{
	if (!m_wndPropList)
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
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	setPropertyValues();
	
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

	VideoFolder = new CMFCPropertyGridFileProperty(L"Video Folder", VideoFolderValue, NULL, _T("Choose output directory of captured video files"));
	FixationFile = new CMFCPropertyGridFileProperty(L"File", TRUE, FixationTargetFilenameValue, _T("Choose your custom fixation target from file"));
	FixationScreen = new CMFCPropertyGridProperty(L"Screen", FixationScreenValue, _T("Pick the display for fixation target presentation. If empty, setup and connect another monitor to your computer."), NULL, NULL, NULL);
	FixationSize = new CMFCPropertyGridProperty(L"Scale", FixationTargetSizeValue, _T("Scale the size of the custom fixation target in percent (%)"), NULL, NULL, NULL);
	Patch = new CMFCPropertyGridProperty(L"Patch");
	RasterSize = new CMFCPropertyGridProperty(L"Raster Size", RasterSizeValue, _T("Choose the raster size in pixel per degree"), NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), PatchColorValue, NULL, _T("Choose the desired raster color"));
	Color->EnableOtherButton(L"Other..");
	ICANDI = new CMFCPropertyGridProperty(L"ICANDI");
	TargetView = new CMFCPropertyGridProperty(L"Target View");
	PhysParam = new CMFCPropertyGridProperty(L"Physical Parameters");
	SubjectCalibration = new CMFCPropertyGridProperty(L"Subject Calibration");
	RemoteControl = new CMFCPropertyGridProperty(L"Remote Control");
	RemoteCapability = new CMFCPropertyGridProperty(L"Capability", RemoteControlValue, _T("Enable or Disable Remote Control Function"), NULL, NULL, NULL);
	AOSACA_IP = new CMFCPropertyGridProperty(L"AOSACA IP", AOSACA_IP_Value, _T("IP Address of computer running AOSACA (port: 1500)"), NULL, _T("ddd_ddd_ddd_ddd"), NULL, L"1234567890.");
	ICANDI_IP = new CMFCPropertyGridProperty(L"ICANDI IP", ICANDI_IP_Value, _T("IP Address of computer running ICANDI (port: 1400)"), NULL, _T("ddd_ddd_ddd_ddd"), NULL, L"1234567890.");

	FlipVertical = new CMFCPropertyGridProperty(L"Flip Vertical", FlipVerticalValue, _T("Flips vertical orientation of Target Screen"), NULL, NULL, NULL);
	InputController = new CMFCPropertyGridProperty(L"Input Controller", InputControllerValue, _T("Select Mouse or Gamepad Controller for Subject Calibration procedure"), NULL, NULL, NULL);

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
	TargetView->AddSubItem(FixationScreen);
	TargetView->AddSubItem(FixationFile);
	TargetView->AddSubItem(FixationSize);
	TargetView->AddSubItem(FlipVertical);
	m_wndPropList.AddProperty(SubjectCalibration);
	SubjectCalibration->AddSubItem(InputController);
	m_wndPropList.AddProperty(RemoteControl);
	RemoteControl->AddSubItem(RemoteCapability);
	RemoteControl->AddSubItem(AOSACA_IP);
	RemoteControl->AddSubItem(ICANDI_IP);
	
	FlipVertical->RemoveAllOptions();
	FlipVertical->AddOption(L"Yes");
	FlipVertical->AddOption(L"No");
	if (pDoc->m_FlipVertical == 1)
		FlipVertical->SetValue(L"No");
	else
		FlipVertical->SetValue(L"Yes");


	FixationScreen->RemoveAllOptions();	// need for removal if fillProperties() is called more than once 
										// (happens if you connect/disconnect monitors)

	CString option;

	for (auto& screen : pDoc->m_Screens) {
		// never parse primary monitor, because it is reserved for operator view
		if (screen.number == 1)	continue;
		// all others: put in list
		option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
		FixationScreen->AddOption(option);
	}

	RemoteCapability->RemoveAllOptions();	// need for removal if fillProperties() is called more than once
	CString capability[4]{ L"NONE", L"AOSACA", L"ICANDI", L"FULL" };
	for (int i = 0; i < 4; i++)
		RemoteCapability->AddOption(capability[i]);

	InputController->RemoveAllOptions(); // same as before
	CString control[2]{ L"Mouse", L"Gamepad" };
	for (int i = 0; i < 2; i++)
		InputController->AddOption(control[i]);

	RemoteCapability->AllowEdit(FALSE);
	InputController->AllowEdit(FALSE);
	VideoFolder->AllowEdit(FALSE);
	
}

void Properties::createPropertyList() {

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
		TRACE(L"Unable to create Property List");

	InitPropList();

}

void Properties::setPropertyValues() {

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	RasterSizeValue = pDoc->raster.size;
	VideoFolderValue = pDoc->m_OutputDir;
	FixationTargetFilenameValue = pDoc->m_FixationTarget;
	FixationTargetSizeValue = pDoc->m_FixationTargetSize;
	ICANDI_IP_Value = pDoc->m_ICANDI_IP;
	AOSACA_IP_Value = pDoc->m_AOSACA_IP;
	InputControllerValue = pDoc->m_InputController;
	RemoteControlValue = pDoc->m_RemoteCtrl;

	COLORREF col = RGB(
		(int)(pDoc->raster.color.r / (1 / 255.0)),
		(int)(pDoc->raster.color.g / (1 / 255.0)),
		(int)(pDoc->raster.color.b / (1 / 255.0))
	);
	
	PatchColorValue = col;
	 
	CString option;

	// fill monitor property
	for (auto& screen : pDoc->m_Screens) {
		// never parse primary monitor, because it is dedicated to operator view
		if (screen.number == 1)
			FixationScreenValue = L"NONE";
		else if (screen.number == pDoc->m_selectedScreen->number) {
			option.Format(L"%s (%ix%i)", screen.name, screen.resolution.x, screen.resolution.y);
			FixationScreenValue = option;
			CIGUIDEView* pView = CIGUIDEView::GetView();
			pView->SendMessage(SCREEN_SELECTED);
		}
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

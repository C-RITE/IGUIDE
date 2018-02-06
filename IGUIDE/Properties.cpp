// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "Properties.h"
#include "IGUIDEDoc.h"
#include "afxdialogex.h"
#include <Shlobj.h>

// Properties dialog
IMPLEMENT_DYNAMIC(Properties, CDialogEx)

Properties::Properties()

{
	VideoFolder = new CMFCPropertyGridFileProperty(L"Folder", L"D:\\Videos"),
	FixationFile = new CMFCPropertyGridFileProperty(L"File", true, NULL, _T("Select custom graphics file"));
	m_pRasterSize = new _variant_t();
	m_pFixationTargetSize = new _variant_t();
	Raster = new CMFCPropertyGridProperty(L"Raster");
	RasterSize = new CMFCPropertyGridProperty(L"Size", m_pRasterSize, NULL, NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), col, NULL, _T("Choose the desired raster color"));
	Color->EnableOtherButton(L"Other..");
	ICANDI = new CMFCPropertyGridProperty(L"ICANDI");
	FixationTarget = new CMFCPropertyGridProperty(L"Fixation Target");
	FixationTargetSize = new CMFCPropertyGridProperty(L"Size", m_pFixationTargetSize, _T("Scale the size of the fixation target (%)"), NULL, NULL, NULL);
}

Properties::~Properties()
{
	
	delete FixationTarget;
	delete m_pFixationTargetSize;
	delete Raster;
	delete m_pRasterSize;
	delete ICANDI;

}

void Properties::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Properties, CDialogEx)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// Properties message handlers

BOOL Properties::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT Properties::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = GetDoc();
	CMFCPropertyGridCtrl* gridctrl = (CMFCPropertyGridCtrl*)wParam;
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty*)lParam;
	_variant_t vt(prop->GetValue());
	CString propName = prop->GetName();
		if (propName == L"Size") {
			if (prop->GetParent() == Raster)
				pDoc->raster.size = vt;
			if (prop->GetParent() == FixationTarget)
				pDoc->m_FixationTargetSize = vt;
		}
		if (propName == L"Color") {
			COLORREF ref = vt;
			pDoc->raster.color = D2D1_COLOR_F(m_pRenderTarget->COLORREF_TO_D2DCOLOR(ref));
		}
		if (propName == L"File") {
			CString ref = vt.bstrVal;
			pDoc->m_FixationTarget = ref;
		}

		pDoc->UpdateAllViews(NULL);

	return 0;
}

void Properties::OnShowWindow(BOOL bShow, UINT nStatus)
{
		// TODO: Add your message handler code here
		CIGUIDEDoc* pDoc = GetDoc();
		_variant_t rs(pDoc->raster.size);
		_variant_t fts(pDoc->m_FixationTargetSize);
		_variant_t ft(pDoc->m_FixationTarget);
		RasterSize->SetValue(rs);
		FixationTargetSize->SetValue(fts);
		FixationFile->SetValue(ft);
		COLORREF col = RGB(
			(int)(pDoc->raster.color.r / (1 / 255.0)),
			(int)(pDoc->raster.color.g / (1 / 255.0)),
			(int)(pDoc->raster.color.b / (1 / 255.0))
		);
		Color->SetColor(col);
	
}

int Properties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT Rect;
	GetClientRect(&Rect);
	MapWindowPoints(this, &Rect);
	GridCtrl.Create(WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP, Rect, this, NULL);
	GridCtrl.AddProperty(Raster);
	Raster->AddSubItem(RasterSize);
	Raster->AddSubItem(Color);
	GridCtrl.AddProperty(ICANDI);
	ICANDI->AddSubItem(VideoFolder);
	GridCtrl.AddProperty(FixationTarget);
	FixationTarget->AddSubItem(FixationFile);
	FixationTarget->AddSubItem(FixationTargetSize);
	GridCtrl.setLabelWidth(100);
	// TODO:  Add your specialized creation code here

	return 0;

}
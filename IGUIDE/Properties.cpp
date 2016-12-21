// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "IGUIDE.h"
#include "Properties.h"
#include "IGUIDEDoc.h"
#include "afxdialogex.h"


// Properties dialog

IMPLEMENT_DYNAMIC(Properties, CDialogEx)

Properties::Properties()

{
	m_RasterSize = new _variant_t((float)0);
	Raster = new CMFCPropertyGridProperty(L"Raster");
	Size = new CMFCPropertyGridProperty(L"Size", m_RasterSize, NULL, NULL, NULL, NULL);
	COLORREF col = D2D1::ColorF::DarkGreen;
	Color = new CMFCPropertyGridColorProperty(_T("Color"), col, NULL, _T("Specifies the default tag color"));
	Color->EnableOtherButton(L"Other..");
}

Properties::~Properties()
{
	delete m_RasterSize;
	delete Raster;
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

	// TODO:  Add extra initialization here

	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT Properties::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CMFCPropertyGridCtrl* gridctrl = (CMFCPropertyGridCtrl*)wParam;
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty*)lParam;
	_variant_t vt(prop->GetValue());
	CString propName = prop->GetName();
	if (propName == L"Size") {
		pDoc->raster.size = vt;
	}
	if (propName == L"Color") {
		D2D1_COLOR_F color;
		BYTE* col = &vt.bVal;
		color.r = (float)*col; col++;
		color.g = (float)*col; col++;
		color.b = (float)*col;
		color.a = 1.0f;
		pDoc->raster.color = color;
	}

	return 0;
}

void Properties::OnShowWindow(BOOL bShow, UINT nStatus)
{
		CDialogEx::OnShowWindow(bShow, nStatus);
		CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
		_variant_t vt(pDoc->raster.size);
		Size->SetValue(vt);
	// TODO: Add your message handler code here
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
	Raster->AddSubItem(Size);
	Raster->AddSubItem(Color);

	// TODO:  Add your specialized creation code here

	return 0;
}

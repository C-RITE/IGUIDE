
// GridTargetsDoc.cpp : implementation of the CGridTargetsDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GridTargets.h"
#endif

#include "GridTargetsDoc.h"

#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;

// CGridTargetsDoc

IMPLEMENT_DYNCREATE(CGridTargetsDoc, CDocument)

BEGIN_MESSAGE_MAP(CGridTargetsDoc, CDocument)
	ON_COMMAND(ID_FILE_IMPORT, &CGridTargetsDoc::OnFileImport)
END_MESSAGE_MAP()


// CGridTargetsDoc construction/destruction

CGridTargetsDoc::CGridTargetsDoc()
{
	// TODO: add one-time construction code here

	m_pGrid = new Grid();
	mousePos = NULL;

}

CGridTargetsDoc::~CGridTargetsDoc()
{
	delete m_pGrid;
	delete mousePos;
	
}

// Get Doc, made for other classes that need access to attributes

CGridTargetsDoc* CGridTargetsDoc::GetDoc()
{
	CMDIChildWnd * pChild =
		((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();

	if (!pChild)
		return NULL;

	CDocument * pDoc = pChild->GetActiveDocument();

	if (!pDoc)
		return NULL;

	// Fail if doc is of wrong kind
	if (!pDoc->IsKindOf(RUNTIME_CLASS(CGridTargetsDoc)))
		return NULL;

	return (CGridTargetsDoc *)pDoc;
}


BOOL CGridTargetsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;

}




// CGridTargetsDoc serialization

void CGridTargetsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CGridTargetsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CGridTargetsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGridTargetsDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGridTargetsDoc diagnostics

#ifdef _DEBUG
void CGridTargetsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGridTargetsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGridTargetsDoc commands


BOOL CGridTargetsDoc::CheckFOV()
{

	if (raster.boundary.size() < 4) {
		AfxMessageBox(_T("Please draw raster edges in Target View window first!"), MB_OK);
		return FALSE;
	}

	return TRUE;

}

BOOL CGridTargetsDoc::CalcMeanEdge() {
	
	// calculate mean of all 4 edges
	int perimeter;

	if (raster.boundary.size() > 3) {
		perimeter =
			abs(raster.boundary[1].x - raster.boundary[0].x) + abs(raster.boundary[1].y - raster.boundary[0].y) +
			abs(raster.boundary[2].x - raster.boundary[1].x) + abs(raster.boundary[2].y - raster.boundary[1].y) +
			abs(raster.boundary[3].x - raster.boundary[2].x) + abs(raster.boundary[3].y - raster.boundary[2].y) +
			abs(raster.boundary[0].y - raster.boundary[3].x) + abs(raster.boundary[0].y - raster.boundary[3].y);
		raster.meanEdge = perimeter / 4;
		return TRUE;
	}
	
	return FALSE;

}

CPoint CGridTargetsDoc::compute2DPolygonCentroid(const CPoint* vertices, int vertexCount)
{
	CPoint centroid = { 0, 0 };
	double signedArea = 0.0;
	double x0 = 0.0; // Current vertex X
	double y0 = 0.0; // Current vertex Y
	double x1 = 0.0; // Next vertex X
	double y1 = 0.0; // Next vertex Y
	double a = 0.0;  // Partial signed area

					 // For all vertices except last
	int i = 0;
	for (i = 0; i<vertexCount - 1; ++i)
	{
		x0 = vertices[i].x;
		y0 = vertices[i].y;
		x1 = vertices[i + 1].x;
		y1 = vertices[i + 1].y;
		a = x0*y1 - x1*y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;
	}

	// Do last vertex separately to avoid performing an expensive
	// modulus operation in each iteration.
	x0 = vertices[i].x;
	y0 = vertices[i].y;
	x1 = vertices[0].x;
	y1 = vertices[0].y;
	a = x0*y1 - x1*y0;
	signedArea += a;
	centroid.x += (x0 + x1)*a;
	centroid.y += (y0 + y1)*a;

	signedArea *= 0.5;
	centroid.x /= (6.0*signedArea);
	centroid.y /= (6.0*signedArea);

	return centroid;
}



void CGridTargetsDoc::OnFileImport()
{
	// TODO: Add your command handler code here

	HRESULT hr = _ShowWICFileOpenDialog(AfxGetMainWnd()->GetSafeHwnd(), strFile);
	UpdateAllViews(NULL);
}


HRESULT CGridTargetsDoc::_ShowWICFileOpenDialog(HWND hWndOwner, CStringW& strFile)
{

	// create IFileOpenDialog instance.
	CComPtr<IFileOpenDialog> pIFileOpenDialog;
	HRESULT hr = pIFileOpenDialog.CoCreateInstance(CLSID_FileOpenDialog);

	// get an array of WIC decoders friendly names and extensions
	COMDLG_FILTERSPEC* pFilterSpecArray = NULL;
	UINT cbFilterSpecCount = 0;
	
	if (SUCCEEDED(hr))
	{
		hr = _GetWICFileOpenDialogFilterSpecs(pFilterSpecArray, cbFilterSpecCount);
	}

	// set the filter
	if (SUCCEEDED(hr))
	{
		hr = pIFileOpenDialog->SetFileTypes(cbFilterSpecCount, pFilterSpecArray);
	}

	// show the file open dialog, and get the chosen file
	if (SUCCEEDED(hr))
	{
		hr = pIFileOpenDialog->Show(hWndOwner);
	}

	// get the chosen full path and file name
	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pIShellItem;
		hr = pIFileOpenDialog->GetResult(&pIShellItem);
		if (SUCCEEDED(hr))
		{
			LPWSTR pszName = NULL;
			hr = pIShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName);
			strFile = pszName;
			CoTaskMemFree(pszName);
		}
	}

	// do cleanup
	for (UINT nIndex = 0; nIndex < cbFilterSpecCount; nIndex++)
	{
		delete[]pFilterSpecArray[nIndex].pszName;
		delete[]pFilterSpecArray[nIndex].pszSpec;
	}
	delete[]pFilterSpecArray;

	UpdateAllViews(NULL);

	return hr;
}

HRESULT CGridTargetsDoc::_GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount)
{
	ATLASSERT(NULL == pFilterSpecArray);
	cbFilterSpecCount = 1; // we'll use the last one for "All WIC files" 
	CStringW strAllSpecs;

	// create IWICImagingFactory instance
	CComPtr<IWICImagingFactory> pIWICImagingFactory;
	HRESULT hr = pIWICImagingFactory.CoCreateInstance(CLSID_WICImagingFactory);

	// create WIC decoders enumerator
	CComPtr<IEnumUnknown> pIEnum;
	if (SUCCEEDED(hr))
	{
		DWORD dwOptions = WICComponentEnumerateDefault;
		hr = pIWICImagingFactory->CreateComponentEnumerator(WICDecoder, dwOptions, &pIEnum);
	}

	if (SUCCEEDED(hr))
	{
		CComPtr<IUnknown> pElement;
		ULONG cbActual = 0;
		// count enumerator elements
		while (S_OK == pIEnum->Next(1, &pElement, &cbActual))
		{
			++cbFilterSpecCount;
			pElement = NULL;
		}

		// alloc COMDLG_FILTERSPEC array
		pFilterSpecArray = new COMDLG_FILTERSPEC[cbFilterSpecCount];

		// reset enumaration an loop again to fill filter specs array
		pIEnum->Reset();
		COMDLG_FILTERSPEC* pFilterSpec = pFilterSpecArray;
		while (S_OK == pIEnum->Next(1, &pElement, &cbActual))
		{

			CComQIPtr<IWICBitmapDecoderInfo> pIWICBitmapDecoderInfo = pElement;
			// get necessary buffer size for friendly name and extensions
			UINT cbName = 0, cbFileExt = 0;
			pIWICBitmapDecoderInfo->GetFriendlyName(0, NULL, &cbName);
			pIWICBitmapDecoderInfo->GetFileExtensions(0, NULL, &cbFileExt);

			// get decoder friendly name
			(*pFilterSpec).pszName = new WCHAR[cbName];
			pIWICBitmapDecoderInfo->GetFriendlyName(cbName, (WCHAR*)(*pFilterSpec).pszName, &cbName);

			// get extensions; wee need to replace some characters according to the specs
			CStringW strSpec;
			pIWICBitmapDecoderInfo->GetFileExtensions(cbFileExt, CStrBuf(strSpec, cbFileExt), &cbFileExt);
			strSpec.Replace(L',', L';');
			strSpec.Replace(L".", L"*.");
			size_t size = strSpec.GetLength() + 1;
			(*pFilterSpec).pszSpec = new WCHAR[size];
			wcscpy_s((wchar_t*)(*pFilterSpec).pszSpec, size, strSpec.GetString());

			// append to "All WIC files" specs 
			strSpec += L";";
			strAllSpecs += strSpec;

			++pFilterSpec;
			pElement = NULL;
		}

		// set "All WIC files" specs 
		strAllSpecs.TrimRight(_T(';'));
		(*pFilterSpec).pszName = new WCHAR[wcslen(L"All WIC files") + 1];
		wcscpy_s((wchar_t*)(*pFilterSpec).pszName, wcslen(L"All WIC files") + 1, L"All WIC files");
		(*pFilterSpec).pszSpec = new WCHAR[strAllSpecs.GetLength() + 1];
		wcscpy_s((wchar_t*)(*pFilterSpec).pszSpec, strAllSpecs.GetLength() + 1, strAllSpecs.GetString());
	}
	return S_OK;
}
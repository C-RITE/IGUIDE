
// GridTargetsDoc.cpp : implementation of the CGridTargetsDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GridTargets.h"
#endif

#include "GridTargetsDoc.h"
#include <math.h>

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
	raster.meanEdge = 0;
	raster.meanAlpha = 0;

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

	if (raster.corner.size() < 4) {
		AfxMessageBox(_T("Please draw raster corners (clockwise) in Target View window first!"), MB_OK);
		return FALSE;
	}

	return TRUE;

}

float CGridTargetsDoc::CalcEdgeLength(Edge k) {
	
	float a, b, c;

	a = abs(k.q.x - k.p.x);
	b = abs(k.q.y - k.p.y);

	c = sqrt(pow(a, 2) + pow(b, 2));
	
	return c;

}

CString CGridTargetsDoc::getTraceInfo() {

	CString trace;
	Edge k = { { 0.f, 0.f }, { (float)m_pGrid->centerOffset.x, (float)m_pGrid->centerOffset.y }, NULL };
	float beta = 360 - computeDisplacementAngle(k);
	float dist = m_pGrid->center.x - ((m_pGrid->nerve.right - m_pGrid->nerve.left) / 2);

	trace.Format(L"alpha:\t\t%f (deg)\nbeta:\t\t\%f (deg)\ngamma:\t\t\%f (deg)\nmean edge:\t%f (px)\nfov2disc:\t%f (px)",
		raster.meanAlpha,
		beta,
		raster.meanAlpha + beta,
		raster.meanEdge,
		dist);
	return trace;

}


CD2DPointF CGridTargetsDoc::compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount)
{
	CD2DPointF centroid = { 0, 0 };
	float signedArea = 0.0;
	float x0 = 0.0; // Current vertex X
	float y0 = 0.0; // Current vertex Y
	float x1 = 0.0; // Next vertex X
	float y1 = 0.0; // Next vertex Y
	float a = 0.0;  // Partial signed area

					// For all vertices
	int i = 0;
	for (i = 0; i<vertexCount; ++i)
	{
		x0 = vertices[i].x;
		y0 = vertices[i].y;
		x1 = vertices[(i + 1) % vertexCount].x;
		y1 = vertices[(i + 1) % vertexCount].y;
		a = x0*y1 - x1*y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;
	}

	signedArea *= 0.5;
	centroid.x /= (6.0*signedArea);
	centroid.y /= (6.0*signedArea);

	return centroid;

}

float CGridTargetsDoc::computeDisplacementAngle(Edge k) {

	float a, b;
	float alpha;
	float pi = atan(1) * 4;

	if (k.q.x >= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.q.y - k.p.y);
		alpha = 90 + (atan(a / b)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.p.y - k.q.y);
		alpha = 180 + (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x >= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.p.y - k.q.y);
		alpha = 270 + (atan(a / b)) * 180 / pi;
	}

	return alpha;

}

void CGridTargetsDoc::computeDisplacementAngles() {


	ATLTRACE(_T("\n"));

	for (int i = 0; i < raster.perimeter.size(); i++) {
		raster.perimeter[i].alpha = computeDisplacementAngle(raster.perimeter[i]);
	
		if (raster.perimeter[i].alpha >= 270)
			raster.perimeter[i].alpha = abs(raster.perimeter[i].alpha - 90);
		else if (raster.perimeter[i].alpha >= 180 && raster.perimeter[i].alpha < 270)
			raster.perimeter[i].alpha = abs(raster.perimeter[i].alpha - 0);
		else if (raster.perimeter[i].alpha >= 90 && raster.perimeter[i].alpha < 180)
			raster.perimeter[i].alpha = abs(raster.perimeter[i].alpha - 270);
		else if (raster.perimeter[i].alpha >= 0 && raster.perimeter[i].alpha < 90)
			raster.perimeter[i].alpha = abs(raster.perimeter[i].alpha - 180);
	}

	Edge k = { { raster.mid.x, raster.mid.y },{ raster.perimeter[0].p.x, raster.perimeter[0].p.y}, NULL };
	float theta = computeDisplacementAngle(k);
	ATLTRACE(_T("theta is %f\n"), theta);

	if (theta >= 90 && theta < 180 && raster.perimeter[0].q.y > raster.perimeter[0].p.y)
		for (int i = 0; i < raster.perimeter.size(); i++) {
			raster.perimeter[i].alpha += 270;
		}

	if (theta >= 0 && theta < 90 && raster.perimeter[0].q.y > raster.perimeter[0].p.y)
		for (int i = 0; i < raster.perimeter.size(); i++) {
			raster.perimeter[i].alpha += 180;
		}

	if (theta <= 360 && theta > 270 && raster.perimeter[0].p.x > raster.perimeter[0].q.x)
		for (int i = 0; i < raster.perimeter.size(); i++) {
			raster.perimeter[i].alpha += 90;
		}

	if (theta <= 270 && theta > 180 && raster.perimeter[0].p.y > raster.perimeter[0].q.y)
		for (int i = 0; i < raster.perimeter.size(); i++) {
			raster.perimeter[i].alpha += 0;
		}


	for (int i = 0; i < raster.perimeter.size(); i++) {
		ATLTRACE(_T("alpha is %f\n"), raster.perimeter[i].alpha);
	}
	
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
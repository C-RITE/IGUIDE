#include "stdafx.h"
#include "Fundus.h"


Fundus::Fundus()
{
	picture = NULL;
}


Fundus::~Fundus()
{
}


void Fundus::paint(CHwndRenderTarget* pRenderTarget)
{
	if (picture)
	{
		CD2DSizeF size = picture->GetSize();
		CPoint point = (size.width / 2, size.height / 2);
		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(-180.f, -418.f);
		pRenderTarget->SetTransform(matrix);
		D2D1_MATRIX_3X2_F identity = D2D1::IdentityMatrix();
		pRenderTarget->DrawBitmap(picture, CD2DRectF(0, 0, size.width *.725, size.height *.725));
		pRenderTarget->SetTransform(identity);
	}
}


BOOL Fundus::openFundus(CHwndRenderTarget* pRenderTarget)
{
	picture = new CD2DBitmap(pRenderTarget, filename);
	picture->Create(pRenderTarget);
	return 0;
}



HRESULT Fundus::_ShowWICFileOpenDialog(HWND hWndOwner)
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
			filename = pszName;
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

	//UpdateAllViews(NULL);

	return hr;
}

HRESULT Fundus::_GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount)
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
#include "stdafx.h"
#include "IGUIDE.h"
#include "IGUIDEDoc.h"
#include "Fundus.h"


Fundus::Fundus() :
	picture(NULL),
	calibration(FALSE),
	filename(NULL)
{
}


Fundus::~Fundus()
{
	delete filename;
}


void Fundus::Paint(CHwndRenderTarget* pRenderTarget)
{

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	if (picture && calibration && (pDoc->m_pGrid->overlay & FUNDUS))
	{
		float ppd = 1/pDoc->m_pGrid->dpp;
		float ppdimage =  pDoc->m_pDlgCalibration->m_D2DStatic.k.length * pDoc->m_pDlgCalibration->m_sFactor / 15;
		float factor = ppd / ppdimage;

		CD2DSizeF size = picture->GetSize();

		D2D1_MATRIX_3X2_F identity = D2D1::IdentityMatrix();
		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(
			D2D1::Size(factor,factor),
			D2D1::Point2F(pDoc->m_pDlgCalibration->m_D2DStatic.k.p.x * pDoc->m_pDlgCalibration->m_sFactor,
				pDoc->m_pDlgCalibration->m_D2DStatic.k.p.y * pDoc->m_pDlgCalibration->m_sFactor));
		D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Translation(
			pDoc->m_pGrid->center.x - pDoc->m_pDlgCalibration->m_D2DStatic.k.p.x * pDoc->m_pDlgCalibration->m_sFactor,
			pDoc->m_pGrid->center.y - pDoc->m_pDlgCalibration->m_D2DStatic.k.p.y * pDoc->m_pDlgCalibration->m_sFactor);
		pRenderTarget->SetTransform(scale * translate);
		pRenderTarget->DrawBitmap(picture, CD2DRectF(0, 0, size.width, size.height));
		pRenderTarget->SetTransform(identity);
	}

}


HRESULT Fundus::_ShowWICFileOpenDialog(HWND hWndOwner)
{
	filename = new CStringW();
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
	// Set the default extension to be ".jpg" file.
	if (SUCCEEDED(hr))
		hr = pIFileOpenDialog->SetFileTypeIndex(cbFilterSpecCount);

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
			*filename = pszName;
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
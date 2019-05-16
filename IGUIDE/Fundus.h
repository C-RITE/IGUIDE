#pragma once
#include "Calibration.h"
#include <memory>
class Fundus
{

public:
	Fundus();
	~Fundus();
	void Paint(CHwndRenderTarget* pRenderTarget);
	CD2DBitmap*						picture;
	CStringW						filename;
	BOOL							calibration;

	HRESULT _ShowWICFileOpenDialog(HWND hWndOwner);
	HRESULT _GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount);
};


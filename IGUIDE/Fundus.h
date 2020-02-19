#pragma once
#include "Calibration.h"
#include <memory>
class Fundus
{

public:
	Fundus();
	~Fundus();
	void Paint(CHwndRenderTarget* pRenderTarget);

	CD2DBitmap*						picture;			// the fundus as bitmap
	CStringW						filename;			// name of fundus file
	CStringW						mru_folder;			// most recently used folder
	BOOL							calibration;		// is calibration complete?

	HRESULT _ShowWICFileOpenDialog(HWND hWndOwner);
	HRESULT _GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount);

};


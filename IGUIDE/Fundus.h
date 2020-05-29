#pragma once
#include "Calibration.h"
#include <memory>
class Fundus
{

public:
	Fundus();
	~Fundus();
	void Paint(CHwndRenderTarget* pRenderTarget);

	CD2DBitmap*						picture;			// the fundus as d2d object
	HBITMAP							fundus;				// Handle to fundus image
	CImage							fundusIMG;			// Use this to load from file
	CStringW						filename;			// name of fundus file
	CStringW						mru_folder;			// most recently used folder
	BOOL							calibration;		// is calibration complete?
	Edge							calibResult;		// result of fundus calibration

	HRESULT _ShowWICFileOpenDialog(HWND hWndOwner);
	HRESULT _GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount);

};


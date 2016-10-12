#pragma once
class Fundus
{
public:
	Fundus();
	~Fundus();
	void paint(CHwndRenderTarget* pRenderTarget);
	BOOL openFundus(CHwndRenderTarget* pRenderTarget);
	CD2DBitmap*		picture;
	CStringW		filename;

	HRESULT _ShowWICFileOpenDialog(HWND hWndOwner);
	HRESULT _GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount);
};


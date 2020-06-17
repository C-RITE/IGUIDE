#pragma once

// RegionTreeCtrl

class RegionTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(RegionTreeCtrl)

public:
	RegionTreeCtrl();
	virtual ~RegionTreeCtrl();

protected:
	COLORREF crTextWhite, crBkgrnd;
	HBRUSH brBkGnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCstDrw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:

	struct Color_Font
	{
		COLORREF color;
		LOGFONT  logfont;
	};
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont;

	int selItemIndex;				// item index of current selection
	int selItemRegion;				// selected item's parent

public:

	void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);
	void SetItemColor(HTREEITEM hItem, COLORREF color);
	void SetItemBold(HTREEITEM hItem, BOOL bBold);
	BOOL GetItemFont(HTREEITEM hItem, LOGFONT* plogfont);
	BOOL GetItemBold(HTREEITEM hItem);
	COLORREF GetItemColor(HTREEITEM hItem);

public:
	afx_msg void OnPaint();
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	void getCurrentSelection(int& region, int& index);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#pragma once
#include "Edge.h"
class Calibration;

// Calibration dialog

class D2DStatic : public CStatic
{
	DECLARE_DYNAMIC(D2DStatic)

public:
	D2DStatic(Calibration* Dialog);
	virtual ~D2DStatic();
	virtual void PreSubclassWindow();

	Edge					k;
	int						m_clicked;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDraw2d(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	CD2DSolidColorBrush*	m_pBrushWhite;
	Calibration*			m_pDlg;
	CD2DPointF				m_curMousePos;
	CD2DBitmap*				fundus;

public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


class Calibration : public CDialog
{
	DECLARE_DYNAMIC(Calibration)

public:
	Calibration(CWnd* pParent = NULL);   // standard constructor
	virtual ~Calibration();
	void ClientResize(int nWidth, int nHeight);
	
// Dialog Data

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALIBRATION };
#endif

public:
	D2DStatic				m_D2DStatic;
	RECT					m_WorkArea;
	CRect					m_ClientRect;
	double					m_sFactor;
	double					rx, ry;
	double					Shrink2Fit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

};


#pragma once
#include "resource.h"
#include "PropertyGrid.h"

// Properties dialog

class Properties : public CDialogEx
{
	DECLARE_DYNAMIC(Properties)

public:
	Properties(CWnd* pParent = NULL);   // standard constructor
	virtual ~Properties();
	CPropertyGrid m_ctrlGrid;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTIES };
#endif

	DECLARE_MESSAGE_MAP()
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnInitDialog();
};
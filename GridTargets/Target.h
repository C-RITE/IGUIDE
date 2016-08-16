#pragma once


// Target dialog

class Target : public CDialog
{
	DECLARE_DYNAMIC(Target)

public:
	Target(CWnd* pParent = NULL);   // standard constructor
	virtual ~Target();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TARGET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

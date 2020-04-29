#pragma once


// PatchInfo dialog

class PatchInfo : public CDialogEx
{
	DECLARE_DYNAMIC(PatchInfo)

public:
	PatchInfo(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PatchInfo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PATCHINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString filename;
	CString directory;
};


// GridTargetsDoc.h : interface of the CGridTargetsDoc class
//

#pragma once
#include "Target.h"
#include "Grid.h"
#include "afxrendertarget.h"
#include <vector>

using namespace std;

class CGridTargetsDoc : public CDocument
{
protected: // create from serialization only
	CGridTargetsDoc();
	DECLARE_DYNCREATE(CGridTargetsDoc)

// Attributes
public:

	Grid*					m_pGrid;							// grid class
	struct Raster
	{
		vector<CD2DPointF>	boundary;							// raster dimensions as seen by subject
		int					meanEdge;							// average of 4 raster edges
		CPoint				mid;								// triangulate mid point of raster
	};
	Raster					raster;
	
	CPoint					center;								// center of main window
	CPoint*					mousePos;							// current mouse position
	CStringW				strFile;							// filename of fundus picture

// Operations
public:
	BOOL CheckFOV();
	static CGridTargetsDoc* GetDoc();
	HRESULT _ShowWICFileOpenDialog(HWND hWndOwner, CStringW& strFile);
	HRESULT CGridTargetsDoc::_GetWICFileOpenDialogFilterSpecs(COMDLG_FILTERSPEC*& pFilterSpecArray, UINT& cbFilterSpecCount);
	BOOL CalcMeanEdge();
	CPoint compute2DPolygonCentroid(const CPoint* vertices, int vertexCount);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CGridTargetsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	afx_msg void OnFileImport();

};

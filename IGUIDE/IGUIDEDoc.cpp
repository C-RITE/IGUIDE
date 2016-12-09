
// IGUIDEDoc.cpp : implementation of the CIGUIDEDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IGUIDE.h"
#include "Fundus.h"
#endif

#include "IGUIDEDoc.h"
#include "Grid.h"
#include <math.h>

#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;

// CIGUIDEDoc

IMPLEMENT_DYNCREATE(CIGUIDEDoc, CDocument)

BEGIN_MESSAGE_MAP(CIGUIDEDoc, CDocument)
	ON_COMMAND(ID_FILE_IMPORT, &CIGUIDEDoc::OnFileImport)
	ON_COMMAND(ID_EDIT_PROPERTIES, &CIGUIDEDoc::OnEditProperties)
	ON_COMMAND(ID_OVERLAY_GRID, &CIGUIDEDoc::OnOverlayGrid)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_GRID, &CIGUIDEDoc::OnUpdateOverlayGrid)
	ON_COMMAND(ID_OVERLAY_RADIUS, &CIGUIDEDoc::OnOverlayRadius)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_RADIUS, &CIGUIDEDoc::OnUpdateOverlayRadius)
	ON_COMMAND(ID_OVERLAY_FOVEA, &CIGUIDEDoc::OnOverlayFovea)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_FOVEA, &CIGUIDEDoc::OnUpdateOverlayFovea)
	ON_COMMAND(ID_OVERLAY_OPTICDISC, &CIGUIDEDoc::OnOverlayOpticdisc)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_OPTICDISC, &CIGUIDEDoc::OnUpdateOverlayOpticdisc)
	ON_COMMAND(ID_OVERLAY_CROSSHAIR, &CIGUIDEDoc::OnOverlayCrosshair)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_CROSSHAIR, &CIGUIDEDoc::OnUpdateOverlayCrosshair)
	ON_COMMAND(ID_OVERLAY_FUNDUS, &CIGUIDEDoc::OnOverlayFundus)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_FUNDUS, &CIGUIDEDoc::OnUpdateOverlayFundus)
	ON_COMMAND(ID_OVERLAY_TRACEINFO, &CIGUIDEDoc::OnOverlayTraceinfo)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_TRACEINFO, &CIGUIDEDoc::OnUpdateOverlayTraceinfo)
END_MESSAGE_MAP()


// CIGUIDEDoc construction/destruction

CIGUIDEDoc::CIGUIDEDoc()
{
	// TODO: add one-time construction code here

	m_pGrid = new Grid();
	m_pFundus = new Fundus();
	m_pDlgProperties = new Properties();
	mousePos = NULL;
	raster.size = 1.28;
	raster.meanAlpha = 0;
	m_pDlgCalibration = new Calibration();

}

CIGUIDEDoc::~CIGUIDEDoc()
{
	delete m_pGrid;
	delete m_pFundus;
	delete mousePos;
	delete m_pDlgCalibration;
	delete m_pDlgProperties;

}

// Get Doc, made for other classes that need access to attributes

CIGUIDEDoc* CIGUIDEDoc::GetDoc()
{
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);

	CMDIChildWnd * pChild =
		((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();

	ASSERT_VALID(pChild);

	CDocument * pDoc = pChild->GetActiveDocument();

	if (!pDoc) {

		POSITION posTemplate = pApp->GetFirstDocTemplatePosition();

		while (posTemplate != NULL) {
			CDocTemplate* pTemplate = pApp->GetNextDocTemplate(posTemplate);
			ASSERT_VALID(pTemplate);
			ASSERT_KINDOF(CDocTemplate, pTemplate);
			POSITION posDocument = pTemplate->GetFirstDocPosition();
			while (posDocument != NULL) {
				CDocument* pDoc = pTemplate->GetNextDoc(posDocument);
				ASSERT_VALID(pDoc);
				ASSERT_KINDOF(CDocument, pDoc);
				return (CIGUIDEDoc *)pDoc;
			}
		}
	}

}


BOOL CIGUIDEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;

}


// CIGUIDEDoc serialization

void CIGUIDEDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CIGUIDEDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CIGUIDEDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CIGUIDEDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIGUIDEDoc diagnostics

#ifdef _DEBUG
void CIGUIDEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIGUIDEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIGUIDEDoc commands


BOOL CIGUIDEDoc::CheckFOV()
{

	if (raster.corner.size() < 4) {
		AfxMessageBox(_T("Please draw raster corners (clockwise) in Target View window first!"), MB_OK);
		ShowCursor(TRUE);
		return FALSE;
	}
	return TRUE;

}

float CIGUIDEDoc::CalcEdgeLength(Edge k) {
	
	float a, b, c;

	a = abs(k.q.x - k.p.x);
	b = abs(k.q.y - k.p.y);

	c = sqrt(pow(a, 2) + pow(b, 2));
	
	return c;

}

CString CIGUIDEDoc::getTraceInfo() {

	POSITION pos = GetFirstViewPosition();
	CWnd* target = GetNextView(pos);
	CString trace;
	Edge k;
	k.q.x = (float)m_pGrid->centerOffset.x;
	k.q.y = (float)m_pGrid->centerOffset.y;

	float beta = 360 - ComputeOrientationAngle(k);
	float dist = m_pGrid->center.x - ((m_pGrid->nerve.right - m_pGrid->nerve.left) / 2);
	
	trace.Format(L"alpha:\t\t%f (deg)\nbeta:\t\t\%f (deg)\ngamma:\t\t\%f (deg)\nsize:\t\t%f (deg)\nscale.x:\t%f\nscale.y:\t%f\nfov2disc:\t%f (px)\nhost ppd:\t%f\nclient ppd:\t%f",
		raster.meanAlpha,
		beta,
		raster.meanAlpha + beta,
		raster.size,
		raster.scale.x,
		raster.scale.y,
		dist,
		1 / m_pGrid->dpp,
		raster.meanEdge/raster.size);
	if (m_pGrid->overlay & TRACEINFO)
	return trace;
	return NULL;

}

CD2DPointF CIGUIDEDoc::compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount)
{
	CD2DPointF centroid = { 0, 0 };
	float signedArea = 0.0;
	float x0 = 0.0; // Current vertex X
	float y0 = 0.0; // Current vertex Y
	float x1 = 0.0; // Next vertex X
	float y1 = 0.0; // Next vertex Y
	float a = 0.0;  // Partial signed area

					// For all vertices
	int i = 0;
	for (i = 0; i<vertexCount; ++i)
	{
		x0 = vertices[i].x;
		y0 = vertices[i].y;
		x1 = vertices[(i + 1) % vertexCount].x;
		y1 = vertices[(i + 1) % vertexCount].y;
		a = x0*y1 - x1*y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;
	}

	signedArea *= 0.5;
	centroid.x /= (6.0*signedArea);
	centroid.y /= (6.0*signedArea);

	return centroid;

}


float CIGUIDEDoc::ComputeDisplacementAngle(Edge k) {

	float a, b;
	float alpha;
	float pi = atan(1) * 4;



	if (k.q.x >= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(a / b)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.p.y - k.q.y);
		alpha = (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x >= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.p.y - k.q.y);
		alpha = (atan(a / b)) * 180 / pi;
	}

	return alpha;

}

float CIGUIDEDoc::ComputeOrientationAngle(Edge k) {

	float a, b;
	float alpha;
	float pi = atan(1) * 4;

	if (k.q.x >= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y <= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.q.y - k.p.y);
		alpha = 90 + (atan(a / b)) * 180 / pi;
	}

	else if (k.q.x <= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.p.y - k.q.y);
		alpha = 180 + (atan(b / a)) * 180 / pi;
	}

	else if (k.q.x >= k.p.x && k.q.y >= k.p.y) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.p.y - k.q.y);
		alpha = 270 + (atan(a / b)) * 180 / pi;
	}

	return alpha;

}

void CIGUIDEDoc::ComputeDisplacementAngles() {


	ATLTRACE(_T("\n"));
	Edge k = raster.perimeter[0];
	float theta = ComputeOrientationAngle(k);

	for (int i = 0; i < raster.perimeter.size(); i++) {
		raster.perimeter[i].alpha = ComputeDisplacementAngle(raster.perimeter[i]);
		if (theta > 90 && theta <= 180)
			raster.perimeter[i].alpha += 90;
		if (theta > 180 && theta <= 270)
			raster.perimeter[i].alpha += 180;
		if (theta > 270 && theta <= 360)
			raster.perimeter[i].alpha += 270;
	}
		for (int i = 0; i < raster.perimeter.size(); i++)
			ATLTRACE(_T("alpha is %f\n"), raster.perimeter[i].alpha);

}


void CIGUIDEDoc::OnFileImport()
{
	// TODO: Add your command handler code here

	m_pFundus->calibration = FALSE;

	if (nullptr != m_pFundus->picture)
	{
		m_pFundus->picture->Destroy();
	}

	HRESULT hr = m_pFundus->_ShowWICFileOpenDialog(AfxGetMainWnd()->GetSafeHwnd());
	if (hr != S_OK)
		return;

	UpdateAllViews(NULL);
	m_pDlgCalibration->DoModal();
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnEditProperties()
{
	m_pDlgProperties->DoModal();
	// TODO: Add your command handler code here
}

void CIGUIDEDoc::OnOverlayGrid()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & GRID)
		m_pGrid->overlay = m_pGrid->overlay & (~GRID);
	else
		m_pGrid->overlay = m_pGrid->overlay | GRID;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayGrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & GRID);
}


void CIGUIDEDoc::OnOverlayRadius()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & DEGRAD)
		m_pGrid->overlay = m_pGrid->overlay & (~DEGRAD);
	else
		m_pGrid->overlay = m_pGrid->overlay | DEGRAD;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayRadius(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & DEGRAD);
}


void CIGUIDEDoc::OnOverlayFovea()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & FOVEA)
		m_pGrid->overlay = m_pGrid->overlay & ~FOVEA & ~FOVEOLA;
	else
		m_pGrid->overlay = m_pGrid->overlay | FOVEA | FOVEOLA;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayFovea(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & FOVEA);
}


void CIGUIDEDoc::OnOverlayOpticdisc()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & OPTICDISC)
		m_pGrid->overlay = m_pGrid->overlay & (~OPTICDISC);
	else
		m_pGrid->overlay = m_pGrid->overlay | OPTICDISC;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayOpticdisc(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & OPTICDISC);
}


void CIGUIDEDoc::OnOverlayCrosshair()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & CROSSHAIR)
		m_pGrid->overlay = m_pGrid->overlay & (~CROSSHAIR);
	else
		m_pGrid->overlay = m_pGrid->overlay | CROSSHAIR;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayCrosshair(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & CROSSHAIR);
}


void CIGUIDEDoc::OnOverlayFundus()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & FUNDUS)
		m_pGrid->overlay = m_pGrid->overlay & (~FUNDUS);
	else
		m_pGrid->overlay = m_pGrid->overlay | FUNDUS;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayFundus(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & FUNDUS);
}


void CIGUIDEDoc::OnOverlayTraceinfo()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & TRACEINFO)
		m_pGrid->overlay = m_pGrid->overlay & (~TRACEINFO);
	else
		m_pGrid->overlay = m_pGrid->overlay | TRACEINFO;
	UpdateAllViews(NULL);
}


void CIGUIDEDoc::OnUpdateOverlayTraceinfo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & TRACEINFO);
}

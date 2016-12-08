
// GridTargetsDoc.cpp : implementation of the CGridTargetsDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GridTargets.h"
#include "Fundus.h"
#endif

#include "GridTargetsDoc.h"
#include "Grid.h"
#include <math.h>

#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace D2D1;

// CGridTargetsDoc

IMPLEMENT_DYNCREATE(CGridTargetsDoc, CDocument)

BEGIN_MESSAGE_MAP(CGridTargetsDoc, CDocument)
	ON_COMMAND(ID_FILE_IMPORT, &CGridTargetsDoc::OnFileImport)
	ON_COMMAND(ID_EDIT_PROPERTIES, &CGridTargetsDoc::OnEditProperties)
	ON_COMMAND(ID_OVERLAY_GRID, &CGridTargetsDoc::OnOverlayGrid)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_GRID, &CGridTargetsDoc::OnUpdateOverlayGrid)
	ON_COMMAND(ID_OVERLAY_RADIUS, &CGridTargetsDoc::OnOverlayRadius)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_RADIUS, &CGridTargetsDoc::OnUpdateOverlayRadius)
	ON_COMMAND(ID_OVERLAY_FOVEA, &CGridTargetsDoc::OnOverlayFovea)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_FOVEA, &CGridTargetsDoc::OnUpdateOverlayFovea)
	ON_COMMAND(ID_OVERLAY_OPTICDISC, &CGridTargetsDoc::OnOverlayOpticdisc)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_OPTICDISC, &CGridTargetsDoc::OnUpdateOverlayOpticdisc)
	ON_COMMAND(ID_OVERLAY_CROSSHAIR, &CGridTargetsDoc::OnOverlayCrosshair)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_CROSSHAIR, &CGridTargetsDoc::OnUpdateOverlayCrosshair)
	ON_COMMAND(ID_OVERLAY_FUNDUS, &CGridTargetsDoc::OnOverlayFundus)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_FUNDUS, &CGridTargetsDoc::OnUpdateOverlayFundus)
	ON_COMMAND(ID_OVERLAY_TRACEINFO, &CGridTargetsDoc::OnOverlayTraceinfo)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_TRACEINFO, &CGridTargetsDoc::OnUpdateOverlayTraceinfo)
END_MESSAGE_MAP()


// CGridTargetsDoc construction/destruction

CGridTargetsDoc::CGridTargetsDoc()
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

CGridTargetsDoc::~CGridTargetsDoc()
{
	delete m_pGrid;
	delete m_pFundus;
	delete mousePos;
	delete m_pDlgCalibration;
	delete m_pDlgProperties;

}

// Get Doc, made for other classes that need access to attributes

CGridTargetsDoc* CGridTargetsDoc::GetDoc()
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
				return (CGridTargetsDoc *)pDoc;
			}
		}
	}

}


BOOL CGridTargetsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;

}


// CGridTargetsDoc serialization

void CGridTargetsDoc::Serialize(CArchive& ar)
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
void CGridTargetsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CGridTargetsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGridTargetsDoc::SetSearchContent(const CString& value)
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

// CGridTargetsDoc diagnostics

#ifdef _DEBUG
void CGridTargetsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGridTargetsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGridTargetsDoc commands


BOOL CGridTargetsDoc::CheckFOV()
{

	if (raster.corner.size() < 4) {
		AfxMessageBox(_T("Please draw raster corners (clockwise) in Target View window first!"), MB_OK);
		ShowCursor(TRUE);
		return FALSE;
	}
	return TRUE;

}

float CGridTargetsDoc::CalcEdgeLength(Edge k) {
	
	float a, b, c;

	a = abs(k.q.x - k.p.x);
	b = abs(k.q.y - k.p.y);

	c = sqrt(pow(a, 2) + pow(b, 2));
	
	return c;

}

CString CGridTargetsDoc::getTraceInfo() {

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

CD2DPointF CGridTargetsDoc::compute2DPolygonCentroid(const CD2DPointF* vertices, int vertexCount)
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


float CGridTargetsDoc::ComputeDisplacementAngle(Edge k) {

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

float CGridTargetsDoc::ComputeOrientationAngle(Edge k) {

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

void CGridTargetsDoc::ComputeDisplacementAngles() {


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


void CGridTargetsDoc::OnFileImport()
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


void CGridTargetsDoc::OnEditProperties()
{
	m_pDlgProperties->DoModal();
	// TODO: Add your command handler code here
}

void CGridTargetsDoc::OnOverlayGrid()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & GRID)
		m_pGrid->overlay = m_pGrid->overlay & (~GRID);
	else
		m_pGrid->overlay = m_pGrid->overlay | GRID;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayGrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & GRID);
}


void CGridTargetsDoc::OnOverlayRadius()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & DEGRAD)
		m_pGrid->overlay = m_pGrid->overlay & (~DEGRAD);
	else
		m_pGrid->overlay = m_pGrid->overlay | DEGRAD;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayRadius(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & DEGRAD);
}


void CGridTargetsDoc::OnOverlayFovea()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & FOVEA)
		m_pGrid->overlay = m_pGrid->overlay & ~FOVEA & ~FOVEOLA;
	else
		m_pGrid->overlay = m_pGrid->overlay | FOVEA | FOVEOLA;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayFovea(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & FOVEA);
}


void CGridTargetsDoc::OnOverlayOpticdisc()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & OPTICDISC)
		m_pGrid->overlay = m_pGrid->overlay & (~OPTICDISC);
	else
		m_pGrid->overlay = m_pGrid->overlay | OPTICDISC;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayOpticdisc(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & OPTICDISC);
}


void CGridTargetsDoc::OnOverlayCrosshair()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & CROSSHAIR)
		m_pGrid->overlay = m_pGrid->overlay & (~CROSSHAIR);
	else
		m_pGrid->overlay = m_pGrid->overlay | CROSSHAIR;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayCrosshair(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & CROSSHAIR);
}


void CGridTargetsDoc::OnOverlayFundus()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & FUNDUS)
		m_pGrid->overlay = m_pGrid->overlay & (~FUNDUS);
	else
		m_pGrid->overlay = m_pGrid->overlay | FUNDUS;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayFundus(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & FUNDUS);
}


void CGridTargetsDoc::OnOverlayTraceinfo()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & TRACEINFO)
		m_pGrid->overlay = m_pGrid->overlay & (~TRACEINFO);
	else
		m_pGrid->overlay = m_pGrid->overlay | TRACEINFO;
	UpdateAllViews(NULL);
}


void CGridTargetsDoc::OnUpdateOverlayTraceinfo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & TRACEINFO);
}

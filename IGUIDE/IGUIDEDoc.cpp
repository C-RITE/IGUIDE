
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
	mousePos = NULL;
	raster.size = 1.28;
	raster.meanAlpha = 0;
	m_pDlgCalibration = new Calibration();
	m_ScreenPixelPitch = 96;
	m_ScreenDistance = 100;
	m_FixationTargetSize = 100;
	getScreens();

}


bool CIGUIDEDoc::getScreens() {

	Monitors monitors;
	if (m_Screens.size() > 0)	// change in display configuration requires empty vector
		m_Screens.clear();
	m_Screens = monitors.getScreens();
	return true;

}

CIGUIDEDoc::~CIGUIDEDoc()
{
	
	delete m_pGrid;
	delete m_pFundus;
	delete mousePos;
	delete m_pDlgCalibration;
		
}

// Get Doc, made for other classes that need access to attributes

CIGUIDEDoc* CIGUIDEDoc::GetDoc()
{

	CFrameWndEx * pFrame = (CFrameWndEx *)(AfxGetApp()->m_pMainWnd);
	return (CIGUIDEDoc *)pFrame->GetActiveDocument();
	
}

BOOL CIGUIDEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	CString path;
	int FTS, SCR;

	path = AfxGetApp()->GetProfileString(L"Settings", L"FixationTarget", NULL);
	if (path.IsEmpty()) {
		WCHAR homedir[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, homedir))) {
			path = homedir;
			path.Append(_T("\\Pictures\\"));
		}
	}
	m_FixationTarget = path;

	path = AfxGetApp()->GetProfileString(L"Settings", L"OutputDir", NULL);
	if (path.IsEmpty()) {
			path.Append(_T("C:\\"));
		}

	m_OutputDir = path;

	FTS = AfxGetApp()->GetProfileInt(L"Settings", L"FixationTargetSize", 0);
	if (!FTS) FTS = 100;
	m_FixationTargetSize = FTS;

	SCR = AfxGetApp()->GetProfileInt(L"Settings", L"Display", 0);
	if (!SCR) SCR = 1;
	m_FixationTargetScreen = SCR;

	UINT nl;
	LPBYTE calib, ptr;
	DWORD sz = sizeof(CD2DPointF);
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Calibration", &calib, &nl) > 0) {
		CD2DPointF data;
		ptr = calib;
		for (size_t t = 0; t < nl / sz; t++) {
			data = (CD2DPointF*)calib;
			raster.corner.push_back(data);
			calib += sz;
		}
	calib = ptr;
	}

	LPBYTE rcol;
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"RasterColor", &rcol, &nl) > 0)
		memcpy(&raster.color, rcol, sizeof(D2D1_COLOR_F));

	LPBYTE rsize;
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"RasterSize", &rsize, &nl)> 0)
		memcpy(&raster.size, rsize, sizeof(double));

	delete calib,ptr;
	delete rcol;
	delete rsize;

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


bool CIGUIDEDoc::CheckFOV()
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

//	POSITION pos = GetFirstViewPosition();
//	CWnd* target = GetNextView(pos);
	CString trace;
	Edge k;

	if (!m_pGrid->patchlist.empty()) {
		k.q.x = m_pGrid->patchlist.back().coords.x;
		k.q.y = m_pGrid->patchlist.back().coords.y;
	}

	double beta = 360 - ComputeOrientationAngle(k);
	double dist = m_pGrid->center.x - ((m_pGrid->nerve.right - m_pGrid->nerve.left) / 2);

	trace.Format(L"alpha:\t\t%f (deg)\nbeta:\t\t%f (deg)\ngamma:\t\t%f (deg)\nsize:\t\t%f (deg)\nscale.x:\t%f\nscale.y:\t%f\nfov2disc:\t%f (px)\nhost ppd:\t%f\nclient ppd:\t%f",
		raster.meanAlpha,
		beta,
		raster.meanAlpha + beta,
		raster.size,
		raster.scale.x,
		raster.scale.y,
		dist,
		1 / m_pGrid->dpp,
		raster.meanEdge/raster.size
	);

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
	centroid.x /= (6.0f*signedArea);
	centroid.y /= (6.0f*signedArea);

	return centroid;

}


double CIGUIDEDoc::ComputeDisplacementAngle(Edge k) {

	double a, b;
	double alpha = 0;
	double pi = atan(1) * 4;


	if (abs(k.p.y - k.q.y) < abs(k.p.x - k.q.x)) {
		a = abs(k.q.x - k.p.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(b / a)) * 180 / pi;
	}

	else if (abs(k.p.y - k.q.y) > abs(k.p.x - k.q.x)) {
		a = abs(k.p.x - k.q.x);
		b = abs(k.q.y - k.p.y);
		alpha = (atan(a / b)) * 180 / pi;
	}

	return alpha;

}

double CIGUIDEDoc::ComputeOrientationAngle(Edge k) {

	if (k.q.x == 0 && k.q.y == 0)
		return 0;

	double a, b;
	double alpha;
	double pi = atan(1) * 4;

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
	double theta = ComputeOrientationAngle(k);

	for (size_t i = 0; i < raster.perimeter.size(); i++) {
		raster.perimeter[i].alpha = ComputeDisplacementAngle(raster.perimeter[i]);
		if (theta > 90 && theta <= 180)
			raster.perimeter[i].alpha = 180 - raster.perimeter[i].alpha;
		if (theta > 180 && theta <= 270)
			raster.perimeter[i].alpha = 270 - raster.perimeter[i].alpha;
		if (theta > 270 && theta <= 360)
			raster.perimeter[i].alpha = 360 - raster.perimeter[i].alpha;
	}
		for (size_t i = 0; i < raster.perimeter.size(); i++)
			ATLTRACE(_T("alpha is %f\n"), raster.perimeter[i].alpha);

}

bool CIGUIDEDoc::CheckCalibrationValidity()
{

	//calc mean of all angles
	double sum = 0;
	for (size_t i = 0; i < raster.perimeter.size(); i++)
		sum += raster.perimeter[i].alpha;
	double mean = sum / raster.perimeter.size();
	
	//calc variance
	double temp = 0;
	for (size_t i = 0; i < raster.perimeter.size(); i++)
		temp += (raster.perimeter[i].alpha - mean)*(raster.perimeter[i].alpha - mean);
	double var = temp / raster.perimeter.size();

	//calc standard deviation
	double stddev = sqrt(var);

	if (stddev > 5)
		return false;
	return true;

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

	UpdateAllViews(NULL);
	if (!m_pFundus->picture->IsValid()) {
		CStringW message;
		message.Format(L"Failed to load image!\n%s", *m_pFundus->filename);
		AfxMessageBox(message, MB_OK);
		delete m_pFundus->filename;
		return;
	}
	m_pDlgCalibration->DoModal();
	UpdateAllViews(NULL);
	
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



void CIGUIDEDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your message handler code here
	
	AfxGetApp()->WriteProfileInt(L"Settings", L"Overlays", (int)(m_pGrid->overlay));
	AfxGetApp()->WriteProfileString(L"Settings", L"FixationTarget", m_FixationTarget);
	AfxGetApp()->WriteProfileString(L"Settings", L"OutputDir", m_OutputDir);
	AfxGetApp()->WriteProfileInt(L"Settings", L"FixationTargetSize", m_FixationTargetSize);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"RasterSize", (LPBYTE)&raster.size, sizeof(double));
	const DWORD dataSize = static_cast<DWORD>(raster.corner.size() * sizeof(CD2DPointF));
	if (raster.corner.size() == 4) AfxGetApp()->WriteProfileBinary(L"Settings", L"Calibration", (LPBYTE)&raster.corner[0].x, dataSize);
	AfxGetApp()->WriteProfileInt(L"Settings", L"Display", m_FixationTargetScreen);
	D2D1_COLOR_F rcol = raster.color;
	AfxGetApp()->WriteProfileBinary(L"Settings", L"RasterColor", (LPBYTE)&rcol, sizeof(rcol));
	CDocument::OnCloseDocument();

}
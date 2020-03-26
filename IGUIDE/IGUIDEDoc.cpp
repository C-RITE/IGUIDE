
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
#include "MainFrm.h"
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
	ON_COMMAND(ID_OVERLAY_GRID, &CIGUIDEDoc::OnOverlayGrid)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_GRID, &CIGUIDEDoc::OnUpdateOverlayGrid)
	ON_COMMAND(ID_OVERLAY_PATCHES, &CIGUIDEDoc::OnOverlayPatches)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_PATCHES, &CIGUIDEDoc::OnUpdateOverlayPatches)
	ON_COMMAND(ID_OVERLAY_OPTICDISC, &CIGUIDEDoc::OnOverlayOpticdisc)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_OPTICDISC, &CIGUIDEDoc::OnUpdateOverlayOpticdisc)
	ON_COMMAND(ID_OVERLAY_CROSSHAIR, &CIGUIDEDoc::OnOverlayCrosshair)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_CROSSHAIR, &CIGUIDEDoc::OnUpdateOverlayCrosshair)
	ON_COMMAND(ID_OVERLAY_FUNDUS, &CIGUIDEDoc::OnOverlayFundus)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_FUNDUS, &CIGUIDEDoc::OnUpdateOverlayFundus)

#ifdef DEBUG
	ON_COMMAND(ID_OVERLAY_TRACEINFO, &CIGUIDEDoc::OnOverlayTraceinfo)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_TRACEINFO, &CIGUIDEDoc::OnUpdateOverlayTraceinfo)
#endif // DEBUG

	ON_COMMAND(ID_OVERLAY_QUICKHELP, &CIGUIDEDoc::OnOverlayQuickhelp)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_QUICKHELP, &CIGUIDEDoc::OnUpdateOverlayQuickhelp)
	ON_COMMAND(ID_OVERLAY_LOCATION, &CIGUIDEDoc::OnOverlayLocation)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_LOCATION, &CIGUIDEDoc::OnUpdateOverlayLocation)
	ON_COMMAND(ID_OVERLAY_TARGETZONE, &CIGUIDEDoc::OnOverlayTargetzone)
	ON_UPDATE_COMMAND_UI(ID_OVERLAY_TARGETZONE, &CIGUIDEDoc::OnUpdateOverlayTargetzone)
END_MESSAGE_MAP()


// CIGUIDEDoc construction/destruction

CIGUIDEDoc::CIGUIDEDoc()
{
	// TODO: add one-time construction code here

	m_pGrid = new Grid();
	m_pFundus = new Fundus();
	m_pDlgCalibration = new Calibration();
	m_raster.meanAlpha = 0;
	m_RemoteCtrl = L"NONE";
	m_InputController = L"Mouse";

	overlaySettings = 0;
	defocus = L"0";

	overlayVisible = true;
	calibrationComplete = false;
		
}


CIGUIDEDoc::~CIGUIDEDoc()
{
	m_Controller.shutdown();
	delete m_pGrid;
	delete m_pFundus;
	delete m_pDlgCalibration;

}


BOOL CIGUIDEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	CString data;

	// parse settings stored in registry or set default

	data = AfxGetApp()->GetProfileString(L"Settings", L"FixationTarget", NULL);
	if (data.IsEmpty()) {
		WCHAR homedir[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, homedir))) {
			data = homedir;
			data.Append(_T("\\Pictures\\"));
		}
	}

	m_FixationTarget = data;

	data = AfxGetApp()->GetProfileString(L"Settings", L"OutputDir", NULL);
	if (data.IsEmpty()) {
		WCHAR homedir[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, homedir))) {
			data = homedir;
			data.Append(_T("\\Videos\\"));
		}
	}

	m_OutputDir = data;

	data = AfxGetApp()->GetProfileString(L"Settings", L"FundusFolder", NULL);
	if (data.IsEmpty()) {
		WCHAR homedir[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, homedir))) {
			data = homedir;
		}
	}

	m_pFundus->mru_folder = data;

	data = AfxGetApp()->GetProfileString(L"Settings", L"AOSACA IP", NULL);
	if (data.IsEmpty()) {
		data.Format(_T("192.168.0.1"));
	}

	m_AOSACA_IP = data;

	data = AfxGetApp()->GetProfileString(L"Settings", L"ICANDI IP", NULL);
	if (data.IsEmpty()) {
		data.Format(_T("127.0.0.1"));
	}

	m_ICANDI_IP = data;

	m_pGrid->overlay = AfxGetApp()->GetProfileInt(L"Settings", L"Overlays", -1);
	if (m_pGrid->overlay == -1) {
		m_pGrid->overlay = 7;
	}

	m_FixationTargetSize = AfxGetApp()->GetProfileInt(L"Settings", L"FixationTargetSize", 100);
	
	int screen = AfxGetApp()->GetProfileInt(L"Settings", L"Display", -1);
	for (auto it = m_Monitors.m_Devices.begin(); it != m_Monitors.m_Devices.end(); it++) {
		if (it->number == screen) {
			m_Monitors.m_pSelectedDevice = it._Ptr;
		}
	}

	m_FlipVertical = AfxGetApp()->GetProfileString(L"Settings", L"FlipVertical", L"False");
	m_FlipHorizontal = AfxGetApp()->GetProfileString(L"Settings", L"FlipHorizontal", L"False");
	m_RemoteCtrl = AfxGetApp()->GetProfileString(L"Settings", L"RemoteControl", L"NONE");

	UINT nl;
	LPBYTE calib, ptr;
	DWORD sz = sizeof(CD2DPointF);
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Calibration", &calib, &nl) > 0) {
		CD2DPointF data;
		ptr = calib;
		for (size_t t = 0; t < nl / sz; t++) {
			data = (CD2DPointF*)calib;
			m_raster.corner.push_back(data);
			calib += sz;
		}
		calib = ptr;
		if(CheckCalibrationValidity())
			UpdateAllViews(NULL);
	}

	m_InputController = AfxGetApp()->GetProfileString(L"Settings", L"Controller", L"Mouse");
	if (m_raster.corner.size() > 0) {
		if (m_InputController == L"Gamepad")
			m_Controller.m_bActive = false;
	}

	LPBYTE rcol;
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"RasterColor", &rcol, &nl) > 0)
		memcpy(&m_raster.color, rcol, sizeof(D2D1_COLOR_F));

	m_raster.size = AfxGetApp()->GetProfileInt(L"Settings", L"RasterSize", 600);
		
	delete calib, ptr;
	delete rcol;
	
	return TRUE;

}

void CIGUIDEDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your message handler code here

	AfxGetApp()->WriteProfileInt(L"Settings", L"Overlays", (int)(m_pGrid->overlay));
	if (m_Monitors.m_pSelectedDevice)
		AfxGetApp()->WriteProfileInt(L"Settings", L"Display", m_Monitors.m_pSelectedDevice->number);
	AfxGetApp()->WriteProfileInt(L"Settings", L"RasterSize", m_raster.size);
	AfxGetApp()->WriteProfileInt(L"Settings", L"FixationTargetSize", m_FixationTargetSize);
	AfxGetApp()->WriteProfileString(L"Settings", L"FixationTarget", m_FixationTarget);
	AfxGetApp()->WriteProfileString(L"Settings", L"OutputDir", m_OutputDir);
	AfxGetApp()->WriteProfileString(L"Settings", L"FundusFolder", m_pFundus->mru_folder);
	AfxGetApp()->WriteProfileString(L"Settings", L"AOSACA IP", m_AOSACA_IP);
	AfxGetApp()->WriteProfileString(L"Settings", L"ICANDI IP", m_ICANDI_IP);
	AfxGetApp()->WriteProfileString(L"Settings", L"Controller", m_InputController);
	AfxGetApp()->WriteProfileString(L"Settings", L"FlipVertical", m_FlipVertical);
	AfxGetApp()->WriteProfileString(L"Settings", L"FlipHorizontal", m_FlipHorizontal);
	AfxGetApp()->WriteProfileString(L"Settings", L"RemoteControl", m_RemoteCtrl);
	
	D2D1_COLOR_F rcol = m_raster.color;
	AfxGetApp()->WriteProfileBinary(L"Settings", L"RasterColor", (LPBYTE)&rcol, sizeof(rcol));

	CDocument::OnCloseDocument();

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

	if (m_raster.corner.size() < 4) {

		int answer = AfxGetMainWnd()->MessageBox(L"No subject calibration data.\nCalibrate now?", L"Attention", MB_ICONHAND | MB_YESNO);

		if (answer == IDYES) {
			PostMessage(CIGUIDEView::GetView()->m_hWnd, WM_KEYDOWN, VK_F12, 1);
			if (!m_Monitors.m_pSelectedDevice) {
				m_Monitors.selectionDialog();
				CIGUIDEView* pView = CIGUIDEView::GetView();
				pView->SendMessage(SCREEN_SELECTED);
			}
		}

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

	CString trace;
	Edge k;

	CIGUIDEView* view = CIGUIDEView::GetView();
	CD2DPointF center(CANVAS / 2, CANVAS / 2);
	
	if (!m_pGrid->patchlist.empty()) {
		k.q.x = m_pGrid->patchlist.back().coords.x;
		k.q.y = m_pGrid->patchlist.back().coords.y;
	}

	double beta = 360 - ComputeOrientationAngle(k);
	double dist = center.x - ((m_pGrid->nerve.right - m_pGrid->nerve.left) / 2);

	/*trace.Format(L"alpha:\t\t%f (deg)\nbeta:\t\t%f (deg)\ngamma:\t\t%f (deg)\nsize:\t\t%f (deg)\nscale.x:\t%f\nscale.y:\t%f\nfov2disc:\t%f (px)\nhost ppd:\t%f\nclient ppd:\t%f\nmousepos:\tx:%f,y:%f",
		m_raster.meanAlpha,
		beta,
		m_raster.meanAlpha + beta,
		m_raster.size,
		m_raster.scale.x,
		m_raster.scale.y,
		dist,
		PPD,
		m_raster.meanEdge/m_raster.size,
	);*/

	CD2DPointF mousepos = view->getMousePos();
	CD2DPointF mousedist = view->getMouseDist();
	CD2DPointF mousestart = view->getMouseStart();

	trace.Format(L"mouse_X:\t%f\nmouse_Y:\t%f\nmouse_start_X:\t%f\nmouse_start_Y:\t%f\nmouse_dst_X:\t%f\nmouse_dst_Y:\t%f", mousepos.x, mousepos.y, mousestart.x, mousestart.y, mousedist.x, mousedist.y);

	if (m_pGrid->overlay & TRACEINFO)
		return trace;

	return NULL;

}

vector<CString> CIGUIDEDoc::getQuickHelp() {

	CString helpArray[3];
	helpArray[0].Format(L"ICANDI hotkeys\n===============================\nKEY:\t\tACTION:\n\n<R>\t\tReset Ref.-Frame\n<SPACE>\t\tSave Video");
	helpArray[1].Format(L"IGUIDE hotkeys\n===============================\nKEY:\tACTION:\n\n<F1>\tToggle Quick Help\n<F2>\tToggle Overlays\n<F3>\tToggle Fixation Target\n<F12>\t(Re-)Calibrate Subject");
	helpArray[2].Format(L"AOSACA hotkeys\n===============================\nNUM-KEY:\tACTION:\n\n<ENTER>\t\tFlatten Mirror\n<+>\t\tIncrease Defocus\n<->\t\tDecrease Defocus\n<0>\t\tZeroize Defocus");

	vector<CString> help(helpArray, helpArray+3);
	
	return help;

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
	for (i = 0; i < vertexCount; ++i)
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
	Edge k = m_raster.perimeter[0];
	double theta = ComputeOrientationAngle(k);

	for (size_t i = 0; i < m_raster.perimeter.size(); i++) {
		m_raster.perimeter[i].alpha = ComputeDisplacementAngle(m_raster.perimeter[i]);
		if (theta > 90 && theta <= 180)
			m_raster.perimeter[i].alpha = 180 - m_raster.perimeter[i].alpha;
		if (theta > 180 && theta <= 270)
			m_raster.perimeter[i].alpha = 270 - m_raster.perimeter[i].alpha;
		if (theta > 270 && theta <= 360)
			m_raster.perimeter[i].alpha = 360 - m_raster.perimeter[i].alpha;
	}
		for (size_t i = 0; i < m_raster.perimeter.size(); i++)
			ATLTRACE(_T("alpha is %f\n"), m_raster.perimeter[i].alpha);

}

CD2DRectF CIGUIDEDoc::ComputeTargetZone() {

	CD2DRectF zone;
	
	CD2DPointF rasterMid = m_raster.mid;
	CD2DPointF monRes = m_Monitors.m_pSelectedDevice->resolution;
	
	float edgeLength = m_raster.meanEdge;

	CD2DSizeF dim;
	dim.width = monRes.x / edgeLength;
	dim.height = monRes.y / edgeLength;

	zone.top = rasterMid.y / edgeLength;
	zone.bottom = (monRes.y - rasterMid.y) / edgeLength;
	zone.left = rasterMid.x / edgeLength;
	zone.right = (monRes.x - rasterMid.x) / edgeLength;

	return zone;

}

bool CIGUIDEDoc::CheckCalibrationValidity()
{

	//calc mean of all angles
	double sum = 0;
	for (size_t i = 0; i < m_raster.perimeter.size(); i++)
		sum += m_raster.perimeter[i].alpha;
	double mean = sum / m_raster.perimeter.size();
	
	//calc variance
	double temp = 0;
	for (size_t i = 0; i < m_raster.perimeter.size(); i++)
		temp += (m_raster.perimeter[i].alpha - mean)*(m_raster.perimeter[i].alpha - mean);
	double var = temp / m_raster.perimeter.size();

	//calc standard deviation
	double stddev = sqrt(var);

	if (stddev > 5)
		return false;
	
	return calibrationComplete = true;

}

void CIGUIDEDoc::OnFundusImport()
{
	// TODO: Add your command handler code here

	m_pFundus->calibration = FALSE;
	m_pFundus->filename.SetString(L"");

	if (nullptr != m_pFundus->picture)
	{
		delete m_pFundus->picture;
		m_pFundus->picture = NULL;
	}

	m_pFundus->_ShowWICFileOpenDialog(AfxGetMainWnd()->GetSafeHwnd());

	UpdateAllViews(NULL);

	if (m_pFundus->picture) m_pDlgCalibration->DoModal();
	
	m_pGrid->overlay = m_pGrid->overlay | FUNDUS;

	UpdateAllViews(NULL);
}


void CIGUIDEDoc::ToggleOverlay()
{
	// TODO: Add your implementation code here.
	if (overlayVisible) {
		if ((m_pGrid->overlay & QUICKHELP) && (m_pGrid->overlay & FUNDUS)) {
			overlaySettings = m_pGrid->overlay;
			m_pGrid->overlay = QUICKHELP | FUNDUS;
		}
		else if (m_pGrid->overlay & FUNDUS) {
			overlaySettings = m_pGrid->overlay;
			m_pGrid->overlay = FUNDUS;
		}
		else if (m_pGrid->overlay & QUICKHELP) {
			overlaySettings = m_pGrid->overlay;
			m_pGrid->overlay = QUICKHELP;
		}
		else {
			overlaySettings = m_pGrid->overlay;
			m_pGrid->overlay = 0;
		}
		//UpdateAllViews(NULL);
		overlayVisible = false;
		return;
	}

	else {
		m_pGrid->overlay = overlaySettings;
		//UpdateAllViews(NULL);
		overlayVisible = true;
	}

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

void CIGUIDEDoc::OnOverlayPatches()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & PATCHES)
		m_pGrid->overlay = m_pGrid->overlay & (~PATCHES);
	else
		m_pGrid->overlay = m_pGrid->overlay | PATCHES;

	UpdateAllViews(NULL);
}

void CIGUIDEDoc::OnUpdateOverlayPatches(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & PATCHES);
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



void CIGUIDEDoc::OnOverlayQuickhelp()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & QUICKHELP) {
		m_pGrid->overlay = m_pGrid->overlay & (~QUICKHELP);
		overlaySettings = overlaySettings & (~QUICKHELP);
	}

	else {
		m_pGrid->overlay = m_pGrid->overlay | QUICKHELP;
		overlaySettings = overlaySettings | QUICKHELP;
	}
	
}

void CIGUIDEDoc::OnUpdateOverlayLocation(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & LOCATION);
}

void CIGUIDEDoc::OnOverlayLocation()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & LOCATION) {
		m_pGrid->overlay = m_pGrid->overlay & (~LOCATION);
		overlaySettings = overlaySettings & (~LOCATION);
	}

	else {
		m_pGrid->overlay = m_pGrid->overlay | LOCATION;
		overlaySettings = overlaySettings | LOCATION;
	}

}


void CIGUIDEDoc::OnUpdateOverlayQuickhelp(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & QUICKHELP);
}


void CIGUIDEDoc::OnOverlayTargetzone()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & CROSSHAIR)
		m_pGrid->overlay = m_pGrid->overlay & (~TARGETZONE);
	else
		m_pGrid->overlay = m_pGrid->overlay | TARGETZONE;

}


void CIGUIDEDoc::OnUpdateOverlayTargetzone(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & TARGETZONE);
}

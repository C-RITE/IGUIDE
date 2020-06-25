
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
	m_pCurrentOutputDir = NULL;
	m_raster.meanAlpha = 0;
	m_RemoteCtrl = L"NONE";
	m_InputController = L"Mouse";
	m_Overlap = 50;
	overlaySettings = 0;
	defocus = L"0";

	overlayVisible = true;
	calibrationComplete = false;

	createNetComThread();
		
}


CIGUIDEDoc::~CIGUIDEDoc()
{
	m_Controller.shutdown();

	SetEvent(m_hNetMsg[2]); // break netcom thread loop to shutdown gracefully
	WaitForSingleObject(m_hNetComThread, INFINITE);

	delete m_pGrid;
	delete m_pFundus;
	delete m_pDlgCalibration;
	delete[] m_pInputBuf;

	CloseHandle(m_hNetMsg[0]);
	CloseHandle(m_hNetMsg[1]);
	CloseHandle(m_hNetMsg[2]);
	delete[] m_hNetMsg;
	CloseHandle(m_hNetComThread);

}

void CIGUIDEDoc::createNetComThread() {

	m_hNetMsg = new HANDLE[3];
	m_hNetMsg[0] = CreateEvent(NULL, FALSE, FALSE, L"IGUIDE_GUI_NETCOMM_AOSACA_EVENT");
	m_hNetMsg[1] = CreateEvent(NULL, FALSE, FALSE, L"IGUIDE_GUI_NETCOMM_ICANDI_EVENT");
	m_hNetMsg[2] = CreateEvent(NULL, FALSE, FALSE, L"THREAD_EXIT");
	m_pInputBuf = new CString[2];
	m_hNetComThread = ::CreateThread(NULL, 0, ThreadNetMsgProc, this, 0, &m_thdID);

}


DWORD WINAPI CIGUIDEDoc::ThreadNetMsgProc(LPVOID lpParameter)
{
	CIGUIDEDoc *parent = (CIGUIDEDoc *)lpParameter;
	VERIFY(parent != NULL);
	
	// exit thread loop when true
	bool exit = false;

	CString input;
	NetMsg message;
	const int start = 0;
	int split;

	// Wait for the event to be signaled, then parse the message into queue
	while (!exit) {

		switch (::WaitForMultipleObjects(3, parent->m_hNetMsg, FALSE, INFINITE)) {

		case WAIT_OBJECT_0:			// AOSACA message
			input = parent->m_pInputBuf[0];
			split = input.Find(_T("#"), start);
			message.property = input.Mid(start, split);
			message.value = input.Mid(split + 1, input.GetLength());
			parent->digest(message);
			input.Empty();
			break;

		case WAIT_OBJECT_0 + 1:		// ICANDI message
			input = parent->m_pInputBuf[1];
			split = input.Find(_T("#"), start);
			message.property = input.Mid(start, split);
			message.value = input.Mid(split + 1, input.GetLength());
			parent->digest(message);
			input.Empty();
			break;

		case WAIT_OBJECT_0 + 2:
			exit = true;
			break;
		}

	}

	return 0L;

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
	if (m_Monitors.m_pSelectedDevice == NULL)
		m_Monitors.selectionDialog();

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

	m_Overlap = AfxGetApp()->GetProfileInt(L"Settings", L"Overlap", 50);
		
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
	AfxGetApp()->WriteProfileBinary(L"Settings", L"RasterColor", (LPBYTE)&m_raster.color, sizeof(D2D1_COLOR_F));
	AfxGetApp()->WriteProfileInt(L"Settings", L"Overlap", m_Overlap);
	
	CDocument::OnCloseDocument();

}


// CIGUIDEDoc serialization

void CIGUIDEDoc::Serialize(CArchive& ar)
{

	if (ar.IsStoring())
	{
		// TODO: add storing code here

		SerializeHeader(ar);

		// store data header
		if (header[0]) {
			CImage image(m_pFundus->fundusIMG);
			ImageArchive(&image, ar);
		}
		
		// store fundus
		if (header[1]) {
			FundusCalibArchive(ar);
		}

		// store patchlist
		if (header[2]) {
			PatchArchive(ar, &m_pGrid->patchlist);
		}

		// store patchjobs

		if (header[3]) {
			ar << m_pGrid->patchjobs.size();

			// store list of patchjobs

			for each (Patches p in m_pGrid->patchjobs)
				PatchArchive(ar, &p);

			// store current patchjob position

			ar << std::distance(m_pGrid->patchjobs.begin(), m_pGrid->currentPatchJob);
			
			// store current patch position inside patchjob

			ar << m_pGrid->getCurrentPatchJobPos();
			
			// store region pane insertion offset values for each job

			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			auto i = pMain->m_RegionPane.getPatchOffset();
			int value;
			
			ar << i.size();

			for each (value in i)
				ar << value;

		}

	}

	else
	{
		// TODO: add loading code here
		
		SerializeHeader(ar);
		
		// restore header

		if (header[0]) {
			ImageArchive(&m_pFundus->fundusIMG, ar);
			LoadFundus();
		}
		
		// restore fundus

		if (header[1])
			FundusCalibArchive(ar);
		
		// restore patchlist

		if (header[2]) {
			m_pGrid->patchlist.clear();
			PatchArchive(ar, &m_pGrid->patchlist);
			m_pGrid->patchlist.resetIndex();
		}

		// restore patchjobs

		if (header[3]) {
			m_pGrid->patchjobs.clear();
			
			size_t jobsize;
			ar >> jobsize;

			// restore list of patchjobs

			for (int i = 0; i < jobsize; i++) {
				Patches p;
				PatchArchive(ar, &p);
				m_pGrid->patchjobs.push_back(p);
			}

			// restore current patchjob position

			int pos;
			ar >> pos;
			auto currentPatchJob = m_pGrid->patchjobs.begin() + pos;

			m_pGrid->currentPatchJob = currentPatchJob;

			// restore current patch 

			int i = 0;
			ar >> pos;
			Patches::iterator patchIndex = m_pGrid->patchjobs.begin()->begin();
			
			while (i < pos) {
				patchIndex++;
				i++;
			}

			m_pGrid->currentPatch = patchIndex;

			// restore region pane insertion offset values for each job

			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

			i = 0;
			ar >> jobsize;
			vector<int>offset;
			int element;

			while (i < jobsize) {
				ar >> element;
				offset.push_back(element);
				i++;
			}

			pMain->m_RegionPane.setPatchOffset(offset);

		}

		restoreRegionPane();
		UpdateAllViews(NULL);

	}
	
}

void CIGUIDEDoc::restoreRegionPane() {

	AfxGetMainWnd()->SendMessage(CLEAR_REGIONPANE, NULL, NULL);
	
	auto itp = m_pGrid->patchlist.begin();

	for (auto it = m_pGrid->patchjobs.begin(); it != m_pGrid->patchjobs.end(); it++)
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {

			AfxGetMainWnd()->SendMessage(
				PATCH_TO_REGIONPANE,
				(WPARAM) & *it2,
				(LPARAM)it2->region);

			if ((itp->region == it2->region) && (it2->index > 0)) {

				AfxGetMainWnd()->SendMessage(
					UPDATE_REGIONPANE,
					(WPARAM)std::distance(it->begin(), it2),
					(LPARAM)NULL);

				AfxGetMainWnd()->SendMessage(
					PATCH_TO_REGIONPANE,
					(WPARAM) & *it2,
					(LPARAM)it2->region);

			}

			if (itp != std::prev(m_pGrid->patchlist.end()))
				itp++;

		}

	itp = m_pGrid->patchlist.begin();

	if (itp != m_pGrid->patchlist.end()) {

		while (itp != std::prev(m_pGrid->patchlist.end())) {

			if (itp->region == 0 && itp->index > 0)
				AfxGetMainWnd()->SendMessage(
					PATCH_TO_REGIONPANE,
					(WPARAM) & *itp,
					(LPARAM)0);
			itp++;

		}

	}

}

void CIGUIDEDoc::SerializeHeader(CArchive& ar) {

	if (ar.IsStoring()) {
		if (m_pFundus->fundus)
			header[0] = true;
		else
			header[0] = false;

		if (m_pFundus->calibration)
			header[1] = true;
		else
			header[1] = false;

		if (m_pGrid->patchlist.size() > 0)
			header[2] = true;
		else
			header[2] = false;

		if (m_pGrid->patchjobs.size() > 0)
			header[3] = true;
		else
			header[3] = false;

		for (int i = 0; i < sizeof(header); i++)
			ar << header[i];
	}

	else{

		for (int i = 0; i < sizeof(header); i++)
			ar >> header[i];

	}

}

void CIGUIDEDoc::PatchArchive(CArchive& ar, Patches* ps){

	int listSize;

	if (ar.IsStoring())
	{
		listSize = ps->size();
		ar << listSize;

		for (auto it = ps->begin(); it != ps->end(); it++) {
			ar << it->color;
			ar << it->coordsDEG;
			ar << it->coordsPX;
			ar << it->defocus;
			ar << it->index;
			ar << it->locked;
			ar << it->rastersize;
			ar << it->region;
			ar << it->timestamp;
			ar << it->vidfilename;
			ar << it->visited;
		}

	}

	else
	{
		ps->clear();
		Patch p;
		ar >> listSize;
		
		for (int i = 0; i < listSize; i++) {
			ar >> p.color;
			ar >> p.coordsDEG;
			ar >> p.coordsPX;
			ar >> p.defocus;
			ar >> p.index;
			ar >> p.locked;
			ar >> p.rastersize;
			ar >> p.region;
			ar >> p.timestamp;
			ar >> p.vidfilename;
			ar >> p.visited;

			ps->push_back(p);

		}

	}

}

void CIGUIDEDoc::FundusCalibArchive(CArchive& ar) {

	if (ar.IsStoring())
	{
		ar << m_pFundus->calibResult.p.x;
		ar << m_pFundus->calibResult.p.y;
		ar << m_pFundus->calibResult.q.x;
		ar << m_pFundus->calibResult.q.y;
		ar << m_pFundus->calibResult.length;
		ar << m_pDlgCalibration->m_sFactor;
	}

	else 
	{
		ar >> m_pFundus->calibResult.p.x;
		ar >> m_pFundus->calibResult.p.y;
		ar >> m_pFundus->calibResult.q.x;
		ar >> m_pFundus->calibResult.q.y;
		ar >> m_pFundus->calibResult.length;
		ar >> m_pDlgCalibration->m_sFactor;

		m_pFundus->calibration = true;

	}

}

void CIGUIDEDoc::ImageArchive(CImage* pImage, CArchive& ar)
{
	HRESULT hr;
	if (ar.IsStoring())
	{
		// create a stream
		IStream* pStream = SHCreateMemStream(NULL, 0);
		ASSERT(pStream != NULL);
		if (pStream == NULL)
			return;

		// write the image to a stream rather than file (the stream in this case is just a chunk of memory automatically allocated by the stream itself)
		pImage->Save(pStream, Gdiplus::ImageFormatPNG); // Note: IStream will automatically grow up as necessary.

// find the size of memory written (i.e. the image file size)
		STATSTG statsg;
		hr = pStream->Stat(&statsg, STATFLAG_NONAME);
		ASSERT(hr == S_OK);
		ASSERT(statsg.cbSize.QuadPart < ULONG_MAX);
		ULONG nImgBytes = ULONG(statsg.cbSize.QuadPart);    // any image that can be displayed had better not have more than MAX_ULONG bytes

// go to the start of the stream
		LARGE_INTEGER seekPos;
		seekPos.QuadPart = 0;
		hr = pStream->Seek(seekPos, STREAM_SEEK_SET, NULL);
		ASSERT(hr == S_OK);

		// get data in stream into a standard byte array
		char* bytes = new char[nImgBytes];
		ULONG nRead;
		hr = pStream->Read(bytes, nImgBytes, &nRead);   // read the data from the stream into normal memory.  nRead should be equal to statsg.cbSize.QuadPart.
		ASSERT(hr == S_OK);
		ASSERT(nImgBytes == nRead);

		// write data to archive and finish
		ar << nRead;    // need to save the amount of memory needed for the file, since we will need to read this amount later
		ar.Write(bytes, nRead);     // write the data to the archive file from the stream memory
		pStream->Release();
		delete[] bytes;
	}
	else
	{
		// get the data from the archive
		ULONG nBytes;
		ar >> nBytes;
		char* bytes = new char[nBytes]; // ordinary memory to hold data from archive file
		UINT nBytesRead = ar.Read(bytes, nBytes);   // read the data from the archive file
		ASSERT(nBytesRead == UINT(nBytes));

		// make the stream
		IStream* pStream = SHCreateMemStream(NULL, 0);
		ASSERT(pStream != NULL);
		if (pStream == NULL)
			return;

		// put the archive data into the stream
		ULONG nBytesWritten;
		pStream->Write(bytes, nBytes, &nBytesWritten);
		ASSERT(nBytes == nBytesWritten);
		if (nBytes != nBytesWritten)
			return;

		// go to the start of the stream
		LARGE_INTEGER seekPos;
		seekPos.QuadPart = 0;
		hr = pStream->Seek(seekPos, STREAM_SEEK_SET, NULL);
		ASSERT(hr == S_OK);

		// load the stream into CImage and finish
		pImage->Load(pStream);  // pass the archive data to CImage
		pStream->Release();
		delete[] bytes;
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

void CIGUIDEDoc::digest(NetMsg msg) {
	
	if (msg.property == L"ICANDI_VIDEOFOLDER") {
		m_OutputDir_ICANDI = msg.value;
		CMainFrame* main = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		SetEvent(main->m_hSaveEvent);
	}

	if (msg.property == L"AOSACA_DEFOCUS") {
		defocus = msg.value;
	}

}


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
		k.q.x = m_pGrid->patchlist.back().coordsDEG.x;
		k.q.y = m_pGrid->patchlist.back().coordsDEG.y;
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
	helpArray[1].Format(L"IGUIDE hotkeys\n===============================\nKEY:\tACTION:\n\n<F1>\tToggle Quick Help\n<F2>\tToggle Overlays\n<F3>\tToggle Fixation Target\n<F12>\t(Re-)Calibrate Subject\n<SHIFT+MW> Grow/Shrink POI\n<X+MW> Grow/Shrink POI in X\n<Y+MW> Grow/Shrink POI in Y\n<N>\tnext patch\n<B>\tprevious patch");
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

}

CD2DRectF CIGUIDEDoc::ComputeTargetZone() {

	if (!CheckCalibrationValidity() && !m_Monitors.m_pSelectedDevice)
		return CD2DRectF{ 0,0,0,0 };
	
	CD2DPointF rasterMid = m_raster.mid;
	CD2DSizeF monRes = m_Monitors.m_pSelectedDevice->resolution;
	
	float edgeLength = m_raster.meanEdge;

	CD2DSizeF dim;
	dim.width = monRes.width / edgeLength;
	dim.height = monRes.height / edgeLength;

	CD2DSizeF offset;
	offset.width = (monRes.width / 2 - rasterMid.x) / edgeLength;
	offset.height = (monRes.height / 2 - rasterMid.y) / edgeLength;

	CD2DRectF zone(
		(dim.width - offset.width) / 2,
		(dim.height - offset.height ) / 2,
		(dim.width + offset.width) / 2,
		(dim.height + offset.height) / 2);

	return zone;

}

void CIGUIDEDoc::LoadFundus() {

	if (!m_pFundus->filename.IsEmpty()) {
		m_pFundus->fundusIMG.Destroy();
		m_pFundus->fundusIMG.Load(m_pFundus->filename);
		m_pFundus->fundus = (HBITMAP)m_pFundus->fundusIMG;

	}

	else if (m_pFundus->fundusIMG) {
		if (m_pFundus->picture)
			return;
		m_pFundus->fundus = (HBITMAP)m_pFundus->fundusIMG;
	}

	UpdateAllViews(NULL);

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
	
	if (nullptr != m_pFundus->picture)
	{
		delete m_pFundus->picture;
		m_pFundus->picture = NULL;
	}

	m_pFundus->_ShowWICFileOpenDialog(AfxGetMainWnd()->GetSafeHwnd());

	LoadFundus();

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

	UpdateAllViews(NULL);

}


void CIGUIDEDoc::OnUpdateOverlayQuickhelp(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & QUICKHELP);

	UpdateAllViews(NULL);

}


void CIGUIDEDoc::OnOverlayTargetzone()
{
	// TODO: Add your command handler code here
	if (m_pGrid->overlay & TARGETZONE)
		m_pGrid->overlay = m_pGrid->overlay & (~TARGETZONE);
	else
		m_pGrid->overlay = m_pGrid->overlay | TARGETZONE;

	UpdateAllViews(NULL);

}


void CIGUIDEDoc::OnUpdateOverlayTargetzone(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGrid->overlay & TARGETZONE);
}

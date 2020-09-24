#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"

using namespace std;

Patches::Patches() : index(1), uID(0), regCount(0)
{
}

DWORD WINAPI Patches::ThreadWaitDigest(LPVOID lpParameter)
{
	CIGUIDEDoc *pDoc = (CIGUIDEDoc *)lpParameter;

	WaitForSingleObject(pDoc->m_hWaitDigest, INFINITE);
	pDoc->m_pGrid->currentPatch->timestamp = pDoc->timestamp;
	pDoc->m_pGrid->currentPatch->vidnumber = pDoc->vidnumber;
	pDoc->m_pGrid->currentPatch->index = _ttoi(pDoc->vidnumber);
	pDoc->m_pGrid->currentPatch->wavelength = pDoc->wavelength;
	pDoc->m_pGrid->currentPatch->vidfilename = pDoc->vidfilename;
	pDoc->m_pGrid->currentPatch->vidlength = pDoc->vidlength;

	// now we're ready to save
	SetEvent(pDoc->m_hSaveEvent);

	return 0L;

}

void Patches::commit(Patches::iterator patch) {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	
	// if connected to ICANDI, use same timestamp and vidnumber
	if (*pDoc->m_pActiveConnections == ICANDI || *pDoc->m_pActiveConnections == BOTH) {
		DWORD thdID;
		// wait until all messages from ICANDI are digested
		::CreateThread(NULL, 0, ThreadWaitDigest, pDoc, 0, &thdID);
	}
	
	else // use internal values
	{
		CString systime;
		CMainFrame::GetSysTime(systime);

		patch->timestamp = systime.GetString();
		patch->wavelength = L"N/A";
		patch->vidfilename = L"N/A";
		patch->vidlength = L"N/A";
		patch->vidnumber = L"N/A";
		patch->vidlength = L"N/A";
		patch->index = index++;

	}
	
	patch->locked = true;
	patch->defocus = pDoc->getCurrentDefocus();

	ATLTRACE(_T("current patch: uid[%d] index[%d] region[%d]\n"), patch->uID, patch->index, patch->region);
	
}

void Patches::makePatchMatrix(SIZE wheel, CPoint pos, CD2DPointF posDeg, float rsDeg) {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	D2D1_COLOR_F color = pDoc->m_raster.color;
	int rsize = pDoc->m_raster.size;

	// make m-by-n matrix of patches controlled by mousewheel

	for (float i = -(float)wheel.cx / 2; i < (float)wheel.cx / 2; i++) {

		Patch p;

		p.coordsDEG.x = posDeg.x + rsDeg * i + rsDeg / 2;
		p.coordsPX.x = (pos.x + rsDeg * PPD * i) + rsDeg * PPD / 2;

		for (float j = -(float)wheel.cy / 2; j < (float)wheel.cy / 2; j++) {

			p.coordsDEG.y = posDeg.y + rsDeg * j + rsDeg / 2;
			p.coordsPX.y = (pos.y + rsDeg * PPD * j) + rsDeg * PPD / 2;
			p.rastersize = rsize;
			p.color = color;
			p.locked = false;
			p.defocus = L"0";
			p.region = -1;

			this->push_back(p);

		}

	}

}

Patches::iterator Patches::add(Patch p)
{
	p.uID = uID++;
 	this->push_back(p);

	return std::prev(this->end());

}

Patches::iterator Patches::implant(Patches::iterator it, Patch p)
{
	p.uID = uID++;

	return this->insert(it, p);

}

int Patches::getProgress(int region, int &size) {
	
	int progress = 0;
	
	for (auto it = this->begin(); it != this->end(); it++) {
		if (it->region == region) {
			if (it->locked == true)
				progress++;
			size++;
		}
	}

	return progress;

}

void Patches::addRegion(Patches &region) {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

	float rsDeg = (float)pDoc->m_raster.videodim / pDoc->m_raster.size;
	
	regCount++;

	for (auto it = region.begin(); it != region.end(); it++) {

		it->region = regCount;
		it->index = -1;								// yet unknown at this point, thus -1
		this->add(*it);

		AfxGetMainWnd()->SendMessage(
			PATCH_TO_REGIONPANE,
			(WPARAM)&this->back(),
			(LPARAM)it->region);
	}
	
}

void Patches::reset() {

	uID = this->size();
	
	int index = 0;
	int regCount = 1;

	for (auto it = this->begin(); it != this->end(); it++) {
		if (it->index > index)
			index = it->index;
		if (it->region > regCount)
			regCount++;
	}
	
	this->index = ++index;
	this->regCount = regCount;

}

void Patches::setOverlap(float overlap, float rsDeg) {

	CD2DPointF mid; // middle of region matrix

	float dxf, dyf;

	mid.x = ((this->back().coordsDEG.x + rsDeg/2) + (this->front().coordsDEG.x - rsDeg/2)) / 2;
	mid.y = ((this->back().coordsDEG.y + rsDeg/2) + (this->front().coordsDEG.y - rsDeg/2)) / 2;

	for (auto it = this->begin(); it != this->end(); it++) {
		dxf = mid.x - it->coordsDEG.x;
		dyf = mid.y - it->coordsDEG.y;
		it->coordsDEG.x += dxf * overlap / 100;
		it->coordsDEG.y += dyf * overlap / 100;
	}

	mid.x = (this->back().coordsPX.x - this->front().coordsPX.x) / 2;
	mid.y = (this->back().coordsPX.y - this->front().coordsPX.y) / 2;

	for (auto it = this->begin(); it != this->end(); it++) {
		dxf = mid.x - it->coordsPX.x;
		dyf = mid.y - it->coordsPX.y;
		it._Ptr->coordsPX.x += dxf * (overlap / 100);
		it._Ptr->coordsPX.y += dyf * (overlap / 100);
	}

}

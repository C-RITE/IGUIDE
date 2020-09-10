#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"
#include <sstream>

using namespace std;


Patches::Patches() : filename(L"IGUIDE.csv"), fileTouched(false), index(1)
{
}

DWORD WINAPI Patches::ThreadWaitDigest(LPVOID lpParameter)
{
	CIGUIDEDoc *pDoc = (CIGUIDEDoc *)lpParameter;

	WaitForMultipleObjects(1, pDoc->m_hWaitDigest, TRUE, INFINITE);
	pDoc->m_pGrid->currentPatch->timestamp = pDoc->timestamp;
	pDoc->m_pGrid->currentPatch->index = _ttoi(pDoc->vidnumber);
	pDoc->m_pGrid->currentPatch->wavelength = pDoc->wavelength;

	// now we're ready to save
	SetEvent(pDoc->m_hSaveEvent);

	return 0L;

}

void Patches::GetSysTime(CString &buf) {

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y_%m_%d_%H_%M_%S", timeinfo);
	buf = buffer;

}

void Patches::commit(Patches::iterator &patch) {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();

	// if connected to ICANDI, use same timestamp and vidnumber for index
	if (*pDoc->m_pActiveConnections == ICANDI || *pDoc->m_pActiveConnections == BOTH) {
		DWORD thdID;
		// wait until all messages from ICANDI are digested
		::CreateThread(NULL, 0, ThreadWaitDigest, pDoc, 0, &thdID);
	}
	
	else // use internal values
	{
		CString systime;
		GetSysTime(systime);
		patch->timestamp = systime.GetString();
		patch->index = index++;
		patch->wavelength = L"N/A";
	}
	
	patch->locked = true;
	patch->defocus = pDoc->getCurrentDefocus();
	
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

void Patches::duplicate(Patches::iterator &current)
{
	current = this->insert(current, *current);
	current->locked = false;
	ATLTRACE(_T("patchlist: size[%d]\n"), this->size());
}

void Patches::untouch() {

	fileTouched = false;
	filename = "IGUIDE.csv";

}


bool Patches::SaveToFile(CString directory) {

	wstringstream sstream;
	CString strPrefix, strNumber, strDegX, strDegY, strDefocus, strWaveLength;
	CString header("subject(prefix),timestamp(YEAR_MONTH_DAY_HRS_MIN_SEC),video#,region,x-pos(deg),y-pos(deg),z-pos(defocus),wavelength(nm)");

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		if (it->locked) {

			strPrefix.Format(_T("%s"), subject);
			strNumber.Format(_T("%.3d"), it->index);
			strDegX.Format(_T("%.2f"), it->coordsDEG.x);
			strDegY.Format(_T("%.2f"), it->coordsDEG.y);
			strDefocus.Format(_T("%s"), it._Ptr->defocus);
			strWaveLength.Format(_T("%s"), it._Ptr->wavelength);
					   
			sstream
				<< subject.GetString()
				<< ","
				<< it->timestamp.GetString()
				<< ","
				<< "v" << strNumber.GetString()
				<< ","
				<< it->region
				<< ","
				<< strDegX.GetString()
				<< ","
				<< strDegY.GetString()
				<< ","
				<< strDefocus.GetString()
				<< ","
				<< strWaveLength.GetString()
				<< std::endl;

		}

	}
	//CFileDialog FileDlg(FALSE, L"csv", L"targets", OFN_OVERWRITEPROMPT, NULL, NULL, NULL, 1);

	/*if (FileDlg.DoModal() == IDOK) {
		CString pathName = FileDlg.GetPathName();
		CStdioFile outputFile(pathName, CFile::modeWrite | CFile::modeCreate | CFile::typeUnicode);
		outputFile.WriteString((sstream.str().c_str()));
		outputFile.Close();
		return TRUE;
	}*/


	if (!fileTouched) {
		GetSysTime(timestamp);
		filename = subject + "_" + timestamp + "_" + filename;
	}

	try {
		CStdioFile outputFile(directory + filename, CFile::typeText | CFile::modeWrite | CFile::modeCreate);
    		outputFile.WriteString(header + "\n" + sstream.str().c_str());
		outputFile.Close();
		fileTouched = true;
	}

	catch (CFileException* pe)
	{

		TCHAR szCause[255];
		CString strFormatted;
		pe->GetErrorMessage(szCause, 255);

		strFormatted = _T("The data file could not be saved because of this error: \n");
		strFormatted += szCause;
		AfxMessageBox(strFormatted);

		fileTouched = FALSE;
		filename = "IGUIDE.csv";

		pe->Delete();

		return FALSE;

	}

	return TRUE;

}
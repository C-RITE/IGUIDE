#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"
#include <sstream>

using namespace std;


Patches::Patches() : filename(L"IGUIDE.csv"), fileTouched(false), index(1)
{
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
	CString systime;
	GetSysTime(systime);

	patch->locked = true;
	patch->timestamp = systime.GetString();
	patch->index = index++;
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


void Patches::untouch() {

	fileTouched = false;
	filename = "IGUIDE.csv";

}


bool Patches::SaveToFile(CString directory) {

	wstringstream sstream;
	CString strNumber, strDegX, strDegY, strDefocus;
	CString header("YEAR_MONTH_DAY_HRS_MIN_SEC,#VIDEO,Region,Index,POSx(deg),POSy(deg),Defocus");

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		if (it->locked) {

			strNumber.Format(_T("%.3d"), it->index);
			strDegX.Format(_T("%.2f"), it._Ptr->coordsDEG.x);
			strDegY.Format(_T("%.2f"), it._Ptr->coordsDEG.y);
			strDefocus.Format(_T("%s"), it._Ptr->defocus);

			sstream
				<< it->timestamp.GetString()
				<< ","
				<< "v" << strNumber.GetString()
				<< ","
				<< it->region
				<< ","
				<< it->index
				<< ","
				<< strDegX.GetString()
				<< ","
				<< strDegY.GetString()
				<< ","
				<< strDefocus.GetString()
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
		filename = timestamp + "_" + filename;
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
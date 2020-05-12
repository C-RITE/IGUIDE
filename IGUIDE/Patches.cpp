#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"
#include <sstream>

using namespace std;


Patches::Patches() : filename(L"IGUIDE.csv"), fileTouched(false), finished(false), index(1)
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

bool Patches::commit() {

	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CString systime;
	GetSysTime(systime);

	if (this->back().locked == true)
		return false;

	this->back().locked = true;
	this->back().timestamp = systime.GetString();
	this->back().index = index;
	this->back().area = 1;
	this->back().defocus = pDoc->getCurrentDefocus();
	index++;

	last = this->back();

	cleanup();
	
	return true;

}

bool Patches::checkComplete() {

	// Check if all patches are commited (i.e. locked)
	finished = false;
	int locked = 0;

	for (auto it = this->begin(); it != this->end(); it++) {
		if (it->locked == true)
			locked++;
	}
	
	if (locked == this->size()) {
		finished = true;
	}

	return finished;

}

void Patches::revertLast() {

	this->push_back(last);
	this->back().locked = false;

}

void Patches::delPatch() {

	if (this->size() > 0) {
		this->last = this->back();
		this->pop_back();
	}

}

void Patches::setOverlap(float overlap, float rsDeg) {

	CD2DPointF mid; // middle of POI matrix

	float dxf, dyf;

	mid.x = (this->back().coordsDEG.x + rsDeg/2) + (this->front().coordsDEG.x - rsDeg/2) / 2;
	mid.y = (this->back().coordsDEG.y + rsDeg/2) + (this->front().coordsDEG.y - rsDeg/2) / 2;

#ifdef DEBUG
	ATLTRACE(_T("back patch (x,y)\t: %f, %f\n"), this->back().coordsDEG.x + rsDeg/2, this->back().coordsDEG.y + rsDeg/2);
	ATLTRACE(_T("front patch (x,y)\t: %f, %f\n"), this->front().coordsDEG.x - rsDeg / 2, this->front().coordsDEG.y - rsDeg / 2);
	return;
#endif // DEBUG

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
		it._Ptr->_Myval.coordsPX.x += dxf * (overlap / 100);
		it._Ptr->_Myval.coordsPX.y += dyf * (overlap / 100);
	}

}


void Patches::untouch() {

	fileTouched = false;
	filename = "IGUIDE.csv";

}

void Patches::cleanup() {

	for (auto it = this->begin(); it != this->end();) {
		if (!it->locked)	
			it = erase(it);
		else
			it++;
	}

}

bool Patches::SaveToFile(CString directory) {

	wstringstream sstream;
	int number = 1;
	CString strNumber, strDegX, strDegY, strDefocus;
	CString header("YEAR_MONTH_DAY_HRS_MIN_SEC,#VIDEO,Area,POSx(deg),POSy(deg),Defocus");

	for (auto it = this->begin(); it != this->end(); ++it)

	{

		strNumber.Format(_T("%.3d"), number++);
		strDegX.Format(_T("%.2f"), it._Ptr->_Myval.coordsDEG.x);
		strDegY.Format(_T("%.2f"), it._Ptr->_Myval.coordsDEG.y);
		strDefocus.Format(_T("%s"), it._Ptr->_Myval.defocus);

		sstream
			<< it->timestamp.GetString()
			<< ","
			<< "v" << strNumber.GetString()
			<< ","
			<< it->area
			<< ","
			<< strDegX.GetString()
			<< ","
			<< strDegY.GetString()
			<< ","
			<< strDefocus.GetString()
			<< std::endl;

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
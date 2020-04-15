#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"
#include <sstream>

using namespace std;


Patches::Patches() : filepath(L".\\"), filename(L"IGUIDE.csv"), fileTouched(FALSE)
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

	CString systime;
	GetSysTime(systime);

	if (this->back().locked == true)
		return false;

	this->back().locked = true;
	this->back().timestamp = systime.GetString();

	last = this->back();

	cleanup();
	SaveToFile();

	return true;

}

bool Patches::checkComplete() {

	// Check if all patches are commited (i.e. locked)
	int locked = 0;
	bool result = false;

	for (auto it = this->begin(); it != this->end(); it++) {
		if (it->locked == true)
			locked++;
	}
	
	if (locked == this->size())
		result = true;

	return result;

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

bool Patches::SaveToFile() {

	wstringstream sstream;
	int number = 1;
	CIGUIDEDoc* pDoc = CMainFrame::GetDoc();
	CString path = pDoc->m_OutputDir;
	filepath.Format(_T("%s"), path);
	CString strNumber, strDegX, strDegY, strDefocus;
	CString header("Data-Format: YEAR_MONTH_DAY_HRS_MIN_SEC, #VIDEO, POSx(deg), POSy(deg),Defocus");

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
		CStdioFile outputFile(filepath + filename, CFile::typeText | CFile::modeWrite | CFile::modeCreate);
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
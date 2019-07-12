#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include "MainFrm.h"
#include <sstream>
#include <hdf5.h>
#include <H5Cpp.h>

using namespace std;


Patches::Patches() : filepath(L".\\"), filename(L"IGUIDE.csv"), fileTouched(FALSE),numPatches(0)
{
}


Patches::~Patches()
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
	if (!fileTouched) {
		strftime(h5Name, 100, "%Y_%m_%d_%H_%M_%S_IGUIDE.h5", timeinfo);
	}

}

void Patches::lockIn(){
	CString systime;
	Patch dummy = this->back();
	GetSysTime(systime);
	this->back().locked = true;
	this->back().timestamp = systime.GetString();
	cleanup();
	SaveToFile();
	this->push_back(dummy);
	numPatches++;
}

char* Patches::getNumPatches() {
	char strNumPatches[3];
	sprintf(strNumPatches, "%.3d", numPatches);
	return strNumPatches;
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
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	CString path = pDoc->m_OutputDir;
	filepath.Format(_T("%s\\"), path);
	CString strNumber, strDegX, strDegY, strDefocus;
	CString header("Data-Format: YEAR_MONTH_DAY_HRS_MIN_SEC, #VIDEO, POSx(deg), POSy(deg),Defocus");

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		strNumber.Format(_T("%.3d"), number++);
		strDegX.Format(_T("%.2f"), it._Ptr->_Myval.coords.x);
		strDegY.Format(_T("%.2f"), it._Ptr->_Myval.coords.y);
		strDefocus.Format(_T("%.2f"), it._Ptr->_Myval.defocus);

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
		CString holder = L"_IGUIDE.h5";
		hid_t file = H5Fcreate(h5Name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		hid_t group = H5Gcreate(file, "/Patches", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Gclose(group);
		H5Fclose(file);
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
		return FALSE;

	}
	SaveSequence();
	return TRUE;
}

void Patches::SaveSequence() {
	hsize_t dimsf[2];
	dimsf[0] = 1;
	dimsf[1] = 3;
	try {
	hid_t dataspace = H5Screate_simple(2, dimsf, NULL);
	hid_t file = H5Fopen(h5Name, H5F_ACC_RDWR,H5P_DEFAULT);
	hid_t group = H5Gopen(file, "Patches", H5P_DEFAULT);
	//first one to write to it's own dataset; second to write into one dataset
	//hid_t dataset = H5Dcreate(group, this->getNumPatches(), H5T_NATIVE_FLOAT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	hid_t dataset = H5Dcreate(group, this->getNumPatches(), H5T_NATIVE_FLOAT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	float XYZ[3];
	XYZ[0] = this->back().coords.x;
	XYZ[1] = this->back().coords.y;
	XYZ[2] = _ttof(this->back().defocus);

	H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, XYZ);
	
	H5Dclose(dataset);
	H5Gclose(group);
	H5Fclose(file);
	}
	// catch failure caused by the H5File operations
	catch (H5::FileIException error)
	{
		error.printErrorStack();
	}
	// catch failure caused by the DataSet operations
	catch (H5::DataSetIException error)
	{
		error.printErrorStack();
	}
	// catch failure caused by the DataSpace operations
	catch (H5::DataSpaceIException error)
	{
		error.printErrorStack();
	}
	// catch failure caused by the DataSpace operations
	catch (H5::DataTypeIException error)
	{
		error.printErrorStack();
	}
	
}
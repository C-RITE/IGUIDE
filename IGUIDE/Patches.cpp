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

}

void Patches::lockIn(){
	CString systime;
	Patch dummy = this->back();
	GetSysTime(systime);
	this->back().locked = true;
	this->back().timestamp = systime.GetString();
	cleanup();
	//SaveToFile();
	SaveSequence();
	this->push_back(dummy);
	numPatches++;
}

int Patches::getNumPatches() {
	return numPatches;
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
	H5std_string FILE_NAME("test.h5");
	H5std_string DATASET_NAME("Patches");
	hsize_t dimsf[2];
	dimsf[0] = 10;
	dimsf[1] = 3;
	H5::DataSpace dataspace(2, dimsf);
	float XYZ[10][3] = { 0.0 };
	int i = 0;
	for (auto it = this->begin(); it != this->end(); ++it)
	{
		XYZ[i][0] = it._Ptr->_Myval.coords.x;
		XYZ[i][1] = it._Ptr->_Myval.coords.y;
		XYZ[i][2] = _ttof(it._Ptr->_Myval.defocus);
		i++;
	}
	
	try {
		H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);
		H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
		H5::DataSet dataset = file.createDataSet(DATASET_NAME, datatype, dataspace);
		dataset.write(XYZ, H5::PredType::NATIVE_FLOAT);
		H5Dclose(dataset.getId());
		H5Fclose(file.getId());
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
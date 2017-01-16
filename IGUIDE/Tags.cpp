#include "stdafx.h"
#include "Tags.h"
#include "IGUIDEDoc.h"
#include <sstream>

using namespace std;


Tags::Tags() : filepath(L".\\"), filename(L"IGUIDE.csv")
{
}


Tags::~Tags()
{
}

bool Tags::SaveToFile() {

	wstringstream sstream;
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	int number = 0;

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		sstream
			<< "v" << number 
			<< ";" 
			<< it._Ptr->_Myval.coords.x
			<< ";"
			<< it._Ptr->_Myval.coords.y
			<< "\n";
		number++;
	}

	//CFileDialog FileDlg(FALSE, L"csv", L"targets", OFN_OVERWRITEPROMPT, NULL, NULL, NULL, 1);

	/*if (FileDlg.DoModal() == IDOK) {
		CString pathName = FileDlg.GetPathName();
		CStdioFile outputFile(pathName, CFile::modeWrite | CFile::modeCreate | CFile::typeUnicode);
		outputFile.WriteString((sstream.str().c_str()));
		outputFile.Close();
		return TRUE;
	}*/
	CStdioFile outputFile(filepath + filename, CFile::modeWrite | CFile::modeCreate | CFile::typeUnicode);
	outputFile.WriteString((sstream.str().c_str()));
	outputFile.Close();

	return FALSE;

}
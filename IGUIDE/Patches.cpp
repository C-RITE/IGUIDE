#include "stdafx.h"
#include "Patches.h"
#include "IGUIDEDoc.h"
#include <sstream>

using namespace std;


Patches::Patches() : filepath(L".\\"), filename(L"IGUIDE.csv")
{
}


Patches::~Patches()
{
}

bool Patches::SaveToFile() {

	wstringstream sstream;
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	int number = 1;
	CString path = pDoc->m_pDlgProperties->VideoFolder.GetValue();
	filepath.Format(_T("%s\\"), path);
	CString strNumber, strDegX, strDegY;	
	CString header("Video number,deg(x),deg(y)");

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		strNumber.Format(_T("%.3d"), number++);
		strDegX.Format(_T("%.2f"), it._Ptr->_Myval.coords.x);
		strDegY.Format(_T("%.2f"), it._Ptr->_Myval.coords.y);
		
		sstream
			<< "v" << strNumber.GetString()
			<< ","
			<< strDegX.GetString()
			<< ","
			<< strDegY.GetString()
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
	try {
		CStdioFile outputFile(filepath + filename, CFile::modeWrite | CFile::modeCreate | CFile::typeText);
		outputFile.WriteString(header + "\n" + sstream.str().c_str());
		outputFile.Close();
	}
	catch (CFileException* pe)
	{
		TCHAR szCause[255];
		CString strFormatted;

		pe->GetErrorMessage(szCause, 255);

		strFormatted = _T("The data file could not be saved because of this error: \n");
		strFormatted += szCause;
		AfxMessageBox(strFormatted);
		return FALSE;
	}

	return TRUE;
}
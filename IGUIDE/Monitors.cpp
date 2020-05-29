#include "stdafx.h"
#include "Monitors.h"
#include "resource.h"
#include <iostream>

using namespace std;

static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{

	MONITORINFOEX monInf;
	monInf.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMon, &monInf);

	DISPLAY_DEVICE dd, dd2;
	dd.cb = sizeof(DISPLAY_DEVICE);
	dd2.cb = sizeof(DISPLAY_DEVICE);

	Monitors* pThis = reinterpret_cast<Monitors*>(pData);

	EnumDisplayDevices(NULL, pThis->devIndex, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
	EnumDisplayDevices(dd.DeviceName, 0, &dd2, EDD_GET_DEVICE_INTERFACE_NAME);

	pThis->screen.name = dd2.DeviceString;
	pThis->screen.display = monInf.szDevice;
	pThis->screen.monitor = monInf.dwFlags;
	pThis->screen.area = *lprcMonitor;
	pThis->screen.resolution = CPoint(monInf.rcMonitor.right - monInf.rcMonitor.left, monInf.rcMonitor.bottom - monInf.rcMonitor.top);
	pThis->screen.number = pThis->devIndex + 1;
	pThis->m_Devices.push_back(pThis->screen);
	pThis->devIndex++;

	return TRUE;

}

// Monitor selection dialog

class MonitorSelectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(MonitorSelectDialog)
public:
	MonitorSelectDialog();
	virtual ~MonitorSelectDialog();

	std::vector<Device>	Devices;
	CListBox			monitorList;
	size_t				selectedItem;
	CString				deviceName;				// used for selecting device from listbox

	
	void ReadDevices(std::vector<Device>monitors);

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECTSCREEN };
#endif
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
protected:

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnSelchangeMonitorlist();

};

BEGIN_MESSAGE_MAP(MonitorSelectDialog, CDialogEx)
	ON_LBN_SELCHANGE(IDC_MONITORLIST, &MonitorSelectDialog::OnSelchangeMonitorlist)
	ON_MESSAGE(WM_DISPLAYCHANGE, &MonitorSelectDialog::OnDisplayChange)
END_MESSAGE_MAP()


// MonitorSelectDialog implementation
IMPLEMENT_DYNAMIC(MonitorSelectDialog, CDialogEx)

MonitorSelectDialog::MonitorSelectDialog()
	: CDialogEx(IDD_SELECTSCREEN)
{
}

MonitorSelectDialog::~MonitorSelectDialog()
{
}

void MonitorSelectDialog::ReadDevices(std::vector<Device>monitors) {

	Devices = monitors;

}

BOOL MonitorSelectDialog::OnInitDialog() {

	CDialogEx::OnInitDialog();

	for (auto& screen : Devices) {
		CString dev;
		dev.Format(L"%d: %s (%ix%i)",screen.number, screen.name, screen.resolution.x, screen.resolution.y);
		monitorList.AddString(dev);
	}

	return true;
}



// Monitors class itself

Monitors::Monitors()
{
	refresh();
}

Monitors::~Monitors()
{
}

void Monitors::refresh() {

	m_pSelectedDevice = NULL;
	m_Devices.clear();
	devIndex = 0;

	EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);

	erasePrimaryDisplay(); 

}

void Monitors::erasePrimaryDisplay() {

	if (m_Devices.size() > 1)
		for (auto it = m_Devices.begin(); it != m_Devices.end(); it++)
			if (it->monitor == MONITORINFOF_PRIMARY) {
				it = m_Devices.erase(it);
				if (it == m_Devices.end())
					break;
			}

}

void Monitors::selectionDialog() {

	MonitorSelectDialog selectMonitorDlg;
	selectMonitorDlg.ReadDevices(m_Devices);
	int nRet = selectMonitorDlg.DoModal();

	// Handle the return value from DoModal
	switch (nRet)
	{
	case -1:
		AfxMessageBox(_T("Dialog box could not be created!"));
		break;
	case IDABORT:
		// Do something
		break;
	case IDOK:
		// Do something
		selectDeviceFromListBoxItem(selectMonitorDlg.deviceName);
		break;
	case IDCANCEL:
		// Do something
		break;
	default:
		// Do something
		break;
	};

}

void Monitors::selectDeviceFromListBoxItem(CString deviceName) {
	
	deviceName.Truncate(1);
	for (auto& screen : m_Devices) {
		CString number;
		number.Format(L"%d", screen.number);
		if (number == deviceName)
			m_pSelectedDevice = &screen;
	}

}


void MonitorSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	// TODO: Add your specialized code here and/or call the base class
	DDX_Control(pDX, IDC_MONITORLIST, monitorList);
	CDialogEx::DoDataExchange(pDX);
}


void MonitorSelectDialog::OnSelchangeMonitorlist()
{
	// TODO: Add your control notification handler code here
	CListBox* pMonitorList;
	pMonitorList = (CListBox*)GetDlgItem(IDC_MONITORLIST);
	selectedItem = pMonitorList->GetCurSel();
	pMonitorList->GetText(selectedItem, deviceName);
}

afx_msg LRESULT MonitorSelectDialog::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{

	this->EndDialog(0);
	return 0L;

}

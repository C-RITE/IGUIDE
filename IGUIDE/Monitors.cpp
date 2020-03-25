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
	pThis->screen.monitor = hMon;
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

	std::vector<Device>Devices;
	CListBox monitorList;
	void ReadDevices(std::vector<Device>monitors);

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECTSCREEN };
#endif
public:


	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);
};

BEGIN_MESSAGE_MAP(MonitorSelectDialog, CDialogEx)
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


void MonitorSelectDialog::OnOK() {

}

// Monitors class itself

Monitors::Monitors()
{
	m_pSelectedDevice = NULL;
	devIndex = 0;
	refresh();
}

Monitors::~Monitors()
{
}

void Monitors::refresh() {

	EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);

}

void Monitors::select() {

	MonitorSelectDialog selectMonitorDlg;
	selectMonitorDlg.ReadDevices(m_Devices);
	selectMonitorDlg.DoModal();

}


void MonitorSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	// TODO: Add your specialized code here and/or call the base class
	DDX_Control(pDX, IDC_LIST1, monitorList);
	CDialogEx::DoDataExchange(pDX);
}

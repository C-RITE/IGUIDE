#include "stdafx.h"
#include "Monitors.h"
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
	pThis->screen.monitor = hMon;
	pThis->screen.area = *lprcMonitor;
	pThis->screen.resolution = CPoint(monInf.rcMonitor.right - monInf.rcMonitor.left, monInf.rcMonitor.bottom - monInf.rcMonitor.top);
	pThis->screen.number = pThis->devIndex + 1;
	pThis->screens.push_back(pThis->screen);
	pThis->devIndex++;

	return TRUE;

}

Monitors::Monitors()
{
	devIndex = 0;
	EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
}

Monitors::~Monitors()
{
}


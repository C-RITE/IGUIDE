#pragma once
#include <vector>

struct Device {
	RECT area;				// display area in pixels;
	CString name;			// display identifier
	CString display;		// the display enum
	CPoint resolution;		// screen resolution
	int number;				// display number
	HMONITOR monitor;		// monitor handle
};

class Monitors
{

public:

	Monitors();
	~Monitors();

	void				select();
	void				refresh();

	std::vector<Device>	m_Devices;							// all detected monitors
	Device*				m_pSelectedDevice;					// target monitor
	Device				screen;								// used for callback to populate vector of screens
	int					devIndex;							// used for callback device enumaration

	std::vector<Device> getDevices() { return m_Devices; };

};
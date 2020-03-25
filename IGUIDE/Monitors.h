#pragma once
#include <vector>

struct Device {
	RECT area;				// display area in pixels;
	CString name;			// display identifier
	CString display;		// the display enum
	CPoint resolution;		// screen resolution
	int number;				// display number
	DWORD monitor;			// to check for primary display
};

class Monitors
{

public:

	Monitors();
	~Monitors();


	void				selectionDialog();
	void				erasePrimaryDisplay();				// disallow primary display for target screen
	void				refresh();

	void				selectDeviceFromListBoxItem(CString selection);

	std::vector<Device>	m_Devices;							// all detected display devices
	Device*				m_pSelectedDevice;					// used for target view
	Device				screen;								// used for callback to populate vector of screens
	int					devIndex;							// used for callback device enumaration

	std::vector<Device> getDevices() { return m_Devices; };

};
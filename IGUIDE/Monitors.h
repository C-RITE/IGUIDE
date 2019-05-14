#pragma once
#include <vector>

struct Screen {
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

	Screen screen;
	int devIndex;

	Monitors();
	~Monitors();

	std::vector<Screen> getScreens() { return screens; };
	std::vector<Screen> screens;
	
};


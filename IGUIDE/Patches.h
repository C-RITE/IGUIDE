#pragma once
#include <list>

struct Patch {

	CD2DPointF		coordsDEG;
	CD2DPointF		coordsPX;
	D2D1_COLOR_F	color;
	CString			timestamp;
	double			rastersize;
	bool			locked;
	CString			defocus;

};

class Patches : public std::list<Patch>
{

public:
	Patches();
	CString			filepath;
	CString			filename;
	
	bool			SaveToFile();
	bool			isFileTouched() { return fileTouched; };
	void			lockIn();
	void			untouch();
	void			setDiscretion(CD2DSizeF discretion);	// set distance between patches in a group (POI, ROI)

private:
	
	CD2DSizeF		discretion;						
	CString			timestamp;
	bool			fileTouched;
	void			GetSysTime(CString &buf);
	void			cleanup();

};
#pragma once
#include <list>

struct Patch {

	CD2DPointF		coordsDEG;
	CD2DPointF		coordsPX;
	D2D1_COLOR_F	color;
	CString			timestamp;
	double			rastersize;
	bool			locked;
	int				index;
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
	BOOL			lockIn();
	void			revertLast();
	void			delPatch();
	void			untouch();
	void			setOverlap(CD2DSizeF overlap);	// set distance between patches in a group (POI, ROI)

private:
	
	CD2DSizeF		discretion;						
	CString			timestamp;
	Patch			last;
	bool			fileTouched;
	bool			finished;
	void			GetSysTime(CString &buf);
	void			cleanup();

};
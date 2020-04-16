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

enum Element
{
	INIT = 0,
	NEXT = 1,
	PREV = 2
};

class Patches : public std::list<Patch>
{

public:
	Patches();
	CString			filepath;
	CString			filename;
	
	bool			SaveToFile();
	bool			isFileTouched() { return fileTouched; };
	bool			commit();
	bool			checkComplete();
	void			revertLast();
	void			delPatch();
	void			untouch();
	void			setOverlap(CD2DSizeF overlap);	// set distance between patches in a patch matrix (POI, ROI)

private:
	
	CD2DSizeF		overlap;						
	CString			timestamp;
	Patch			last;
	int				index;
	bool			fileTouched;
	bool			finished;
	void			GetSysTime(CString &buf);
	void			cleanup();

};
#pragma once
#include <list>

struct Patch {

	CD2DPointF		coordsDEG;
	CD2DPointF		coordsPX;
	D2D1_COLOR_F	color;
	CString			timestamp;
	double			rastersize;
	bool			locked;
	bool			visited;
	int				index;
	int				region;
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
	CString			filename;
	
	bool			SaveToFile(CString directory);
	bool			isFileTouched() { return fileTouched; };
	bool			isFinished() { return finished; };
	bool			commit();
	bool			checkComplete();
	void			revertLast();
	void			delPatch();
	void			untouch();
	void			setOverlap(float overlap, float rsDeg);	

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
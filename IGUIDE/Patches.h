#pragma once
#include <list>

struct Patch {

	CD2DPointF		coords;
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
	~Patches();
	CString			filepath;
	CString			filename;
	
	bool			SaveToFile();
	bool			isFileTouched() { return fileTouched; };
	void			lockIn();
	void			untouch();

private:
	
	CString			timestamp;
	bool			fileTouched;
	void			GetSysTime(CString &buf);
	void			cleanup();

};
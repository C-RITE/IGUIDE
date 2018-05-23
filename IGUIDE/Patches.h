#pragma once
#include <list>

struct Patch {

	CD2DPointF		coords;
	D2D1_COLOR_F	color;
	CString			timestamp;
	double			rastersize;
	bool			locked;
};

class Patches : public std::list<Patch>
{

public:
	Patches();
	~Patches();
	CString			filepath;
	CString			filename;
	
	bool			SaveToFile();
	void			lockIn();

private:
	
	CString			timestamp;
	bool			fileopen;
	void			GetSysTime(CString &buf);
	void			cleanup();

};
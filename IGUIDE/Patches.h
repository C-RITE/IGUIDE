#pragma once
#include <list>
class Patches :
	public std::list<Patches>
{
public:
	Patches();
	~Patches();

	CD2DPointF		coords;
	D2D1_COLOR_F	color;
	CString			filepath;
	CString			filename;
	float			rastersize;
	bool			locked;

	bool SaveToFile();
};
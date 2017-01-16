#pragma once
#include <list>
class Tags :
	public std::list<Tags>
{
public:
	Tags();
	~Tags();

	CD2DPointF		coords;
	D2D1_COLOR_F	color;
	CString			filepath;
	CString			filename;
	float			rastersize;
	bool			locked;

	bool SaveToFile();
};
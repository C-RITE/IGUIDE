#pragma once
#include <vector>
class Tags :
	public std::vector<Tags>
{
public:
	Tags();
	~Tags();

	CD2DPointF		coords;
	D2D1_COLOR_F	color;
	float			rastersize;
};
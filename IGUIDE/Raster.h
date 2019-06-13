#pragma once
#include <vector>
#include "Edge.h"
using namespace std;

class Raster
{

public:
	vector<CD2DPointF>	corner;								// raster corners from subject view
	vector<Edge>		perimeter;							// raster perimeter
	double				size;								// raster size
	D2D1_COLOR_F		color;								// raster color
	CD2DPointF			scale;								// for resizing operator's view
	double				meanAlpha;							// displacement angle
	double				meanEdge;							// mean edge
	CD2DPointF			mid;								// triangulated mid point of raster

	Raster();
	~Raster();

};


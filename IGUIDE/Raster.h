#pragma once
#include <vector>
#include "Edge.h"
using namespace std;

class Raster
{


public:
	vector<CD2DPointF>	corner;								// raster corners from subject view
	vector<Edge>		perimeter;							// raster perimeter
	float				size;								// raster size
	CD2DPointF			scale;								// for resizing operator's view
	float				meanAlpha;							// displacement angle
	float				meanEdge;							// mean edge
	CD2DPointF			mid;								// triangulate mid point of raster

	Raster();
	~Raster();

};


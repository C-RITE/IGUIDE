#pragma once
#include <vector>
#include "Edge.h"
using namespace std;

class Raster
{

public:
	const int			videodim{ 512 };					// dimension of ICANDI video in pixel
	vector<CD2DPointF>	corner;								// raster corners from subject view
	vector<Edge>		perimeter;							// raster perimeter
	int					size;								// raster size in pixel per degree
	D2D1_COLOR_F		color;								// raster color
	CD2DPointF			scale;								// size relative to main window (operator view)
	double				meanAlpha;							// displacement angle
	double				meanEdge;							// mean edge
	CD2DPointF			mid;								// triangulated mid point of raster

	Raster();
	~Raster();

};


#include "stdafx.h"
#include "raster.h"


Raster::Raster() :
size(0),
corner(),
perimeter(),
scale(0, 0),
meanAlpha(0),
meanEdge(0),
mid(0, 0),
color(D2D1::ColorF(D2D1::ColorF::DarkGreen))
{
}


Raster::~Raster()
{
}
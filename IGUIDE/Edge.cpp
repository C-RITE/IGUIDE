#include "stdafx.h"
#include "Edge.h"


Edge::Edge():
	p({0,0}),
	q({0,0}),
	length(0),
	alpha(0)
{
}

int Edge::getLength() {
	
	double a = p.x - q.x;
	double b = p.y - q.y;

	int c = sqrt(a*a + b * b);

	return c;

}


Edge::~Edge()
{
}

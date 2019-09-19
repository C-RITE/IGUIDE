#pragma once

class Edge
{

public:

	CD2DPointF p, q;
	double length, alpha;

	Edge();
	Edge(CD2DPointF p, CD2DPointF q) :p(p), q(q) {};

	~Edge();
};


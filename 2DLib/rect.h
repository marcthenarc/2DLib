#pragma once

#include "point.h"
#include "size.h"

class Rect
{
public:

	Size S;
	Point P;

	Rect();
	Rect(const Point& p, const Size& s) : P(p), S(s) { }
};
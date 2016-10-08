/* --------------------------------------------------------------------------

rect.h

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A rectangle class.

-----------------------------------------------------------------------------*/
#pragma once

#include "point.h"
#include "size.h"

class Rect
{
public:

	Size S;
	Point P;

	Rect() { }
	Rect(const Point& p, const Size& s) : P(p), S(s) { }
};

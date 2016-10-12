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
	Rect(const Point &p, const Point &q) : P(p), S((q - p).X, (q - p).Y) { }

	void Grow(int g)
	{
		P.X -= g;
		P.Y -= g;

		S.W += g * 2;
		S.H += g * 2;
	}

	void Shrink(int s)
	{
		P.X += s;
		P.Y += s;

		S.W += s * 2;
		S.H -= s * 2;
	}

	Point GetBottomRight() const
	{
		return Point(P.X + S.W, P.Y + S.H);
	}
};

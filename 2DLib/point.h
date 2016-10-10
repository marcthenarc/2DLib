/* --------------------------------------------------------------------------

point.h

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A point class.  Soon to be replaced by glm.

-----------------------------------------------------------------------------*/
#pragma once

class Point
{
public:

	int X, Y;

	Point();
	Point(int x, int y);

	bool operator == (const Point& p) const;
	bool operator != (const Point& p) const;
	void operator += (const Point& p);

	static const Point OutOfBounds;
};


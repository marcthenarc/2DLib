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

	Point() : X(0), Y(0) { }
	Point(int x, int y) : X(x), Y(y) { }
};

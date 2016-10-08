/* --------------------------------------------------------------------------

buffer.h

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

Buffer class in which we manipulate the color data.

-----------------------------------------------------------------------------*/
#pragma once

#include <string>
#include <vector>
#include "color.h"
#include "rect.h"

class Buffer
{
	std::vector<Color> colors;
	Size size;

	Rect LimitArea(const Rect &r);

public:

	Buffer();
	Buffer(const Size& s, const Color& c);

	bool SaveAsTGA(const std::string &filename);
	bool ReadFromTGA(const std::string &filename);

	void Set(const Point &p, const Color& c);
	const Color& Get(const Point &p);

	void DrawSquare(const Rect& r, const Color& c);

	void DrawAxis(const Point& start, const Point& finish, const Color& c);
};

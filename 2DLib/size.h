/* --------------------------------------------------------------------------

size.h

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A size class.  

-----------------------------------------------------------------------------*/

#pragma once

class Size
{
public:

	int W, H;

	Size() : W(0), H(0) { }

	Size(int w, int h) : W(w), H(h) { }
};


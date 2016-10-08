/* --------------------------------------------------------------------------

color.h

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A Color data type based on glm with constants.

-----------------------------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>

typedef glm::vec4 Color;

namespace RGBA
{
	static const Color White(1.f, 1.f, 1.f, 1.f);
	static const Color Black(0.0f, 0.0f, 0.0, 1.f);
	static const Color Grey(0.5f, 0.5f, 0.5f, 1.f);

	void GetAsBGR(const Color& color, unsigned char *bgr);
	void SetAsRGB(Color& color, const unsigned char *rgb);
};
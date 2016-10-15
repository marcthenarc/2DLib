/* --------------------------------------------------------------------------

color.cpp

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A Color data type based on glm with constants.

-----------------------------------------------------------------------------*/
#include "color.h"

void RGBA::GetAsBGR(const Color& color, unsigned char *bgr)
{
	bgr[0] = (unsigned char)(color.b * 255.f);
	bgr[1] = (unsigned char)(color.g * 255.f);
	bgr[2] = (unsigned char)(color.r * 255.f);
}

void RGBA::SetAsRGB(Color& color, const unsigned char *c)
{
	color.b = (float)c[0] / 255.f;
	color.g = (float)c[1] / 255.f;
	color.r = (float)c[2] / 255.f;
}

void RGBA::GetAsBGRA(const Color& color, unsigned char *bgr, size_t size)
{
	bgr[0] = (unsigned char)(color.b * 255.f);
	bgr[1] = (unsigned char)(color.g * 255.f);
	bgr[2] = (unsigned char)(color.r * 255.f);

	if (size == 4)
		bgr[3] = (unsigned char)(color.a * 255.f);
}

void RGBA::GetAsRGBA(const Color& color, unsigned char *bgr, size_t size)
{
	bgr[0] = (unsigned char)(color.r * 255.f);
	bgr[1] = (unsigned char)(color.g * 255.f);
	bgr[2] = (unsigned char)(color.b * 255.f);

	if (size == 4)
		bgr[3] = (unsigned char)(color.a * 255.f);
}

void RGBA::SetAsRGBA(Color& color, const unsigned char *c, size_t size)
{
	color.b = (float)c[0] / 255.f;
	color.g = (float)c[1] / 255.f;
	color.r = (float)c[2] / 255.f;
	color.a = (size == 4) ? (float)c[3] / 255.f : 1.f;
}

std::ostream & operator << (std::ostream &os, const Color &c)
{
	os << "<r=" << c.r << ", g=" << c.g << ", b=" << c.b << ", a=" << c.a << ">";
	return os;
}

#include "color.h"

const Color Color::White(1.0f, 1.0f, 1.0);
const Color Color::Black(0.0f, 0.0f, 0.0);
const Color Color::Grey(0.5f, 0.5f, 0.5f);
const Color Color::Background = Black;
const Color Color::Default = Black;

Color::Color() : R(0.f), G(0.f), B(0.f), A(1.f)
{

}

Color::Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a)
{

}

void Color::GetAsBGR(unsigned char *c) const
{
	c[0] = (unsigned char)(B * 255.f);
	c[1] = (unsigned char)(G * 255.f);
	c[2] = (unsigned char)(R * 255.f);
}

void Color::SetAsBGR(unsigned char *c)
{
	B = (float)c[0] / 255.f;
	G = (float)c[1] / 255.f;
	R = (float)c[2] / 255.f;
}

Color& Color::operator += (const Color& c)
{
	R += c.R;
	G += c.G;
	B += c.B;

	return *this;
}

Color Color::operator + (const Color &c)
{
	Color C = *this;
	C += c;

	return C;
}

Color& Color::operator -= (const Color& c)
{
	R -= c.R;
	G -= c.G;
	B -= c.B;

	return *this;
}

Color Color::operator - (const Color &c)
{
	Color C = *this;
	C -= c;

	return C;
}

Color& Color::operator *= (float f)
{
	R *= f;
	G *= f;
	B *= f;

	return *this;
}

Color Color::operator * (float f)
{
	Color C = *this;
	C *= f;

	return C;
}

Color& operator * (float f, Color& c)
{
	c.R *= f;
	c.G *= f;
	c.B *= f;

	return c;
}

Color operator * (float f, const Color& c)
{
	Color C = c;
	C *= f;

	return C;
}

Color& Color::operator *= (const Color& c)
{
	R *= c.R;
	G *= c.G;
	B *= c.B;

	return *this;
}

Color Color::operator * (const Color &c)
{
	Color C = *this;
	C *= c;

	return C;
}


#pragma once

class Color
{
public:

	float R,G,B,A;

	Color();
	Color(float r, float g, float b, float a = 1.f);

	void GetAsBGR(unsigned char *c) const;
	void SetAsBGR(unsigned char *c);

	Color& operator += (const Color& c);
	Color operator + (const Color &c);
	Color& operator -= (const Color& c);
	Color operator - (const Color &c);
	Color& operator *= (float f);
	Color operator * (float f);
	Color& operator *= (const Color &c);
	Color operator * (const Color &c);

	static const Color White;
	static const Color Black;
	static const Color Grey;
	static const Color Background;
	static const Color Default;
};

Color& operator *= (float f, const Color c);
Color operator * (float f, const Color& c);
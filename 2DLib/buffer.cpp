/* --------------------------------------------------------------------------

buffer.cpp

This file is part of 2DLib. (C) Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

Buffer class in which we manipulate the color data.

-----------------------------------------------------------------------------*/
#include "buffer.h"

Buffer::Buffer()
{
}

Buffer::Buffer(const Size& s, const Color& c) : size(s)
{
	// Force a resize of the array with the chose color.
	colors.clear();
	colors.resize(size.W * size.H, c);
}

static unsigned char bgr[3];
static unsigned char bgra[4];

bool Buffer::SaveAsTGA(const std::string &filename, bool with_alpha)
{
	FILE *fp = fopen(filename.c_str(), "wb");

	if (fp)
	{
		// TGAs are stored as blue-green-red components (alpha optional).
		unsigned char *comps = (with_alpha) ? bgra : bgr;
		size_t comps_size = (with_alpha) ? 4 : 3;

		// 18 byte header.  This is a version 2 (top-down, left-right), non-compressed, 24 or 32 bit image.
		unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		(unsigned char)(size.W & 0x00FF), (unsigned char)(size.W >> 8),
		(unsigned char)(size.H & 0x00FF), (unsigned char)(size.H >> 8),
		(unsigned char)(comps_size << 3), (unsigned char)0x20
	   };

		// Write header
		fwrite(header, 18, 1, fp);

		// Write all data as BGR components.
		for (size_t i=0; i<colors.size(); i++)
		{
			RGBA::GetAsBGRA(colors[i], comps, comps_size);
			fwrite(comps, comps_size, 1, fp);
		}

		//// NOTE:  No footer is written.  All readers I encountered ignored the extra "developper" data.

		fclose(fp);

		return true;
	}

	return false;
}

bool Buffer::ReadFromTGA(const std::string &filename)
{
	FILE *fp = fopen(filename.c_str(), "rb");

	if (fp)
	{
		// make sure the array is empty.
		colors.clear();

		unsigned char header[18];

		// 18 byte header.  This only reads version 2 (top-down, left-right), non-compressed, 32 bit image.
		fread(header, 18, 1, fp);

		// Get dimensions
		size.W = (((int)header[13]) << 8) + (int)header[12];
		size.H = (((int)header[15]) << 8) + (int)header[14];

		size_t comps_size = header[16] >> 3;

		unsigned char *comps = (comps_size == 4) ? bgra : bgr;

		int max = size.W * size.H;

		Color c;

		// Read all data as BGR components.
		for (int i = 0; i<max; i++)
		{
			fread(comps, comps_size, 1, fp);

			if (feof(fp))
				return false;

			RGBA::SetAsRGBA(c, comps, comps_size);
			colors.push_back(c);
		}

		fclose(fp);

		return true;
	}

	return false;
}

void Buffer::Set(const Point &p, const Color& c)
{
	// Check under/over flow.
	if (p.X >= 0 && p.X < size.W && p.Y >= 0 && p.Y < size.H)
		colors[p.Y * size.W + p.X] = c;
}

static const Color nullColor;

const Color & Buffer::Get(const Point &p)
{
	// Check under/over flow.
	if (p.X >= 0 && p.X < size.W && p.Y >= 0 && p.Y < size.H)
		return colors[p.Y * size.W + p.X];

	// When failing, return the color black.
	return nullColor;
}

Rect Buffer::LimitArea(const Rect &r)
{
	Rect nr = r;

	if (nr.P.X < 0)
	{
		nr.S.W += nr.P.X;
		nr.P.X = 0;
	}

	if (nr.P.Y < 0)
	{
		nr.S.H += nr.P.Y;
		nr.P.Y = 0;
	}

	if (nr.P.X + nr.S.W >= size.W)
	{
		nr.S.W = size.W - nr.P.X;
	}

	if (nr.P.Y + nr.S.H >= size.H)
	{
		nr.S.H = size.H - nr.P.Y;
	}

	return nr;
}

void Buffer::DrawSquare(const Rect& r, const Color& c)
{
	Rect newRect = LimitArea(r);

	if (newRect.S.H >= 0 && newRect.S.W >= 0)
	{
		int keep = newRect.P.Y * size.W + newRect.P.X;

		for (int j = 0; j<newRect.S.H; j++)
		{
			int start = keep;

			for (int i = 0; i < newRect.S.W; i++, start++)
			{
				// Only draw the borders.
				if (j == 0 || j == newRect.S.H - 1 || i == 0 || i == newRect.S.W - 1)
					colors[start] = c;
			}

			keep += size.W;
		}
	}
}

void Buffer::FillSquare(const Rect& r, const Color& c)
{
	Rect newRect = LimitArea(r);

	if (newRect.S.H >= 0 && newRect.S.W >= 0)
	{
		int keep = newRect.P.Y * size.W + newRect.P.X;

		for (int j=0; j<newRect.S.H; j++)
		{
			int start = keep;

			for (int i=0; i<newRect.S.W; i++, start++)
				colors[start] = c;

			keep += size.W;
		}
	}
}

void Buffer::DrawAxis(const Point& start, const Point& finish, const Color& c)
{
	// X axis
	if (start.Y == finish.Y && start.Y >= 0 && finish.Y < size.H)
	{
		int SX = start.X, FX = finish.X;

		// Invert in case
		if (start.X > finish.X)
		{
			SX = finish.X;
			FX = start.X;
		}

		// Boundary
		if (SX < 0)
			SX = 0;

		if (FX >= size.W)
			FX = size.W - 1;

		int begin = start.Y * size.W + SX;
		int end = start.Y * size.W + FX;

		for (int j = begin; j < end; j++)
			colors[j] = c;		
	}
	// Y axis
	else if (start.X == finish.X && start.X >= 0 && finish.X < size.W)
	{
		int SY = start.Y, FY = finish.Y;

		// Invert in case
		if (start.Y > finish.Y)
		{
			SY = finish.Y;
			FY = start.Y;
		}

		// Boundary
		if (SY < 0)
			SY = 0;

		if (FY >= size.H)
			FY = size.H - 1;

		int begin = SY * size.W + start.X;
		int end = begin + (FY - SY) * size.W;

		for (int j = begin; j <= end; j += size.W)
			colors[j] = c;
	}
}

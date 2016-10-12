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
	Reset(c);
}

void Buffer::Reset(const Size& s, const Color& c)
{
	size = s;
	Reset(c);
}

void Buffer::Reset(const Color& c)
{
	// Force a resize of the array with the chosen color.
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

const Color & Buffer::Get(const Point &p) const
{
	// Check under/over flow.
	if (p.X >= 0 && p.X < size.W && p.Y >= 0 && p.Y < size.H)
		return colors[p.Y * size.W + p.X];

	// When failing, return the color black.
	return nullColor;
}

void Buffer::LimitPoint(Point &p)
{
	if (p.X < 0)
		p.X = 0;

	if (p.Y < 0)
		p.Y = 0;

	if (p.X >= size.W)
		p.X = size.W - 1;

	if (p.Y >= size.H)
		p.Y = size.H - 1;
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

void Buffer::DrawRect(const Rect& r, const Color& c)
{
	Rect s = LimitArea(r);

	Point p1 = r.P, p2 = p1 + Point(s.S.W, 0), p3 = p1 + Point(0, s.S.H), p4 = s.GetBottomRight();
	
	DrawHorizontalLine(p1, p2, c);
	DrawHorizontalLine(p3, p4, c);

	DrawVerticalLine(p1, p3, c);
	DrawVerticalLine(p2, p4, c);
}

void Buffer::FillRect(const Rect& r, const Color& c)
{
	Rect newRect = LimitArea(r);

	Point p1 = newRect.P;
	Point p2 = p1 + Point(newRect.S.W, 0);
	Point end = newRect.GetBottomRight();

	bool quit = false;

	while (!quit)
	{
		quit = (p2 == end);

		DrawHorizontalLine(p1, p2, c);

		p1 += Point(0, 1);
		p2 += Point(0, 1);
	}
}

void Buffer::DrawHorizontalLine(const Point& start, const Point& end, const Color& c)
{
	Point s = start, e = end;

	// Make sure we don't bust.
	LimitPoint(s);
	LimitPoint(e);

	// Same Y, start is smaller than end.
	if (s.Y == e.Y && s.X <= e.X)
	{
		int ptr = s.Y * size.W + s.X;
		int ptr2 = e.Y * size.W + e.X;

		// start and ends overlap.
		while (ptr <= ptr2)
			colors[ptr++] = c;
	}
}

void Buffer::DrawVerticalLine(const Point& start, const Point& end, const Color& c)
{
	Point s = start, e = end;

	// Make sure we don't bust.
	LimitPoint(s);
	LimitPoint(e);

	// Same Y, start is smaller than end.
	if (s.X == e.X && s.Y <= e.Y)
	{
		int ptr = s.Y * size.W + s.X;
		int ptr2 = e.Y * size.W + e.X;

		// start and ends overlap.
		while (ptr <= ptr2)
		{
			colors[ptr] = c;
			ptr += size.W;
		}
	}
}

bool Buffer::Scan(const Point &start, const Point &end, ScanDirection dir, ScanState state, const Color &c, Point& hit)
{
	bool right = (start.X <= end.X);
	bool down = (start.Y <= end.Y);

	hit = start;
	Point stop = end;

	while (hit != stop)
	{
		if (Get(hit) == c)
		{
			if (state == MUST_FIND)
				return true;
		}
		else
		{
			if (state == MUST_ONLY_FIND)
				return false;
		}

		if (dir == HORZ)
			hit += Point((right) ? 1 : -1, 0);
		else
			hit += Point(0, (down) ? 1 : -1);
	}

	return (state == MUST_ONLY_FIND);
}

Rect Buffer::IsolateRect(const Rect& r, const Color& avoid)
{
	int left = r.P.X + 1, top = r.P.Y + 1, right = r.GetBottomRight().X - 1, bottom = r.GetBottomRight().Y - 1;

	Point hit; // will not be used but Scan needs it.
	
	// Scan horizontaly and go down until we hit something.
	while (Scan(Point(left, top), Point(right, top), HORZ, MUST_ONLY_FIND, avoid, hit))
		top += 1;

	// Scan verticaly and go left until we hit something.
	while (Scan(Point(left, top), Point(left, bottom), VERT, MUST_ONLY_FIND, avoid, hit))
		left += 1;

	// Scan horizontaly and go upon until we hit something.
	while (Scan(Point(left, bottom), Point(right, bottom), HORZ, MUST_ONLY_FIND, avoid, hit))
		bottom -= 1;

	// Scan verticaly and go right until we hit something.
	while (Scan(Point(right, top), Point(right, bottom), VERT, MUST_ONLY_FIND, avoid, hit))
		right -= 1;

	return Rect(Point(left, top), Point(right, bottom));
}

void Buffer::CopyLineFromBuffer(int dst, int src, int size,  const Buffer& from)
{
	for (int i = 0; i <= size; i++, dst++, src++)
		colors[dst] = from.colors[src];
}

void Buffer::CopyRectFromBuffer(const Rect& dst, const Rect& src, const Buffer& from)
{
	int left1 = dst.P.Y * size.W + dst.P.X;
	int right1 = dst.P.Y * size.W + (dst.P.X + dst.S.W);

	int left2 = src.P.Y * from.size.W + src.P.X;
	int right2 = src.P.Y * from.size.W + (src.P.X + src.S.W);
	int end = src.S.H * from.size.W + right2;

	bool quit = false;

	while(!quit)
	{
		// One last time before quitting the loop.
		if (right2 == end)
			quit = true;

		CopyLineFromBuffer(left1, left2, right1 - left1, from);

		left1 += size.W;
		right1 += size.W;

		left2 += from.size.W;
		right2 += from.size.W;
	}
}
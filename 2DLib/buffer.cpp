/* --------------------------------------------------------------------------

buffer.cpp

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

---

Parts from ReadFromPNG() and SaveFromPNG() are from a demo program at http://zarb.org/~gc/html/libpng.html
Copyright 2002-2010 Guillaume Cottenceau under the X11 license:

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

------

Buffer class in which we manipulate the color data.

-----------------------------------------------------------------------------*/
#include "buffer.h"
#include <iostream>
#include <png.h>

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

bool Buffer::ReadFromPNG(const std::string &filename)
{
	int m_nW, m_nH;
	png_bytep * row_pointers;
	png_byte m_nColorType;
	png_byte m_nBitDepth;


	png_structp png_ptr;
	//int number_of_passes;
	png_infop info_ptr;
	png_byte header[8];    // 8 is the maximum size that can be checked

						   /* open file and test for it being a png */
	FILE *fp = fopen(filename.c_str(), "rb");

	if (!fp)
		PNG_Exception(filename, "[read_png_file] couldn't open file for reading");

	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
		PNG_Exception(filename, "[read_png_file] File %s is not recognized as a PNG file");

	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		PNG_Exception(filename, "[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
		PNG_Exception(filename, "[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[read_png_fileme, ] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	m_nW = png_get_image_width(png_ptr, info_ptr);
	m_nH = png_get_image_height(png_ptr, info_ptr);
	m_nColorType = png_get_color_type(png_ptr, info_ptr);
	m_nBitDepth = png_get_bit_depth(png_ptr, info_ptr);

	//number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[read_png_file] Error during read_image");

	row_pointers = new png_bytep[sizeof(png_bytep) * m_nH];

	for (int y = 0; y<m_nH; y++)
		row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];

	png_read_image(png_ptr, row_pointers);
	/*
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
	PNG_Exception(filename, "[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
	"(lacks the alpha channel)");

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
	PNG_Exception(filename, "[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
	PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
	*/
	fclose(fp);

	return true;
}

// Note:  I wanted to create a single array of unsigned char's in which each
// row stored in row_pointers would a be a pointer to a location in the array.
// For some reason, it doesn't work.  Maybe the PNG engine checks on the length
// of each continuous row to match the width ...

// So I use a new and a delete.

bool Buffer::SaveAsPNG(const std::string &filename, bool with_alpha)
{
	std::vector<unsigned char *> all_rows;
	unsigned char comps[4];
	int k = 0;

	for (int j=0; j<size.H; j++)
	{
		std::vector<unsigned char> *row = new std::vector<unsigned char>();

		for (int i=0; i<size.W; i++, k++)
		{
			RGBA::GetAsRGBA(colors[k], comps, 4);

			row->push_back(comps[0]);
			row->push_back(comps[1]);
			row->push_back(comps[2]);
			row->push_back(comps[3]);
		}

		all_rows.push_back(row->data());
	}

	png_bytep* row_pointers = (png_bytep *)all_rows.data();
	png_byte m_nColorType = 6;
	png_byte m_nBitDepth = 8;

	png_structp png_ptr;
	//int number_of_passes;
	png_infop info_ptr;
	//	png_byte header[8];    // 8 is the maximum size that can be checked

	/* create file */
	FILE *fp = fopen(filename.c_str(), "wb");

	if (!fp)
		PNG_Exception(filename, "[write_png_file] File %s could not be opened for writing");

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		PNG_Exception(filename, "[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
		PNG_Exception(filename, "[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, size.W, size.H,
		m_nBitDepth, m_nColorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	fclose(fp);

	// Clean up
	for (auto row : all_rows)
		delete row;

	return true;
}

void Buffer::Sanitize()
{
	for (auto& c : colors)
	{
		if (c.a == 0.f)
			c = RGBA::NoAlpha;
	}
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
	{
		int z = p.Y * size.W + p.X;
		return colors[p.Y * size.W + p.X];
	}

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

void Buffer::LimitRect(Rect &r)
{
	if (r.left < 0)
		r.left = 0;

	if (r.top < 0)
		r.top = 0;

	if (r.right >= size.W)
		r.right = size.W - 1;

	if (r.bottom >= size.H)
		r.bottom = size.H - 1;
}

void Buffer::DrawRect(const Rect& r, const Color& c)
{
	Rect lr = r;
	LimitRect(lr);

	Point p1 = lr.GetTopLeft(), p2 = lr.GetTopRight(), p3 = lr.GetBottomLeft(), p4 = lr.GetBottomRight();

	DrawHorizontalLine(p1, p2, c);
	DrawHorizontalLine(p3, p4, c);

	DrawVerticalLine(p1, p3, c);
	DrawVerticalLine(p2, p4, c);
}

void Buffer::FillRect(const Rect& r, const Color& c)
{
	Rect lr = r;
	LimitRect(lr);

	Point p1 = lr.GetTopLeft();
	Point p2 = lr.GetTopRight();
	Point end = lr.GetBottomLeft() + Point(0, 1);

	while (p1 != end)
	{
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
		int ptr1 = s.Y * size.W + s.X;
		int ptr2 = e.Y * size.W + e.X;

		// start and ends overlap.
		while (ptr1 <= ptr2)
			colors[ptr1++] = c;
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
		int ptr1 = s.Y * size.W + s.X;
		int ptr2 = e.Y * size.W + e.X;

		// start and ends overlap.
		while (ptr1 <= ptr2)
		{
			colors[ptr1] = c;
				ptr1 += size.W;
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
		Color hc = Get(hit);

		// Check hit.  Consider transparent pixels to be one of the same regardless of color.
		if (hc == c) // || (c == RGBA::NoAlpha && hc.a == 0.f))
		{
			if (state == MUST_FIND)
				return true;
		}
		else if (hit.X >= this->size.W)
			return false;
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
	Point notUsed; // will not be used but Scan() needs it in some other case.

	Rect lrc = r;
	LimitRect(lrc);
	
	// Scan horizontally and go down until we hit something.
	while (Scan(lrc.GetTopLeft(), lrc.GetTopRight(), HORZ, MUST_ONLY_FIND, avoid, notUsed))
		lrc.top++;

	// Scan vertically and go left until we hit something.
	while (Scan(lrc.GetTopLeft(), lrc.GetBottomLeft(), VERT, MUST_ONLY_FIND, avoid, notUsed))
		lrc.left++;

	// Scan horizontally and go upon until we hit something.
	while (Scan(lrc.GetBottomLeft(), lrc.GetBottomRight(), HORZ, MUST_ONLY_FIND, avoid, notUsed))
		lrc.bottom--;

	// Scan vertically and go right until we hit something.
	while (Scan(lrc.GetTopRight(), lrc.GetBottomRight(), VERT, MUST_ONLY_FIND, avoid, notUsed))
		lrc.right--;

	return lrc;
}

void Buffer::CopyLineFromBuffer(int dst, int src, int size,  const Buffer& from)
{
	for (int i = 0; i <= size; i++, dst++, src++)
		colors[dst] = from.colors[src];
}

void Buffer::CopyRectFromBuffer(const Rect& dst, const Rect& src, const Buffer& from)
{
	int left1 = dst.top * size.W + dst.left;
	int right1 = dst.top * size.W + dst.right;

	int left2 = src.top * from.size.W + src.left;
	int right2 = src.top * from.size.W + src.right;
	int end = (src.bottom + 1) * from.size.W + src.left;

	while(left2 != end)
	{
		CopyLineFromBuffer(left1, left2, right1 - left1, from);

		left1 += size.W;
		right1 += size.W;

		left2 += from.size.W;
		right2 += from.size.W;
	}
}
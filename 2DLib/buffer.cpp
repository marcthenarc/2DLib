#include "Buffer.h"

Buffer::Buffer()
{
}

Buffer::Buffer(const Size& s, const Color& c) : size(s)
{
	// Force a resize of the array with the chose color.
	colors.clear();
	colors.resize(size.W * size.H, c);
}

bool Buffer::SaveAsTGA(const std::string &filename)
{
	FILE *fp = fopen(filename.c_str(), "wb");

	if (fp)
	{
		// TGAs are stored as blue-green-red components.
		unsigned char bgr[3];

		// 18 byte header.  This is a version 2 (top-down, left-right), non-compressed, 24 bit image.
		unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		(unsigned char)(size.W & 0x00FF), (unsigned char)(size.W >> 8),
		(unsigned char)(size.H & 0x00FF), (unsigned char)(size.H >> 8),
		24, 0x20
	   };

		// Write header
		fwrite(header, 18, 1, fp);

		// Write all data as BGR components.
		for (size_t i=0; i<colors.size(); i++)
		{
			colors[i].GetAsBGR(bgr);
			fwrite(bgr, 3, 1, fp);
		}

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

		unsigned char bgr[3];
		unsigned char header[18];

		// 18 byte header.  This only reads version 2 (top-down, left-right), non-compressed, 24 bit image.
		fread(header, 18, 1, fp);

		// Get dimensions
		size.W = (((int)header[13]) << 8) + (int)header[12];
		size.H = (((int)header[15]) << 8) + (int)header[11];

		int max = size.W * size.H;

		Color c;

		// Read all data as BGR components.
		for (int i=0; i<max; i++)
		{
			fread(bgr, 3, 1, fp);

			if (feof(fp))
				return false;

			c.SetAsBGR(bgr);
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

		for (int j=0; j<newRect.S.H; j++)
		{
			int start = keep;

			for (int i=0; i<newRect.S.W; i++, start++)
				colors[start] = c;

			keep += size.W;
		}
	}
}

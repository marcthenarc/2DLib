#include "point.h"

const Point Point::OutOfBounds(-1, -1);

Point::Point()
	: X(0)
	, Y(0)
{

}

Point::Point(int x, int y)
	: X(x)
	, Y(y)
{

}

bool Point::operator == (const Point& p) const
{
	return (X == p.X && Y == p.Y);
}

bool Point::operator != (const Point& p) const
{
	return (X != p.X || Y != p.Y);
}

void Point::operator += (const Point& p)
{
	X += p.X;
	Y += p.Y;
}



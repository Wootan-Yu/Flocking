#include "Vec2.h"
#include <math.h>

float PI = 3.1415926535'8979323846'2643383279f;

Vec2::Vec2()
{}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float rhs) const
{
	return Vec2(x / rhs, y * rhs);
}

Vec2 Vec2::operator * (const float rhs) const
{
	return Vec2(x * rhs, y * rhs);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x) && (y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x) && (y != rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float Vec2::magnitude() const //also known as 'length'
{
	return sqrtf((x * x) + (y * y));
}

float Vec2::dist(const Vec2& rhs) const
{
	float difference_vec_x = rhs.x - x;
	float difference_vec_y = rhs.y - y;
	return sqrtf(difference_vec_x * difference_vec_x + difference_vec_y * difference_vec_y);
}

Vec2 Vec2::normalize() const
{
	float length = magnitude();
	if(length != 0)
		return Vec2(x / length, y / length);
}

float Vec2::angle(const Vec2& rhs) const
{
	return atan2f(rhs.y - y, rhs.x - x); // returns radians
	// to get degrees, multiply (180/PI constant)
	// usage: atan2f(rhs.y - y, rhs.x - x) * (180/3.1415926539)
}

Vec2 Vec2::limit(float max) const
{
	if (magnitude() > max)
	{
		normalize();
		return Vec2(x * max, y * max);
	}
}

float Vec2::heading() const
{
	return atan2(y, x) * 180 / PI; //returns angle in degree
}
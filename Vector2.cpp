#include "Vector2.h"
#include <cmath>
#include <limits>

Vector2::Vector2() : x(0.f), y(0.f) {}
Vector2::Vector2(const Vector2& v) : x(v.x), y(v.y) {}
Vector2::Vector2(float x, float y) : x(x), y(y) {}
Vector2::Vector2(int x, int y) :
	x(static_cast<float>(x)), y(static_cast<float>(y)) {}

// 長さを得る
float Vector2::Length() const
{
	return std::sqrtf(LengthSquare());
}

float Vector2::LengthSquare() const
{
	return x * x + y * y;
}

// ベクトルの角度
float Vector2::Angle() const
{
	return std::atan2f(y, x);
}

//内積
float Vector2::Dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

//外積
float Vector2::Cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

// ベクトルを回転する[rad]
Vector2& Vector2::Rotate(float radius)
{
	float c = std::cosf(radius);
	float s = std::sinf(radius);
	Vector2 tmp(*this);
	x = c * tmp.x - s * tmp.y;
	y = s * tmp.x + c * tmp.y;
	return *this;
}

// ベクトルを正規化する
Vector2& Vector2::Normalize()
{
	float len = Length();
	if (len != 0.f) {
		x /= len;
		y /= len;
	}
	return *this;
}

Vector2& Vector2::operator=(const Vector2 &v)
{
	x = v.x;
	y = v.y;
	return *this;
}

Vector2& Vector2::operator+=(const Vector2 &v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2 &v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2& Vector2::operator/=(float f)
{
	x /= f;
	y /= f;
	return *this;
}

Vector2 Vector2::operator-()
{
	return Vector2(-x, -y);
}

bool Vector2::operator==(const Vector2& v) const
{
	return std::fabsf(v.x - x) > std::numeric_limits<float>::epsilon()
		&& std::fabsf(v.y - y) > std::numeric_limits<float>::epsilon();
}

bool Vector2::operator!=(const Vector2& v) const
{
	return !(std::fabsf(v.x - x) > std::numeric_limits<float>::epsilon()
		&& std::fabsf(v.y - y) > std::numeric_limits<float>::epsilon());
}

Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 operator/(const Vector2& v, float f)
{
	return Vector2(v.x / f, v.y / f);
}
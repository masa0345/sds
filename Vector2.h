#pragma once

// 2次元ベクトル
struct Vector2
{
	float x, y;

	Vector2();
	Vector2(const Vector2& v);
	Vector2(float x, float y);
	Vector2(int x, int y);

	//長さを得る
	float Length() const;
	float LengthSquare() const;
	//ベクトルの角度
	float Angle() const;

	//内積
	float Dot(const Vector2& v) const;
	//外積
	float Cross(const Vector2& v) const;

	//ベクトルを回転する[rad]
	Vector2& Rotate(float radius);
	//ベクトルを正規化する
	Vector2& Normalize();

	Vector2& operator=(const Vector2& v);
	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator/=(float f);

	Vector2 operator-();

	bool operator==(const Vector2& v) const;
	bool operator!=(const Vector2& v) const;
};

Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator/(const Vector2& v, float f);
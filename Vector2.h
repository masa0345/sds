#pragma once

// 2�����x�N�g��
struct Vector2
{
	float x, y;

	Vector2();
	Vector2(const Vector2& v);
	Vector2(float x, float y);
	Vector2(int x, int y);

	//�����𓾂�
	float Length() const;
	float LengthSquare() const;
	//�x�N�g���̊p�x
	float Angle() const;

	//����
	float Dot(const Vector2& v) const;
	//�O��
	float Cross(const Vector2& v) const;

	//�x�N�g������]����[rad]
	Vector2& Rotate(float radius);
	//�x�N�g���𐳋K������
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
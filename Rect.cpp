#include "Rect.h"
#include "Vector2.h"
#include "DebugDraw.h"
#include <cassert>
#include <limits>

// 矩形クラス

Rect::Rect() : right(0), left(0), top(0), bottom(0) {}

Rect::Rect(int left, int top, int right, int bottom)
	: right(right), left(left), top(top), bottom(bottom)
{}

Rect::Rect(float l, float t, float r, float b)
	: right((int)r), left((int)l), top((int)t), bottom((int)b)
{}

void Rect::Set(int left, int top, int right, int bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

int Rect::Width() const
{
	return right - left;
}

int Rect::Height() const
{
	return bottom - top;
}

Vector2 Rect::Center() const
{
	return Vector2((right + left) / 2, (top + bottom) / 2);
}

bool Rect::CheckHit(const Rect& r) const
{
	return (left < r.right && r.left < right && top < r.bottom && r.top < bottom);
}

bool Rect::Inside(const Rect& r) const
{
	return left < r.left && r.right < right && top < r.top && r.bottom < bottom;
}

Rect Rect::TurnH() const
{
	return Rect(-right, top, -left, bottom);
}

Rect Rect::TurnV() const
{
	return Rect(left, -bottom, right, -top);
}

Rect& Rect::operator+=(const Vector2& v)
{
	left += static_cast<int>(v.x);
	right += static_cast<int>(v.x);
	top += static_cast<int>(v.y);
	bottom += static_cast<int>(v.y);

	return *this;
}

Rect& Rect::operator+=(const Rect& r)
{
	left += r.left;
	right += r.right;
	top += r.top;
	bottom += r.bottom;

	return *this;
}

Rect operator+(const Rect& r, const Vector2& v)
{
	return{ r.left + (int)v.x, r.top + (int)v.y,
			r.right + (int)v.x, r.bottom + (int)v.y };
}

// 当たり判定ボックス
HitBox::HitBox(int capacity) : size(0), capacity(capacity)
{
	assert(capacity > 0);
	rects = new Rect[capacity];
}

HitBox::HitBox(int w, int h) : HitBox(1)
{
	rects[0] = { -w / 2, -h / 2, w / 2, h / 2 };
	++size;
}

HitBox::HitBox(const HitBox& hb)
{
	size = hb.size;
	capacity = hb.capacity;
	rects = new Rect[capacity];
	for (int i = 0; i < size; ++i) rects[i] = hb.rects[i];
}

HitBox::~HitBox()
{
	delete[] rects;
}

void HitBox::SetRect(int i, const Rect& r)
{
	assert(i < size);
	rects[i] = r;
}

void HitBox::PushBack(const Rect& r)
{
	if (size >= capacity) {
		auto tmp = new Rect[capacity << 1];
		for (int i = 0; i < capacity; ++i) tmp[i] = rects[i];
		delete[] rects;
		rects = tmp;
		capacity <<= 1;
	}
	rects[size++] = r;
}

void HitBox::Clear()
{
	size = 0;
}

int HitBox::Size() const
{
	return size;
}

const Rect* HitBox::GetRects() const
{
	return rects;
}

bool HitBox::CheckHitRects(const HitBox& o, const Vector2& p, const Vector2& op)
{
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < o.size; ++j) {
			if ((rects[i] + p).CheckHit(o.rects[j] + op)) return true;
		}
	}
	return false;
}

// デバッグ描画
void HitBox::DrawRect(const Vector2& pos, int dir, bool atk)
{
	unsigned color = atk ? 0xff0000 : 0x0000ff;
	for (int i = 0; i < size; ++i) {
		DebugDraw::RectBox(rects[i]+pos, color, false);
	}
}

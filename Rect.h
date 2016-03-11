#pragma once

struct Vector2;

// 矩形
struct Rect
{
	int left, top, right, bottom;

	Rect();
	Rect(int left, int top, int right, int bottom);
	Rect(float l, float t, float r, float b);

	void Set(int left, int top, int right, int bottom);
	int Width() const;
	int Height() const;
	Vector2 Center() const;
	// rectと接触している
	bool CheckHit(const Rect& rect) const;
	// rectが内側に存在する
	bool Inside(const Rect& rect) const;
	// 左右反転させた矩形を得る
	Rect TurnH() const;
	// 上下反転させた矩形を得る
	Rect TurnV() const;

	Rect& operator+=(const Vector2& v);
	Rect& operator+=(const Rect& r);
};
Rect operator+(const Rect& r, const Vector2& v);


// 当たり判定ボックス
class HitBox
{
public:
	HitBox(int capacity = 1);
	HitBox(int w, int h);	// {-w/2,-h/2,w/2,h/2}の矩形判定
	HitBox(const HitBox& hb);
	~HitBox();

	void		SetRect(int i, const Rect& r);
	void		PushBack(const Rect& r);	// 判定ボックスを追加
	void		Clear();					// sizeを0にする
	int			Size() const;				// sizeを返す
	const Rect* GetRects() const;			// 当たり判定配列を得る
	bool		CheckHitRects(const HitBox& other, 
					const Vector2& pos, const Vector2& otherpos);
	void		DrawRect(const Vector2& pos, int dir, bool atk);

private:
	Rect*	rects;			// 矩形配列
	int		size, capacity;	// 配列サイズ
};
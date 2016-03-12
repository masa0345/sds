#pragma once

struct Vector2;

// ��`
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
	// rect�ƐڐG���Ă���
	bool CheckHit(const Rect& rect) const;
	// rect�������ɑ��݂���
	bool Inside(const Rect& rect) const;
	// ���E���]��������`�𓾂�
	Rect TurnH() const;
	// �㉺���]��������`�𓾂�
	Rect TurnV() const;

	Rect& operator+=(const Vector2& v);
	Rect& operator+=(const Rect& r);
};
Rect operator+(const Rect& r, const Vector2& v);


// �����蔻��{�b�N�X
class HitBox
{
public:
	HitBox(int capacity = 1);
	HitBox(int w, int h);	// {-w/2,-h/2,w/2,h/2}�̋�`����
	HitBox(const HitBox& hb);
	~HitBox();

	void		SetRect(int i, const Rect& r);
	void		PushBack(const Rect& r);	// ����{�b�N�X��ǉ�
	void		Clear();					// size��0�ɂ���
	int			Size() const;				// size��Ԃ�
	const Rect* GetRects() const;			// �����蔻��z��𓾂�
	bool		CheckHitRects(const HitBox& other, 
					const Vector2& pos, const Vector2& otherpos);
	void		DrawRect(const Vector2& pos, int dir, bool atk);

private:
	Rect*	rects;			// ��`�z��
	int		size, capacity;	// �z��T�C�Y
};
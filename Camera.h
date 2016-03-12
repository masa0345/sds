#pragma once

#include "Vector2.h"
#include "Rect.h"
#include <list>
#include <memory>

class Shake;
class GameEntity;

class Camera 
{
public:
	Camera();
	void Update();
	Vector2 TransCoord(const Vector2& p, float speed = 1.f) const; //�J�������W�ɕϊ�
	Vector2 TransCoordBackGround(const Vector2& scale) const; // �w�i�p�ϊ�
	Rect GetVisibleRect(int mmx, int mmy) const;
	Vector2 GetPos() const;
	void SetShake(float ampl, float freq, int duration, int damp);
	void SetField(const Rect& field);
	void SetTarget(std::shared_ptr<GameEntity> target);
	void SetPos(const Vector2& p);

private:
	Vector2 pos;
	std::shared_ptr<GameEntity> target;
	Rect field;
	float shakeYVal;
	std::list<Shake*> shake;
};

// �U��
class Shake
{
public:
	Shake(float ampl, float freq, int duration, int damp);
	bool GetShake(float* f);

private:
	int	count;		// �J�E���^
	int duration;	// �p������
	int damping;	// ��������
	float ampl;		// �U��
	float freq;		// �p���g��
};
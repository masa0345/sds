#pragma once

#include "GameEntity.h"
#include "Image.h"

class Effect : public GameEntity
{
public:
	Effect(const Vector2& pos);
	virtual ~Effect();

	virtual bool UpdatePre() override;
	virtual void Update() override;
	virtual void UpdatePosition() override;
	virtual void Draw() const override;
	virtual void DamageFrom(GameEntity*) override;

protected:
	int maxcnt;
	float v, rad_v;
	float a, rad_a;
};

// �c��
class EffectAfterImage : public Effect
{
public:
	EffectAfterImage(const Vector2& p, std::shared_ptr<ImageData> idt, 
		bool dir = false, float angle = 0.f, float scale = 1.f);
	void Update() override;
};

// �T�C�Y�����������c��
class EffectAfterImage2 : public Effect
{
public:
	EffectAfterImage2(const Vector2& p, std::shared_ptr<ImageData> idt,
		bool dir = false, float angle = 0.f, float scale = 1.f);
	void Update() override;
};

// �_�b�V��
class EffectPlayerDash : public Effect
{
public:
	EffectPlayerDash(const Vector2& p, bool dir);
	void Update() override;
};

// �q�b�g�G�t�F�N�g
class EffectHit : public Effect
{
public:
	EffectHit(const Vector2& p, LPHIMG hdl, float scale = 1.f);
};

// �e�؂�
class EffectNoMP : public Effect
{
public:
	EffectNoMP(const Vector2& p);
	void Update() override;
};

// �A�C�e���̌�
class EffectItemLight : public Effect
{
public:
	EffectItemLight(const Vector2& p, int type);
};

// ���@���S
class EffectPlayerDeath : public Effect
{
public:
	EffectPlayerDeath(const Vector2& p);
};

// �Ԕ���
class EffectFExplode : public Effect
{
public:
	EffectFExplode(const Vector2& p, float dec);
};

// ������
class EffectMExplode : public Effect
{
public:
	EffectMExplode(const Vector2& p);
};

// �������z����
class EffectMagnetar : public Effect
{
public:
	EffectMagnetar(const Vector2& p, GameEntity* parent);
};

// �}�b�v�Q�[�g
class EffectNextGate : public Effect
{
public:
	EffectNextGate(const Vector2& p);
};

// �{�X���j
class EffectFlushExplode : public Effect
{
public:
	EffectFlushExplode(const Vector2& p);
};

// WARNING
class EffectWarning : public Effect
{
public:
	EffectWarning();
	void Update() override;
};
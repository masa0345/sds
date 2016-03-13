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

// 残像
class EffectAfterImage : public Effect
{
public:
	EffectAfterImage(const Vector2& p, std::shared_ptr<ImageData> idt, 
		bool dir = false, float angle = 0.f, float scale = 1.f);
	void Update() override;
};

// サイズ減衰無し裏残像
class EffectAfterImage2 : public Effect
{
public:
	EffectAfterImage2(const Vector2& p, std::shared_ptr<ImageData> idt,
		bool dir = false, float angle = 0.f, float scale = 1.f);
	void Update() override;
};

// ダッシュ
class EffectPlayerDash : public Effect
{
public:
	EffectPlayerDash(const Vector2& p, bool dir);
	void Update() override;
};

// ヒットエフェクト
class EffectHit : public Effect
{
public:
	EffectHit(const Vector2& p, LPHIMG hdl, float scale = 1.f);
};

// 弾切れ
class EffectNoMP : public Effect
{
public:
	EffectNoMP(const Vector2& p);
	void Update() override;
};

// アイテムの光
class EffectItemLight : public Effect
{
public:
	EffectItemLight(const Vector2& p, int type);
};

// 自機死亡
class EffectPlayerDeath : public Effect
{
public:
	EffectPlayerDeath(const Vector2& p);
};

// 赤爆発
class EffectFExplode : public Effect
{
public:
	EffectFExplode(const Vector2& p, float dec);
};

// 白爆発
class EffectMExplode : public Effect
{
public:
	EffectMExplode(const Vector2& p);
};

// 白強化吸い寄せ
class EffectMagnetar : public Effect
{
public:
	EffectMagnetar(const Vector2& p, GameEntity* parent);
};

// マップゲート
class EffectNextGate : public Effect
{
public:
	EffectNextGate(const Vector2& p);
};

// ボス撃破
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
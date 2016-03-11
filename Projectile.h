#pragma once

#include "GameEntity.h"

class Projectile : public GameEntity
{
public:
	Projectile(const Vector2& pos);
	virtual ~Projectile();

	virtual bool UpdatePre() override;
	virtual void UpdatePosition() override;
	virtual void DamageFrom(GameEntity*) override;
	virtual Vector2 GetVel() const override;

protected:
	bool HitMap();
	float v, rad_v;
	float a, rad_a;
	int maxcnt;
	int power;
};

class EnemyBullet : public Projectile
{
public:
	EnemyBullet(const Vector2& pos, bool dir, int w);
	virtual void Update() override;
	virtual void SetDamage(Player* p) override;
	virtual void SetDamage(Block* b) override;
};

enum BulletType {
	STAR,
	METEOR,
	CHASER,
	GEAR,
	GHOST,
	STAR_AD,
	METEOR_AD,
	CHASER_AD,
	GEAR_AD,
	GHOST_AD,
	BULLET_TYPE_NUM
};

struct BulletParam {
	int delay, cost, power;
};

class PlayerBullet : public Projectile
{
public:
	PlayerBullet(const Player& p);
	virtual void Update() override;
	static void Load();
	static void Fire(const Player& p, BulletType t);
	static BulletParam GetBulletParam(BulletType i);

protected:
	static BulletParam param[BULLET_TYPE_NUM];
};


// 自機弾　黄
class BulletStar : public PlayerBullet
{
public:
	BulletStar(const Player& p);
	void Update() override;
	void SetDamage(Enemy*) override;
	void SetDamage(Block*) override;
	void SetDamage(Projectile*) override;
};

// 自機弾　赤
class BulletMeteor : public PlayerBullet
{
public:
	BulletMeteor(const Player& p);
	void Update() override;
	void SetDamage(Enemy*) override;
	void SetDamage(Block*) override;
	void SetDamage(Projectile*) override;
};

// 自機弾　青
class BulletChaser : public PlayerBullet
{
public:
	BulletChaser(const Player& p);
	void Update() override;
	void SetDamage(Enemy*) override;
	void SetDamage(Block*) override;
	void SetDamage(Projectile*) override;
private:
	std::shared_ptr<GameEntity> target;
};

// 自機弾　白
class BulletGear : public PlayerBullet
{
public:
	BulletGear(const Player& p);
	void Update() override;
	void SetDamage(Enemy*) override;
	void SetDamage(Block*) override;
	void SetDamage(Projectile*) override;
private:
	int hitnum;
	std::vector<GameEntity*> victim;
};

// 自機弾　黒
class BulletGhost : public PlayerBullet
{
public:
	BulletGhost(const Player& p);
};

// 自機弾　黄強化
class BulletDual : public PlayerBullet
{
public:
	BulletDual(const Player& p);
};

// 自機弾　赤強化
class BulletNova : public PlayerBullet
{
public:
	BulletNova(const Player& p);
};

// 自機弾　青強化
class BulletSpear : public PlayerBullet
{
public:
	BulletSpear(const Player& p);
};

// 自機弾　白強化
class BulletMagnetar : public PlayerBullet
{
public:
	BulletMagnetar(const Player& p);
};

// 自機弾　黒強化
class BulletPhantom : public PlayerBullet
{
public:
	BulletPhantom(const Player& p);
	void Update() override;
	void Draw() const override;
private:
	std::shared_ptr<Player> player;
};
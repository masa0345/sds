#pragma once

#include "GameEntity.h"

class Enemy : public GameEntity
{
public:
	Enemy();
	virtual ~Enemy();

	virtual void UpdatePosition() override;
	virtual void UpdateLate() override;
	virtual void DamageFrom(GameEntity*) override;
	virtual void SetDamage(Player* p) override;
	virtual bool Targetable() const override;

	virtual void BlockHit(const Vector2& p, const Vector2& v, int bw, int bh);
	virtual void BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh);

	bool IsFix() const;

protected:
	void DropItem();
	int power;			// 攻撃力
	int itemNum;		// アイテム所持数
	int dropRate;		// ドロップ率
	int dropType;		// アイテムタイプ
	bool fix, target;	// 固定タイプ, 自機弾のタゲ対象になるか
	int score;
	byte hitmap;
};

// 敵0
class EnemyType0 : public Enemy
{
public:
	EnemyType0();
	void Update() override;
};

// 敵1
class EnemyType1 : public Enemy
{
public:
	EnemyType1();
	void Update() override;
};

// 敵3
class EnemyType3 : public Enemy
{
public:
	EnemyType3();
	void Update() override;
	void UpdateColResponse() override;
	void BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) override;
	void BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) override;
};

// ボス1
class EnemyStage1Boss : public Enemy
{
	friend class EventStage1Boss;
public:
	EnemyStage1Boss();
	void Update() override;
	void UpdateLate() override;
private:
	float mem_y;
};

class EnemyBoss1Unit : public Enemy
{
public:
	EnemyBoss1Unit(Enemy* p);
	void Update() override;
	void SetDamage(Player* p) override;
private:
	Enemy* parent;
};

// チュートリアル
class EnemyGuide : public Enemy
{
public:
	EnemyGuide();
	void Update() override;
	void Draw() const override;
	void AddKillCount();
	void CreateSampleEnemies();
private:
	int killCnt;
	std::shared_ptr<Player> player;
};











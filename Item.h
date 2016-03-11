#pragma once

#include "GameEntity.h"

class Item : public GameEntity
{
public:
	Item(const Vector2& pos);
	virtual ~Item();

	virtual void UpdatePosition() override;
	virtual void UpdateLate() override;
	virtual void DamageFrom(GameEntity*) override;

protected:
	int type;
	int maxcnt;
	float v, rad_v;
	float a, rad_a;
};

// 武器アイテム
class ItemWeapon : public Item
{
public:
	ItemWeapon(const Vector2& p, int type);
	ItemWeapon(const Vector2& p, int type, int mp);
	void Update() override;
	void Draw() const override;
	void SetDamage(Player* p) override;
	void SetDamage(Item* i) override;
private:
	int mp;
	int velCnt;
};

// 回復アイテム
class ItemHeal : public Item
{
public:
	ItemHeal(const Vector2& p, int type);
	void Update() override;
	void UpdateColResponse() override;
	void Draw() const override;
	void SetDamage(Player* p) override;
	void SetDamage(Block* b) override;

private:
	int val; // 回復量
};
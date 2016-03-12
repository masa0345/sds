#pragma once

#include "GameEntity.h"

class Block : public GameEntity
{
public:
	Block();
	virtual ~Block();

	virtual void Update() override;
	virtual void DamageFrom(GameEntity*) override;
	virtual void SetDamage(Player* p) override;
	virtual void SetDamage(Enemy* p) override;
	bool IsHalf() const;
protected:
	bool half;
};

// 破壊可能なブロック
class BlockBreakable : public Block
{
public:
	BlockBreakable();
	void UpdateLate() override;

protected:
	void DropItem();
	int itemNum, dropRate, score;
};

// 上下移動リフト
class BlockLift : public Block
{
public:
	BlockLift();
	void Update() override;
};

class LiftChild;

// 右回転リフト
class BlockRotaLiftR : public Block
{
public:
	BlockRotaLiftR();
	void Update() override;
private:
	float rad_v;
	std::vector<std::shared_ptr<LiftChild>> lift;
};

// 左回転リフト
class BlockRotaLiftL : public Block
{
public:
	BlockRotaLiftL();
	void Update() override;
private:
	float rad_v;
	std::vector<std::shared_ptr<LiftChild>> lift;
};

// マップゲート
class BlockNextMapGate : public Block
{
public:
	BlockNextMapGate();
	void Update() override;
	virtual void SetDamage(Player* p) override;
	virtual void SetDamage(Enemy* p) override;
};
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

// �j��\�ȃu���b�N
class BlockBreakable : public Block
{
public:
	BlockBreakable();
	void UpdateLate() override;

protected:
	void DropItem();
	int itemNum, dropRate, score;
};

// �㉺�ړ����t�g
class BlockLift : public Block
{
public:
	BlockLift();
	void Update() override;
};

class LiftChild;

// �E��]���t�g
class BlockRotaLiftR : public Block
{
public:
	BlockRotaLiftR();
	void Update() override;
private:
	float rad_v;
	std::vector<std::shared_ptr<LiftChild>> lift;
};

// ����]���t�g
class BlockRotaLiftL : public Block
{
public:
	BlockRotaLiftL();
	void Update() override;
private:
	float rad_v;
	std::vector<std::shared_ptr<LiftChild>> lift;
};

// �}�b�v�Q�[�g
class BlockNextMapGate : public Block
{
public:
	BlockNextMapGate();
	void Update() override;
	virtual void SetDamage(Player* p) override;
	virtual void SetDamage(Enemy* p) override;
};
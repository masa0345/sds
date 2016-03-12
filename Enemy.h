#pragma once

#include "GameEntity.h"

//enum BulletType;

class Enemy : public GameEntity
{
public:
	Enemy();
	virtual ~Enemy();

	virtual void UpdatePosition() override;
	virtual void UpdateLate() override;
	virtual void DamageFrom(GameEntity*) override;
	virtual void SetDamage(Player* p) override;

	virtual void BlockHit(const Vector2& p, const Vector2& v, int bw, int bh);
	virtual void BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh);

	bool IsFix() const;

protected:
	void DropItem();
	int power;			// �U����
	int itemNum;		// �A�C�e��������
	int dropRate;		// �h���b�v��
	int dropType;		// �A�C�e���^�C�v
	bool fix, target;	// �Œ�^�C�v, ���@�e�̃^�Q�ΏۂɂȂ邩
	int score;
	byte hitmap;
};

// �G0
class EnemyType0 : public Enemy
{
public:
	EnemyType0();
	void Update() override;
};

// �G1
class EnemyType1 : public Enemy
{
public:
	EnemyType1();
	void Update() override;
};

// �G3
class EnemyType3 : public Enemy
{
public:
	EnemyType3();
	void Update() override;
	void UpdateColResponse() override;
	void BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) override;
	void BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) override;
};














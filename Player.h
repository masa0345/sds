#pragma once

#include "GameEntity.h"

class JoypadInput;
enum BulletType;

class Player : public GameEntity 
{
public:
	Player(const Vector2& pos, std::shared_ptr<JoypadInput> input);
	virtual ~Player();

	virtual void Update() override;
	virtual void UpdateColResponse() override;
	virtual void DamageFrom(GameEntity*) override;
	void SetPos(const Vector2& pos) override;

	void CalcDamage(int damage);
	bool PickItem(int type, int mp);
	bool IsShift() const;
	void BlockHit(const Vector2& p, const Vector2& v, int bw, int bh);
	void BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh);
	
	BulletType GetBulletType() const;
	int GetMP() const;
	int GetMPMax() const;
	std::shared_ptr<JoypadInput> GetInput() const;
	bool CanPickItem() const;
	bool IsShooting() const;

	void SetMP(int mp);
	void AddMP(int mp);

private:
	void StatePattern();

	Vector2 prev;
	int animCnt;
	int mp, mpmax;
	int jump;
	byte hitmap;
	int fireCnt, itemCnt;
	BulletType bulType;
	std::shared_ptr<JoypadInput> input;
};
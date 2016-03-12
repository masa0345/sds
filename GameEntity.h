#pragma once

#include "Vector2.h"
#include "Rect.h"
#include "common.h"
#include <list>
#include <memory>

enum ObjectType
{
	PLAYER,
	ENEMY,
	BLOCK,
	PROJECTILE,
	ITEM,
	EFFECT,
	UNKNOWN,
};

enum class Priority
{
	BACK_EFFECT,
	ENEMY,
	PLAYER,
	EFFECT,
	ENEMY_PROJ,
	PLAYER_PROJ,
	ITEM,
	FRONT_EFFECT
};

//class Sprite;

class Player;
class Enemy;
class Block;
class Projectile;
class Item;
class Effect;
struct ImageData;
class Camera;
class Stage;

class GameEntity
{
public:
	explicit GameEntity(const Vector2& pos);
	virtual ~GameEntity();

	virtual void Update() = 0;
	virtual void Draw() const;

	virtual bool UpdatePre();			// Update()�̑O�ɌĂ΂��
	virtual void UpdateLate();			// �Ō�ɌĂ΂��
	virtual void UpdatePosition();		// ���W�X�V
	virtual void UpdateColResponse();	// �}�b�v����

	inline bool GetExist() const;
	inline bool GetRemoveFlag() const;
	inline ObjectType GetObjectType() const;
	inline const Vector2& GetPos() const;
	inline bool GetDirection() const;
	inline Priority GetPriority() const;
	inline int GetWidth() const;
	inline int GetHeight() const;
	virtual Vector2 GetVel() const;
	virtual bool Targetable() const;

	virtual void SetPos(const Vector2& pos);
	inline void SetVel(const Vector2& vel);
	inline void SetXVel(float x);
	inline void SetYVel(float y);
	inline void SetPriority(Priority pr);
	inline void SetDirection(bool dir);
	inline void AddHP(int hp);

	virtual void DamageFrom(GameEntity* c) = 0;
	virtual void SetDamage(Player* c) {}
	virtual void SetDamage(Enemy* c) {}
	virtual void SetDamage(Block* c) {}
	virtual void SetDamage(Projectile* c) {}
	virtual void SetDamage(Item* c) {}

	bool	CheckDamageFrom(const GameEntity& e) const;
	
	float	DistSqGameEntity(std::shared_ptr<GameEntity> e) const; // �I�u�W�F�N�g�Ԃ̋���
	float	AtanGameEntity(std::shared_ptr<GameEntity> e) const; // �I�u�W�F�N�g�Ԃ̊p�x
	std::shared_ptr<GameEntity> FindNearest(ObjectType t) const; // �ł��߂��I�u�W�F�N�g��Ԃ�

protected:
	bool		exist;
	bool		dir;
	ObjectType	otype;
	Vector2		pos, vel;
	Priority	priority; // �l�������������珇�Ɏ��s�����
	std::shared_ptr<ImageData> img;
	std::shared_ptr<HitBox> atkHitbox, dmgHitbox;
	byte		hitFilter;	// ���r�b�g�̃L�������m�̔�����s��Ȃ�

	int			hp, hpmax;
	int			stateCnt;
	int			state;
	int			width, height;
	int			invincible;		// > 0 �Ŗ��G
	bool		ignoreInv;		// �U�������G�ђ�

private:
	bool		rmflag;

public:
	static void InitManager();
	static void UpdateAll();
	static void DrawAll();
	static std::shared_ptr<GameEntity> Create(std::shared_ptr<GameEntity> e);
	static void SetStage(std::shared_ptr<Stage> st);
	
protected:
	static std::shared_ptr<Stage> stage;
private:
	static std::list<std::shared_ptr<GameEntity>> manager;
	static std::list<GameEntity*> atkColobj, dmgColobj; // �����蔻����`�F�b�N����I�u�W�F�N�g
};

inline bool GameEntity::GetExist() const
{
	return exist && !rmflag;
}

inline bool GameEntity::GetRemoveFlag() const
{
	return rmflag;
}

inline ObjectType GameEntity::GetObjectType() const
{
	return otype;
}

inline const Vector2& GameEntity::GetPos() const
{
	return pos;
}

inline bool GameEntity::GetDirection() const
{
	return dir;
}

inline Priority GameEntity::GetPriority() const
{
	return priority;
}

inline int GameEntity::GetHeight() const
{
	return height;
}

inline int GameEntity::GetWidth() const
{
	return width;
}

inline void GameEntity::SetVel(const Vector2& v)
{
	vel.x = dir ? -v.x : v.x;
	vel.y = v.y;
}

inline void GameEntity::SetXVel(float x)
{
	vel.x = dir ? -x : x;
}

inline void GameEntity::SetYVel(float y)
{
	vel.y = y;
}

inline void GameEntity::SetPriority(Priority pr)
{
	priority = pr;
}

inline void GameEntity::SetDirection(bool d) 
{
	dir = d;
}

inline void GameEntity::SetStage(std::shared_ptr<Stage> st)
{
	stage = st;
}

inline void GameEntity::AddHP(int h)
{
	hp += h;
	if (hp > hpmax) hp = hpmax;
}

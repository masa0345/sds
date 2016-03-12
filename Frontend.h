#pragma once

#include <vector>
#include <list>
#include <memory>
#include "Vector2.h"

struct ImageData;
class Player;
class Enemy;
enum BulletType;

class Frontend
{
public:
	Frontend();
	virtual ~Frontend();

	virtual void Update();
	virtual void Draw() const;

	bool GetExist() const;

	static void InitManager();
	static void Create(std::shared_ptr<Frontend>);
	static void UpdateAll();
	static void DrawAll();
protected:
	bool exist;
	Vector2 pos;
	//std::vector<ImageData> img;

	static std::list<std::shared_ptr<Frontend>> manager;
};

class FrontendPlayerGauge : public Frontend
{
public:
	FrontendPlayerGauge(Player* p);
	virtual void Update();
	virtual void Draw() const;
	void SetTarget(Player* p);

private:
	Player* target;
	BulletType type;
	int stateCnt, hp, hpmax, mp, mpmax, fire;
	const int HP_WIDTH = 249;
	const int MP_WIDTH = 249;
	const int HP_HEIGHT = 12;
	const int MP_HEIGHT = 8;
};
#pragma once

#include "Camera.h"
#include <vector>

class Player;

struct EnemyMapPos {
	int x, y, type;
	int area = -1;
};

class Stage
{
public:
	Stage();
	int LoadStage();		// �X�e�[�W�ǂݍ���
	void PlaceEnemies();	// �G�z�u
	bool CheckAreaScroll() const;
	void GoNextArea();
	void GoNextMap();

	Camera* GetCamera();
	std::pair<int, int> GetMapMaxXY() const;
	int GetStageNum() const;
	int GetMapNum() const;
	int GetAreaNum() const;
	bool GetLoadNext() const;
	std::shared_ptr<Player> GetPlayer() const;
	const std::vector<EnemyMapPos>& GetEnemyMapPos() const;
	const int* GetArea(int mn) const;
	void SetStageMapNum(int stg, int map);
	void SetPlayer(std::shared_ptr<Player> p);
	void AddScore(int s);
	void SetLoadNext(bool load);
	
private:
	std::shared_ptr<Player> player;
	Camera camera;
	bool clear;
	int timer;
	int stock;
	int score, prevScore;
	int deathCnt;
	int rank;
	int stgNum, mapNum, areaNum;
	int mmx, mmy;
	bool loadNext;
	int area[10][5];
	std::vector<EnemyMapPos> enemy;
};

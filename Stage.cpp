#include "Stage.h"
#include "Player.h"
#include "common.h"
#include "MapChip.h"
#include "Enemy.h"
#include "Block.h"
#include <DxLib.h>

namespace {
	const int ENEMY_LOAD_MAX = 100;

	struct StageLoadData
	{
		int		px, py, scx, scy;
		int		mmx, mmy;
		bool	dir;
		int		area[10][5];
		EnemyMapPos	enemy[ENEMY_LOAD_MAX];
		unsigned long	map[MMX][MMY];
	};
}


Stage::Stage() {
	clear = false;
	timer = 0;
	stock = 3;
	score = prevScore = 0;
	deathCnt = 0;
	rank = 0;
	loadNext = false;
	enemy.resize(ENEMY_LOAD_MAX);
}

int Stage::LoadStage() {
	char fname[32];
	sprintf_s(fname, "data/stage%d/%d/map.dat", stgNum, mapNum);
	int fp = FileRead_open(fname);
	if (fp == NULL) {
		MessageBox(GetMainWindowHandle(), "�X�e�[�W�ǂݍ��݃G���[", "", MB_OK);
		//exitFlag = true;
		return -1;
	}
	StageLoadData data;
	FileRead_read(&data, sizeof(StageLoadData), fp);
	FileRead_close(fp);

	player->SetPos({ data.px*MW + player->GetWidth() / 2,
					 data.py*MH + player->GetHeight() / 2 });
	player->SetDirection(!data.dir);
	camera.SetPos({ data.scx*MW, data.scy*MH });
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			area[i][j] = data.area[i][j];
		}
	}
	mmx = data.mmx + 1;
	mmy = data.mmy + 1;

	camera.SetField({ 0, 0, mmx*MW, mmy*MH });

	for (int i = 0; i < ENEMY_LOAD_MAX; i++) {
		if (data.enemy[i].x == -1) break;
		enemy[i] = data.enemy[i];
	}
	MapChip::Instance()->Set(*this, data.map);

	return 0;
}

void Stage::PlaceEnemies() {
	for (auto& e : enemy) {
		if (e.area != -1 /*&& e.area == mapNum*/) {
			std::shared_ptr<GameEntity> o = nullptr;
			switch (e.type) {
			case 0: o = GameEntity::Create(std::make_shared<EnemyType0>()); break;
			case 1: o = GameEntity::Create(std::make_shared<EnemyType1>()); break;
			case 2: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			case 3: o = GameEntity::Create(std::make_shared<EnemyType3>()); break;
			case 4: o = GameEntity::Create(std::make_shared<BlockLift>()); break;
			case 5: o = GameEntity::Create(std::make_shared<BlockRotaLiftR>()); break;
			case 6: o = GameEntity::Create(std::make_shared<BlockRotaLiftL>()); break;
			case 7: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			case 8: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			case 9: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			}
			if (o) {
				o->SetPos({ e.x*MW + o->GetWidth()/2, e.y*MH + o->GetHeight()/2 });
			}
		}
	}
}

Camera* Stage::GetCamera() {
	return &camera;
}

std::pair<int, int> Stage::GetMapMaxXY() const {
	return{ mmx, mmy };
}

int Stage::GetStageNum() const {
	return stgNum;
}

int Stage::GetMapNum() const {
	return mapNum;
}

std::shared_ptr<Player> Stage::GetPlayer() const {
	return player;
}

const std::vector<EnemyMapPos>& Stage::GetEnemyMapPos() const {
	return enemy;
}

void Stage::SetStageMapNum(int stg, int map) {
	stgNum = stg;
	mapNum = map;
}

void Stage::SetPlayer(std::shared_ptr<Player> p) {
	player = p;
	camera.SetTarget(p);
}

void Stage::AddScore(int s) {
	score += s;
}
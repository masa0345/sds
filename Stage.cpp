#include "Stage.h"
#include "Player.h"
#include "common.h"
#include "MapChip.h"
#include "Enemy.h"
#include "Block.h"
#include "Sound.h"
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
	areaNum = 0;
	loadNext = false;
	enemy.resize(ENEMY_LOAD_MAX);
}

int Stage::LoadStage() {
	char fname[32];
	sprintf_s(fname, "data/stage%d/%d/map.dat", stgNum, mapNum);
	int fp = FileRead_open(fname);
	if (fp == NULL) {
		MessageBox(GetMainWindowHandle(), "ステージ読み込みエラー", "", MB_OK);
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
	areaNum = 0;

	camera.SetField({ area[areaNum][0]*MW, area[areaNum][1]*MH,
		area[areaNum][2]*MW+MW, area[areaNum][3]*MH+MH });

	for (int i = 0; i < ENEMY_LOAD_MAX; i++) {
		if (data.enemy[i].x == -1) break;
		enemy[i] = data.enemy[i];
	}
	MapChip::Instance()->Set(*this, data.map);

	camera.Update();

	return 0;
}

void Stage::PlaceEnemies() {
	for (auto& e : enemy) {
		if (e.area != -1 && e.area == areaNum) {
			std::shared_ptr<GameEntity> o = nullptr;
			switch (e.type) {
			case 0: o = GameEntity::Create(std::make_shared<EnemyType0>()); break;
			case 1: o = GameEntity::Create(std::make_shared<EnemyType1>()); break;
			case 2: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			case 3: o = GameEntity::Create(std::make_shared<EnemyType3>()); break;
			case 4: o = GameEntity::Create(std::make_shared<BlockLift>()); break;
			case 5: o = GameEntity::Create(std::make_shared<BlockRotaLiftR>()); break;
			case 6: o = GameEntity::Create(std::make_shared<BlockRotaLiftL>()); break;
			case 7: o = GameEntity::Create(std::make_shared<BlockNextMapGate>()); break;
			case 8: o = GameEntity::Create(std::make_shared<BlockBreakable>()); break;
			case 9: o = GameEntity::Create(std::make_shared<EnemyStage1Boss>()); break;
			}
			if (o) {
				o->SetPos({ e.x*MW + o->GetWidth()/2, e.y*MH + o->GetHeight()/2 });
			}
		}
	}
}

bool Stage::CheckAreaScroll() const {
	return (area[areaNum][4] == 0 && player->GetPos().x > area[areaNum][2] * MW) ||
		(area[areaNum][4] == 1 && player->GetPos().x < area[areaNum][0] * MW + MW);
}

void Stage::GoNextArea() {
	++areaNum;
	GameEntity::RemoveExceptPlayer();
	PlaceEnemies();
	camera.SetField({ area[areaNum][0] * MW, area[areaNum][1] * MH,
		area[areaNum][2] * MW + MW, area[areaNum][3] * MH + MH });
}

void Stage::GoNextMap() {
	++mapNum;
	GameEntity::RemoveExceptPlayer();
	LoadStage();
	PlaceEnemies();
	player->InitState();
	loadNext = false;
}

void Stage::LoadBGM()
{
	Sound::Instance()->LoadBGM("stage" + std::to_string(stgNum));
	Sound::Instance()->LoadBGM("boss" + std::to_string(stgNum));
}

void Stage::PlayBGM(bool boss)
{
	if (boss) {
		Sound::Instance()->Play("boss" + std::to_string(stgNum));
	} else {
		Sound::Instance()->Play("stage" + std::to_string(stgNum));
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

int Stage::GetAreaNum() const {
	return areaNum;
}

bool Stage::GetLoadNext() const {
	return loadNext;
}

std::shared_ptr<Player> Stage::GetPlayer() const {
	return player;
}

const std::vector<EnemyMapPos>& Stage::GetEnemyMapPos() const {
	return enemy;
}

const int* Stage::GetArea(int mn) const {
	return area[mn];
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

void Stage::SetLoadNext(bool load)
{
	loadNext = load;
}
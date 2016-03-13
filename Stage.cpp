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

	//スコア基準時間
	const int score_base[] = {
		0, 15000, 2000, 2000, 2000, 2000,
	};
	//ランク基準点
	const int rank_base[] = {
		-9999, 13000, 8000, 8000, 8000, 8000,
	};
}


Stage::Stage() {
	stock = 3;
	timer = 0;
	score = 0;
	areaNum = 0;
	rank = 0;
	clear = false;
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

void Stage::ForwardTimer() {
	if(!clear) ++timer;
}

//ステージクリア点を加算&ランク算出　ボーナス + クリアタイム + 残りHP - 死亡回数
void Stage::AddClearScore(int bonus)
{
	score += bonus + (score_base[stgNum] - timer) / 2 + player->GetHP() * 3 - 1000 * (3-stock) * 3;
	if (score < 0) score = 0;

	int mid = score_base[stgNum];
	if (score > mid + 2999) rank = 0;	// S
	else if (InRange(mid + 999, score, mid + 3000)) rank = 1; // A
	else if (InRange(mid - 1001, score, mid + 1000)) rank = 2; // B
	else if (InRange(mid - 3001, score, mid - 1000)) rank = 3; // C
	else rank = 4; // D
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

bool Stage::GetClear() const
{
	return clear;
}

int Stage::GetStock() const
{
	return stock;
}

int Stage::GetScore() const
{
	return score;
}

int Stage::GetRank() const
{
	return rank;
}

int Stage::GetTimer() const
{
	return timer;
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

const char * Stage::GetTimerToString(int cnt) const
{
	if (cnt < 0) cnt = 0;
	static char t[16];
	int m = cnt / 3600;
	int s = (cnt % 3600) / 60;
	int ms = (cnt % 60) * 10 / 6;

	sprintf_s(t, "%02d:%02d:%02d", m, s, ms);

	return t;
}

const char * Stage::GetTimerToString() const
{
	return GetTimerToString(timer);
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

void Stage::SetClear(bool c)
{
	clear = c;
}

void Stage::SetStock(int s)
{
	stock = s;
}

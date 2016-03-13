#include "Scene.h"
#include "Bright.h"
#include "Input.h"
#include "Sound.h"
#include "Image.h"
#include "Projectile.h"
#include "Stage.h"
#include "Player.h"
#include "Frontend.h"
#include "MapChip.h"
#include "Event.h"
#include "Save.h"
#include "Font.h"
#include <DxLib.h>

namespace {
	std::vector<std::shared_ptr<Font>> fonts;
}

SceneManager::SceneManager() : gameExit(false)
{
	Image::Instance()->Init("data/image/image.csv");
	Sound::Instance()->Init();
	PlayerBullet::Load();
	input = std::make_shared<JoypadInput>(DX_INPUT_PAD1 | DX_INPUT_KEY);
	Scene::SetInput(input);
	scene = new SceneDebugStart();
}

SceneManager::~SceneManager()
{
	if (scene) delete scene;
}

void SceneManager::Update()
{
	input->Update();
	Sound::Instance()->UpdateState();
	Bright::Instance()->Update();

	Scene* ns = scene->Update();
	if (ns != scene) {
		delete scene;
		scene = ns;
	}
	if (input->Get(INPUT_ESC) == 1) gameExit = true;
}

bool SceneManager::GetGameExit() const
{
	return gameExit;
}

std::shared_ptr<JoypadInput> Scene::input = nullptr;
void Scene::SetInput(std::shared_ptr<JoypadInput> in) {
	input = in;
}

Scene::Scene() : state(0), stateCnt(0)
{
}

// デバッグ用 タイトルを飛ばしてステージからスタート
SceneDebugStart::SceneDebugStart() 
{
}

Scene* SceneDebugStart::Update()
{
	fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 24, 4, "data/font/ume-pgc5.ttf"));
	fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 16, 3, "data/font/ume-pgc5.ttf", DX_FONTTYPE_ANTIALIASING_EDGE));
	auto stage = std::make_shared<Stage>();
	stage->SetStageMapNum(1, 0);
	stage->LoadBGM();
	return new SceneStageStart(stage);
}

// ステージ開始
SceneStageStart::SceneStageStart(std::shared_ptr<Stage> s) : stage(s)
{
	GameEntity::InitManager();
	Frontend::InitManager();
	auto p = std::make_shared<Player>(input);
	stage->SetPlayer(p);
	GameEntity::SetStage(stage);
	GameEntity::Create(p);
	stage->LoadStage();
	stage->PlaceEnemies();
	stage->PlayBGM(false);
	images.push_back(Image::Instance()->Load("stage_name")->at(stage->GetStageNum())); //[0]
	images.push_back(Image::Instance()->Load("cross")->at(0));	//[1]
	images.push_back(Image::Instance()->Load("num")->at(stage->GetStock())); //[2]
}

Scene* SceneStageStart::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(255, 0)) {
			++state;
			stateCnt = 0;
		}
		break;
	case 1:
		if (stateCnt < 30) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
			DrawGraph(150, 120 - (30 - stateCnt), images[0], TRUE);
			DrawGraph(150, 120 + (30 - stateCnt), images[0], TRUE);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 30);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30 - (30 - stateCnt), images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16 - (30 - stateCnt), images[2], TRUE);
		} else if (stateCnt < 40) {
			DrawGraph(150, 120, images[0], TRUE);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30, images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16, images[2], TRUE);
		} else if (stateCnt < 90) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - stateCnt * 3);
			DrawGraph(150, 120, images[0], TRUE);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30, images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16, images[2], TRUE);
		} else {
			return new SceneGameMain(stage);
		}
	}
	++stateCnt;

	return this;
}

// ゲームメイン
SceneGameMain::SceneGameMain(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneGameMain::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	if (!stage->GetPlayer()->GetExist()) {
		return new ScenePlayerDeath(stage);
	}
	if (stage->CheckAreaScroll()) {
		return new SceneAreaScroll(stage);
	}
	if (stage->GetLoadNext()) {
		return new SceneMoveMap(stage);
	}
	if (stage->GetClear()) {
		return new SceneStageClear(stage);
	}
	if (Event::GetEventFlag()) {
		return new SceneEvent(stage);
	}

	return this;
}

// 部屋移動
SceneAreaScroll::SceneAreaScroll(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneAreaScroll::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	float speed = 10.f;
	Vector2 cp = stage->GetCamera()->GetPos();
	Vector2 pp = stage->GetPlayer()->GetPos();
	int ph = stage->GetPlayer()->GetHeight(), pw = stage->GetPlayer()->GetWidth();
	auto area = stage->GetArea(stage->GetAreaNum() + 1);
	if (cp.x < area[0] * MW || cp.y < area[1] * MH ||
		cp.x > area[2] * MW + MW - WINDOW_WIDTH || 
		cp.y > area[3] * MW + MW - WINDOW_HEIGHT) {
		if (cp.x < area[0] * MW) cp.x += speed;
		else if (cp.x > area[2] * MW + MW - WINDOW_WIDTH) cp.x -= speed;
		if (cp.y < area[1] * MH) cp.y += speed;
		else if (cp.y > area[3] * MW + MW - WINDOW_HEIGHT) cp.y -= speed;
	} else if (pp.y + ph / 2 - cp.y > WINDOW_HEIGHT - 120 && 
			cp.y + 10 < area[3] * MH - WINDOW_HEIGHT) {
		cp.y += speed;
	} else if (pp.y - ph / 2 - cp.y < 120 && cp.y - 10 > area[1] * MH) {
		cp.y -= speed;
	} else {
		stage->GoNextArea();
		return new SceneGameMain(stage);
	}
	stage->GetCamera()->SetPos(cp);

	return this;
}

// マップ移動
SceneMoveMap::SceneMoveMap(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneMoveMap::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(0, 12)) state = 1;
		break;
	case 1:
		stage->GoNextMap();
		state = 2;
		break;
	case 2:
		if (Bright::Instance()->ChangeBright(255, 12)) {
			return new SceneGameMain(stage);
		}
		break;
	}
	return this;
}

// イベントシーン
SceneEvent::SceneEvent(std::shared_ptr<Stage> s) : stage(s)
{
	input->SetNoInputFlag(true);
}

Scene* SceneEvent::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	if (Event::GetEventFlag()) {
		Event::curEvent->Update();
		return this;
	}

	input->SetNoInputFlag(false);
	return new SceneGameMain(stage);
}

// 自機死亡
ScenePlayerDeath::ScenePlayerDeath(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* ScenePlayerDeath::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	int stock = stage->GetStock();
	switch (state) {
	case 0:
		if (stateCnt == 60) {
			if (stock > 0) {
				state = 1;
				stateCnt = 0;
				stage->SetStock(stock - 1);
			} else {
				state = 2;
				stateCnt = 0;
			}
			Sound::Instance()->FadeOut("default", 120);
		}
		break;
	case 1:
		if (Bright::Instance()->ChangeBright(0, 120)) {
			return new SceneStageStart(stage);
		}
		break;
	case 2:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (stateCnt > 60) {
			stateCnt = 0;
			state++;
		}
		break;
	case 3:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (stateCnt > 120) {
			stateCnt = 0;
			state++;
		}
		break;
	case 4:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (Bright::Instance()->ChangeBright(0, 120)) {
			Save::Instance()->SaveStageResult(stage);
			Save::Instance()->SaveScore();
			//return GAME_TITLE;
		}
		break;
	}
	++stateCnt;

	return this;
}

// ステージクリア
SceneStageClear::SceneStageClear(std::shared_ptr<Stage> s) : stage(s)
{
	images.push_back(Image::Instance()->Load("clear")->at(0)); // [0]
	images.push_back(Image::Instance()->Load("result")->at(0)); // [1]
	images.push_back(Image::Instance()->Load("rank")->at(0)); // [2]
	images.push_back(Image::Instance()->Load("rank")->at(1)); // [3]
	images.push_back(Image::Instance()->Load("rank")->at(2)); // [4]
	images.push_back(Image::Instance()->Load("rank")->at(3)); // [5]
	images.push_back(Image::Instance()->Load("rank")->at(4)); // [6]
}

Scene* SceneStageClear::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	int sx = 120, sy = 40;
	switch (state) {
	case 0:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 30);
		DrawGraph(sx, sy, images[0], TRUE);
		if (stateCnt == 30) {
			stateCnt = 0;
			state++;
		}
		Save::Instance()->SaveStageResult(stage);
		Save::Instance()->SaveScore();
		break;
	case 1:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		if (stateCnt == 60) {
			stateCnt = 0;
			state++;
		}
		break;
	case 2:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
		}
		break;
	case 3:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stateCnt*stage->GetScore() / 90);
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
		}
		break;
	case 4:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stage->GetScore());

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		DrawGraph(sx + 83, sy + 205, images[2 + stage->GetRank()], TRUE);
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
			Sound::Instance()->FadeOut("default", 240);
		}
		break;
	case 5:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stage->GetScore());
		DrawGraph(sx + 83, sy + 205, images[2 + stage->GetRank()], TRUE);

		if (stateCnt > 120) {
			if (Bright::Instance()->ChangeBright(0, 120)) {
				Sound::Instance()->StopAll();
				//return STAGE_SELECT;
			}
		}
		break;
	}
	++stateCnt;

	return this;
}


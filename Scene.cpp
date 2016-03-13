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
#include <DxLib.h>

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

Scene::Scene() : state(0)
{
}

// デバッグ用 タイトルを飛ばしてステージからスタート
SceneDebugStart::SceneDebugStart() 
{
}

Scene* SceneDebugStart::Update()
{
	return new SceneStageStart(1, 2);
}

// ステージ開始
SceneStageStart::SceneStageStart(int sn, int mn)
{
	GameEntity::InitManager();
	Frontend::InitManager();
	auto p = std::make_shared<Player>(input);
	stage = std::make_shared<Stage>();
	stage->SetPlayer(p);
	stage->SetStageMapNum(sn, mn);
	GameEntity::SetStage(stage);
	GameEntity::Create(p);
	stage->LoadStage();
	stage->PlaceEnemies();
}

Scene* SceneStageStart::Update()
{
	return new SceneGameMain(stage);
}

// ゲームメイン
SceneGameMain::SceneGameMain(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene * SceneGameMain::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	if (stage->CheckAreaScroll()) {
		return new SceneAreaScroll(stage);
	}
	if (stage->GetLoadNext()) {
		return new SceneMoveMap(stage);
	}

	return this;
}

// 部屋移動
SceneAreaScroll::SceneAreaScroll(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene * SceneAreaScroll::Update()
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

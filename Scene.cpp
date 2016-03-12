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

Scene::Scene()
{
}

// デバッグ用 タイトルを飛ばしてステージからスタート
SceneDebugStart::SceneDebugStart() 
{
}

Scene* SceneDebugStart::Update()
{
	return new SceneStageStart(1, 0);
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
SceneGameMain::SceneGameMain(std::shared_ptr<Stage> s)
{
	stage = s;
}

Scene * SceneGameMain::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	return this;
}

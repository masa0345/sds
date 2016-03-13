#pragma once

#include <vector>
#include <memory>

class Scene;
class JoypadInput;
class Stage;

//シーン管理クラス
class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update();
	bool GetGameExit() const;

private:
	bool gameExit;
	Scene* scene;
	std::shared_ptr<JoypadInput> input;
};

class Scene
{
public:
	Scene();
	virtual Scene* Update() = 0;
	static void SetInput(std::shared_ptr<JoypadInput> i);
protected:
	int state, stateCnt;
	std::vector<int> images;
	static std::shared_ptr<JoypadInput> input;
};

// デバッグ初期化シーン
class SceneDebugStart : public Scene
{
public:
	SceneDebugStart();
	Scene* Update() override;
};

// ステージ開始
class SceneStageStart : public Scene
{
public:
	SceneStageStart(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// ゲームメイン
class SceneGameMain : public Scene
{
public:
	SceneGameMain(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// 部屋移動
class SceneAreaScroll : public Scene
{
public:
	SceneAreaScroll(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// マップ移動
class SceneMoveMap : public Scene
{
public:
	SceneMoveMap(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// イベントシーン
class SceneEvent : public Scene
{
public:
	SceneEvent(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// 自機死亡
class ScenePlayerDeath : public Scene
{
public:
	ScenePlayerDeath(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// ステージクリア
class SceneStageClear : public Scene
{
public:
	SceneStageClear(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};
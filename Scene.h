#pragma once

#include <vector>
#include <memory>

//シーンの状態
/*
typedef enum GameState {
	GAME_TITLE_INIT,
	GAME_TITLE,
	OPTION,
	KEY_CONFIG,
	STAGE_SELECT,
	STAGE_START,
	GAME_MAIN,
	PAUSE,
	AREA_SCROLL,
	MOVE_MAP,
	GAME_OVER,
	GAME_CLEAR,
	PLAYER_DEATH,
	EVENT,
	MESSAGE,
	DEBUG_START,
	GS_ERROR
};*/

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
	int state;
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
	SceneStageStart(int sn, int mn = 0);
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
#pragma once

#include <vector>
#include <memory>

//�V�[���̏��
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

//�V�[���Ǘ��N���X
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

// �f�o�b�O�������V�[��
class SceneDebugStart : public Scene
{
public:
	SceneDebugStart();
	Scene* Update() override;
};

// �X�e�[�W�J�n
class SceneStageStart : public Scene
{
public:
	SceneStageStart(int sn, int mn = 0);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// �Q�[�����C��
class SceneGameMain : public Scene
{
public:
	SceneGameMain(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// �����ړ�
class SceneAreaScroll : public Scene
{
public:
	SceneAreaScroll(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// �}�b�v�ړ�
class SceneMoveMap : public Scene
{
public:
	SceneMoveMap(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// �C�x���g�V�[��
class SceneEvent : public Scene
{
public:
	SceneEvent(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};
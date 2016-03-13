#pragma once

#include <vector>
#include <memory>

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
	int state, stateCnt;
	std::vector<int> images;
	static std::shared_ptr<JoypadInput> input;
};

// �f�o�b�O�������V�[��
class SceneDebugStart : public Scene
{
public:
	SceneDebugStart();
	Scene* Update() override;
};

// �^�C�g���̑O
class SceneTitleInit : public Scene
{
public:
	SceneTitleInit();
	Scene* Update() override;
};

// �X�e�[�W�J�n
class SceneStageStart : public Scene
{
public:
	SceneStageStart(std::shared_ptr<Stage> s);
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

// ���@���S
class ScenePlayerDeath : public Scene
{
public:
	ScenePlayerDeath(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// �X�e�[�W�N���A
class SceneStageClear : public Scene
{
public:
	SceneStageClear(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};

// ���j���[�t���V�[��
class SceneWithMenu : public Scene
{
public:
	SceneWithMenu();
protected:
	void MoveCursor(int x, int y);
	int MoveCursorX(int menuNum, bool loop = true);
	int MoveCursorY(int menuNum, bool loop = true);

	int curX, curY;
	std::vector<bool> curFlag;
};


// �^�C�g�����
class SceneTitle : public SceneWithMenu
{
public:
	SceneTitle(int sc = 0);
	Scene* Update() override;
private:
	int backScroll;
};

// �X�e�[�W�I��
class SceneStageSelect : public SceneWithMenu
{
public:
	SceneStageSelect(int sc = 0);
	Scene* Update() override;
private:
	int backScroll;
	std::shared_ptr<Stage> stage;
};

// �I�v�V����

// �|�[�Y
class ScenePause : public SceneWithMenu
{
public:
	ScenePause(std::shared_ptr<Stage> s);
	Scene* Update() override;
private:
	std::shared_ptr<Stage> stage;
};



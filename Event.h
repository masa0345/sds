#pragma once

#include <memory>

class Stage;
class Message;
class EnemyStage1Boss;
class EnemyGuide;

class Event
{
	friend class SceneEvent;
public:
	Event(std::shared_ptr<Stage> st);
	static bool GetEventFlag();
	static void SetEvent(std::shared_ptr<Event> e);

protected:
	virtual void Update() = 0;
	int state, stateCnt;
	std::shared_ptr<Stage> stage;
	std::unique_ptr<Message> message;
	static std::shared_ptr<Event> curEvent;
	static bool IsMessageDisplay();
};

// �{�X�o��
class EventStage1Boss : public Event
{
public:
	EventStage1Boss(std::shared_ptr<Stage> s, EnemyStage1Boss* b);
protected:
	void Update() override;
private:
	EnemyStage1Boss* boss;
};

// ���b�Z�[�W��\�����邾��
class EventMessage : public Event
{
public:
	EventMessage(std::shared_ptr<Stage> s, const char* name);
protected:
	void Update() override;
};

// �`���[�g���A���F���틭���ƓG�o��
class EventGuideSpawnEnemy : public Event
{
public:
	EventGuideSpawnEnemy(std::shared_ptr<Stage> s, EnemyGuide* g);
protected:
	void Update() override;
private:
	EnemyGuide* guide;
};
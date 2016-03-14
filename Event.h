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

// ボス登場
class EventStage1Boss : public Event
{
public:
	EventStage1Boss(std::shared_ptr<Stage> s, EnemyStage1Boss* b);
protected:
	void Update() override;
private:
	EnemyStage1Boss* boss;
};

// メッセージを表示するだけ
class EventMessage : public Event
{
public:
	EventMessage(std::shared_ptr<Stage> s, const char* name);
protected:
	void Update() override;
};

// チュートリアル：武器強化と敵出現
class EventGuideSpawnEnemy : public Event
{
public:
	EventGuideSpawnEnemy(std::shared_ptr<Stage> s, EnemyGuide* g);
protected:
	void Update() override;
private:
	EnemyGuide* guide;
};
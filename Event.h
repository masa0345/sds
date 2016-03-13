#pragma once

#include <memory>

class Stage;
class EnemyStage1Boss;

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
	static std::shared_ptr<Event> curEvent;
};

// É{ÉXìoèÍ
class EventStage1Boss : public Event
{
public:
	EventStage1Boss(std::shared_ptr<Stage> s, EnemyStage1Boss* b);
protected:
	void Update() override;
private:
	EnemyStage1Boss* boss;
};
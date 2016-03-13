#include "Event.h"
#include "Player.h"
#include "Enemy.h"
#include "Frontend.h"
#include "MapChip.h"
#include "Sound.h"
#include "Image.h"
#include "Stage.h"
#include "Input.h"
#include "Effect.h"

std::shared_ptr<Event> Event::curEvent = nullptr;

Event::Event(std::shared_ptr<Stage> st) : stage(st), state(0), stateCnt(0)
{
}

bool Event::GetEventFlag() {
	return curEvent != nullptr;
}

void Event::SetEvent(std::shared_ptr<Event> e) {
	curEvent = e;
}

// ボス登場
EventStage1Boss::EventStage1Boss(std::shared_ptr<Stage> s, EnemyStage1Boss * b) 
	: Event(s), boss(b)
{
}

void EventStage1Boss::Update()
{
	auto c = stage->GetCamera();
	switch (state) {
	case 0:
		Sound::Instance()->FadeOut("default", 30);
		if (stateCnt == 40) {
			boss->pos.y -= 1000.f;
			stage->GetPlayer()->SetIgnoreCamera(true);
			c->SetDefaultPos();
			c->EventMove(boss->pos.x - c->GetPos().x - WINDOW_WIDTH / 2.f, 0.f, 60);
			state = 1;
			stateCnt = 0;
		}
		break;
	case 1:
		if (c->EventMove()) {
			boss->img->drawflag = true;
			boss->img->num = 5;
			c->EventMove(-120.f, 0.f, 150);
			state = 2;
			stateCnt = 0;
		}
		break;
	case 2:
		if (stateCnt == 101) {
			boss->vel.y = 20.f;
		}
		if (stateCnt == 40) {
			stage->GetPlayer()->GetInput()->SetVirtualInput(INPUT_RIGHT, 1);
		}
		if (c->EventMove()) {
			boss->img->num = 6;
			boss->vel.y = 0.f;
			Sound::Instance()->Play("ボス着地");
			stage->GetPlayer()->GetInput()->SetVirtualInput(INPUT_RIGHT, 0);
			c->SetShake(24.f, 75.f, 60, 60);
			c->ReturnScrollPos(180);
			state = 3;
			stateCnt = 0;
		}
		break;
	case 3:
		if (stateCnt == 120) {
			GameEntity::Create(std::make_shared<EffectWarning>());
			boss->img->num = 0;
			state = 4;
			stateCnt = 0;
		}
		break;
	case 4:
		if (c->ReturnScrollPos(180)) {
			boss->target = true;
			boss->invincible = 0;
			stage->GetPlayer()->SetIgnoreCamera(false);
			stage->PlayBGM(true);
			curEvent = nullptr;
		}
		break;
	}
	++stateCnt;
}

#include "GameEntity.h"
#include "Image.h"
#include "Stage.h"
#include <DxLib.h>

#undef max

std::shared_ptr<Stage> GameEntity::stage = nullptr;
std::list<std::shared_ptr<GameEntity>> GameEntity::manager;
std::list<GameEntity*> GameEntity::atkColobj, GameEntity::dmgColobj;

GameEntity::GameEntity(const Vector2& pos) :
	pos(pos),
	exist(true),
	rmflag(false),
	dir(RIGHT),
	otype(UNKNOWN),
	priority(Priority::EFFECT),
	img(std::make_shared<ImageData>()),
	atkHitbox(nullptr), dmgHitbox(nullptr),
	hitFilter(0),
	hp(0), hpmax(-1),
	state(0), stateCnt(0),
	width(0), height(0), 
	invincible(0), ignoreInv(false)
{
}

GameEntity::~GameEntity()
{
}


void GameEntity::Draw() const {
	if (!img) return;
	if (!img->drawflag || img->hdl == 0) return;
	Vector2 p = stage->GetCamera()->TransCoord(pos);
	SetDrawBlendMode(img->blendmode, img->alpha);
	DrawRotaGraphF(p.x+img->ofs_x, p.y+img->ofs_y, 
		img->exRate, img->angle, img->hdl->at(img->num), TRUE, (int)dir);

	//if (dmgHitbox) dmgHitbox->DrawRect(p, dir, false);
	//if (atkHitbox) atkHitbox->DrawRect(p, dir, true);
}


bool GameEntity::UpdatePre()
{
	if (!exist) rmflag = true;
	return !exist;
}

void GameEntity::UpdateLate()
{
	++stateCnt;
	// 当たり判定登録
	if (exist) {
		if (atkHitbox) atkColobj.push_back(this);
		if (dmgHitbox) dmgColobj.push_back(this);
	}
}

void GameEntity::UpdatePosition()
{
	// 座標更新
	pos.y += vel.y;
	pos.x += dir ? -vel.x : vel.x;
}

void GameEntity::UpdateColResponse() { }

bool GameEntity::CheckDamageFrom(const GameEntity& e) const {
	// 同じオブジェクト
	if (&e == this) return false;
	// フィルター
	if (e.hitFilter & hitFilter) return false;
	// 無敵判定
	if (!e.ignoreInv && invincible > 0) return false;
	// ブロック判定はwidth, heightで行う
	if (e.otype == BLOCK) {
		Rect r(-width / 2, -height / 2, width / 2, height / 2);
		return (e.atkHitbox->GetRects()[0] + e.pos).CheckHit(r+pos);
	}
	return e.atkHitbox->CheckHitRects(*dmgHitbox, e.pos, pos, e.dir, dir);
}

float GameEntity::DistSqGameEntity(std::shared_ptr<GameEntity> e) const {
	return (e->pos-pos).LengthSquare();
}

float GameEntity::AtanGameEntity(std::shared_ptr<GameEntity> e) const {
	if (!e) return 0.f;
	return (e->pos - pos).Angle();
}

// 自分が向いている方向の中で一番近くのotypeのオブジェクトを見つける
std::shared_ptr<GameEntity> GameEntity::FindNearest(ObjectType t) const {
	std::shared_ptr<GameEntity> ret = nullptr;
	float mindist = std::numeric_limits<float>::max();
	for (auto e : manager) {
		if (e->otype == t && e->Targetable() && e->exist && 
			((dir && e->pos.x < pos.x) || (!dir && pos.x < e->pos.x))) {
			float d = DistSqGameEntity(e);
			if (d < mindist) {
				ret = e;
				mindist = d;
			}
		}
	}
	return ret;
}

void GameEntity::InitManager() {
	manager.clear();
	atkColobj.clear();
	dmgColobj.clear();
}

void GameEntity::UpdateAll() {
	if (manager.empty()) return;

	// rmflag == trueのオブジェクトを削除
	manager.erase(
		std::remove_if(manager.begin(), manager.end(),
			[](std::shared_ptr<GameEntity> p) { return p->GetRemoveFlag(); }),
		manager.end());
	
	// 更新
	for (auto e : manager) {
		if (e->UpdatePre()) continue;
		e->Update();
		e->UpdatePosition();
		e->UpdateColResponse();
		e->UpdateLate();
	}
	stage->GetCamera()->Update();

	// 当たり判定
	for (auto o1 : dmgColobj) {
		for (auto o2 : atkColobj) {
			if (o1->CheckDamageFrom(*o2)) o1->DamageFrom(o2);
		}
	}
	DebugDraw::String({ 0, 16 }, 0xffffff, "Entity: %d %d %d", manager.size(), dmgColobj.size(), atkColobj.size());
	dmgColobj.clear();
	atkColobj.clear();

	// priorityが小さい順にソート
	std::stable_sort(manager.begin(), manager.end(),
		[](std::shared_ptr<GameEntity> a, std::shared_ptr<GameEntity> b)
	{
		return a->GetPriority() < b->GetPriority();
	});

	stage->ForwardTimer();
}

void GameEntity::DrawAll() {
	for (auto e : manager) e->Draw();
}

std::shared_ptr<GameEntity> GameEntity::Create(std::shared_ptr<GameEntity> e) {
	manager.push_back(e);
	return e;
}

// プレイヤー以外全て削除
void GameEntity::RemoveExceptPlayer()
{
	manager.erase(
		std::remove_if(manager.begin(), manager.end(),
			[&](std::shared_ptr<GameEntity> p) { 
		return p->GetObjectType() != PLAYER;
	}), manager.end());
}

bool GameEntity::Targetable() const {
	return true;
}
void GameEntity::SetPos(const Vector2& p)
{
	pos = p;
}

 Vector2 GameEntity::GetVel() const
{
	return Vector2(dir ? -vel.x : vel.x, vel.y);
}

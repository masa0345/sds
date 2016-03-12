#include "Block.h"
#include "Image.h"
#include "Sound.h"
#include "Stage.h"
#include "Item.h"
#include "Player.h"
#include "Enemy.h"
#include "Effect.h"
#include <DxLib.h>

Block::Block() : GameEntity(pos) {
	otype = BLOCK;
	priority = Priority::ENEMY;
	hitFilter = 0;
	ignoreInv = true;
	half = false;
}

Block::~Block() {}

void Block::Update() {}

void Block::DamageFrom(GameEntity* e) {
	e->SetDamage(this);
}
void Block::SetDamage(Player* p) {
	if (half) {
		p->BlockHalfHit(pos, vel, width, height);
	} else {
		p->BlockHit(pos, vel, width, height);
	}
}

void Block::SetDamage(Enemy* e) {
	if (half) {
		e->BlockHalfHit(pos, vel, width, height);
	} else {
		e->BlockHit(pos, vel, width, height);
	}
}

bool Block::IsHalf() const {
	return half;
}

// 破壊可能ブロック
BlockBreakable::BlockBreakable() {
	width = 64;
	height = 64;
	atkHitbox = std::make_shared<HitBox>(width, height);
	dmgHitbox = std::make_shared<HitBox>(width, height);
	hp = hpmax = 100;
	itemNum = 10;
	dropRate = 10;
	score = 100;
	img->hdl = Image::Instance()->Load("block");
}
void BlockBreakable::UpdateLate() {
	if (hpmax != -1 && hp <= 0) {
		hp = 0;
		exist = false;
		stage->AddScore(score);
		Sound::Instance()->Play("敵死亡");
		DropItem();
	}
	GameEntity::UpdateLate();
}
void BlockBreakable::DropItem() {
	int r = 1 + GetRand(99);
	if (r < dropRate) {
		Create(std::make_shared<ItemWeapon>(pos, -1));
		Sound::Instance()->Play("アイテムドロップ");
	}
	int n = itemNum - GetRand(itemNum / 5);
	for (int i = 0; i < n; ++i) Create(std::make_shared<ItemHeal>(pos, -1));
}

// 上下移動リフト
BlockLift::BlockLift() {
	width = 144;
	height = 32;
	atkHitbox = std::make_shared<HitBox>(width, height);
	dmgHitbox = std::make_shared<HitBox>(width, height);
	img->hdl = Image::Instance()->Load("lift");
}
void BlockLift::Update() {
	vel.y = sinf(PI * 2.f / 240.f * (stateCnt % 240)) * 2.f;
}

//子リフト
class LiftChild : public Block 
{
public:
	LiftChild() {
		half = true;
		width = 144;
		height = 48;
		atkHitbox = std::make_shared<HitBox>(width, height);
		dmgHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("rot_lift");
		img->ofs_y = -8;
	}
	void SetPos(const Vector2& p) override {
		Vector2 d = { p.x - pos.x, p.y - pos.y };
		if (d.LengthSquare() < 100.f) {
			// 微小な移動は速度にする
			vel = { p.x - pos.x, p.y - pos.y };
		} else {
			pos = p;
		}
	}
};

// 右回転リフト
BlockRotaLiftR::BlockRotaLiftR() { 
	lift.resize(4);
	for (auto& l : lift) {
		l = std::make_shared<LiftChild>();
		Create(l);
	}
	rad_v = PI / 240.f;
}
void BlockRotaLiftR::Update() {
	float r = 144;
	for (unsigned i = 0; i < lift.size(); ++i) {
		Vector2 lp;
		lp.x = pos.x + r + r*cosf(PI2 / lift.size()*i + rad_v*stateCnt);
		lp.y = pos.y + r + r*sinf(PI2 / lift.size()*i + rad_v*stateCnt);
		lift[i]->SetPos(lp);
	}
	if (stateCnt == 360) stateCnt = 0;
}

// 左回転リフト
BlockRotaLiftL::BlockRotaLiftL() {
	lift.resize(4);
	for (auto& l : lift) {
		l = std::make_shared<LiftChild>();
		Create(l);
	}
	rad_v = PI / 240.f;
}
void BlockRotaLiftL::Update() {
	float r = 144;
	for (unsigned i = 0; i < lift.size(); ++i) {
		Vector2 lp;
		lp.x = pos.x + r + r*cosf(PI2 / lift.size()*i - rad_v*stateCnt);
		lp.y = pos.y + r + r*sinf(PI2 / lift.size()*i - rad_v*stateCnt);
		lift[i]->SetPos(lp);
	}
	if (stateCnt == 360) stateCnt = 0;
}

BlockNextMapGate::BlockNextMapGate() {
	width = MW;
	height = MH * 4;
	atkHitbox = std::make_shared<HitBox>(width, height);
}
void BlockNextMapGate::Update() {
	if (stateCnt % 12 == 0) {
		Create(std::make_shared<EffectNextGate>(pos));
	}
}
void BlockNextMapGate::SetDamage(Player * p) {
	stage->SetLoadNext(true);
}
void BlockNextMapGate::SetDamage(Enemy * p){ }

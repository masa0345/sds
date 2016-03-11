#include "Item.h"
#include "Image.h"
#include "Sound.h"
#include "Player.h"
#include "Stage.h"
#include "Effect.h"
#include "Projectile.h"
#include "Input.h"
#include "MapChip.h"
#include "Block.h"
#include <DxLib.h>


Item::Item(const Vector2& pos) : GameEntity(pos) {
	otype = ITEM;
	priority = Priority::ITEM;
	v = rad_v = a = rad_a = 0.f;
	maxcnt = 0;
	type = -1;
	ignoreInv = true;
}

Item::~Item() {}


void Item::UpdatePosition() {
	Vector2 dv(v*cosf(rad_v) + a*cosf(rad_a), v*sinf(rad_v) + a*sinf(rad_a));

	pos += vel + dv;

	if (a != 0.f) {
		rad_v = dv.Angle();
		v = dv.Length();
	}
}

void Item::UpdateLate() {
	GameEntity::UpdateLate();
	if (stateCnt == maxcnt) exist = false;
}

void Item::DamageFrom(GameEntity* e) {
	e->SetDamage(this);
}

// 武器アイテム
ItemWeapon::ItemWeapon(const Vector2& p, int type) : Item(p) {
	auto player = stage->GetPlayer();
	if (type == -1) { // ランダム
		if (player->GetBulletType() == STAR || player->GetBulletType() == STAR_AD) {
			this->type = GetRand(4);
		} else {
			this->type = 1 + GetRand(3);
		}
	} else {
		this->type = type;
	}
	mp = player->GetMPMax();
	width = 30;
	height = 30;
	maxcnt = 1200;
	velCnt = 0;
	atkHitbox = std::make_shared<HitBox>(width, height);
	v = RandFloat(1.f, 6.f);
	rad_v = RandFloat0(-PI);
	a = v / 20.f;
	rad_a = rad_v + PI;
	img->hdl = Image::Instance()->Load("item");
	img->num = this->type % 5;
}
ItemWeapon::ItemWeapon(const Vector2& p, int type, int m) : ItemWeapon(p, type) {
	maxcnt = 600;
	mp = m;
	state = 3;
	v = 3.f;
	rad_v = -PI / 2.f;
	a = 0.05f;
	rad_a = PI / 2.f;
}

void ItemWeapon::Update() {
	auto player = stage->GetPlayer();
	switch (state) {
	case 0: // 停止
		if (stateCnt > 20) v = a = 0.f;
		vel.y = sinf(PI * 2 / 120 * (stateCnt % 120)) / 4;
		// プレイヤーに近い場合はボタンを押すと引き寄せられる
		if (player->GetInput()->Get(INPUT_S) > 0 &&
			DistSqGameEntity(player) < 300 * 300) state = 1;
		break;
	case 1: // 引き寄せる
		if (player->GetInput()->Get(INPUT_S) > 0 && player->CanPickItem()) {
			if (velCnt == 60) velCnt = 30;
			if (velCnt > 30) v = 8.f;
			else v = sinf(PI / 60.f * velCnt) * 8.f;
			++velCnt;
		} else {
			// ボタンを離すとゆっくり止まる
			v = sinf(PI / 60.f * velCnt) * 6.f;
			if (velCnt % 60 == 0) {
				velCnt = 0;
				state = 0;
			} else ++velCnt;
		}
		rad_v = AtanGameEntity(player);
		break;
	case 2: // 捨てる
		if (stateCnt > 90) state = 0;
		break;
	}
	if (stateCnt % 4 == 0) Create(std::make_shared<EffectItemLight>(pos, type));

	// 残り2秒で点滅
	if (maxcnt - stateCnt < 120) img->drawflag = !img->drawflag;
}

void ItemWeapon::Draw() const {
	GameEntity::Draw();
	if (!img->drawflag || img->hdl == 0) return;
	Vector2 p = stage->GetCamera()->TransCoord(pos);
	SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(180.f * sinf(PI / 2.f + PI / 60.f * stateCnt)));
	DrawRotaGraphF(p.x + img->ofs_x, p.y + img->ofs_y,
		img->exRate, img->angle, img->hdl->at(3), TRUE);
}

void ItemWeapon::SetDamage(Player* p) {
	if (p->PickItem(type, mp)) {
		exist = false;
	} else {
		velCnt = 0;
		state = 0;
	}
}

void ItemWeapon::SetDamage(Item* i) {

}

// 回復アイテム
ItemHeal::ItemHeal(const Vector2& p, int type) : Item(p) {
	// type -> 0: HP, 1: MP
	if (type == -1) { // ランダム
		this->type = GetRand(1);
	} else {
		this->type = type;
	}
	val = 4;
	width = 12;
	height = 12;
	atkHitbox = std::make_shared<HitBox>(width, height);
	maxcnt = 600;
	v = RandFloat0(2.f);
	rad_v = RandFloat(PI, PI2);
	a = 0.05f;
	rad_a = PI / 2.f;
	img->hdl = Image::Instance()->Load("healitem");
	img->num = 1 - this->type;
}

void ItemHeal::Update() {
	// 回転
	if (rad_v > PI / 2.f && rad_v < PI * 3.f / 2.f) img->angle -= PI / 64.f;
	else if ((rad_v >= 0 && rad_v < PI / 2.f) || (rad_v > PI * 3.f / 2.f && rad_v < PI2)) img->angle += PI / 64.f;

	// 残り2秒で点滅
	if (maxcnt - stateCnt < 120) img->drawflag = !img->drawflag;
}
void ItemHeal::UpdateColResponse() {
	// マップ判定
	MapChip::CorrectVal cv = { pos,{ vel.x, vel.y + sinf(rad_v)*v }, 0 };
	MapChip::ColPoint cp[] = {
		{ 0, -height / 2, HIT_BOTTOM },
		{ 0, +height / 2, HIT_TOP | HIT_SLOPE },
		{ +width / 2, 0, HIT_LEFT },
		{ -width / 2, 0, HIT_RIGHT },
	};
	a = 0.05f;
	if (MapChip::Instance()->Hit(&cv, cp, 4)) {
		a = 0.f;
		rad_v = PI / 2.f;
	}
	pos = cv.p;
}

void ItemHeal::Draw() const {
	GameEntity::Draw();
	if (!img->drawflag || img->hdl == 0) return;
	Vector2 p = stage->GetCamera()->TransCoord(pos);
	SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(180.f * sinf(PI / 2.f + PI / 30 * stateCnt)));
	DrawRotaGraphF(p.x + img->ofs_x, p.y + img->ofs_y,
		img->exRate, img->angle, img->hdl->at(2), TRUE);
}

void ItemHeal::SetDamage(Player* p) {
	if (type == 0) p->AddHP(val);
	else p->AddMP(val);
	Sound::Instance()->Play("回復アイテム");
	exist = false;
}

void ItemHeal::SetDamage(Block* b) {
	float bx = b->GetPos().x, by = b->GetPos().y;
	int bw = b->GetWidth(), bh = b->GetHeight();
	if (!b->IsHalf()) {
		a = 0.f;
		rad_v = PI / 2.f;
	}
	if (Rect(pos.x - width / 2, pos.y + height / 2, pos.x + width / 2, pos.y + height / 2).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by - bh / 4)) && vel.y >= 0) {
		pos.y = by - bh / 2 - height / 2;
		vel.y = 0.f;
		pos.x += b->GetVel().x;
		v = b->GetVel().y;
		if (b->IsHalf()) {
			a = 0.f;
			v = 0.f;
		}
	}
}

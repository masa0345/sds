#include "Enemy.h"
#include "Image.h"
#include "Sound.h"
#include "Stage.h"
#include "Effect.h"
#include "Item.h"
#include "Projectile.h"
#include "Player.h"
#include "MapChip.h"
#include <DxLib.h>

Enemy::Enemy() : GameEntity(pos) {
	otype = ENEMY;
	priority = Priority::ENEMY;
	hitFilter = 2;
	power = 0;
	itemNum = 20;
	dropRate = 40;
	dropType = -1;
	fix = false;
	target = true;
	score = 0;
}

Enemy::~Enemy() {}

void Enemy::UpdatePosition() {
	pos += vel;
}

void Enemy::UpdateLate() {
	if (hpmax != -1 && hp <= 0) {
		hp = 0;
		exist = false;
		stage->AddScore(score);
		Sound::Instance()->Play("ìGéÄñS");
		DropItem();
	}
	GameEntity::UpdateLate();
}

void Enemy::DamageFrom(GameEntity* e) {
	e->SetDamage(this);
}

void Enemy::SetDamage(Player* p) {
	p->CalcDamage(power);
}

void Enemy::DropItem() {
	int r = 1 + GetRand(99);
	if (r < dropRate) {
		Create(std::make_shared<ItemWeapon>(pos, dropType));
		Sound::Instance()->Play("ÉAÉCÉeÉÄÉhÉçÉbÉv");
	}
	int n = itemNum - GetRand(itemNum / 5);
	for (int i = 0; i < n; ++i) Create(std::make_shared<ItemHeal>(pos, -1));
}

bool Enemy::IsFix() const {
	return fix;
}

void Enemy::BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) { }
void Enemy::BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) { }

// ìG0
EnemyType0::EnemyType0() {
	width = 48;
	height = 48;
	hp = hpmax = 150;
	score = 300;
	img->hdl = Image::Instance()->Load("enemy0");
	dmgHitbox = std::make_shared<HitBox>(width, height);
}
void EnemyType0::Update() {
	if (vel.x > 0) dir = RIGHT;
	else if (vel.x < 0) dir = LEFT;
	if (stateCnt % 60 == 0) state = ++state % 4;

	switch (state) {
	case 0: case 2:
		if (stateCnt % 60 == 1)
			Create(std::make_shared<EnemyBullet>(pos, dir, width/2));
		vel.x = 0;
		break;
	case 1:
		vel.x = -sin(PI / 60 * stateCnt) * 4;
		break;
	case 3:
		vel.x = sin(PI / 60 * stateCnt) * 4;
		break;
	}
	if (img->num == 5) img->num = 0;
	else if (img->num != 0 && stateCnt % 6 == 0) img->num++;
	else if (stateCnt % 120 == 48 && img->num == 0) img->num = 1;
}

// ìG1
EnemyType1::EnemyType1() {
	width = 40;
	height = 40;
	dmgHitbox = std::make_shared<HitBox>(width, height);
	atkHitbox = std::make_shared<HitBox>(width + 12, height + 12);
	hp = hpmax = 150;
	power = 120;
	score = 330;
	img->hdl = Image::Instance()->Load("enemy1");
}
void EnemyType1::Update() {
	vel.x = sinf(stateCnt * PI / 120.f) * 2.f;
	vel.y = cosf(stateCnt * PI / 60.f) * 3.f;
	img->num = GetRand(1);
}

// ìG3
EnemyType3::EnemyType3() {
	width = 50;
	height = 90;
	dmgHitbox = std::make_shared<HitBox>(width, height);
	atkHitbox = std::make_shared<HitBox>(width, height);
	hp = hpmax = 400;
	power = 80;
	score = 600;
	dropRate = 70;
	itemNum = 30;
	img->hdl = Image::Instance()->Load("enemy3");
}
void EnemyType3::Update() {
	if (vel.x > 0) dir = RIGHT;
	else if (vel.x < 0) dir = LEFT;

	switch (state) {
	case 0: // ç∂Ç÷à⁄ìÆ
		img->num = 1;
		vel.x = -2.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		// é©ã@Ç™ãﬂÇ√Ç≠Ç∆çUåÇ
		if (DistSqGameEntity(stage->GetPlayer()) < 200 * 200 &&
			fabsf(stage->GetPlayer()->GetPos().y - pos.y) < 100) {
			dir = stage->GetPlayer()->GetPos().x < pos.x; 
			vel.x = 0.f;
			state = 4;
			stateCnt = 0;
		}
		break;
	case 1: // í‚é~
		img->num = 0;
		vel.x = 0.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		break;
	case 2: // âEÇ÷à⁄ìÆ
		img->num = 1;
		vel.x = 2.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		// é©ã@Ç™ãﬂÇ√Ç≠Ç∆çUåÇ
		if (DistSqGameEntity(stage->GetPlayer()) < 200 * 200 &&
			fabsf(stage->GetPlayer()->GetPos().y - pos.y) < 100) {
			dir = stage->GetPlayer()->GetPos().x < pos.x;
			vel.x = 0.f;
			state = 4;
			stateCnt = 0;
		}
		break;
	case 3: // í‚é~
		img->num = 0;
		vel.x = 0.f;
		if (stateCnt > 60) {
			state = 0;
			stateCnt = 0;
		}
		break;
	case 4: // ç\Ç¶
		img->num = 2;
		vel.x = 0.f;
		if (stateCnt > 30) {
			++state;
			stateCnt = 0;
			vel.x = dir ? -12.f : 12.f;
		}
		break;
	case 5: // çUåÇ
		img->num = 3;
		atkHitbox->SetRect(0, {-66, -45, 66, 45});
		power = 164;
		vel.x += dir ? 0.5f : -0.5f;
		if (stateCnt > 10) {
			state = dir ? 3 : 1;
			stateCnt = 0;
			atkHitbox->SetRect(0, { -width / 2, -height / 2, width / 2, height / 2 });
			power = 80;
		}
		break;
	}

	// èdóÕ
	if (vel.y < 8.f) vel.y += 0.8f;
	else vel.y = 8.f;
}
void EnemyType3::UpdateColResponse() {
	// É}ÉbÉvîªíË
	MapChip::CorrectVal cv = { pos, vel, 0 };
	MapChip::ColPoint cp[] = {
		{ 0, -height / 2, HIT_BOTTOM },
		{ 0, +height / 2, HIT_TOP | HIT_SLOPE },
		{ +width / 2, -height / 4, HIT_LEFT },
		{ +width / 2, +height / 4, HIT_LEFT },
		{ -width / 2, -height / 4, HIT_RIGHT },
		{ -width / 2, +height / 4, HIT_RIGHT },
	};
	hitmap = MapChip::Instance()->Hit(&cv, cp, 6);
	vel = cv.v;
	pos = cv.p;
}
void EnemyType3::BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) {
	int w = width, h = height;
	float bx = p.x, by = p.y, bvx = v.x, bvy = v.y;
	//ç∂âE
	if (Rect(pos.x - w / 2, pos.y - h / 2 + 14, pos.x, pos.y + h / 2 - 14).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by + bh / 2))) {
		if (hitmap & HIT_LEFT) {
			if (bvx > 0) hp = 0;
		} else {
			pos.x = bx + bw / 2.f + w / 2.f;
			hitmap |= HIT_RIGHT;
		}
	}
	if (Rect(pos.x, pos.y - h / 2 + 14, pos.x + w / 2, pos.y + h / 2 - 14).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by + bh / 2))) {
		if (hitmap & HIT_RIGHT) {
			if (bvx < 0) hp = 0;
		} else {
			pos.x = bx - bw / 2.f - w / 2.f;
			hitmap |= HIT_LEFT;
		}
	}
	//è„â∫
	if (Rect(pos.x - w / 2 + 12, pos.y - h / 2, pos.x + w / 2 - 12, pos.y).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by + bh / 2)) && vel.y <= 0) {
		if (hitmap & (HIT_TOP | HIT_SLOPE)) {
			if (bvy > 0) hp = 0;
		} else {
			pos.y = by + bh / 2.f + h / 2.f;
			vel.y = 1.f;
			hitmap |= HIT_BOTTOM;
		}
	}
	if (Rect(pos.x - w / 2 + 12, pos.y, pos.x + w / 2 - 12, pos.y + h / 2).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by + bh / 2))) {
		if (hitmap & HIT_BOTTOM) {
			if (bvy < 0) hp = 0;
		} else {
			pos.y = by - bh / 2.f - h / 2.f;
			vel.y = 0.f;
			pos.x += bvx;
			hitmap |= HIT_TOP;
		}
	}
}
void EnemyType3::BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) {
	float bx = p.x, by = p.y;
	if (Rect(pos.x - width / 2 + 12, pos.y + height / 2, pos.x + width / 2 - 12, pos.y + height / 2).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by - bh / 4)) && vel.y >= 0) {
		if (!(hitmap & HIT_BOTTOM)) {
			pos.y = by - bh / 2.f - height / 2.f;
			vel.y = 0.f;
			pos.x += v.x;
			hitmap |= HIT_TOP;
		}
	}
}
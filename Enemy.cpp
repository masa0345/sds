#include "Enemy.h"
#include "Image.h"
#include "Sound.h"
#include "Stage.h"
#include "Effect.h"
#include "Item.h"
#include "Projectile.h"
#include "Player.h"
#include "MapChip.h"
#include "Frontend.h"
#include "Event.h"
#include <DxLib.h>
#include <unordered_map>

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
		Sound::Instance()->Play("敵死亡");
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

bool Enemy::Targetable() const
{
	return target;
}

void Enemy::DropItem() {
	int r = GetRand(99);
	if (r < dropRate) {
		Create(std::make_shared<ItemWeapon>(pos, dropType));
		Sound::Instance()->Play("アイテムドロップ");
	}
	int n = itemNum - GetRand(itemNum / 5);
	for (int i = 0; i < n; ++i) Create(std::make_shared<ItemHeal>(pos, -1));
}

bool Enemy::IsFix() const {
	return fix;
}


void Enemy::BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) { }
void Enemy::BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) { }

// 敵0
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

// 敵1
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

// 敵3
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
	case 0: // 左へ移動
		img->num = 1;
		vel.x = -2.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		// 自機が近づくと攻撃
		if (DistSqGameEntity(stage->GetPlayer()) < 200 * 200 &&
			fabsf(stage->GetPlayer()->GetPos().y - pos.y) < 100) {
			dir = stage->GetPlayer()->GetPos().x < pos.x; 
			vel.x = 0.f;
			state = 4;
			stateCnt = 0;
		}
		break;
	case 1: // 停止
		img->num = 0;
		vel.x = 0.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		break;
	case 2: // 右へ移動
		img->num = 1;
		vel.x = 2.f;
		if (stateCnt > 60) {
			++state;
			stateCnt = 0;
		}
		// 自機が近づくと攻撃
		if (DistSqGameEntity(stage->GetPlayer()) < 200 * 200 &&
			fabsf(stage->GetPlayer()->GetPos().y - pos.y) < 100) {
			dir = stage->GetPlayer()->GetPos().x < pos.x;
			vel.x = 0.f;
			state = 4;
			stateCnt = 0;
		}
		break;
	case 3: // 停止
		img->num = 0;
		vel.x = 0.f;
		if (stateCnt > 60) {
			state = 0;
			stateCnt = 0;
		}
		break;
	case 4: // 構え
		img->num = 2;
		vel.x = 0.f;
		if (stateCnt > 30) {
			++state;
			stateCnt = 0;
			vel.x = dir ? -12.f : 12.f;
		}
		break;
	case 5: // 攻撃
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

	// 重力
	if (vel.y < 8.f) vel.y += 0.8f;
	else vel.y = 8.f;
}
void EnemyType3::UpdateColResponse() {
	// マップ判定
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
	//左右
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
	//上下
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

namespace {
	std::vector<std::pair<int, int>> bossOfs;
	std::unordered_map<int, std::shared_ptr<HitBox>> bossAtkBox, bossDmgBox;

	void LoadBossHitBox() {
		int fp, i = 0;
		fp = FileRead_open("data/csv/boss1hit.csv");
		if (fp == NULL) {
			MessageBox(GetMainWindowHandle(), "ボスデータ読み込みに失敗しました。", "error", MB_OK);
			return;
		}
		int n, atk;
		Rect r;
		while (FileRead_eof(fp) == 0) {
			FileRead_scanf(fp, "%d,%d,%d,%d,%d,%d", 
				&n, &r.left, &r.top, &r.right, &r.bottom, &atk);
			if (atk == 0) {
				if (bossDmgBox[n] == nullptr) bossDmgBox[n] = std::make_shared<HitBox>();
				bossDmgBox[n]->PushBack(r);
			} else {
				if (bossAtkBox[n] == nullptr) bossAtkBox[n] = std::make_shared<HitBox>();
				bossAtkBox[n]->PushBack(r);
			}
		}
		FileRead_close(fp);

		fp = FileRead_open("data/csv/boss1ofs.csv");
		if (fp == NULL) {
			MessageBox(GetMainWindowHandle(), "ボスデータ読み込みに失敗しました。", "error", MB_OK);
			return;
		}
		std::pair<int, int> ofs;
		while (FileRead_eof(fp) == 0) {
			if (FileRead_getc(fp) == '/') {
				while (FileRead_getc(fp) != '\n');
			}
			FileRead_scanf(fp, "%d,%d", &ofs.first, &ofs.second);
			bossOfs.push_back(ofs);
		}
		FileRead_close(fp);
	}
}

EnemyStage1Boss::EnemyStage1Boss() {
	dir = LEFT;
	width = 128;
	height = 256;
	dmgHitbox = std::make_shared<HitBox>(width, height);
	dropRate = 0;
	itemNum = 0;
	hp = hpmax = 4200;
	fix = true;
	target = false;
	invincible = 1;
	img->hdl = Image::Instance()->Load("boss1");
	img->drawflag = false;
	LoadBossHitBox();
}

void EnemyStage1Boss::Update() {
	switch (state) {
	case 0: // イベント開始
		if (DistSqGameEntity(stage->GetPlayer()) < 600 * 600) {
			Event::SetEvent(std::make_shared<EventStage1Boss>(stage, this));
			++state;
		}
		break;
	case 1: // イベント待機
		if (Event::GetEventFlag()) break;
		target = true;
		invincible = 0;
		img->drawflag = true;
		state = 12;
		stateCnt = 0;
		Frontend::Create(std::make_shared<FrontendBossGauge>(this));
		break;
	case 2: //立ち
		img->num = 0;
		if (stateCnt == 60) {
			state = 3;
			stateCnt = 0;
		}
		if (pos.x - stage->GetPlayer()->GetPos().x > 0) dir = LEFT;
		else dir = RIGHT;
		break;
	case 3: //構え
		img->num = 1;
		if (stateCnt == 120) {
			state = 4;
			stateCnt = 0;
		}
		break;
	case 4: //突進１
		img->num = 2;
		if (dir) vel.x = -18;
		else vel.x = 18;
		power = 180;
		state++;
		Sound::Instance()->Play("ボスダッシュ");
		break;
	case 5: //突進２
		img->num = 2;
		if ((pos.x - stage->GetArea(stage->GetAreaNum())[0] * MW < MW * 3) && dir ||
			(stage->GetArea(stage->GetAreaNum())[2] * MW - pos.x < MW * 3) && !dir) {
			vel.x = 0;
			stateCnt = 0;
			state = 6;
		}
		Create(std::make_shared<EffectAfterImage2>(pos, img, dir));
		break;
	case 6: //ジャンプ準備
		if (stateCnt < 10) img->num = 1;
		else img->num = 0;
		if (stateCnt == 30) {
			state = 7;
			stateCnt = 0;
			mem_y = pos.y; //ｙ座標記憶
			stage->GetCamera()->SetShake(12.f, 55.f, 15, 15);
			Sound::Instance()->Play("ボスジャンプ");
		}
		break;
	case 7: //ジャンプ
		if (stateCnt < 10) {
			img->num = 3;
			vel.y = -10;
		} else {
			img->num = 4;
			vel.y = -20;
		}
		if (stateCnt == 30) { //y -=500
			state = 8;
			stateCnt = 0;
			vel.y = 0;
		}
		break;
	case 8: //滞空
		img->drawflag = false;
		target = false;
		invincible = 1;
		if (stateCnt < 120) {
			pos.x = stage->GetPlayer()->GetPos().x;
		} else if (stateCnt == 180) {
			img->drawflag = true;
			target = true;
			invincible = 0;
			state = 9;
			stateCnt = 0;
			img->num = 5;
		}
		break;
	case 9: //降下
		img->num = 5;
		vel.y = 20;
		power = 440;
		if (stateCnt > 25) {
			state = 10;
			stateCnt = 0;
			vel.y = 0;
			pos.y = mem_y; //ｙ座標補正
			stage->GetCamera()->SetShake(24.f, 75.f, 40, 30);
			Sound::Instance()->Play("ボス着地");
		}
		break;
	case 10: //着地
		img->num = 6;
		if (stateCnt == 120) {
			state = 11;
			stateCnt = 0;
		}
		break;
	case 11: //立ち
		img->num = 0;
		if (stateCnt == 60) {
			state = 12;
			stateCnt = 0;
		}
		if (pos.x - stage->GetPlayer()->GetPos().x > 0) dir = LEFT;
		else dir = RIGHT;
		break;
	case 12: //振り上げ
		img->num = 7;
		if (stateCnt == 160) {
			state = 13;
			stateCnt = 0;
			power = 260;
			Create(std::make_shared<EnemyBoss1Unit>(this));
			Sound::Instance()->Play("ボス振りおろし");
		}
		break;
	case 13: //振りおろし
		if (stateCnt < 10) img->num = 8;
		else img->num = 9;
		if (stateCnt == 10) {
			stage->GetCamera()->SetShake(24.f, 75.f, 40, 30);
			Sound::Instance()->Play("ボス振りおろし2");
		}
		if (stateCnt == 140) {
			state = 2;
			stateCnt = 0;
		}
		break;
	case 14: //撃破
		if (stateCnt == 1) Create(std::make_shared<EffectFlushExplode>(pos));
		img->num = 10;
		invincible = 1;
		target = false;
		hp = 0;
		vel.x = vel.y = 0;
		if (stateCnt == 140) {
			img->drawflag = false;
		}
		if (stateCnt == 320) {
			exist = false;
			stage->SetClear(true);
			stage->AddClearScore(3000);
		}
		break;
	}
	img->ofs_x = bossOfs[img->num].first;
	img->ofs_y = bossOfs[img->num].second;
	dmgHitbox = bossDmgBox[img->num];
	atkHitbox = bossAtkBox[img->num];
	if (dir == LEFT) img->ofs_x *= -1;
}

void EnemyStage1Boss::UpdateLate()
{
	if (hp < 0) {
		state = 14;
		stateCnt = 0;
	}
	GameEntity::UpdateLate();
}

EnemyBoss1Unit::EnemyBoss1Unit(Enemy* p) : parent(p)
{
	dir = p->GetDirection();
	pos.x = dir ? (stage->GetArea(stage->GetAreaNum())[0] + 6.f)*MW :
		(stage->GetArea(stage->GetAreaNum())[2] - 3.f)*MW;
	pos.y = (stage->GetArea(stage->GetAreaNum())[1] - 6.f)*MH;
	vel.y = 10.f;
	width = 40;
	height = 40;
	dmgHitbox = std::make_shared<HitBox>(width, height);
	atkHitbox = std::make_shared<HitBox>(width, height);
	hp = hpmax = 50;
	invincible = 1;
	itemNum = 20;
	dropRate = 100;
	power = 100;
	score = 200;
	img->hdl = Image::Instance()->Load("enemy2");
}

void EnemyBoss1Unit::Update()
{
	if (parent->GetHP() <= 0) {
		exist = false;
		return;
	}
	float angle;
	switch (state) {
	case 0:
		vel.y -= 0.1f;
		if (vel.y < 0.f) {
			vel.y = 0.f;
			invincible = 0;
			stateCnt = 0;
			++state;
		}
		break;
	case 1:
		angle = AtanGameEntity(stage->GetPlayer());
		vel.x = 1.5f * cosf(angle);
		vel.y = 1.5f * sinf(angle);
		if (stateCnt == 300) {
			stateCnt = 0;
			++state;
			atkHitbox->SetRect(0, { -width,-height,width,height });
			Create(std::make_shared<EffectFExplode>(pos, 6.f));
		}
		break;
	case 2:
		if (stateCnt == 10) exist = false;
		break;
	}
	if (pos.x < stage->GetPlayer()->GetPos().x) dir = RIGHT;
	else dir = LEFT;
	img->num = (stateCnt / 2) % 4;
}

void EnemyBoss1Unit::SetDamage(Player * p)
{
	switch (state) {
	case 1: 
		state = 2;
		stateCnt = 0;
		atkHitbox->SetRect(0, { -width,-height,width,height });
		Create(std::make_shared<EffectFExplode>(pos, 6.f));
		break;
	case 2:
		p->CalcDamage(power);
		break;
	}
}

#include "Projectile.h"
#include "MapChip.h"
#include "Player.h"
#include "Image.h"
#include "Sound.h"
#include "Effect.h"
#include "Enemy.h"
#include "Block.h"
#include "Stage.h"
#include "Input.h"
#include <DxLib.h>


Projectile::Projectile(const Vector2& pos) : GameEntity(pos) {
	otype = PROJECTILE;
	v = rad_v = a = rad_a = 0.f;
	maxcnt = -1;
	power = 0;
}

Projectile::~Projectile() {}

bool Projectile::UpdatePre() {
	GameEntity::UpdatePre();
	if (stateCnt == maxcnt) exist = false;
	if (hpmax != -1 && hp <= 0) exist = false;
	return !exist;
}

void Projectile::UpdatePosition() {
	Vector2 dv(v*cosf(rad_v) + a*cosf(rad_a), v*sinf(rad_v) + a*sinf(rad_a));

	pos += vel + dv;

	if (a != 0.f) {
		rad_v = dv.Angle();
		v = dv.Length();
	}
}

void Projectile::DamageFrom(GameEntity* e) {
	e->SetDamage(this);
}

Vector2 Projectile::GetVel() const {
	Vector2 dv(v*cosf(rad_v) + a*cosf(rad_a), v*sinf(rad_v) + a*sinf(rad_a));
	return vel + dv;
}

bool Projectile::HitMap() {
	float vx = vel.x + v * cosf(rad_v) + a * cosf(rad_a);
	float vy = vel.y + v * sinf(rad_v) + a * sinf(rad_a);
	MapChip::CorrectVal p = { {pos.x+vx, pos.y+vy}, vel, 0 };
	MapChip::ColPoint map[] = {
		{ 0, -height / 2, HIT_BOTTOM },
		{ 0, +height / 2, HIT_TOP | HIT_SLOPE },
		{ -width / 2, 0, HIT_RIGHT | HIT_RSLOPE },
		{ +width / 2, 0, HIT_LEFT | HIT_LSLOPE },
	};
	return MapChip::Instance()->Hit(&p, map, 4) != 0;
}

// ìGíe
EnemyBullet::EnemyBullet(const Vector2& p, bool d, int w) : Projectile(p) {
	hp = hpmax = 10;
	v = 8.f;
	width = 25;
	height = 15;
	atkHitbox = std::make_shared<HitBox>(width, height);
	dmgHitbox = std::make_shared<HitBox>(width, height);
	dir = d;
	rad_v = dir ? PI : 0.f;
	pos.x += dir ? -w : w;
	maxcnt = 40;
	power = 100;
	img->hdl = Image::Instance()->Load("enemy_bullet0");
	hitFilter = 2;
	priority = Priority::ENEMY_PROJ;
}
void EnemyBullet::Update() {
	if (HitMap()) {
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("star")));
		exist = false;
	}
	Create(std::make_shared<EffectAfterImage>(pos, img, dir));
}
void EnemyBullet::SetDamage(Player* p) {
	p->CalcDamage(power);
	Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("star")));
	exist = false;
}
void EnemyBullet::SetDamage(Block* b) {
	Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("star")));
	exist = false;
}

// é©ã@íe
BulletParam PlayerBullet::param[BULLET_TYPE_NUM];

PlayerBullet::PlayerBullet(const Player& p) : Projectile(Vector2()) {
	priority = Priority::PLAYER_PROJ;
	hitFilter = 1;
	dir = p.GetDirection();
	pos.x = dir ? p.GetPos().x - 60 : p.GetPos().x + 60;
	pos.y = p.GetPos().y - 20;
}

void PlayerBullet::Update() {};

void PlayerBullet::Load() {
	int fp, i = 0;
	fp = FileRead_open("data/csv/shot.csv");
	if (fp == NULL) {
		MessageBox(GetMainWindowHandle(), "ÉVÉáÉbÉgÉfÅ[É^ì«Ç›çûÇ›Ç…é∏îsÇµÇ‹ÇµÇΩÅB", "error", MB_OK);
		//exitFlag = true;
		return;
	}
	while (FileRead_eof(fp) == 0) {
		if (FileRead_getc(fp) == '/') {
			while (FileRead_getc(fp) != '\n');
			if (i>0) i--;
		}
		FileRead_scanf(fp, "%d,%d,%d", &param[i].delay, &param[i].cost, &param[i].power);
		i++;
	}
	FileRead_close(fp);
}

BulletParam PlayerBullet::GetBulletParam(BulletType i) {
	return param[i];
}


// é©ã@íeÅ@â©
BulletStar::BulletStar(const Player& p) : PlayerBullet(p) {
	v = 12.f;
	rad_v = dir ? PI : 0.f;
	width = 25;
	height = 15;
	maxcnt = 30;
	power = param[STAR].power;
	img->hdl = Image::Instance()->Load("star");
	if (p.IsShift()) power = (int)(power * 1.2f);
	atkHitbox = std::make_shared<HitBox>(width, height);
	Sound::Instance()->Play("shot_star");
}
void BulletStar::Update() {
	float angle = AtanGameEntity(FindNearest(ENEMY));
	if (InRange(-PI/6, angle, 0) || InRange(PI*5/6, angle, PI))
		rad_v -= PI / 900.f;
	else if (InRange(0, angle, PI/6) || InRange(-PI, angle, -PI*5/6))
		rad_v += PI / 900.f;
	
	if (HitMap()) {
		Create(std::make_shared<EffectHit>(pos, img->hdl, 0.5f));
		exist = false;
	}
	Create(std::make_shared<EffectAfterImage>(pos, img, dir));
}
void BulletStar::SetDamage(Enemy* e) {
	e->AddHP(-power);
	Create(std::make_shared<EffectHit>(pos, img->hdl, 0.5f));
	exist = false;
	Sound::Instance()->Play("çUåÇÉqÉbÉg");
}
void BulletStar::SetDamage(Block* e) {
	if (e->IsHalf()) return;
	e->AddHP(-power);
	Create(std::make_shared<EffectHit>(pos, img->hdl, 0.5f));
	exist = false;
	Sound::Instance()->Play("çUåÇÉqÉbÉg");
}
void BulletStar::SetDamage(Projectile* p) {
	p->AddHP(-power);
	exist = false;
}

// é©ã@íeÅ@ê‘
BulletMeteor::BulletMeteor(const Player& p) : PlayerBullet(p) {
	v = 1.f;
	a = 0.3f;
	rad_v = rad_a = dir ? PI : 0.f;
	width = 30;
	height = 30;
	maxcnt = 60;
	power = param[METEOR].power;
	img->hdl = Image::Instance()->Load("fire");
	if (p.IsShift()) power = (int)(power * 1.2f);
	atkHitbox = std::make_shared<HitBox>(width, height);
	Sound::Instance()->Play("shot_meteor");
}
void BulletMeteor::Update() {
	float angle;
	switch (state) {
	case 0: // íe
		angle = AtanGameEntity(FindNearest(ENEMY));
		if (InRange(-PI / 6, angle, 0) || InRange(PI * 5 / 6, angle, PI))
			rad_v -= PI / 500.f;
		else if (InRange(0, angle, PI / 6) || InRange(-PI, angle, -PI * 5 / 6))
			rad_v += PI / 500.f;
		if (HitMap()) state = 1;
		img->num = 1;
		Create(std::make_shared<EffectAfterImage>(pos, img, dir));
		img->num = 0;
		break;
	case 1:
		Create(std::make_shared<EffectFExplode>(pos, 4.f));
		Sound::Instance()->Play("îöî≠");
		state = 2;
		break;
	case 2: // îöïó
		v = a = 0.f;
		width += 32;
		height += 32;
		atkHitbox->SetRect(0, { -width / 2, -height / 2, width / 2, height / 2 });
		if (width > 140) exist = false;
		break;
	}
}
void BulletMeteor::SetDamage(Enemy* e) {
	e->AddHP(-power);
	if (state == 0) state = 1;
}
void BulletMeteor::SetDamage(Block* e) {
	if (e->IsHalf()) return;
	e->AddHP(-power);
	if (state == 0) state = 1;
}
void BulletMeteor::SetDamage(Projectile* p) {
	p->AddHP(-power);
	if (state == 0) state = 1;
}

// é©ã@íeÅ@ê¬
BulletChaser::BulletChaser(const Player& p) : PlayerBullet(p) {
	v = 8.f;
	rad_v = dir ? PI : 0.f;
	width = 20;
	height = 20;
	maxcnt = 80;
	power = param[CHASER].power;
	target = FindNearest(ENEMY);
	img->hdl = Image::Instance()->Load("chaser");
	if (p.IsShift()) power = (int)(power * 1.2f);
	atkHitbox = std::make_shared<HitBox>(width, height);
	Sound::Instance()->Play("shot_chaser");
}
void BulletChaser::Update() {
	if (!target || !target->GetExist()) {
		target = FindNearest(ENEMY);
	}
	float angle = AtanGameEntity(target);
	if ((dir && (InRange(-PI, angle, -PI2 / 3) || InRange(PI2 / 3, angle, PI))) ||
		(!dir && InRange(-PI / 3, angle, PI / 3))) {
		rad_v = angle;
	}
	img->angle = stateCnt * PI / 20.f;
	Create(std::make_shared<EffectAfterImage>(pos, img, dir));
}
void BulletChaser::SetDamage(Enemy* e) {
	e->AddHP(-power);
	Create(std::make_shared<EffectHit>(pos, img->hdl, 0.3f));
	exist = false;
	Sound::Instance()->Play("çUåÇÉqÉbÉg");
}
void BulletChaser::SetDamage(Block* e) {
	if (e->IsHalf()) return;
	e->AddHP(-power);
	Create(std::make_shared<EffectHit>(pos, img->hdl, 0.3f));
	exist = false;
	Sound::Instance()->Play("çUåÇÉqÉbÉg");
}
void BulletChaser::SetDamage(Projectile* p) {
	p->AddHP(-power);
	exist = false;
}

// é©ã@íeÅ@îí
BulletGear::BulletGear(const Player& p) : PlayerBullet(p) {
	v = 14.f;
	a = -0.48f;
	rad_v = rad_a = dir ? PI : 0.f;
	width = 60;
	height = 60;
	maxcnt = 85;
	hitnum = 0;
	power = param[GEAR].power;
	img->hdl = Image::Instance()->Load("gear");
	if (p.IsShift()) power = (int)(power * 1.2f);
	atkHitbox = std::make_shared<HitBox>(width, height);
	Sound::Instance()->Play("shot_gear");
}
void BulletGear::Update() {
	switch (state) {
	case 0:
		if (stateCnt == 30) ++state;
		break;
	case 1:
		v = a = 0.f;
		if (stateCnt == 60) {
			++state;
			img->drawflag = false;
			power = param[GEAR].power * 4;
			Create(std::make_shared<EffectMExplode>(pos));
			Sound::Instance()->Play("îöî≠");
		}
		break;
	case 2:
		if (66 < stateCnt && stateCnt < 76) {
			width += 8;
			height += 8;
			atkHitbox->SetRect(0, { -width / 2, -height / 2, width / 2, height / 2 });
		}
	}
	if (HitMap()) v = a = 0.f;
	img->angle = sinf(PI / 60.f*stateCnt) * PI;
}
void BulletGear::SetDamage(Enemy* e) {
	if (!e->IsFix()) {
		Vector2 ep = e->GetPos();
		ep.x += cosf(rad_v) * v / 2.f;
		e->SetPos(ep);
	}
	switch (state) {
	case 0:
		if (stateCnt % 3 == 0 && hitnum < 3) {
			e->AddHP(-power);
			++hitnum;
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		}
		break;
	case 1:
		if (stateCnt % 3 == 0 && hitnum < 6) {
			e->AddHP(-power);
			++hitnum;
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		}
		break;
	case 2:
		if (std::find(victim.begin(), victim.end(), e) != victim.end()) return;
		//stage->GetCamera()->SetShake(4.f, 55.f, 4, 16);
		e->AddHP(-power);
		victim.push_back(e);
		++hitnum;
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
		break;
	}
}
void BulletGear::SetDamage(Block* e) {
	if (e->IsHalf()) return;
	v = a = 0.f;

	switch (state) {
	case 0:
		if (stateCnt % 3 == 0 && hitnum < 3) {
			e->AddHP(-power);
			++hitnum;
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		}
		break;
	case 1:
		if (stateCnt % 3 == 0 && hitnum < 6) {
			e->AddHP(-power);
			++hitnum;
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		}
		break;
	case 2:
		if (std::find(victim.begin(), victim.end(), e) != victim.end()) return;
		e->AddHP(-power);
		victim.push_back(e);
		++hitnum;
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
		break;
	}
}
void BulletGear::SetDamage(Projectile* p) {
	p->AddHP(-power);
}


// é©ã@íeÅ@çï
class BulletGhostPart : public PlayerBullet 
{
public:
	BulletGhostPart(const Player& p, int st) : PlayerBullet(p) {
		v = 3.f;
		a = 0.1f;
		rad_v = rad_a = dir ? PI : 0.f;
		width = 15;
		height = 15;
		maxcnt = 80;
		power = param[GHOST].power;
		state = st;
		img->hdl = Image::Instance()->Load("ghost");
		if (p.IsShift()) power = (int)(power * 1.2f);
		hitRect = {-width/2, -height/2, width/2, height/2};
		atkHitbox = std::make_shared<HitBox>(10);
		for (int i = 0; i < 10; ++i) {
			atkHitbox->PushBack(hitRect);
			prev[i] = pos;
		}
		dir = RIGHT;
	}
	void Update() override {
		switch (state) {
		case 0:
			vel.y = sinf(PI / 16.f * stateCnt) * stateCnt / 3.f;
			break;
		case 1:
			vel.y = sinf(PI / 16.f * stateCnt + PI) * stateCnt / 3.f;
			break;
		}
		// écëúïîï™Ç…Ç‡ìñÇΩÇËîªíË
		for (int i = 0; i < 10; ++i) {
			atkHitbox->SetRect(i, hitRect + (prev[i] - pos));
		}
		prev[stateCnt % 10] = pos;
		img->exRate = RandFloat(1.f, 4.f);
		Create(std::make_shared<EffectAfterImage>(pos, img, dir, img->angle, img->exRate));
	}
	void SetDamage(Enemy* e) override {
		if (std::find(victim.begin(), victim.end(), e) != victim.end()) return;
		e->AddHP(-power);
		victim.push_back(e);
		Create(std::make_shared<EffectHit>(pos, img->hdl));
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	// ÉuÉçÉbÉNÇ‚ìGíeÇ…ÇÕìñÇΩÇÁÇ»Ç¢
	void SetDamage(Block* b) override { }
	void SetDamage(Projectile* p) override { }
private:
	Rect hitRect;
	Vector2 prev[10];
	std::vector<GameEntity*> victim;
};
BulletGhost::BulletGhost(const Player& p) : PlayerBullet(p) {
	for (int i = 0; i < 2; ++i)
		Create(std::make_shared<BulletGhostPart>(p, i));
	exist = false;
	Sound::Instance()->Play("shot_ghost");
}


// é©ã@íeÅ@â©ã≠âª
class BulletDualPart : public PlayerBullet
{
public:
	BulletDualPart(const Player& p, int i) : PlayerBullet(p) {
		v = 25.f;
		rad_v= dir ? PI : 0.f;
		width = 25;
		height = 8;
		maxcnt = 24;
		pos.y = p.GetPos().y - 30.f + 16.f * i;
		power = param[STAR_AD].power;
		atkHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("dual");
		img->ofs_x = dir ? 12 : -12;
		if (p.IsShift()) power = (int)(power * 1.2f);
	}
	void Update() override {
		if (HitMap()) {
			Vector2 d(v*cosf(rad_v), v*sinf(rad_v));
			Create(std::make_shared<EffectHit>(pos+d, Image::Instance()->Load("star"), 0.5f));
			exist = false;
		}
		Create(std::make_shared<EffectAfterImage>(pos, img, dir));
	}
	void SetDamage(Enemy* e) override {
		e->AddHP(-power);
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("star"), 0.5f));
		exist = false;
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	void SetDamage(Block* b) override {
		if (b->IsHalf()) return;
		b->AddHP(-power);
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("star"), 0.5f));
		exist = false;
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	void SetDamage(Projectile* p) override {
		p->AddHP(-power);
		exist = false;
	}
};
BulletDual::BulletDual(const Player& p) : PlayerBullet(p) {
	for (int i = 0; i < 2; ++i)
		Create(std::make_shared<BulletDualPart>(p, i));
	exist = false;
	Sound::Instance()->Play("shot_star");
}

// é©ã@íeÅ@ê‘ã≠âª
class BulletNovaPart : public PlayerBullet
{
public:
	BulletNovaPart(const Player& p, int i) : PlayerBullet(p) {
		v = 2.f;
		rad_v = rad_a = dir ? PI : 0.f;
		width = 30;
		height = 10;
		maxcnt = 50;
		pos.y = p.GetPos().y - 8.f - GetRand(24);
		power = param[METEOR_AD].power;
		state = i;
		atkHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("fire");
		img->num = 2;
		if (p.IsShift()) power = (int)(power * 1.2f);
	}
	void Update() override {
		float angle;
		switch (state) {
		case 0:
			a = 0.35f;
			angle = AtanGameEntity(FindNearest(ENEMY));
			if (InRange(-PI / 6, angle, 0) || InRange(PI * 5 / 6, angle, PI))
				rad_v -= PI / 300.f;
			else if (InRange(0, angle, PI / 6) || InRange(-PI, angle, -PI * 5 / 6))
				rad_v += PI / 300.f;
			break;
		case 1:
			rad_v -= PI / 20.f;
			if (stateCnt == 2) {
				v = RandFloat0(3.f);
				rad_v = dir ? PI : 0.f;
				state = 0;
			}
			break;
		case 2:
			rad_v -= PI / 40.f;
			if (stateCnt == 4) {
				v = RandFloat0(4.f);
				rad_v = dir ? PI : 0.f;
				state = 0;
			}
			break;
		case 3:
			Create(std::make_shared<EffectFExplode>(pos, 2.f));
			Sound::Instance()->Play("îöî≠");
			++state;
			break;
		case 4:
			v = a = 0;
			width += 130 / 4;
			height += 150 / 4;
			atkHitbox->SetRect(0, { -width / 2, -height / 2, width / 2, height / 2 });
			if (width > 160) exist = false;
			break;
		}
		if (state < 3) {
			if (HitMap()) state = 3;
			img->num = 3;
			Create(std::make_shared<EffectAfterImage>(pos, img, dir));
			img->num = 2;
		}
	}
	void SetDamage(Enemy* e) override {
		e->AddHP(-power);
		if (state < 3) state = 3;
	}
	void SetDamage(Block* b) override {
		if (b->IsHalf()) return;
		b->AddHP(-power);
		if (state < 3) state = 3;
	}
	void SetDamage(Projectile* p) override {
		p->AddHP(-power);
		if (state < 3) state = 3;
	}
};
BulletNova::BulletNova(const Player& p) : PlayerBullet(p) {
	for (int i = 0; i < 3; ++i)
		Create(std::make_shared<BulletNovaPart>(p, i));
	exist = false;
	Sound::Instance()->Play("shot_meteor");
}

// é©ã@íeÅ@ê¬ã≠âª
class BulletSpearPart : public PlayerBullet
{
public:
	BulletSpearPart(const Player& p, int i) : PlayerBullet(p) {
		rad_v = dir ? PI : 0.f;
		width = 20;
		height = 20;
		maxcnt = 80;
		state = i;
		power = param[CHASER_AD].power;
		atkHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("chaser");
		if (p.IsShift()) power = (int)(power * 1.2f);
		hitRect = { -width / 2, -height / 2, width / 2, height / 2 };
		atkHitbox = std::make_shared<HitBox>(10);
		atkHitbox->PushBack(hitRect);
	}
	void Update() override {
		float angle = PI / 5.f;
		if (stateCnt == 6) state = 3;
		switch (state) {
		case 0: // è„íe
			a = 3.f;
			rad_a = dir ? -angle + PI : -angle;
			break;
		case 1: // íÜêSíe
			a = 3.f * cosf(angle);
			rad_a = dir ? PI : 0.f;
			break;
		case 2: // â∫íe
			a = 3.f;
			rad_a = dir ? angle + PI : angle;
			break;
		case 3: // í‚é~
			v = a = 0.f;
			img->angle = stateCnt * PI / 24.f;
			if (stateCnt == 50) {
				state = 4;
				v = 35.f;
				rad_v = AtanGameEntity(FindNearest(ENEMY));
				if (dir && rad_v == 0.f) rad_v = PI;
				img->hdl = Image::Instance()->Load("spear");
				img->angle = rad_v;
				if (!Sound::Instance()->IsPlaying("shot_spear")) {
					Sound::Instance()->Play("shot_spear");
				}
				atkHitbox->Clear();
				for (int i = 0; i < 10; ++i) {
					atkHitbox->PushBack(hitRect);
					prev[i] = pos;
				}
				dir = RIGHT;
			}
			break;
		case 4: // ÉåÅ[ÉUÅ[
			for (int i = 0; i < 10; ++i) {
				atkHitbox->SetRect(i, hitRect + (prev[i] - pos));
			}
			prev[stateCnt % 10] = pos;
			Create(std::make_shared<EffectAfterImage>(pos, img, dir, img->angle));
			break;
		}
		if (state < 4 && HitMap()) exist = false;
	}
	void SetDamage(Enemy* e) override {
		if (state == 4) {
			if (std::find(victim.begin(), victim.end(), e) != victim.end()) return;
			e->AddHP(-power);
			victim.push_back(e);
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		} else {
			e->AddHP(-power / 10);
			exist = false;
		}
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("chaser"), 0.4f));
	}
	void SetDamage(Block* b) override {
		if (b->IsHalf()) return;
		if (state == 4) {
			if (std::find(victim.begin(), victim.end(), b) != victim.end()) return;
			b->AddHP(-power);
			victim.push_back(b);
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		} else {
			b->AddHP(-power / 10);
			exist = false;
		}
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("chaser"), 0.4f));
	}
	void SetDamage(Projectile* p) override {
		if (state == 4) {
			if (std::find(victim.begin(), victim.end(), p) != victim.end()) return;
			p->AddHP(-power);
			victim.push_back(p);
			Sound::Instance()->Play("çUåÇÉqÉbÉg");
		} else {
			p->AddHP(-power / 10);
			exist = false;
		}
		Create(std::make_shared<EffectHit>(pos, Image::Instance()->Load("chaser"), 0.4f));
	}
private:
	Rect hitRect;
	Vector2 prev[10];
	std::vector<GameEntity*> victim;
};
BulletSpear::BulletSpear(const Player& p) : PlayerBullet(p) {
	for (int i = 0; i < 3; ++i)
		Create(std::make_shared<BulletSpearPart>(p, i));
	exist = false;
	Sound::Instance()->Play("shot_chaser");
}

// é©ã@íeÅ@îíã≠âª
class BulletMagnetarPart1 : public PlayerBullet
{
public: // ñ{ëÃ
	BulletMagnetarPart1(const Player& p) : PlayerBullet(p) {
		v = 4.f;
		rad_v = rad_a = dir ? PI : 0.f;
		width = 80;
		height = 80;
		maxcnt = 90;
		power = param[GEAR_AD].power;
		atkHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("magnetar");
		if (p.IsShift()) power = (int)(power * 1.2f);
	}
	void Update() override {
		img->num = stateCnt / 6 % 18;
		img->angle += PI / 120.f;
		img->exRate = 0.9f + 0.1f * sinf(stateCnt / 6.f * PI / 12.f);
		// è„â∫ÉLÅ[Ç≈ëÄçÏ
		int u = stage->GetPlayer()->GetInput()->Get(INPUT_UP);
		int d = stage->GetPlayer()->GetInput()->Get(INPUT_DOWN);
		if (u > 0) {
			vel.y -= 0.05f;
		} else if (d > 0) {
			vel.y += 0.05f;
		} else {
			if (vel.y > 0.f) vel.y -= 0.05f;
			else if (vel.y < 0.f) vel.y += 0.05f;
			else vel.y = 0.f;
		}
	}
	void SetDamage(Enemy* e) override {
		if (stateCnt % 4 != 0) return;
		e->AddHP(-power);
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	void SetDamage(Block* b) override {
		if (stateCnt % 4 != 0) return;
		b->AddHP(-power);
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	void SetDamage(Projectile* p) override {
		if (stateCnt % 4 != 0) return;
		p->AddHP(-power);
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
};
class BulletMagnetarPart2 : public PlayerBullet
{
public: // ãzÇ¢çûÇ›
	BulletMagnetarPart2(const Player& p) : PlayerBullet(p) {
		v = 4.f;
		rad_v = rad_a = dir ? PI : 0.f;
		width = 280;
		height = 280;
		maxcnt = 90;
		atkHitbox = std::make_shared<HitBox>(width, height);
	}
	void Update() override {
		if (stateCnt % 5 == 0) {
			Create(std::make_shared<EffectMagnetar>(pos, this));
		}
		// è„â∫ÉLÅ[Ç≈ëÄçÏ
		int u = stage->GetPlayer()->GetInput()->Get(INPUT_UP);
		int d = stage->GetPlayer()->GetInput()->Get(INPUT_DOWN);
		if (u > 0) {
			vel.y -= 0.05f;
		} else if (d > 0) {
			vel.y += 0.05f;
		} else {
			if (vel.y > 0.f) vel.y -= 0.05f;
			else if (vel.y < 0.f) vel.y += 0.05f;
			else vel.y = 0.f;
		}
	}
	void SetDamage(Enemy* e) override {
		if (e->IsFix()) return;
		e->SetVel(e->GetVel() / 4.f);
		Vector2 ep = e->GetPos();
		if (pos.x - ep.x > 30.f) ep.x += 4.f;
		else if (pos.x - ep.x < -30.f) ep.x -= 4.f;
		if (pos.y - ep.y > 30.f) ep.y += 2.f;
		else if (pos.y - ep.y < -30.f) ep.y -= 2.f;
		e->SetPos(ep);
	}
	void SetDamage(Block* b) override { }
	void SetDamage(Projectile* p) override { }
};
BulletMagnetar::BulletMagnetar(const Player& p) : PlayerBullet(p) {
	Create(std::make_shared<BulletMagnetarPart1>(p));
	Create(std::make_shared<BulletMagnetarPart2>(p));
	exist = false;
	Sound::Instance()->Play("shot_magne");
}

// é©ã@íeÅ@çïã≠âª
class BulletPhantomPart : public PlayerBullet
{
public:
	BulletPhantomPart(const Vector2& p, int h, bool d) : PlayerBullet(*stage->GetPlayer()) {
		dir = d;
		v = 2.f;
		rad_v = dir ? PI : 0.f;
		a = 0.15f;
		rad_a = rad_v + RandFloat(PI / 12.f);
		width = 15;
		height = 15;
		pos.x = p.x;
		pos.y = p.y + RandFloat(h / 2.f - 16.f);
		maxcnt = 60 + GetRand(30);
		power = param[GHOST_AD].power;
		atkHitbox = std::make_shared<HitBox>(width, height);
		img->hdl = Image::Instance()->Load("ghost");
	}
	void Update() override {
		if (stateCnt % 4 == 0) {
			++width;
			++height;
			atkHitbox->SetRect(0, { -width / 2, -height / 2, width / 2, height / 2 });
		}
		if (stateCnt % 2 == 0) {
			Create(std::make_shared<EffectAfterImage>(pos, img, dir, img->angle, img->exRate));
		}
		img->exRate = (float)stateCnt / maxcnt / 2.f + RandFloat(1.f, 1.8f);
	}
	void SetDamage(Enemy* e) override {
		if (std::find(victim.begin(), victim.end(), e) != victim.end()) return;
		e->AddHP(-power);
		victim.push_back(e);
		Create(std::make_shared<EffectHit>(pos, img->hdl));
		Sound::Instance()->Play("çUåÇÉqÉbÉg");
	}
	// ÉuÉçÉbÉNÇ‚ìGíeÇ…ÇÕìñÇΩÇÁÇ»Ç¢
	void SetDamage(Block* b) override { }
	void SetDamage(Projectile* p) override { }
private:
	std::vector<GameEntity*> victim;
};
BulletPhantom::BulletPhantom(const Player& p) : PlayerBullet(p) {
	img->hdl = Image::Instance()->Load("gate");
	img->blendmode = DX_BLENDMODE_ADD;
	Sound::Instance()->Play("shot_phantom_gate");
	player = stage->GetPlayer();
	player->AddMP(-param[GHOST_AD].cost * 2);
}
void BulletPhantom::Update() {
	if (!player->IsShooting() || player->GetBulletType() != GHOST_AD) {
		exist = false;
		return;
	}
	switch (state) {
	case 0:
		width = 1;
		height += 14;
		if (height == 14 * 8) ++state;
		break;
	case 1:
		width += 8;
		if (width == 25) ++state;
		break;
	case 2:
		if (stateCnt % 2 == 0) {
			if (player->GetMP() < param[GHOST_AD].cost) {
				Create(std::make_shared<EffectNoMP>(player->GetPos()));
			} else {
				player->AddMP(-param[GHOST_AD].cost);
				Create(std::make_shared<BulletPhantomPart>(pos, height, dir));
				Sound::Instance()->Play("shot_phantom");
			}
		}
		break;
	}
	img->num = stateCnt / 6 % 4;
}
void BulletPhantom::Draw() const {
	if (!img) return;
	if (!img->drawflag || img->hdl == 0) return;
	Vector2 p = stage->GetCamera()->TransCoord(pos);
	SetDrawBlendMode(img->blendmode, img->alpha);
	DrawModiGraphF(
		p.x - width / 2, p.y - height / 2, 
		p.x + width / 2, p.y - height / 2, 
		p.x + width / 2, p.y + height / 2, 
		p.x - width / 2, p.y + height / 2, img->hdl->at(img->num), TRUE);
}

// íeê∂ê¨
void PlayerBullet::Fire(const Player& p, BulletType type) {
	switch (type) {
	case STAR: Create(std::make_shared<BulletStar>(p)); break;
	case METEOR: Create(std::make_shared<BulletMeteor>(p)); break;
	case CHASER: Create(std::make_shared<BulletChaser>(p)); break;
	case GEAR: Create(std::make_shared<BulletGear>(p)); break;
	case GHOST: Create(std::make_shared<BulletGhost>(p)); break;
	case STAR_AD: Create(std::make_shared<BulletDual>(p)); break;
	case METEOR_AD: Create(std::make_shared<BulletNova>(p)); break;
	case CHASER_AD: Create(std::make_shared<BulletSpear>(p)); break;
	case GEAR_AD: Create(std::make_shared<BulletMagnetar>(p)); break;
	case GHOST_AD: Create(std::make_shared<BulletPhantom>(p)); break;
	}
}
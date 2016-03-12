#include "Effect.h"
#include "Image.h"
#include "Projectile.h"
#include "common.h"
#include <DxLib.h>
#include <cmath>


Effect::Effect(const Vector2& pos) : GameEntity(pos) {
	otype = EFFECT;
	priority = Priority::EFFECT;
	v = rad_v = a = rad_a = 0.f;
	maxcnt = 0;
}

Effect::~Effect() {}

bool Effect::UpdatePre() {
	GameEntity::UpdatePre();
	if (stateCnt < 0) {
		stateCnt++;
		return false;
	}
	if (stateCnt >= maxcnt) exist = false;
	return !exist;
}

void Effect::Update() {}

void Effect::UpdatePosition() {
	float ax = a * cosf(rad_a);
	float ay = a * sinf(rad_a);
	
	vel.x = v * cosf(rad_v) + ax;
	vel.y = v * sinf(rad_v) + ay;
	
	pos += vel;

	if (a != 0.f) {
		rad_v = vel.Angle();
		v = vel.Length();
	}
}

void Effect::DamageFrom(GameEntity* e) {
}

// �c��
EffectAfterImage::EffectAfterImage(
	const Vector2& p, std::shared_ptr<ImageData> idt,
	bool dir, float angle, float scale) : Effect(p) 
{
	maxcnt = 10;
	img->hdl = idt->hdl;
	img->angle = angle;
	img->blendmode = DX_BLENDMODE_ALPHA;
	img->exRate = scale;
	img->drawflag = true;
	img->num = idt->num;
	img->ofs_x = idt->ofs_x;
	img->ofs_y = idt->ofs_y;
	this->dir = dir;
	priority = Priority::BACK_EFFECT;
}
void EffectAfterImage::Update() {
	img->alpha = (int)(200.f * (maxcnt - stateCnt) / maxcnt);
	img->exRate -= 0.5f / maxcnt;
}

// �T�C�Y�����������c��
EffectAfterImage2::EffectAfterImage2(
	const Vector2& p, std::shared_ptr<ImageData> idt,
	bool dir, float angle, float scale) : Effect(p)
{
	maxcnt = 10;
	img->hdl = idt->hdl;
	img->angle = angle;
	img->blendmode = DX_BLENDMODE_ALPHA;
	img->exRate = scale;
	img->drawflag = true;
	img->num = idt->num;
	img->ofs_x = idt->ofs_x;
	img->ofs_y = idt->ofs_y;
	this->dir = dir;
	priority = Priority::BACK_EFFECT;
}
void EffectAfterImage2::Update() {
	img->alpha = (int)(80.f * (maxcnt - stateCnt) / maxcnt);
}

// �_�b�V��
EffectPlayerDash::EffectPlayerDash(const Vector2& p, bool dir) :
	Effect({ p.x, p.y + 40 })
{
	maxcnt = 12;
	img->hdl = Image::Instance()->Load("dash");
	this->dir = dir;
}
void EffectPlayerDash::Update() {
	if (stateCnt == 4) img->num = 1;
	if (stateCnt == 8) img->num = 2;
}

// �q�b�g�G�t�F�N�g
class EffectHitPart : public Effect
{
public:
	EffectHitPart(const Vector2& p, LPHIMG hdl, float scale) : Effect(p) {
		maxcnt = 20;
		v = RandFloat(1.f, 6.f);
		rad_v = RandFloat(PI);
		a = v / maxcnt;
		rad_a = rad_v + PI;
		img->blendmode = DX_BLENDMODE_ADD;
		img->hdl = hdl;
		img->angle = rad_v;
		img->exRate = scale + RandFloat(scale / 2.f);
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		img->alpha = (int)(255.f * (maxcnt - stateCnt) / maxcnt);
	}
};
EffectHit::EffectHit(const Vector2& p, LPHIMG hdl, float scale) : Effect(p) {
	for (int i = 0; i < 20; ++i) {
		Create(std::make_shared<EffectHitPart>(p, hdl, scale));
	}
	exist = false;
}

// �e�؂�
EffectNoMP::EffectNoMP(const Vector2& p) : Effect(p) {
	maxcnt = 12;
	pos.y -= 40.f;
	img->hdl = Image::Instance()->Load("tamagire");
	img->blendmode = DX_BLENDMODE_ALPHA;
	priority = Priority::FRONT_EFFECT;
}
void EffectNoMP::Update() {
	float d;
	switch (state) {
	case 0:
		d = 2.f * stateCnt / maxcnt;
		pos.y -= 4.f * sinf(PI / 2.f * d);
		img->alpha = (int)(128.f * d);
		if (stateCnt > maxcnt / 2) ++state;
		break;
	case 1:
		img->alpha = 180;
		break;
	}
}

// �A�C�e���̌�
class EffectItemLightPart : public Effect
{
public:
	EffectItemLightPart(const Vector2& p, int type) : Effect(p) {
		maxcnt = 20;
		v = RandFloat(0.5f, 1.1f);
		rad_v = -PI / 2.f;
		img->hdl = Image::Instance()->Load("item_light");
		img->num = type % STAR_AD;
		img->exRate = 1.f + RandFloat(0.3f);
		img->blendmode = DX_BLENDMODE_ADD;
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		img->angle += PI / 64.f;
		img->alpha = (int)(128 * (maxcnt - stateCnt) / maxcnt);
	}
};
EffectItemLight::EffectItemLight(const Vector2& p, int type) : Effect(p) {
	for (int i = 0; i < 3; ++i) {
		Vector2 pp = pos;
		pos.x += RandFloat(20.f);
		pos.y += RandFloat(15.f);
		Create(std::make_shared<EffectItemLightPart>(pp, type));
	}
	exist = false;
}

// ���@���S
class EffectPlayerDeathPart : public Effect
{
public:
	EffectPlayerDeathPart(const Vector2& p) : Effect(p) {
		float r = RandFloat0(100.f), rnda = RandFloat0(PI2);
		maxcnt = 140;
		pos.x += r * cosf(rnda);
		pos.y += r * sinf(rnda);
		v = 1.5f;
		rad_v = (pos - p).Angle();
		img->hdl = Image::Instance()->Load("player_death");
		img->num = GetRand(2);
		img->angle = RandFloat0(PI2);
		img->blendmode = DX_BLENDMODE_ADD;
		img->exRate = RandFloat(1.f, 2.f);
		angle = RandFloat(PI / 30.f);
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		img->angle += angle;
		img->alpha = (int)(255.f * (maxcnt - stateCnt) / maxcnt);
		v -= 1.5f / maxcnt;
	}
private:
	float angle;
};
EffectPlayerDeath::EffectPlayerDeath(const Vector2& p) : Effect(p) {
	for (int i = 0; i < 120; ++i) {
		Create(std::make_shared<EffectPlayerDeathPart>(p));
	}
	exist = false;
}

// �Ԕ���
class EffectFExplodePart1 : public Effect 
{
public:
	EffectFExplodePart1(const Vector2& p, float dec) : Effect(p) {
		maxcnt = 18;
		stateCnt = -10 + GetRand(15);
		v = 4.f;
		rad_v = RandFloat(PI);
		img->hdl = Image::Instance()->Load("f_explode");
		img->exRate = 0.3f;
		img->blendmode = DX_BLENDMODE_ADD;
		priority = Priority::FRONT_EFFECT;
		decScale = dec;
	}
	void Update() override {
		v -= decScale / maxcnt;
		img->exRate += 0.03f + 0.005f*decScale;
		img->alpha = (int)(255.f * (maxcnt - stateCnt) / maxcnt);
	}
private:
	float decScale;
};
class EffectFExplodePart2 : public Effect
{
public:
	EffectFExplodePart2(const Vector2& p) : Effect(p) {
		maxcnt = 18;
		stateCnt = -8 + GetRand(12);
		v = RandFloat(4.f, 5.f);
		rad_v = RandFloat(PI);
		img->hdl = Image::Instance()->Load("f_explode");
		img->exRate = 1.f + RandFloat(0.3f);
		img->blendmode = DX_BLENDMODE_ALPHA;
		img->num = 1;
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		img->angle += PI / 64.f;
		img->alpha = (int)(255.f * sinf(PI / 2.f + PI / 2.f * stateCnt / maxcnt));
	}
};
EffectFExplode::EffectFExplode(const Vector2& p, float dec) : Effect(p) {
	for (int i = 0; i < 35; ++i)
		Create(std::make_shared<EffectFExplodePart1>(p, dec));
	for (int i = 0; i < 10; ++i)
		Create(std::make_shared<EffectFExplodePart2>(p));
	exist = false;
}

// ������
class EffectMExplodePart1 : public Effect
{
public:
	EffectMExplodePart1(const Vector2& p) : Effect(p) {
		maxcnt = 24;
		stateCnt = -10 + GetRand(15);
		v = 4.f;
		rad_v = RandFloat(PI);
		img->hdl = Image::Instance()->Load("m_explode");
		img->exRate = 0.3f;
		img->blendmode = DX_BLENDMODE_ADD;
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		v -= 4.f / maxcnt;
		img->exRate += 0.012f;
		img->alpha = (int)(255.f * (maxcnt - stateCnt) / maxcnt);
	}
};
class EffectMExplodePart2 : public Effect
{
public:
	EffectMExplodePart2(const Vector2& p) : Effect(p) {
		maxcnt = 60;
		stateCnt = -10 + GetRand(15);
		v = RandFloat(1.5f, 2.5f);
		rad_v = RandFloat(PI, PI2);
		a = 0.08f;
		rad_a = PI / 2.f;
		img->hdl = Image::Instance()->Load("m_explode");
		img->exRate = 1.f + RandFloat(0.3f);
		img->blendmode = DX_BLENDMODE_ALPHA;
		img->num = 1;
		priority = Priority::FRONT_EFFECT;
	}
	void Update() override {
		img->angle += PI / 24.f;
		img->alpha = (int)(255.f * sinf(PI / 2.f + PI / 2.f * stateCnt / maxcnt));
	}
};
EffectMExplode::EffectMExplode(const Vector2& p) : Effect(p) {
	for (int i = 0; i < 30; ++i)
		Create(std::make_shared<EffectMExplodePart1>(p));
	for (int i = 0; i < 12; ++i)
		Create(std::make_shared<EffectMExplodePart2>(p));
	exist = false;
}

// �������z����
class EffectMagnetarPart : public Effect
{
public:
	EffectMagnetarPart(const Vector2& p, GameEntity* pare) : Effect(p) {
		float r = RandFloat(40.f, 160.f), angle = RandFloat0(PI2);
		maxcnt = 15;
		pos.x += r * cosf(angle);
		pos.y += r * sinf(angle);
		v = 4.f;
		rad_v = (p - pos).Angle();
		img->hdl = Image::Instance()->Load("eff_mag");
		img->blendmode = DX_BLENDMODE_ADD;
		img->angle = rad_v;
		parent = pare;
	}
	void Update() override {
		if (!parent || !parent->GetExist()) {
			parent = nullptr;
			exist = false;
			return;
		}
		pos += parent->GetVel();
		img->exRate = 2.5f * (maxcnt - stateCnt) / maxcnt;
		img->alpha = (int)((255.f * stateCnt) / maxcnt);
	}
private:
	GameEntity* parent;
};
EffectMagnetar::EffectMagnetar(const Vector2& p, GameEntity* parent) : Effect(p) {
	for (int i = 0; i < 30; ++i)
		Create(std::make_shared<EffectMagnetarPart>(p, parent));
	exist = false;
}
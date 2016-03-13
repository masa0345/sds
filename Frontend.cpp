#include "Frontend.h"
#include "Image.h"
#include "Player.h"
#include "common.h"
#include "Projectile.h"
#include "Enemy.h"
#include <DxLib.h>

std::list<std::shared_ptr<Frontend>> Frontend::manager;

Frontend::Frontend() : exist(true) {
}

Frontend::~Frontend() {}

void Frontend::Update() {
}

void Frontend::Draw() const {
}

bool Frontend::GetExist() const {
	return exist;
}

void Frontend::InitManager()
{
	manager.clear();
}

void Frontend::Create(std::shared_ptr<Frontend> f)
{
	manager.push_back(f);
}

void Frontend::UpdateAll()
{
	if (manager.empty()) return;

	manager.erase(
		std::remove_if(manager.begin(), manager.end(),
			[](std::shared_ptr<Frontend> p) { return !p->GetExist(); }),
		manager.end());

	for (auto f : manager) f->Update();
}

void Frontend::DrawAll()
{
	for (auto f : manager) f->Draw();
}


// 自機ゲージ
FrontendPlayerGauge::FrontendPlayerGauge(Player* p) {
	SetTarget(p);
	type = STAR;
	hp = p->hp;
	hpmax = p->hpmax;
	mp = p->mp;
	mpmax = p->mpmax;
	fire = p->fireCnt;
	stateCnt = 0;
}

void FrontendPlayerGauge::Update() {
	++stateCnt;
	if (!target) return;
	if (!target->GetExist()) {
		target = nullptr;
		return;
	}
	type = target->bulType;
	hp = target->hp;
	mp = target->mp;
	fire = target->fireCnt;
	if (hp < 0) hp = 0;
}

void FrontendPlayerGauge::Draw() const {
	const int x = 8, y = WINDOW_HEIGHT - 86;

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	DrawGraph(x + 2, y + 2, Image::Instance()->Load("weapon_icon")->at(type), FALSE);

	BulletParam param = PlayerBullet::GetBulletParam(type);
	int alpha = 128;
	if (mp < param.cost) {
		//弾切れ
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawGraph(x + 2, y + 2, Image::Instance()->Load("uicover")->at(1), TRUE);
		alpha = 32;
	} else if (fire%param.delay > 2) {
		//ディレイ
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawGraph(x + 2, y + 2, Image::Instance()->Load("uicover")->at(1), TRUE);
		alpha = 32;
	}
	SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
	DrawGraph(x + 5, y + 60 - (stateCnt % 120) / 2, Image::Instance()->Load("uiline")->at(0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ADD, 96);
	DrawGraph(x + 5, y + 5, Image::Instance()->Load("uicover")->at(0), TRUE);

	int hpw = (int)((float)hp / hpmax * HP_WIDTH);
	int mpw = (int)((float)mp / mpmax * MP_WIDTH);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	DrawGraph(x, y, Image::Instance()->Load("player_gauge")->at(0), TRUE);
	DrawModiGraph(x + 88, y + 42, x + 88 + hpw, y + 42,
		x + 88 + hpw, y + 42 + HP_HEIGHT, x + 88, y + 42 + HP_HEIGHT,
		Image::Instance()->Load("hpmeter")->at(0), FALSE);
	DrawModiGraph(x + 85, y + 58, x + 85 + mpw, y + 58,
		x + 85 + mpw, y + 58 + MP_HEIGHT, x + 85, y + 58 + MP_HEIGHT,
		Image::Instance()->Load("mpmeter")->at(0), FALSE);
	DrawGraph(x + 9, y + 59, Image::Instance()->Load("weapon_name")->at(type), TRUE);
}

void FrontendPlayerGauge::SetTarget(Player* p) {
	target = p;
}


// ボスゲージ
FrontendBossGauge::FrontendBossGauge(Enemy * e) : target(e)
{
}

void FrontendBossGauge::Update()
{
	if (!target) return;
	if (!target->GetExist()) {
		target = nullptr;
		exist = false;
	}
}

void FrontendBossGauge::Draw() const
{
	if (!target || !target->GetExist()) return;
	int hpw = 490 * target->GetHP() / target->GetHPMax();
	DrawGraph(70, 40, Image::Instance()->Load("boss_gauge")->at(0), FALSE);
	DrawModiGraph(75, 46, 75 + hpw, 46, 75 + hpw, 46 + 24, 75, 46 + 24,
		Image::Instance()->Load("boss_meter")->at(0), FALSE);
}

#include "Player.h"
#include "Image.h"
#include "Input.h"
#include "MapChip.h"
#include "Sound.h"
#include "Effect.h"
#include "Enemy.h"
#include "Item.h"
#include "Projectile.h"
#include "Frontend.h"
#include "Stage.h"

namespace {
	enum PlayerState {
		STAND_MOVE,
		SHIFT_MOVE,
		DASH,
		JUMP,
		DASH_JUMP,
		SHIFT_JUMP,
		DAMAGE
	};

	//自機の当たり判定サイズ
	Rect playerhit[] = {
		{ -24,-48, 24, 48 }, //立ち　攻撃
		{ -20,-38, 20, 48 }, //低速
		{ -40,  8, 40, 48 }, //ダッシュ
		{ -24,-38, 24, 48 }, //ジャンプ
		{ -30,-38, 30, 48 }, //歩く
		{ -20,-38, 20, 48 }, //
		{ -30,-38, 30, 48 }, //
	};
}

Player::Player(std::shared_ptr<JoypadInput> input) : 
	GameEntity(pos), input(input)
{
	otype = PLAYER;
	priority = Priority::PLAYER;
	hitFilter = 1;
	img->hdl = Image::Instance()->Load("player");
	hp = hpmax = 1000;
	mp = mpmax = 1000;
	height = 96;
	width = 48;
	jump = 2;
	animCnt;
	state = STAND_MOVE;
	hitmap = 0;
	prev = pos;
	fireCnt = 0;
	itemCnt = 0;
	bulType = STAR;
	ignoreCamera = false;
	dmgHitbox = std::make_shared<HitBox>(width, height);
	Frontend::Create(std::make_shared<FrontendPlayerGauge>(this));
}

Player::~Player() { }

void Player::Update() 
{
	StatePattern();
	// 重力
	if (vel.y < 8.f) vel.y += state == DAMAGE ? 0.4f : 0.8f;
	else vel.y = 8.f;

	if (vel.y < -16.f) vel.y = -13.f;

	if (hitmap & (HIT_SLOPE | HIT_TOP) && jump == 2) {
		vel.y += (fabs(vel.x) + 1) / 2;
	}
	if (jump) jump = 1;

	// 無敵時間
	if (invincible > 0) {
		img->drawflag = invincible % 2 == 1;
		--invincible;
	}
	// アイテム取得クール
	if (itemCnt > 0) {
		--itemCnt;
	}
	// ダッシュ中残像
	if (state == DASH || state == DASH_JUMP) {
		if (stateCnt % 2) Create(std::make_shared<EffectAfterImage2>(pos, img, dir));
	}
	// 死亡
	if (hp <= 0) {
		hp = 0;
		exist = false;
		Create(std::make_shared<EffectPlayerDeath>(pos));
		Sound::Instance()->Play("死亡");
	}
	// 画面外補正
	if (!ignoreCamera) {
		Rect cf = stage->GetCamera()->GetField();
		if (cf.left > pos.x - width / 2) pos.x = (float)(cf.left + width / 2);
		else if (cf.right < pos.x + width / 2) pos.x = (float)(cf.right - width / 2);
		// 落下死
		if (cf.bottom + 120 < pos.y) hp = 0;
	}
	// 当たり判定更新
	dmgHitbox->SetRect(0, playerhit[img->num / 6]);

	if (input->Get(INPUT_D) == 1) {
		auto o = Create(std::make_shared<ItemWeapon>(pos, GEAR));
		o->SetPos({ pos.x, pos.y - 100.f });
		//stage->SetLoadNext(true);
		//Create(std::make_shared<EffectMExplode>(pos));
	}
}

void Player::UpdateColResponse() {
	//マップ当たり判定
	MapChip::CorrectVal p = { pos, vel, jump };

	int w = width, h = height;
	MapChip::ColPoint hitPos[] = {
		{ -w / 4, -h / 2  , HIT_BOTTOM } ,
		{ +w / 4, -h / 2  , HIT_BOTTOM } ,
		{ -w / 4, +h / 2 - 1, HIT_TOP | HIT_LSLOPE } ,
		{ +w / 4, +h / 2 - 1, HIT_TOP | HIT_RSLOPE } ,
		{ -w / 2, +h / 4 + h / 8 , HIT_RIGHT } ,
		{ -w / 2, +h / 8 , HIT_RIGHT } ,
		{ -w / 2, -h / 8 , HIT_RIGHT } ,
		{ -w / 2, -h / 4 - h / 8 , HIT_RIGHT } ,
		{ +w / 2 - 1, +h / 4 + h / 8 , HIT_LEFT } ,
		{ +w / 2 - 1, +h / 8 , HIT_LEFT } ,
		{ +w / 2 - 1, -h / 8 , HIT_LEFT } ,
		{ +w / 2 - 1, -h / 4 - h / 8 , HIT_LEFT } ,
	};

	hitmap = MapChip::Instance()->Hit(&p, prev, hitPos, sizeof(hitPos) / sizeof(hitPos[0]));

	SetPos(p.p);
	jump = p.jump;
	vel.y = p.v.y;
}

void Player::SetPos(const Vector2& p) {
	pos = prev = p;
}

void Player::InitState()
{
	state = STAND_MOVE;
	stateCnt = animCnt = 0;
	vel = { 0.f, 0.f };
	img->num = 1;
}

void Player::DamageFrom(GameEntity* e)
{
	e->SetDamage(this);
}

void Player::CalcDamage(int damage)
{
	state = DAMAGE;
	stateCnt = animCnt = 0;
	hp -= damage;
	vel = { 0.f, -3.f };
	if (jump == 1) jump = 0;
	invincible = 120;
	Sound::Instance()->Play("くらう");
}

bool Player::PickItem(int type, int itemmp) {
	int s = input->Get(INPUT_S);
	if (s == 0 || itemCnt > 0) return false;
	
	if (bulType == type && type < STAR_AD) {
		// 武器を強化して取得
		bulType = (BulletType)(type + STAR_AD);
		mp = (itemmp + mp) / 2;
		Sound::Instance()->Play("拾う2");
	} else if (bulType == type + STAR_AD) {
		// 使用中武器の強化前を取得
		AddMP(itemmp / 4);
		Sound::Instance()->Play("拾う2");
	} else if (bulType == type && type >= STAR_AD) {
		// 強化武器使用中に同武器を取得
		AddMP(itemmp);
		Sound::Instance()->Play("拾う2");
	} else {
		// 別の武器を取得
		bulType = (BulletType)type;
		mp = itemmp;
		Sound::Instance()->Play("拾う");
	}
	itemCnt = 60;
	return true;
}

void Player::StatePattern() 
{
	int l, r, atk, jmp, dash;
	l = input->Get(INPUT_LEFT);
	r = input->Get(INPUT_RIGHT);
	atk = input->Get(INPUT_Z);
	jmp = input->Get(INPUT_X);
	dash = input->Get(INPUT_C);
	bool shift = input->Get(INPUT_D) > 0 || (bulType==GHOST_AD && IsShooting());

	switch (state) {
	case STAND_MOVE:
		if ((l && !r) || (!l && r)) {
			// 移動
			dir = r ? RIGHT : LEFT;
			vel.x = 3.f;
			// 移動アニメーション
			img->num = 24 + (stateCnt / 8) % 6;
			if (atk) {
				img->num = 36 + (stateCnt / 8) % 6;
				++animCnt;
			}
		} else {
			// 立ち
			vel.x = 0.f;
			img->num = 1;
			if (atk) {
				img->num = ++animCnt < 9 ? 3 + (animCnt / 3) % 3 : 5;
			} else {
				animCnt = 0;
			}
		}
		// ダッシュへ移行
		if (dash == 1) {
			vel.x = 7.f;
			state = DASH;
			stateCnt = animCnt = 0;
			Create(std::make_shared<EffectPlayerDash>(pos, dir));
			Sound::Instance()->Play("ダッシュ");
			break;
		}
		// ジャンプへ移行
		if (jmp == 1 && jump) {
			vel.y = -13.f;
			--jump;
			state = JUMP;
			stateCnt = animCnt = 0;
			break;
		} else if (jump != 2) {
			state = JUMP;
			stateCnt = animCnt = 0;
			vel.y = 1.f;
			break;
		}
		// 方向固定モードへ以降
		if (shift) {
			state = SHIFT_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		break;
	case SHIFT_MOVE: // 方向固定
		// 移動
		if (l && !r) {
			++animCnt;
			if (dir == LEFT) {
				vel.x = 2.1f;
				if (atk) {
					img->num = 30 + (animCnt / 8) % 6;
				} else {
					img->num = 6 + (animCnt / 8) % 6;
				}
			} else {
				vel.x = -2.1f;
				if (atk) {
					img->num = 35 - (animCnt / 8) % 6;
				} else {
					img->num = 11 - (animCnt / 8) % 6;
				}
			}
		} else if (!l && r) {
			++animCnt;
			if (dir == RIGHT) {
				vel.x = 2.1f;
				if (atk) {
					img->num = 30 + (animCnt / 8) % 6;
				} else {
					img->num = 6 + (animCnt / 8) % 6;
				}
			}
			else {
				vel.x = -2.1f;
				if (atk) {
					img->num = 35 - (animCnt / 8) % 6;
				} else {
					img->num = 11 - (animCnt / 8) % 6;
				}
			}
		} else {
			// 立ち
			vel.x = 0.f;
			img->num = atk ? 30 : 6;
		}
		// ジャンプへ移行
		if (jmp == 1 && jump) {
			vel.y = -11.f;
			--jump;
			state = SHIFT_JUMP;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("ジャンプ");
			break;
		} else if (jump != 2) {
			state = SHIFT_JUMP;
			stateCnt = animCnt = 0;
			vel.y = 1.f;
			break;
		}
		// 立ちへ移行
		if (!shift) {
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
		}
		break;
	case DASH: // ダッシュ
		if (stateCnt >= 22) {
			vel.x = 0.f;
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// ダッシュジャンプへ移行
		if (jmp == 1 && jump) {
			vel.y = -15;
			--jump;
			state = DASH_JUMP;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("ジャンプ");
			break;
		} else if (jump != 2) {
			state = DASH_JUMP;
			stateCnt = animCnt = 0;
			vel.y = 1.f;
		}
		// ダッシュアニメーション
		if (++animCnt < 12) img->num = 12 + (animCnt / 6) % 2;
		else img->num = 14 + (animCnt / 4) % 2;
		break;
	case JUMP: case DASH_JUMP: // ジャンプ
		if (jump == 2) {
			// 着地
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// 空中での方向転換
		if ((l && !r) || (!l && r)) {
			dir = r ? RIGHT : LEFT;
			vel.x = state == JUMP ? 3.f : 5.f;
		} else {
			vel.x = 0.f;
		}
		// 空中ジャンプ
		if (jmp == 1 && jump) {
			vel.y = -13.f;
			--jump;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("ジャンプ");
			break;
		}
		if (shift) state = SHIFT_JUMP;
		// ジャンプアニメーション
		++animCnt;
		if (animCnt < 3) img->num = 18;
		else if (animCnt < 12) img->num = 19;
		else img->num = vel.y <= 0 ? 20 : 21;
		if (vel.y > 0) img->num = 21;
		if (atk) img->num = 22;
		break;
	case SHIFT_JUMP:
		if (jump == 2) {
			// 着地
			state = shift ? SHIFT_MOVE : STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// 空中での方向転換
		if (l && !r) {
			vel.x = dir == LEFT ? 2.1f : -2.1f;
		} else if(!l && r) {
			vel.x = dir == RIGHT ? 2.1f : -2.1f;
		} else {
			vel.x = 0.f;
		}
		// 空中ジャンプ
		if (jmp == 1 && jump) {
			vel.y = -13.f;
			--jump;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("ジャンプ");
			break;
		}
		if (!shift) state = JUMP;
		// ジャンプアニメーション
		++animCnt;
		if (animCnt < 3) img->num = 18;
		else if (animCnt < 12) img->num = 19;
		else img->num = vel.y <= 0 ? 20 : 21;
		if (vel.y > 0) img->num = 21;
		if (atk) img->num = 22;
		break;
	case DAMAGE: // 被弾
		if (stateCnt > 20) {
			if (jump == 2) {
				state = shift ? SHIFT_MOVE : STAND_MOVE;
			} else {
				state = shift ? SHIFT_JUMP : JUMP;
			}
			stateCnt = animCnt = 0;
		}
		img->num = 23;
		break;
	}

	// アイテムを捨てる
	if (bulType != STAR && input->Get(INPUT_A) == 1) {
		Create(std::make_shared<ItemWeapon>(pos, bulType, mp));
		bulType = STAR;
		mp = mpmax;
		Sound::Instance()->Play("捨てる");
	}

	// 弾生成
	BulletParam param = PlayerBullet::GetBulletParam(bulType);
	if (atk > 3 && state != DASH && state != DAMAGE) {
		++fireCnt;
		//delayが4以下ならfirecntが3のとき、それ以外はfirecnt % delay
		if ((param.delay < 4) ? fireCnt == 3 : fireCnt % param.delay == 3) {
			//mpが足りていればショットを撃つ
			if (mp >= param.cost) {
				PlayerBullet::Fire(*this, bulType);
				mp -= param.cost;
			} else {
				//弾切れのとき
				Sound::Instance()->Play("弾切れ");
				Create(std::make_shared<EffectNoMP>(pos));
			}
		}
	} else if (atk == 0 && fireCnt>0) {
		//攻撃ボタンが押されていなくてfirecntが0でない
		if (fireCnt % param.delay == 2) {
			fireCnt = 0;
		} else {
			fireCnt++;
		}
	}
	if (mp < 0) mp = 0;
}

// ブロックとの衝突応答
void Player::BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) {
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
		if (hitmap & HIT_LSLOPE) {
			vel.y = 10.f;
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
		if (hitmap & HIT_RSLOPE) {
			vel.y = 10.f;
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
			jump = 2;
			vel.y = 4.f;
			pos.x += bvx;
			hitmap |= HIT_TOP;
		}
	}
}
// 上側のみ判定を持つブロックとの衝突応答
void Player::BlockHalfHit(const Vector2& p, const Vector2& v, int bw, int bh) {
	float bx = p.x, by = p.y;
	if (Rect(pos.x - width / 2 + 12, pos.y + height / 2, pos.x + width / 2 - 12, pos.y + height / 2).
		CheckHit(Rect(bx - bw / 2, by - bh / 2, bx + bw / 2, by - bh / 4)) && vel.y >= 0) {
		if (!(hitmap & HIT_BOTTOM)){
			pos.y = by - bh / 2.f - height / 2.f;
			jump = 2;
			vel.y = 4.f;
			pos.x += v.x;
			hitmap |= HIT_TOP;
		}
	}
}

bool Player::IsShift() const {
	return state == SHIFT_JUMP || state == SHIFT_MOVE;
}

BulletType Player::GetBulletType() const {
	return bulType;
}
int Player::GetMP() const {
	return mp;
}
int Player::GetMPMax() const {
	return mpmax;
}
std::shared_ptr<JoypadInput> Player::GetInput() const {
	return input;
}
bool Player::CanPickItem() const {
	return itemCnt == 0;
}
bool Player::IsShooting() const {
	return input->Get(INPUT_Z) > 0 && state != DAMAGE && state != DASH;
}

void Player::SetMP(int mp) {
	this->mp = mp;
}
void Player::AddMP(int m) {
	mp += m;
	if (mp > mpmax) mp = mpmax;
}
void Player::SetIgnoreCamera(bool ic)
{
	ignoreCamera = ic;
}

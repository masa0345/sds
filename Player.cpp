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

	//���@�̓����蔻��T�C�Y
	Rect playerhit[] = {
		{ -24,-48, 24, 48 }, //�����@�U��
		{ -20,-38, 20, 48 }, //�ᑬ
		{ -40,  8, 40, 48 }, //�_�b�V��
		{ -24,-38, 24, 48 }, //�W�����v
		{ -30,-38, 30, 48 }, //����
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
	// �d��
	if (vel.y < 8.f) vel.y += state == DAMAGE ? 0.4f : 0.8f;
	else vel.y = 8.f;

	if (vel.y < -16.f) vel.y = -13.f;

	if (hitmap & (HIT_SLOPE | HIT_TOP) && jump == 2) {
		vel.y += (fabs(vel.x) + 1) / 2;
	}
	if (jump) jump = 1;

	// ���G����
	if (invincible > 0) {
		img->drawflag = invincible % 2 == 1;
		--invincible;
	}
	// �A�C�e���擾�N�[��
	if (itemCnt > 0) {
		--itemCnt;
	}
	// �_�b�V�����c��
	if (state == DASH || state == DASH_JUMP) {
		if (stateCnt % 2) Create(std::make_shared<EffectAfterImage2>(pos, img, dir));
	}
	// ���S
	if (hp <= 0) {
		hp = 0;
		exist = false;
		Create(std::make_shared<EffectPlayerDeath>(pos));
		Sound::Instance()->Play("���S");
	}
	// ��ʊO�␳
	if (!ignoreCamera) {
		Rect cf = stage->GetCamera()->GetField();
		if (cf.left > pos.x - width / 2) pos.x = (float)(cf.left + width / 2);
		else if (cf.right < pos.x + width / 2) pos.x = (float)(cf.right - width / 2);
		// ������
		if (cf.bottom + 120 < pos.y) hp = 0;
	}
	// �����蔻��X�V
	dmgHitbox->SetRect(0, playerhit[img->num / 6]);

	if (input->Get(INPUT_D) == 1) {
		auto o = Create(std::make_shared<ItemWeapon>(pos, GEAR));
		o->SetPos({ pos.x, pos.y - 100.f });
		//stage->SetLoadNext(true);
		//Create(std::make_shared<EffectMExplode>(pos));
	}
}

void Player::UpdateColResponse() {
	//�}�b�v�����蔻��
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
	Sound::Instance()->Play("���炤");
}

bool Player::PickItem(int type, int itemmp) {
	int s = input->Get(INPUT_S);
	if (s == 0 || itemCnt > 0) return false;
	
	if (bulType == type && type < STAR_AD) {
		// ������������Ď擾
		bulType = (BulletType)(type + STAR_AD);
		mp = (itemmp + mp) / 2;
		Sound::Instance()->Play("�E��2");
	} else if (bulType == type + STAR_AD) {
		// �g�p������̋����O���擾
		AddMP(itemmp / 4);
		Sound::Instance()->Play("�E��2");
	} else if (bulType == type && type >= STAR_AD) {
		// ��������g�p���ɓ�������擾
		AddMP(itemmp);
		Sound::Instance()->Play("�E��2");
	} else {
		// �ʂ̕�����擾
		bulType = (BulletType)type;
		mp = itemmp;
		Sound::Instance()->Play("�E��");
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
			// �ړ�
			dir = r ? RIGHT : LEFT;
			vel.x = 3.f;
			// �ړ��A�j���[�V����
			img->num = 24 + (stateCnt / 8) % 6;
			if (atk) {
				img->num = 36 + (stateCnt / 8) % 6;
				++animCnt;
			}
		} else {
			// ����
			vel.x = 0.f;
			img->num = 1;
			if (atk) {
				img->num = ++animCnt < 9 ? 3 + (animCnt / 3) % 3 : 5;
			} else {
				animCnt = 0;
			}
		}
		// �_�b�V���ֈڍs
		if (dash == 1) {
			vel.x = 7.f;
			state = DASH;
			stateCnt = animCnt = 0;
			Create(std::make_shared<EffectPlayerDash>(pos, dir));
			Sound::Instance()->Play("�_�b�V��");
			break;
		}
		// �W�����v�ֈڍs
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
		// �����Œ胂�[�h�ֈȍ~
		if (shift) {
			state = SHIFT_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		break;
	case SHIFT_MOVE: // �����Œ�
		// �ړ�
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
			// ����
			vel.x = 0.f;
			img->num = atk ? 30 : 6;
		}
		// �W�����v�ֈڍs
		if (jmp == 1 && jump) {
			vel.y = -11.f;
			--jump;
			state = SHIFT_JUMP;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("�W�����v");
			break;
		} else if (jump != 2) {
			state = SHIFT_JUMP;
			stateCnt = animCnt = 0;
			vel.y = 1.f;
			break;
		}
		// �����ֈڍs
		if (!shift) {
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
		}
		break;
	case DASH: // �_�b�V��
		if (stateCnt >= 22) {
			vel.x = 0.f;
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// �_�b�V���W�����v�ֈڍs
		if (jmp == 1 && jump) {
			vel.y = -15;
			--jump;
			state = DASH_JUMP;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("�W�����v");
			break;
		} else if (jump != 2) {
			state = DASH_JUMP;
			stateCnt = animCnt = 0;
			vel.y = 1.f;
		}
		// �_�b�V���A�j���[�V����
		if (++animCnt < 12) img->num = 12 + (animCnt / 6) % 2;
		else img->num = 14 + (animCnt / 4) % 2;
		break;
	case JUMP: case DASH_JUMP: // �W�����v
		if (jump == 2) {
			// ���n
			state = STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// �󒆂ł̕����]��
		if ((l && !r) || (!l && r)) {
			dir = r ? RIGHT : LEFT;
			vel.x = state == JUMP ? 3.f : 5.f;
		} else {
			vel.x = 0.f;
		}
		// �󒆃W�����v
		if (jmp == 1 && jump) {
			vel.y = -13.f;
			--jump;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("�W�����v");
			break;
		}
		if (shift) state = SHIFT_JUMP;
		// �W�����v�A�j���[�V����
		++animCnt;
		if (animCnt < 3) img->num = 18;
		else if (animCnt < 12) img->num = 19;
		else img->num = vel.y <= 0 ? 20 : 21;
		if (vel.y > 0) img->num = 21;
		if (atk) img->num = 22;
		break;
	case SHIFT_JUMP:
		if (jump == 2) {
			// ���n
			state = shift ? SHIFT_MOVE : STAND_MOVE;
			stateCnt = animCnt = 0;
			break;
		}
		// �󒆂ł̕����]��
		if (l && !r) {
			vel.x = dir == LEFT ? 2.1f : -2.1f;
		} else if(!l && r) {
			vel.x = dir == RIGHT ? 2.1f : -2.1f;
		} else {
			vel.x = 0.f;
		}
		// �󒆃W�����v
		if (jmp == 1 && jump) {
			vel.y = -13.f;
			--jump;
			stateCnt = animCnt = 0;
			Sound::Instance()->Play("�W�����v");
			break;
		}
		if (!shift) state = JUMP;
		// �W�����v�A�j���[�V����
		++animCnt;
		if (animCnt < 3) img->num = 18;
		else if (animCnt < 12) img->num = 19;
		else img->num = vel.y <= 0 ? 20 : 21;
		if (vel.y > 0) img->num = 21;
		if (atk) img->num = 22;
		break;
	case DAMAGE: // ��e
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

	// �A�C�e�����̂Ă�
	if (bulType != STAR && input->Get(INPUT_A) == 1) {
		Create(std::make_shared<ItemWeapon>(pos, bulType, mp));
		bulType = STAR;
		mp = mpmax;
		Sound::Instance()->Play("�̂Ă�");
	}

	// �e����
	BulletParam param = PlayerBullet::GetBulletParam(bulType);
	if (atk > 3 && state != DASH && state != DAMAGE) {
		++fireCnt;
		//delay��4�ȉ��Ȃ�firecnt��3�̂Ƃ��A����ȊO��firecnt % delay
		if ((param.delay < 4) ? fireCnt == 3 : fireCnt % param.delay == 3) {
			//mp������Ă���΃V���b�g������
			if (mp >= param.cost) {
				PlayerBullet::Fire(*this, bulType);
				mp -= param.cost;
			} else {
				//�e�؂�̂Ƃ�
				Sound::Instance()->Play("�e�؂�");
				Create(std::make_shared<EffectNoMP>(pos));
			}
		}
	} else if (atk == 0 && fireCnt>0) {
		//�U���{�^����������Ă��Ȃ���firecnt��0�łȂ�
		if (fireCnt % param.delay == 2) {
			fireCnt = 0;
		} else {
			fireCnt++;
		}
	}
	if (mp < 0) mp = 0;
}

// �u���b�N�Ƃ̏Փˉ���
void Player::BlockHit(const Vector2& p, const Vector2& v, int bw, int bh) {
	int w = width, h = height;
	float bx = p.x, by = p.y, bvx = v.x, bvy = v.y;
	//���E
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
	//�㉺
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
// �㑤�̂ݔ�������u���b�N�Ƃ̏Փˉ���
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

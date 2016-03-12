#include "Camera.h"
#include "common.h"
#include "GameEntity.h"
#include <cmath>


Camera::Camera() {
	//field = { 0,0,1200,600 };
}

void Camera::Update() {
	// スクロール
	Vector2 tp = target->GetPos();
	int tw = target->GetWidth(), th = target->GetHeight();
	const int SCX = 250, SCY = 120;
	if (tp.x + tw / 2 - pos.x > WINDOW_WIDTH - SCX) {
		pos.x = tp.x + tw / 2 - (WINDOW_WIDTH - SCX);
	} else if (tp.x - tw / 2 - pos.x<SCX) {
		pos.x = tp.x -tw / 2 - SCX;
	}
	if (tp.y + th / 2 - pos.y > WINDOW_HEIGHT - SCY) {
		pos.y = tp.y + th / 2 - (WINDOW_HEIGHT - SCY);
	} else if (tp.y - th / 2 - pos.y < SCY) {
		pos.y = tp.y - th / 2 - SCY;
	}

	//端まできたらスクロールしない
	if (pos.x < field.left)  pos.x = (float)field.left;
	if (pos.x > field.right - WINDOW_WIDTH) pos.x = (float)field.right - WINDOW_WIDTH;
	if (pos.y < field.top)  pos.y = (float)field.top;
	if (pos.y > field.bottom - WINDOW_HEIGHT) pos.y = (float)field.bottom - WINDOW_HEIGHT;
	

	// 振動
	float f;
	shakeYVal = 0.f;
	for (auto it = shake.begin(); it != shake.end(); ) {
		if ((*it)->GetShake(&f)) {
			shakeYVal += f;
			++it;
		}
		else {
			delete *it;
			it = shake.erase(it);
		}
	}
}

Vector2 Camera::TransCoord(const Vector2& p, float speed) const {
	return{ p.x - pos.x*speed, p.y + shakeYVal - pos.y*speed };
}

Vector2 Camera::TransCoordBackGround(const Vector2& sc) const {
	return{ -pos.x*sc.x, -pos.y*sc.y };
}

Rect Camera::GetVisibleRect(int mmx, int mmy) const {
	Rect r;
	r.left = (int)pos.x / MW - 1;
	r.top = (int)(pos.y + shakeYVal) / MH - 1;
	if (r.left < 0) r.left = 0;
	if (r.top < 0) r.right = 0;
	r.right = (r.left + WINDOW_WIDTH / MW + 3);
	r.bottom = (r.top + WINDOW_HEIGHT / MH + 3);
	if (r.right > mmx) r.right = mmx;
	if (r.bottom > mmy) r.bottom = mmy;
	return r;
}

Vector2 Camera::GetPos() const
{
	return pos;
}

Rect Camera::GetField() const
{
	return field;
}

void Camera::SetShake(float ampl, float freq, int duration, int damp) {
	shake.push_back(new Shake(ampl, freq, duration, damp));
}

void Camera::SetField(const Rect& f) {
	field = f;
}

void Camera::SetTarget(std::shared_ptr<GameEntity> t) {
	target = t;
}

void Camera::SetPos(const Vector2& p) {
	pos = p;
}




Shake::Shake(float ampl, float freq, int duration, int damp) :
	count(0), duration(duration), damping(damp), ampl(ampl), freq(freq)
{
}

bool Shake::GetShake(float* f)
{
	++count;
	if (count < duration) {
		// 減衰無し
		*f = ampl * sinf(count * freq * PI / 180.f);
		return true;
	} else if (count < duration + damping) {
		// 減衰あり
		float t = static_cast<float>(duration + damping - count) / damping;
		*f = ampl * t * t * sinf(count * freq * PI / 180.f);
		return true;
	}

	// 振動無し
	*f = 0.f;
	return false;
}
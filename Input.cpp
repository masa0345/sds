#include "Input.h"
#include <DxLib.h>
#include <cassert>
//#include "debug.h"

namespace
{

	// キーコードと文字列の対応
	std::unordered_map<int, std::string> keystr;
	// コマンド維持時間
	const int BUFFER_TIME = 20;
	// キーコード配列のサイズ
	const int KEY_CODE_NUM = 256;

	// vが2のn乗のときのnを得る
	int intlog2(unsigned v)
	{
		int r = (v & 0xaaaaaaaa) != 0;
		r |= ((v & 0xffff0000) != 0) << 4;
		r |= ((v & 0xff00ff00) != 0) << 3;
		r |= ((v & 0xf0f0f0f0) != 0) << 2;
		r |= ((v & 0xcccccccc) != 0) << 1;
		return r;
	}

	// keystr初期化
	void SetKeyStringMap()
	{
		using namespace std;
		keystr.insert(make_pair(KEY_INPUT_BACK, "BackSpace"));	// バックスペースキー
		keystr.insert(make_pair(KEY_INPUT_TAB, "Tab"));	// タブキー
		keystr.insert(make_pair(KEY_INPUT_RETURN, "Enter"));	// エンターキー

		keystr.insert(make_pair(KEY_INPUT_LSHIFT, "LShift"));	// 左シフトキー
		keystr.insert(make_pair(KEY_INPUT_RSHIFT, "RShift"));	// 右シフトキー
		keystr.insert(make_pair(KEY_INPUT_LCONTROL, "LControl"));	// 左コントロールキー
		keystr.insert(make_pair(KEY_INPUT_RCONTROL, "RControl"));	// 右コントロールキー
		keystr.insert(make_pair(KEY_INPUT_ESCAPE, "Esc"));	// エスケープキー
		keystr.insert(make_pair(KEY_INPUT_SPACE, "Space"));	// スペースキー
		keystr.insert(make_pair(KEY_INPUT_PGUP, "PgUp"));	// ＰａｇｅＵＰキー
		keystr.insert(make_pair(KEY_INPUT_PGDN, "PgDn"));	// ＰａｇｅＤｏｗｎキー
		keystr.insert(make_pair(KEY_INPUT_END, "End"));	// エンドキー
		keystr.insert(make_pair(KEY_INPUT_HOME, "Home"));	// ホームキー
		keystr.insert(make_pair(KEY_INPUT_LEFT, "Left"));	// 左キー
		keystr.insert(make_pair(KEY_INPUT_UP, "Up"));	// 上キー
		keystr.insert(make_pair(KEY_INPUT_RIGHT, "Right"));	// 右キー
		keystr.insert(make_pair(KEY_INPUT_DOWN, "Down"));	// 下キー
		keystr.insert(make_pair(KEY_INPUT_INSERT, "Insert"));	// インサートキー
		keystr.insert(make_pair(KEY_INPUT_DELETE, "Delete"));	// デリートキー

		keystr.insert(make_pair(KEY_INPUT_MINUS, "-"));	// －キー
		keystr.insert(make_pair(KEY_INPUT_YEN, "\\"));	// ￥キー
		keystr.insert(make_pair(KEY_INPUT_PREVTRACK, "^"));	// ＾キー
		keystr.insert(make_pair(KEY_INPUT_PERIOD, "."));	// ．キー
		keystr.insert(make_pair(KEY_INPUT_SLASH, "/"));	// ／キー
		keystr.insert(make_pair(KEY_INPUT_LALT, "LAlt"));	// 左ＡＬＴキー
		keystr.insert(make_pair(KEY_INPUT_RALT, "RAlt"));	// 右ＡＬＴキー
		keystr.insert(make_pair(KEY_INPUT_SCROLL, "ScrollLock"));	// ScrollLockキー
		keystr.insert(make_pair(KEY_INPUT_SEMICOLON, ";"));	// ；キー
		keystr.insert(make_pair(KEY_INPUT_COLON, ":"));	// ：キー
		keystr.insert(make_pair(KEY_INPUT_LBRACKET, "["));	// ［キー
		keystr.insert(make_pair(KEY_INPUT_RBRACKET, "]"));	// ］キー
		keystr.insert(make_pair(KEY_INPUT_AT, "@"));	// ＠キー
		keystr.insert(make_pair(KEY_INPUT_BACKSLASH, "\\"));	// ＼キー
		keystr.insert(make_pair(KEY_INPUT_COMMA, ","));	// ，キー
		keystr.insert(make_pair(KEY_INPUT_KANJI, "KANJI"));	// 漢字キー
		keystr.insert(make_pair(KEY_INPUT_CONVERT, "CONVERT"));	// 変換キー
		keystr.insert(make_pair(KEY_INPUT_NOCONVERT, "NOCONVERT"));	// 無変換キー
		keystr.insert(make_pair(KEY_INPUT_KANA, "KANA"));	// カナキー
		keystr.insert(make_pair(KEY_INPUT_APPS, "APPS"));	// アプリケーションメニューキー
		keystr.insert(make_pair(KEY_INPUT_CAPSLOCK, "CaspLock"));	// CaspLockキー
		keystr.insert(make_pair(KEY_INPUT_SYSRQ, "PrintScreen"));	// PrintScreenキー
		keystr.insert(make_pair(KEY_INPUT_PAUSE, "PauseBreak"));	// PauseBreakキー
		keystr.insert(make_pair(KEY_INPUT_LWIN, "LWin"));	// 左Ｗｉｎキー
		keystr.insert(make_pair(KEY_INPUT_RWIN, "RWin"));	// 右Ｗｉｎキー

		keystr.insert(make_pair(KEY_INPUT_NUMLOCK, "NumLock"));	// NumLockキー
		keystr.insert(make_pair(KEY_INPUT_NUMPAD0, "Num0"));	// テンキー０
		keystr.insert(make_pair(KEY_INPUT_NUMPAD1, "Num1"));	// テンキー１
		keystr.insert(make_pair(KEY_INPUT_NUMPAD2, "Num2"));	// テンキー２
		keystr.insert(make_pair(KEY_INPUT_NUMPAD3, "Num3"));	// テンキー３
		keystr.insert(make_pair(KEY_INPUT_NUMPAD4, "Num4"));	// テンキー４
		keystr.insert(make_pair(KEY_INPUT_NUMPAD5, "Num5"));	// テンキー５
		keystr.insert(make_pair(KEY_INPUT_NUMPAD6, "Num6"));	// テンキー６
		keystr.insert(make_pair(KEY_INPUT_NUMPAD7, "Num7"));	// テンキー７
		keystr.insert(make_pair(KEY_INPUT_NUMPAD8, "Num8"));	// テンキー８
		keystr.insert(make_pair(KEY_INPUT_NUMPAD9, "Num9"));	// テンキー９
		keystr.insert(make_pair(KEY_INPUT_MULTIPLY, "*"));	// テンキー＊キー
		keystr.insert(make_pair(KEY_INPUT_ADD, "+"));	// テンキー＋キー
		keystr.insert(make_pair(KEY_INPUT_SUBTRACT, "-"));	// テンキー－キー
		keystr.insert(make_pair(KEY_INPUT_DECIMAL, "."));	// テンキー．キー
		keystr.insert(make_pair(KEY_INPUT_DIVIDE, "/"));	// テンキー／キー
		keystr.insert(make_pair(KEY_INPUT_NUMPADENTER, "Enter"));	// テンキーのエンターキー

		keystr.insert(make_pair(KEY_INPUT_F1, "F1"));	// Ｆ１キー
		keystr.insert(make_pair(KEY_INPUT_F2, "F2"));	// Ｆ２キー
		keystr.insert(make_pair(KEY_INPUT_F3, "F3"));	// Ｆ３キー
		keystr.insert(make_pair(KEY_INPUT_F4, "F4"));	// Ｆ４キー
		keystr.insert(make_pair(KEY_INPUT_F5, "F5"));	// Ｆ５キー
		keystr.insert(make_pair(KEY_INPUT_F6, "F6"));	// Ｆ６キー
		keystr.insert(make_pair(KEY_INPUT_F7, "F7"));	// Ｆ７キー
		keystr.insert(make_pair(KEY_INPUT_F8, "F8"));	// Ｆ８キー
		keystr.insert(make_pair(KEY_INPUT_F9, "F9"));	// Ｆ９キー
		keystr.insert(make_pair(KEY_INPUT_F10, "F10"));	// Ｆ１０キー
		keystr.insert(make_pair(KEY_INPUT_F11, "F11"));	// Ｆ１１キー
		keystr.insert(make_pair(KEY_INPUT_F12, "F12"));	// Ｆ１２キー

		keystr.insert(make_pair(KEY_INPUT_A, "A"));	// Ａキー
		keystr.insert(make_pair(KEY_INPUT_B, "B"));	// Ｂキー
		keystr.insert(make_pair(KEY_INPUT_C, "C"));	// Ｃキー
		keystr.insert(make_pair(KEY_INPUT_D, "D"));	// Ｄキー
		keystr.insert(make_pair(KEY_INPUT_E, "E"));	// Ｅキー
		keystr.insert(make_pair(KEY_INPUT_F, "F"));	// Ｆキー
		keystr.insert(make_pair(KEY_INPUT_G, "G"));	// Ｇキー
		keystr.insert(make_pair(KEY_INPUT_H, "H"));	// Ｈキー
		keystr.insert(make_pair(KEY_INPUT_I, "I"));	// Ｉキー
		keystr.insert(make_pair(KEY_INPUT_J, "J"));	// Ｊキー
		keystr.insert(make_pair(KEY_INPUT_K, "K"));	// Ｋキー
		keystr.insert(make_pair(KEY_INPUT_L, "L"));	// Ｌキー
		keystr.insert(make_pair(KEY_INPUT_M, "M"));	// Ｍキー
		keystr.insert(make_pair(KEY_INPUT_N, "N"));	// Ｎキー
		keystr.insert(make_pair(KEY_INPUT_O, "O"));	// Ｏキー
		keystr.insert(make_pair(KEY_INPUT_P, "P"));	// Ｐキー
		keystr.insert(make_pair(KEY_INPUT_Q, "Q"));	// Ｑキー
		keystr.insert(make_pair(KEY_INPUT_R, "R"));	// Ｒキー
		keystr.insert(make_pair(KEY_INPUT_S, "S"));	// Ｓキー
		keystr.insert(make_pair(KEY_INPUT_T, "T"));	// Ｔキー
		keystr.insert(make_pair(KEY_INPUT_U, "U"));	// Ｕキー
		keystr.insert(make_pair(KEY_INPUT_V, "V"));	// Ｖキー
		keystr.insert(make_pair(KEY_INPUT_W, "W"));	// Ｗキー
		keystr.insert(make_pair(KEY_INPUT_X, "X"));	// Ｘキー
		keystr.insert(make_pair(KEY_INPUT_Y, "Y"));	// Ｙキー
		keystr.insert(make_pair(KEY_INPUT_Z, "Z"));	// Ｚキー

		keystr.insert(make_pair(KEY_INPUT_0, "0"));	// ０キー
		keystr.insert(make_pair(KEY_INPUT_1, "1"));	// １キー
		keystr.insert(make_pair(KEY_INPUT_2, "2"));	// ２キー
		keystr.insert(make_pair(KEY_INPUT_3, "3"));	// ３キー
		keystr.insert(make_pair(KEY_INPUT_4, "4"));	// ４キー
		keystr.insert(make_pair(KEY_INPUT_5, "5"));	// ５キー
		keystr.insert(make_pair(KEY_INPUT_6, "6"));	// ６キー
		keystr.insert(make_pair(KEY_INPUT_7, "7"));	// ７キー
		keystr.insert(make_pair(KEY_INPUT_8, "8"));	// ８キー
		keystr.insert(make_pair(KEY_INPUT_9, "9"));	// ９キー	
	}

}

JoypadInput::JoypadInput(int padnum) :
	padnum(padnum), inputCnt(0), bufferCnt(0)
{
	state.fill(0);
	SetPadKey(INPUT_DOWN, PAD_INPUT_DOWN, KEY_INPUT_DOWN);
	SetPadKey(INPUT_LEFT, PAD_INPUT_LEFT, KEY_INPUT_LEFT);
	SetPadKey(INPUT_RIGHT, PAD_INPUT_RIGHT, KEY_INPUT_RIGHT);
	SetPadKey(INPUT_UP, PAD_INPUT_UP, KEY_INPUT_UP);
	SetPadKey(INPUT_Z, PAD_INPUT_1, KEY_INPUT_Z);
	SetPadKey(INPUT_X, PAD_INPUT_2, KEY_INPUT_X);
	SetPadKey(INPUT_C, PAD_INPUT_3, KEY_INPUT_C);
	SetPadKey(INPUT_A, PAD_INPUT_4, KEY_INPUT_A);
	SetPadKey(INPUT_S, PAD_INPUT_5, KEY_INPUT_S);
	SetPadKey(INPUT_D, PAD_INPUT_6, KEY_INPUT_D);
	SetPadKey(INPUT_P, PAD_INPUT_7, KEY_INPUT_P);
	SetPadKey(INPUT_ESC, PAD_INPUT_9, KEY_INPUT_ESCAPE);

	if (keystr.empty()) SetKeyStringMap();
}

JoypadInput::~JoypadInput()
{
}

void JoypadInput::Update()
{
	auto st = GetJoypadInputState(padnum);
	for (int i = 0; i < 32; ++i, st >>= 1) {
		if (st & 1) // 押されている
			++state[i];
		else if (state[i] > 0) // 離された
			state[i] = -1;
		else // 押されていない
			state[i] = 0;
	}

	// コマンド入力履歴更新
	if (!buffer.empty() && ++inputCnt && ++bufferCnt > BUFFER_TIME) {
		buffer.clear();
		inputCnt = 0;
	}
	for (int i = 0; i < BUTTON_NUM; ++i) {
		if (state[padmap[i]] == 1) {
			if (i < INPUT_Z) {
				buffer.push_back(padmap[i]);
				bufferCnt = 0;
			}
			else {
				bufferCnt = BUFFER_TIME;
			}
			break;
		}
	}

#ifdef DEBUG_DRAW
	int i = 0;
	int y = WINDOW_HEIGHT - 16;
	for (auto it = buffer.begin(); it != buffer.end(); ++i, ++it) {
		if (*it == padmap[INPUT_LEFT]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "←");
		}
		else if (*it == padmap[INPUT_RIGHT]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "→");
		}
		else if (*it == padmap[INPUT_UP]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "↑");
		}
		else if (*it == padmap[INPUT_DOWN]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "↓");
		}
		else if (*it == padmap[INPUT_Z]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "Z");
		}
		else if (*it == padmap[INPUT_X]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "X");
		}
		else if (*it == padmap[INPUT_C]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "C");
		}
	}
	DebugDraw::String(Vector2(64, y), 0xffffff, "%d", inputCnt);
#endif
}

int JoypadInput::Get(Button pad) const
{
	return state[padmap[pad]];
}

int JoypadInput::GetPadNum() const
{
	return padnum;
}

void JoypadInput::SetPad(Button button, int pad)
{
	padmap[button] = intlog2(pad);
	SetJoypadInputToKeyInput(padnum, pad, keymap[button]);
}

void JoypadInput::SetKey(Button button, int key)
{
	keymap[button] = key;
	SetJoypadInputToKeyInput(padnum, 1 << padmap[button], key);
}

void JoypadInput::SetPadKey(Button button, int pad, int key)
{
	padmap[button] = intlog2(pad);
	keymap[button] = key;
	SetJoypadInputToKeyInput(padnum, pad, key);
}

int JoypadInput::GetInputKey() const
{
	char key[KEY_CODE_NUM];
	GetHitKeyStateAll(key);
	for (int i = 0; i < KEY_CODE_NUM; ++i)
		if (key[i]) return i;
	return -1;
}

const std::string& JoypadInput::GetKeyString(int key) const
{
	return keystr.at(key);
}

// コマンドチェック
bool JoypadInput::CheckCommand(int cmd, Button trigger, bool dir, int time)
{
	if (Get(trigger) != 1) return false;
	if (inputCnt > time) {
		buffer.clear();
		inputCnt = 0;
		return false;
	}
	int c;
	for (auto it = buffer.rbegin(); it != buffer.rend(); ++it) {
		c = cmd % 10;
		switch (c) {
		case 2:
			if (padmap[INPUT_DOWN] != *it) return false;
			break;
		case 4:
			if (padmap[dir ? INPUT_RIGHT : INPUT_LEFT] != *it) return false;
			break;
		case 6:
			if (padmap[dir ? INPUT_LEFT : INPUT_RIGHT] != *it) return false;
			break;
		case 8:
			if (padmap[INPUT_UP] != *it) return false;
			break;
		default:
			return false;
		}
		cmd /= 10;
		if (cmd == 0) return true;
	}
	return false;
}
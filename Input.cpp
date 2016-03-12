#include "Input.h"
#include <DxLib.h>
#include <cassert>
//#include "debug.h"

namespace
{

	// �L�[�R�[�h�ƕ�����̑Ή�
	std::unordered_map<int, std::string> keystr;
	// �R�}���h�ێ�����
	const int BUFFER_TIME = 20;
	// �L�[�R�[�h�z��̃T�C�Y
	const int KEY_CODE_NUM = 256;

	// v��2��n��̂Ƃ���n�𓾂�
	int intlog2(unsigned v)
	{
		int r = (v & 0xaaaaaaaa) != 0;
		r |= ((v & 0xffff0000) != 0) << 4;
		r |= ((v & 0xff00ff00) != 0) << 3;
		r |= ((v & 0xf0f0f0f0) != 0) << 2;
		r |= ((v & 0xcccccccc) != 0) << 1;
		return r;
	}

	// keystr������
	void SetKeyStringMap()
	{
		using namespace std;
		keystr.insert(make_pair(KEY_INPUT_BACK, "BackSpace"));	// �o�b�N�X�y�[�X�L�[
		keystr.insert(make_pair(KEY_INPUT_TAB, "Tab"));	// �^�u�L�[
		keystr.insert(make_pair(KEY_INPUT_RETURN, "Enter"));	// �G���^�[�L�[

		keystr.insert(make_pair(KEY_INPUT_LSHIFT, "LShift"));	// ���V�t�g�L�[
		keystr.insert(make_pair(KEY_INPUT_RSHIFT, "RShift"));	// �E�V�t�g�L�[
		keystr.insert(make_pair(KEY_INPUT_LCONTROL, "LControl"));	// ���R���g���[���L�[
		keystr.insert(make_pair(KEY_INPUT_RCONTROL, "RControl"));	// �E�R���g���[���L�[
		keystr.insert(make_pair(KEY_INPUT_ESCAPE, "Esc"));	// �G�X�P�[�v�L�[
		keystr.insert(make_pair(KEY_INPUT_SPACE, "Space"));	// �X�y�[�X�L�[
		keystr.insert(make_pair(KEY_INPUT_PGUP, "PgUp"));	// �o�������t�o�L�[
		keystr.insert(make_pair(KEY_INPUT_PGDN, "PgDn"));	// �o�������c�������L�[
		keystr.insert(make_pair(KEY_INPUT_END, "End"));	// �G���h�L�[
		keystr.insert(make_pair(KEY_INPUT_HOME, "Home"));	// �z�[���L�[
		keystr.insert(make_pair(KEY_INPUT_LEFT, "Left"));	// ���L�[
		keystr.insert(make_pair(KEY_INPUT_UP, "Up"));	// ��L�[
		keystr.insert(make_pair(KEY_INPUT_RIGHT, "Right"));	// �E�L�[
		keystr.insert(make_pair(KEY_INPUT_DOWN, "Down"));	// ���L�[
		keystr.insert(make_pair(KEY_INPUT_INSERT, "Insert"));	// �C���T�[�g�L�[
		keystr.insert(make_pair(KEY_INPUT_DELETE, "Delete"));	// �f���[�g�L�[

		keystr.insert(make_pair(KEY_INPUT_MINUS, "-"));	// �|�L�[
		keystr.insert(make_pair(KEY_INPUT_YEN, "\\"));	// ���L�[
		keystr.insert(make_pair(KEY_INPUT_PREVTRACK, "^"));	// �O�L�[
		keystr.insert(make_pair(KEY_INPUT_PERIOD, "."));	// �D�L�[
		keystr.insert(make_pair(KEY_INPUT_SLASH, "/"));	// �^�L�[
		keystr.insert(make_pair(KEY_INPUT_LALT, "LAlt"));	// ���`�k�s�L�[
		keystr.insert(make_pair(KEY_INPUT_RALT, "RAlt"));	// �E�`�k�s�L�[
		keystr.insert(make_pair(KEY_INPUT_SCROLL, "ScrollLock"));	// ScrollLock�L�[
		keystr.insert(make_pair(KEY_INPUT_SEMICOLON, ";"));	// �G�L�[
		keystr.insert(make_pair(KEY_INPUT_COLON, ":"));	// �F�L�[
		keystr.insert(make_pair(KEY_INPUT_LBRACKET, "["));	// �m�L�[
		keystr.insert(make_pair(KEY_INPUT_RBRACKET, "]"));	// �n�L�[
		keystr.insert(make_pair(KEY_INPUT_AT, "@"));	// ���L�[
		keystr.insert(make_pair(KEY_INPUT_BACKSLASH, "\\"));	// �_�L�[
		keystr.insert(make_pair(KEY_INPUT_COMMA, ","));	// �C�L�[
		keystr.insert(make_pair(KEY_INPUT_KANJI, "KANJI"));	// �����L�[
		keystr.insert(make_pair(KEY_INPUT_CONVERT, "CONVERT"));	// �ϊ��L�[
		keystr.insert(make_pair(KEY_INPUT_NOCONVERT, "NOCONVERT"));	// ���ϊ��L�[
		keystr.insert(make_pair(KEY_INPUT_KANA, "KANA"));	// �J�i�L�[
		keystr.insert(make_pair(KEY_INPUT_APPS, "APPS"));	// �A�v���P�[�V�������j���[�L�[
		keystr.insert(make_pair(KEY_INPUT_CAPSLOCK, "CaspLock"));	// CaspLock�L�[
		keystr.insert(make_pair(KEY_INPUT_SYSRQ, "PrintScreen"));	// PrintScreen�L�[
		keystr.insert(make_pair(KEY_INPUT_PAUSE, "PauseBreak"));	// PauseBreak�L�[
		keystr.insert(make_pair(KEY_INPUT_LWIN, "LWin"));	// ���v�����L�[
		keystr.insert(make_pair(KEY_INPUT_RWIN, "RWin"));	// �E�v�����L�[

		keystr.insert(make_pair(KEY_INPUT_NUMLOCK, "NumLock"));	// NumLock�L�[
		keystr.insert(make_pair(KEY_INPUT_NUMPAD0, "Num0"));	// �e���L�[�O
		keystr.insert(make_pair(KEY_INPUT_NUMPAD1, "Num1"));	// �e���L�[�P
		keystr.insert(make_pair(KEY_INPUT_NUMPAD2, "Num2"));	// �e���L�[�Q
		keystr.insert(make_pair(KEY_INPUT_NUMPAD3, "Num3"));	// �e���L�[�R
		keystr.insert(make_pair(KEY_INPUT_NUMPAD4, "Num4"));	// �e���L�[�S
		keystr.insert(make_pair(KEY_INPUT_NUMPAD5, "Num5"));	// �e���L�[�T
		keystr.insert(make_pair(KEY_INPUT_NUMPAD6, "Num6"));	// �e���L�[�U
		keystr.insert(make_pair(KEY_INPUT_NUMPAD7, "Num7"));	// �e���L�[�V
		keystr.insert(make_pair(KEY_INPUT_NUMPAD8, "Num8"));	// �e���L�[�W
		keystr.insert(make_pair(KEY_INPUT_NUMPAD9, "Num9"));	// �e���L�[�X
		keystr.insert(make_pair(KEY_INPUT_MULTIPLY, "*"));	// �e���L�[���L�[
		keystr.insert(make_pair(KEY_INPUT_ADD, "+"));	// �e���L�[�{�L�[
		keystr.insert(make_pair(KEY_INPUT_SUBTRACT, "-"));	// �e���L�[�|�L�[
		keystr.insert(make_pair(KEY_INPUT_DECIMAL, "."));	// �e���L�[�D�L�[
		keystr.insert(make_pair(KEY_INPUT_DIVIDE, "/"));	// �e���L�[�^�L�[
		keystr.insert(make_pair(KEY_INPUT_NUMPADENTER, "Enter"));	// �e���L�[�̃G���^�[�L�[

		keystr.insert(make_pair(KEY_INPUT_F1, "F1"));	// �e�P�L�[
		keystr.insert(make_pair(KEY_INPUT_F2, "F2"));	// �e�Q�L�[
		keystr.insert(make_pair(KEY_INPUT_F3, "F3"));	// �e�R�L�[
		keystr.insert(make_pair(KEY_INPUT_F4, "F4"));	// �e�S�L�[
		keystr.insert(make_pair(KEY_INPUT_F5, "F5"));	// �e�T�L�[
		keystr.insert(make_pair(KEY_INPUT_F6, "F6"));	// �e�U�L�[
		keystr.insert(make_pair(KEY_INPUT_F7, "F7"));	// �e�V�L�[
		keystr.insert(make_pair(KEY_INPUT_F8, "F8"));	// �e�W�L�[
		keystr.insert(make_pair(KEY_INPUT_F9, "F9"));	// �e�X�L�[
		keystr.insert(make_pair(KEY_INPUT_F10, "F10"));	// �e�P�O�L�[
		keystr.insert(make_pair(KEY_INPUT_F11, "F11"));	// �e�P�P�L�[
		keystr.insert(make_pair(KEY_INPUT_F12, "F12"));	// �e�P�Q�L�[

		keystr.insert(make_pair(KEY_INPUT_A, "A"));	// �`�L�[
		keystr.insert(make_pair(KEY_INPUT_B, "B"));	// �a�L�[
		keystr.insert(make_pair(KEY_INPUT_C, "C"));	// �b�L�[
		keystr.insert(make_pair(KEY_INPUT_D, "D"));	// �c�L�[
		keystr.insert(make_pair(KEY_INPUT_E, "E"));	// �d�L�[
		keystr.insert(make_pair(KEY_INPUT_F, "F"));	// �e�L�[
		keystr.insert(make_pair(KEY_INPUT_G, "G"));	// �f�L�[
		keystr.insert(make_pair(KEY_INPUT_H, "H"));	// �g�L�[
		keystr.insert(make_pair(KEY_INPUT_I, "I"));	// �h�L�[
		keystr.insert(make_pair(KEY_INPUT_J, "J"));	// �i�L�[
		keystr.insert(make_pair(KEY_INPUT_K, "K"));	// �j�L�[
		keystr.insert(make_pair(KEY_INPUT_L, "L"));	// �k�L�[
		keystr.insert(make_pair(KEY_INPUT_M, "M"));	// �l�L�[
		keystr.insert(make_pair(KEY_INPUT_N, "N"));	// �m�L�[
		keystr.insert(make_pair(KEY_INPUT_O, "O"));	// �n�L�[
		keystr.insert(make_pair(KEY_INPUT_P, "P"));	// �o�L�[
		keystr.insert(make_pair(KEY_INPUT_Q, "Q"));	// �p�L�[
		keystr.insert(make_pair(KEY_INPUT_R, "R"));	// �q�L�[
		keystr.insert(make_pair(KEY_INPUT_S, "S"));	// �r�L�[
		keystr.insert(make_pair(KEY_INPUT_T, "T"));	// �s�L�[
		keystr.insert(make_pair(KEY_INPUT_U, "U"));	// �t�L�[
		keystr.insert(make_pair(KEY_INPUT_V, "V"));	// �u�L�[
		keystr.insert(make_pair(KEY_INPUT_W, "W"));	// �v�L�[
		keystr.insert(make_pair(KEY_INPUT_X, "X"));	// �w�L�[
		keystr.insert(make_pair(KEY_INPUT_Y, "Y"));	// �x�L�[
		keystr.insert(make_pair(KEY_INPUT_Z, "Z"));	// �y�L�[

		keystr.insert(make_pair(KEY_INPUT_0, "0"));	// �O�L�[
		keystr.insert(make_pair(KEY_INPUT_1, "1"));	// �P�L�[
		keystr.insert(make_pair(KEY_INPUT_2, "2"));	// �Q�L�[
		keystr.insert(make_pair(KEY_INPUT_3, "3"));	// �R�L�[
		keystr.insert(make_pair(KEY_INPUT_4, "4"));	// �S�L�[
		keystr.insert(make_pair(KEY_INPUT_5, "5"));	// �T�L�[
		keystr.insert(make_pair(KEY_INPUT_6, "6"));	// �U�L�[
		keystr.insert(make_pair(KEY_INPUT_7, "7"));	// �V�L�[
		keystr.insert(make_pair(KEY_INPUT_8, "8"));	// �W�L�[
		keystr.insert(make_pair(KEY_INPUT_9, "9"));	// �X�L�[	
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
		if (st & 1) // ������Ă���
			++state[i];
		else if (state[i] > 0) // �����ꂽ
			state[i] = -1;
		else // ������Ă��Ȃ�
			state[i] = 0;
	}

	// �R�}���h���͗����X�V
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
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "��");
		}
		else if (*it == padmap[INPUT_RIGHT]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "��");
		}
		else if (*it == padmap[INPUT_UP]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "��");
		}
		else if (*it == padmap[INPUT_DOWN]) {
			DebugDraw::String(Vector2(0, y - i * 16), 0xffffff, "��");
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

// �R�}���h�`�F�b�N
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
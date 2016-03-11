#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <array>

// �{�^�����
enum Button
{
	INPUT_RIGHT,
	INPUT_LEFT,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_Z,
	INPUT_X,
	INPUT_C,
	INPUT_A,
	INPUT_S,
	INPUT_D,
	INPUT_P,
	INPUT_ESC,

	BUTTON_NUM
};

class JoypadInput
{
public:
	JoypadInput(int padnum = 0x1001);
	~JoypadInput();

	void	Update();
	int		Get(Button pad) const;
	int		GetPadNum() const;

	void	SetPad(Button button, int pad);
	void	SetKey(Button button, int key);
	void	SetPadKey(Button button, int pad, int key);
	int		GetInputKey() const;
	const std::string& GetKeyString(int key) const;

	// �R�}���h���͂��`�F�b�N����
	// cmd�� 2:�� 4:�� 6:�� 8:�� �ɑΉ������ē���
	bool	CheckCommand(int cmd, Button trigger, bool dir, int time = 20);

private:
	std::array<int, 32>	state;
	std::array<char, BUTTON_NUM>	padmap;
	std::array<char, BUTTON_NUM>	keymap;
	int		padnum;
	int		inputCnt;		// �R�}���h���͎��ԃJ�E���^
	int		bufferCnt;		// �o�b�t�@�ێ����ԃJ�E���^
	std::list<int> buffer;	// �J�[�\���L�[�̓��͗���
};


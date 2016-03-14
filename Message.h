#pragma once

#include <list>
#include <string>
#include <memory>

class Font;
class JoypadInput;

class Message
{
public:
	Message(std::shared_ptr<JoypadInput> input, const char* filename);
	~Message();
	bool Display();
	int GetState() const;

private:
	std::list<std::string>	buf;
	std::string			strMessage;
	std::string			strSpeaker;
	std::string			strIcon;

	Font*	msgFont;
	Font*	spkFont;

	int		x, y;			//������̉E����W
	int		width, height;	//������̏c���Ɨ�
	int		frameX, frameY;	//�g�̍��W
	int		frameW, frameH;	//�g�̑傫��

	bool	msgFlag;			//���b�Z�[�W�\�����t���O
	int		charNum;			//���b�Z�[�W�̑�������
	bool	wait;				//�ҋ@���t���O
	int		cnt, state;
	int		speed;

	std::string	fname;		//���b�Z�[�W�̃t�@�C����
	int		fileTell;		//�ǂݍ��ݍς݂̃t�@�C���|�C���^�̈ʒu

	std::shared_ptr<JoypadInput> input;

	//void	Init();
	bool	LoadMessage();

	bool	DrawMessage();
	void	PutMessage();
	void	NewLine();
	void	NewPage();
};
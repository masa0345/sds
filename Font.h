#pragma once

#include <vector>
#include <string>

class Font
{
public:
	Font(std::string font, int size, int thick, std::string fname, int type = 0x02);
	~Font();

	void	DrawFontString(int x, int y, unsigned dwCol, const char* format, ...);

	int		GetSize();			//�t�H���g�̑傫���𓾂�
	int		GetThick();			//�t�H���g�̑����𓾂�
	int		GetStringWidth(const char* str);	//������̕��𓾂� 

private:
	std::string	fileName;			//�t�H���g�t�@�C���̖��O
	std::string	fontName;			//�t�H���g��
	int		fontThick;		//�t�H���g�̑���
	int		fontSize;			//�t�H���g�̑傫��
	void*	pbFont;			//�m�ۂ����������ւ̃|�C���^
	int		hFont;			//�t�H���g�n���h��

	void	LoadFontData();		//�t�H���g�f�[�^��ǂݍ���
	void	DeleteFontData();	//�t�H���g�f�[�^���폜����

	static std::vector<std::string>	fonts;	//�ǂݍ��ݍς݂̃t�H���g��
};
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "common.h"

typedef const std::vector<int>* LPHIMG;

//�摜�N���X
class Image
{
private:
	Image();
	Image(const Image&);
	Image& operator=(const Image&);
public:
	static Image*	Instance();

private:
	std::unordered_map<std::string, std::vector<int> >	images;	//�摜�Ǘ�
	void			my_exit(int fp = NULL);		//�����I��

public:
	void			Init(const char* fname);	//�t�@�C���ǂݍ���
	LPHIMG			Load(std::string name);		//�O���t�B�b�N�n���h���ւ̂̃|�C���^��Ԃ�
};

struct ImageData
{
	LPHIMG hdl = nullptr;
	int num = 0;
	bool drawflag = true;
	float exRate = 1.f;
	float angle = 0.f;
	int blendmode = 0; // 0:�u�����h�Ȃ�
	int alpha = 254; // �u�����h�l
	int ofs_x= 0, ofs_y = 0;
};

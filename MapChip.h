#pragma once 

#include "common.h"
#include "Image.h"
#include <vector>

class Camera;
class Stage;

enum ColPointType {
	HIT_TOP		= 1,
	HIT_BOTTOM	= 2,
	HIT_RIGHT	= 8,
	HIT_LEFT	= 4,
	HIT_RSLOPE	= 16, //�E�オ��
	HIT_LSLOPE	= 32, //���オ��
	HIT_HALF	= 64,
	HIT_SUPPORT = 128,
	HIT_SLOPE	= HIT_RSLOPE | HIT_LSLOPE,
	HIT_ALL = HIT_TOP | HIT_BOTTOM | HIT_RIGHT | HIT_LEFT | HIT_SLOPE
};




//�}�b�v�N���X
class MapChip
{
private:
	MapChip();
	MapChip(const MapChip&);
	const MapChip& operator=(const MapChip&);

public:
	static MapChip* Instance();

private:
	std::vector<byte> map;
	std::vector<byte> draw[3];
	LPHIMG img, backImg;
	int mmx, mmy;

	byte& Get(int x, int y); //�w�肵���}�b�v�`�b�v���W�̒l��Ԃ�
	byte& MapDrawNum(int layer, int x, int y);	//�w�肵���}�b�v�`�b�v���W�̕`��ԍ���Ԃ�

public:
	//�}�b�v�ɂ��␳�����l
	struct CorrectVal
	{
		Vector2 p, v;
		int		jump;
	};
	//�}�b�v�ƃI�u�W�F�N�g�̏Փ˓_
	struct ColPoint
	{
		int x, y; //���S����̋���
		byte type; //���肷���
	};

	void Init();
	void Draw(const Camera& c);
	void DrawBackGround(const Camera& c);

	byte GetMap(float x, float y); //�ʏ�̍��W����}�b�v�`�b�v�̒l��Ԃ�
	byte Hit(CorrectVal* center);
	byte Hit(CorrectVal* center, ColPoint* p, int pointNum);
	byte Hit(CorrectVal* center, const Vector2& pre, ColPoint* p, int pointNum);

	void Set(const Stage& s, unsigned long map[][MMY]);
};
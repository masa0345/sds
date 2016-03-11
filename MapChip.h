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
	HIT_RSLOPE	= 16, //右上がり
	HIT_LSLOPE	= 32, //左上がり
	HIT_HALF	= 64,
	HIT_SUPPORT = 128,
	HIT_SLOPE	= HIT_RSLOPE | HIT_LSLOPE,
	HIT_ALL = HIT_TOP | HIT_BOTTOM | HIT_RIGHT | HIT_LEFT | HIT_SLOPE
};




//マップクラス
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

	byte& Get(int x, int y); //指定したマップチップ座標の値を返す
	byte& MapDrawNum(int layer, int x, int y);	//指定したマップチップ座標の描画番号を返す

public:
	//マップにより補正される値
	struct CorrectVal
	{
		Vector2 p, v;
		int		jump;
	};
	//マップとオブジェクトの衝突点
	struct ColPoint
	{
		int x, y; //中心からの距離
		byte type; //判定する面
	};

	void Init();
	void Draw(const Camera& c);
	void DrawBackGround(const Camera& c);

	byte GetMap(float x, float y); //通常の座標からマップチップの値を返す
	byte Hit(CorrectVal* center);
	byte Hit(CorrectVal* center, ColPoint* p, int pointNum);
	byte Hit(CorrectVal* center, const Vector2& pre, ColPoint* p, int pointNum);

	void Set(const Stage& s, unsigned long map[][MMY]);
};
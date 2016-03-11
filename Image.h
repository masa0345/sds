#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "common.h"

typedef const std::vector<int>* LPHIMG;

//画像クラス
class Image
{
private:
	Image();
	Image(const Image&);
	Image& operator=(const Image&);
public:
	static Image*	Instance();

private:
	std::unordered_map<std::string, std::vector<int> >	images;	//画像管理
	void			my_exit(int fp = NULL);		//強制終了

public:
	void			Init(const char* fname);	//ファイル読み込み
	LPHIMG			Load(std::string name);		//グラフィックハンドルへののポインタを返す
};

struct ImageData
{
	LPHIMG hdl = nullptr;
	int num = 0;
	bool drawflag = true;
	float exRate = 1.f;
	float angle = 0.f;
	int blendmode = 0; // 0:ブレンドなし
	int alpha = 254; // ブレンド値
	int ofs_x= 0, ofs_y = 0;
};

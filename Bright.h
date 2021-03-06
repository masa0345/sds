#pragma once

#include "Scene.h"

//画面の明るさを管理するクラス
class Bright
{
	//friend GameState CScene::Play();

private:
	Bright();
	Bright(const Bright&);
	Bright& operator=(const Bright&);

public:
	static	Bright* Instance();

private:
	bool	changing;
	int		time, count;
	int		newBright, oldBright, curBright;


public:
	void	Update();
	int		ChangeBright(int bright, int time = 0);
	int		GetBright();
};
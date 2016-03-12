#include "Bright.h"
#include <DxLib.h>

Bright::Bright() : changing(false), curBright(255)
{
}

Bright* Bright::Instance()
{
	static Bright _Instance;
	return &_Instance;
}

//ñæÇÈÇ≥ïœçX
void Bright::Update()
{
	if (!changing) return;

	curBright += (newBright - oldBright) / time;
	if (count++ == time) {
		curBright = newBright;
		changing = false;
	}
	SetDrawBright(curBright, curBright, curBright);
}
int Bright::ChangeBright(int bright, int time)
{
	if (changing) return 0;

	if (time <= 0) {
		curBright = bright;
		SetDrawBright(curBright, curBright, curBright);
		return 1;
	}
	oldBright = curBright;
	newBright = bright;
	this->time = time;
	count = 0;

	if (newBright == oldBright) return 1;

	changing = true;
	return 0;
}

//åªç›ÇÃñæÇÈÇ≥
int Bright::GetBright()
{
	return curBright;
}

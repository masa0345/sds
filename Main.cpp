#include <DxLib.h>
#include <memory>
#include "DebugDraw.h"
#include "Scene.h"
#include "common.h"


// 0`r‚Ü‚Å‚Ì—”
inline float RandFloat0(float r) {
	return r / 1000.f * GetRand(1000);
}
// -r`r‚Ü‚Å‚Ì—”
inline float RandFloat(float r) {
	return -r + r * 2 / 1000.f * GetRand(1000);
}
// r1`r2‚Ü‚Å‚Ì—”
inline float RandFloat(float r1, float r2) {
	return r1 + RandFloat0(r2 - r1);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#ifdef _DEBUG
	//ƒƒOo—Í
	SetOutApplicationLogValidFlag(TRUE);
#else
	SetOutApplicationLogValidFlag(FALSE);
#endif

	ChangeWindowMode(TRUE);
	SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);
	SetMainWindowText(WINDOW_TITLE);
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	if (DxLib_Init() == -1 || SetDrawScreen(DX_SCREEN_BACK) != 0)	return -1;

#ifdef DEBUG_DRAW
	DebugDraw::InitFont();
#endif

	auto game = std::make_shared<SceneManager>();

	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen() && !clsDx())
	{
		game->Update();


#ifdef DEBUG_DRAW
		DebugDraw::Update();
#endif 
		if (game->GetGameExit()) break;
	}

	DxLib_End();

	return 0;
}
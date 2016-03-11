#include <DxLib.h>
#include <memory>
#include "Input.h"
#include "DebugDraw.h"
#include "common.h"

#include "Image.h"
#include "Player.h"
#include "Stage.h"
#include "MapChip.h"
#include "Sound.h"
#include "Projectile.h"

// 0Å`rÇ‹Ç≈ÇÃóêêî
inline float RandFloat0(float r) {
	return r / 1000.f * GetRand(1000);
}
// -rÅ`rÇ‹Ç≈ÇÃóêêî
inline float RandFloat(float r) {
	return -r + r * 2 / 1000.f * GetRand(1000);
}
// r1Å`r2Ç‹Ç≈ÇÃóêêî
inline float RandFloat(float r1, float r2) {
	return r1 + RandFloat0(r2 - r1);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#ifdef _DEBUG
	//ÉçÉOèoóÕ
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

	auto pad1 = std::make_shared<JoypadInput>(DX_INPUT_PAD1 | DX_INPUT_KEY);

	Image::Instance()->Init("data/image/image.csv");
	Sound::Instance()->Init();
	PlayerBullet::Load();

	std::shared_ptr<Player> p = std::make_shared<Player>(Vector2(200.f, 100.f), pad1);
	std::shared_ptr<Stage> stage = std::make_shared<Stage>();
	stage->SetPlayer(p);
	GameEntity::SetStage(stage);
	GameEntity::Create(p);
	stage->SetStageMapNum(1, 0);
	stage->LoadStage();
	stage->PlaceEnemies();

	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen() && !clsDx())
	{
		pad1->Update();
		Sound::Instance()->UpdateState();

		GameEntity::UpdateEntities();

		MapChip::Instance()->Draw(*stage->GetCamera());
		GameEntity::DrawEntities();

		DebugDraw::String({ 0, 0 }, 0xffff00, "ESCÇ≈èIóπ %d", pad1->Get(INPUT_Z));


#ifdef DEBUG_DRAW
		DebugDraw::Update();
#endif 
		if (pad1->Get(INPUT_ESC) == 1) break;
	}

	DxLib_End();

	return 0;
}
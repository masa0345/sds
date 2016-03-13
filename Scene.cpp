#include "Scene.h"
#include "Bright.h"
#include "Input.h"
#include "Sound.h"
#include "Image.h"
#include "Projectile.h"
#include "Stage.h"
#include "Player.h"
#include "Frontend.h"
#include "MapChip.h"
#include "Event.h"
#include "Save.h"
#include "Font.h"
#include <DxLib.h>

namespace {
	std::vector<std::shared_ptr<Font>> fonts;
}

SceneManager::SceneManager() : gameExit(false)
{
	input = std::make_shared<JoypadInput>(DX_INPUT_PAD1 | DX_INPUT_KEY);
	Scene::SetInput(input);
	scene = new SceneTitleInit();
}

SceneManager::~SceneManager()
{
	if (scene) delete scene;
}

void SceneManager::Update()
{
	input->Update();
	Sound::Instance()->UpdateState();
	Bright::Instance()->Update();

	Scene* ns = scene->Update();
	if (ns != scene) {
		delete scene;
		scene = ns;
	}
	if (input->Get(INPUT_ESC) == 1 || scene == nullptr) gameExit = true;
}

bool SceneManager::GetGameExit() const
{
	return gameExit;
}

std::shared_ptr<JoypadInput> Scene::input = nullptr;
void Scene::SetInput(std::shared_ptr<JoypadInput> in) {
	input = in;
}

Scene::Scene() : state(0), stateCnt(0)
{
}

// デバッグ用 タイトルを飛ばしてステージからスタート
SceneDebugStart::SceneDebugStart() 
{
}

Scene* SceneDebugStart::Update()
{
	Image::Instance()->Init("data/image/image.csv");
	Sound::Instance()->Init();
	PlayerBullet::Load();
	fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 24, 4, "data/font/ume-pgc5.ttf"));
	fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 16, 3, "data/font/ume-pgc5.ttf", DX_FONTTYPE_ANTIALIASING_EDGE));
	auto stage = std::make_shared<Stage>();
	stage->SetStageMapNum(1, 0);
	stage->LoadBGM();
	return new SceneStageStart(stage);
}


// タイトルの前
SceneTitleInit::SceneTitleInit()
{
	images.push_back(LoadGraph("data/image/system/x68.png"));
	Bright::Instance()->ChangeBright(0);
}

Scene* SceneTitleInit::Update()
{
	int t;
	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(255, 30)) ++state;
		break;
	case 1: // データロード
		t = GetNowCount();
		Image::Instance()->Init("data/image/image.csv");
		Sound::Instance()->Init();
		//ショットデータ読み込み
		PlayerBullet::Load();
		//セーブデータ読み込み
		Save::Instance()->LoadSaveData();
		//フォントデータ読み込み
		ChangeFont("ＭＳ ゴシック");
		SetFontThickness(3);
		SetFontSize(16);
		ChangeFontType(1);
		fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 24, 4, "data/font/ume-pgc5.ttf"));
		fonts.push_back(std::make_shared<Font>("梅PゴシックC5", 16, 3, "data/font/ume-pgc5.ttf", DX_FONTTYPE_ANTIALIASING_EDGE));
		//ロードにかかった時間
		t = GetNowCount() - t;
		// ロードが1000ms以下なら1000msまで待機
		if (t < 1000) Sleep(1000 - t);
		++state;
		break;
	case 2:
		if (Bright::Instance()->ChangeBright(0, 30)) return new SceneTitle();
	}
	DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0xffffff, TRUE);
	DrawGraph(60, 40, images[0], TRUE);

	return this;
}

// ステージ開始
SceneStageStart::SceneStageStart(std::shared_ptr<Stage> s) : stage(s)
{
	GameEntity::InitManager();
	Frontend::InitManager();
	auto p = std::make_shared<Player>(input);
	stage->SetPlayer(p);
	GameEntity::SetStage(stage);
	GameEntity::Create(p);
	stage->LoadStage();
	stage->PlaceEnemies();
	stage->PlayBGM(false);
	images.push_back(Image::Instance()->Load("stage_name")->at(stage->GetStageNum())); //[0]
	images.push_back(Image::Instance()->Load("cross")->at(0));	//[1]
	images.push_back(Image::Instance()->Load("num")->at(stage->GetStock())); //[2]
}

Scene* SceneStageStart::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(255, 0)) {
			++state;
			stateCnt = 0;
		}
		break;
	case 1:
		if (stateCnt < 30) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
			DrawGraph(150, 120 - (30 - stateCnt), images[0], TRUE);
			DrawGraph(150, 120 + (30 - stateCnt), images[0], TRUE);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 30);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30 - (30 - stateCnt), images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16 - (30 - stateCnt), images[2], TRUE);
		} else if (stateCnt < 40) {
			DrawGraph(150, 120, images[0], TRUE);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30, images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16, images[2], TRUE);
		} else if (stateCnt < 90) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - stateCnt * 3);
			DrawGraph(150, 120, images[0], TRUE);
			DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30, images[1], TRUE);
			DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16, images[2], TRUE);
		} else {
			return new SceneGameMain(stage);
		}
	}
	++stateCnt;

	return this;
}

// ゲームメイン
SceneGameMain::SceneGameMain(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneGameMain::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	if (!stage->GetPlayer()->GetExist()) {
		return new ScenePlayerDeath(stage);
	}
	if (stage->CheckAreaScroll()) {
		return new SceneAreaScroll(stage);
	}
	if (stage->GetLoadNext()) {
		return new SceneMoveMap(stage);
	}
	if (stage->GetClear()) {
		return new SceneStageClear(stage);
	}
	if (Event::GetEventFlag()) {
		return new SceneEvent(stage);
	}
	if (input->Get(INPUT_P) == 1) {
		return new ScenePause(stage);
	}

	return this;
}

// 部屋移動
SceneAreaScroll::SceneAreaScroll(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneAreaScroll::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	float speed = 10.f;
	Vector2 cp = stage->GetCamera()->GetPos();
	Vector2 pp = stage->GetPlayer()->GetPos();
	int ph = stage->GetPlayer()->GetHeight(), pw = stage->GetPlayer()->GetWidth();
	auto area = stage->GetArea(stage->GetAreaNum() + 1);
	if (cp.x < area[0] * MW || cp.y < area[1] * MH ||
		cp.x > area[2] * MW + MW - WINDOW_WIDTH || 
		cp.y > area[3] * MW + MW - WINDOW_HEIGHT) {
		if (cp.x < area[0] * MW) cp.x += speed;
		else if (cp.x > area[2] * MW + MW - WINDOW_WIDTH) cp.x -= speed;
		if (cp.y < area[1] * MH) cp.y += speed;
		else if (cp.y > area[3] * MW + MW - WINDOW_HEIGHT) cp.y -= speed;
	} else if (pp.y + ph / 2 - cp.y > WINDOW_HEIGHT - 120 && 
			cp.y + 10 < area[3] * MH - WINDOW_HEIGHT) {
		cp.y += speed;
	} else if (pp.y - ph / 2 - cp.y < 120 && cp.y - 10 > area[1] * MH) {
		cp.y -= speed;
	} else {
		stage->GoNextArea();
		return new SceneGameMain(stage);
	}
	stage->GetCamera()->SetPos(cp);

	return this;
}

// マップ移動
SceneMoveMap::SceneMoveMap(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* SceneMoveMap::Update()
{
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(0, 12)) state = 1;
		break;
	case 1:
		stage->GoNextMap();
		state = 2;
		break;
	case 2:
		if (Bright::Instance()->ChangeBright(255, 12)) {
			return new SceneGameMain(stage);
		}
		break;
	}
	return this;
}

// イベントシーン
SceneEvent::SceneEvent(std::shared_ptr<Stage> s) : stage(s)
{
	input->SetNoInputFlag(true);
}

Scene* SceneEvent::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	if (Event::GetEventFlag()) {
		Event::curEvent->Update();
		return this;
	}

	input->SetNoInputFlag(false);
	return new SceneGameMain(stage);
}

// 自機死亡
ScenePlayerDeath::ScenePlayerDeath(std::shared_ptr<Stage> s) : stage(s)
{
}

Scene* ScenePlayerDeath::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	int stock = stage->GetStock();
	switch (state) {
	case 0:
		if (stateCnt == 60) {
			if (stock > 0) {
				state = 1;
				stateCnt = 0;
				stage->SetStock(stock - 1);
			} else {
				state = 2;
				stateCnt = 0;
			}
			Sound::Instance()->FadeOut("default", 120);
		}
		break;
	case 1:
		if (Bright::Instance()->ChangeBright(0, 120)) {
			return new SceneStageStart(stage);
		}
		break;
	case 2:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (stateCnt > 60) {
			stateCnt = 0;
			state++;
		}
		break;
	case 3:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (stateCnt > 120) {
			stateCnt = 0;
			state++;
		}
		break;
	case 4:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(90, 220, Image::Instance()->Load("gameover")->at(0), TRUE);
		if (Bright::Instance()->ChangeBright(0, 120)) {
			Save::Instance()->SaveStageResult(stage);
			Save::Instance()->SaveScore();
			return new SceneTitle();
		}
		break;
	}
	++stateCnt;

	return this;
}

// ステージクリア
SceneStageClear::SceneStageClear(std::shared_ptr<Stage> s) : stage(s)
{
	images.push_back(Image::Instance()->Load("clear")->at(0)); // [0]
	images.push_back(Image::Instance()->Load("result")->at(0)); // [1]
	images.push_back(Image::Instance()->Load("rank")->at(0)); // [2]
	images.push_back(Image::Instance()->Load("rank")->at(1)); // [3]
	images.push_back(Image::Instance()->Load("rank")->at(2)); // [4]
	images.push_back(Image::Instance()->Load("rank")->at(3)); // [5]
	images.push_back(Image::Instance()->Load("rank")->at(4)); // [6]
}

Scene* SceneStageClear::Update()
{
	GameEntity::UpdateAll();
	Frontend::UpdateAll();

	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Frontend::DrawAll();

	int sx = 120, sy = 40;
	switch (state) {
	case 0:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 30);
		DrawGraph(sx, sy, images[0], TRUE);
		if (stateCnt == 30) {
			stateCnt = 0;
			state++;
		}
		Save::Instance()->SaveStageResult(stage);
		Save::Instance()->SaveScore();
		break;
	case 1:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 60);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		if (stateCnt == 60) {
			stateCnt = 0;
			state++;
		}
		break;
	case 2:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
		}
		break;
	case 3:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stateCnt*stage->GetScore() / 90);
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
		}
		break;
	case 4:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stage->GetScore());

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, stateCnt * 255 / 90);
		DrawGraph(sx + 83, sy + 205, images[2 + stage->GetRank()], TRUE);
		if (stateCnt == 90) {
			stateCnt = 0;
			state++;
			Sound::Instance()->FadeOut("default", 240);
		}
		break;
	case 5:
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DrawGraph(sx, sy, images[0], TRUE);
		DrawGraph(sx - 20, sy + 60, images[1], TRUE);
		fonts[0]->DrawFontString(sx + 250, sy + 108, 0xffffff, "%s", stage->GetTimerToString());
		fonts[0]->DrawFontString(sx + 250, sy + 161, 0xffffff, "%d", stage->GetScore());
		DrawGraph(sx + 83, sy + 205, images[2 + stage->GetRank()], TRUE);

		if (stateCnt > 120) {
			if (Bright::Instance()->ChangeBright(0, 120)) {
				Sound::Instance()->StopAll();
				return new SceneStageSelect();
			}
		}
		break;
	}
	++stateCnt;

	return this;
}

// メニュー付きシーン
SceneWithMenu::SceneWithMenu() : curX(0), curY(0)
{
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void SceneWithMenu::MoveCursor(int x, int y)
{
	MoveCursorX(x);
	MoveCursorY(y);
}

int SceneWithMenu::MoveCursorX(int menuNum, bool loop)
{
	assert(menuNum > 0);

	if (input->Get(INPUT_RIGHT) % 30 == 1) {
		Sound::Instance()->Play("カーソル移動");
		if (loop) {
			curX = (curX + 1) % menuNum;
		} else {
			if (curX < menuNum - 1) curX++;
		}
		return 1;
	} else if (input->Get(INPUT_LEFT) % 30 == 1) {
		Sound::Instance()->Play("カーソル移動");
		if (loop) {
			curX = (curX + menuNum - 1) % menuNum;
		} else {
			if (curX > 0) curX--;
		}
		return -1;
	}
	return 0;
}

int SceneWithMenu::MoveCursorY(int menuNum, bool loop)
{
	assert(menuNum > 0);
	int ret = 0;
	if (input->Get(INPUT_DOWN) % 30 == 1) {
		Sound::Instance()->Play("カーソル移動");
		if (loop) {
			curY = (curY + 1) % menuNum;
		} else {
			if (curY < menuNum - 1) curY++;
		}
		ret = 1;
	} else if (input->Get(INPUT_UP) % 30 == 1) {
		Sound::Instance()->Play("カーソル移動");
		if (loop) {
			curY = (curY + menuNum - 1) % menuNum;
		} else {
			if (curY > 0) curY--;
		}
		ret = -1;
	}
	//そのカーソルが選択不可ならもう一度移動
	if (!curFlag[curY]) MoveCursorY(menuNum, loop);
	return ret;
}

SceneTitle::SceneTitle(int sc) : backScroll(sc)
{
	images.push_back(Image::Instance()->Load("title_back")->at(0)); // [0]
	images.push_back(Image::Instance()->Load("title_tile")->at(0)); // [1]
	images.push_back(Image::Instance()->Load("logo")->at(0)); // [2]
	images.push_back(Image::Instance()->Load("title_frame")->at(0)); // [3]
	images.push_back(Image::Instance()->Load("title_menu")->at(0)); // [4]
	images.push_back(Image::Instance()->Load("title_btn")->at(0)); // [5]
	curFlag.resize(4, true);
	// セーブデータがあるときはカーソル初期位置は"つづきから"
	if( Save::Instance()->HasSaveData() ) curY = 1;
	// セーブデータがないときは"つづきから"選択不可
	else curFlag[1] = false;
	//curFlag[1] = false;
	curFlag[2] = false;
}

Scene * SceneTitle::Update()
{
	MoveCursorY(4);

	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(255, 30)) {
			++state;
		}
		break;
	case 1:
		if (input->Get(INPUT_Z) == 1) {
			Sound::Instance()->Play("決定");
			switch (curY) {
			case 0: // はじめから
				Save::Instance()->LoadDefaultData();
				return new SceneStageSelect(backScroll);
			case 1: // つづきから
				Save::Instance()->LoadSaveData();
				return new SceneStageSelect(backScroll);
			case 2: // 設定
				return nullptr;
				//curX = m_cfg.GetWinFlag();
				//return OPTION;
			case 3: // 終了
				return nullptr;
			}
		}
		break;
	}

	DrawModiGraph(0, 0, WINDOW_WIDTH, 0, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 0, WINDOW_HEIGHT, images[0], FALSE);
	int ofs = ++backScroll % 48;
	for (int x = 0; x < WINDOW_WIDTH / 48 + 1; ++x) {
		for (int y = 0; y < WINDOW_HEIGHT / 48 + 1; ++y) {
			DrawGraph(x * 48 - ofs, y * 48 - ofs, images[1], TRUE);
		}
	}
	DrawGraph(0, 0, images[3], TRUE);
	DrawGraph(120, 40, images[2], TRUE);

	const char* menu_name[] = {
		{ "はじめから" },
		{ "つづきから" },
		{ "設定" },
		{ "終了" },
	};

	DWORD col;
	int sx = 40, sy = 190, dist = 60;
	for (int i = 0; i < 4; ++i) {
		DrawGraph(sx + 17, sy + 17 + dist*i, images[4], TRUE);
		DrawGraph(sx, sy + dist*i, images[5], TRUE);
		col = (curY == i) ? 0xffffff : 0xaaaaaa;
		if (!curFlag[i]) col = 0x777777;
		fonts[0]->DrawFontString(sx + 60, sy + 27 + dist*i, col, menu_name[i]);
	}

	return this;
}

// ステージ選択
SceneStageSelect::SceneStageSelect(int sc) : backScroll(sc)
{
	curFlag.resize(STAGE_MAX, true);
	for (int i = 2; i < STAGE_MAX; ++i) curFlag[i] = false;
	images.push_back(Image::Instance()->Load("title_back")->at(0)); // [0]
	images.push_back(Image::Instance()->Load("title_tile")->at(0)); // [1]
	images.push_back(Image::Instance()->Load("title_frame")->at(1)); // [2]
	images.push_back(Image::Instance()->Load("title_menu")->at(0)); // [3]
	images.push_back(Image::Instance()->Load("title_btn")->at(0)); // [4]
	images.push_back(Image::Instance()->Load("select")->at(0)); // [5]
	images.push_back(Image::Instance()->Load("select")->at(1)); // [6]
	images.push_back(Image::Instance()->Load("title_board")->at(1)); // [7]
	images.push_back(Image::Instance()->Load("title_board")->at(2)); // [8]
}

Scene* SceneStageSelect::Update()
{
	MoveCursorY(STAGE_MAX);
	switch (state) {
	case 0:
		if (Bright::Instance()->ChangeBright(255, 60)) {
			state++;
		}
	case 1:
		if (input->Get(INPUT_Z) == 1) {
			Sound::Instance()->Play("決定");
			state++;
			stateCnt = 0;
		}
		if (input->Get(INPUT_X) == 1) {
			Sound::Instance()->Play("戻る");
			return new SceneTitle(backScroll);
		}
		break;
	case 2:
		if (Bright::Instance()->ChangeBright(0, 60)) {
			stage = std::make_shared<Stage>();
			stage->SetStageMapNum(curY, 0);
			ClearDrawScreen();
			state++;
		}
		break;
	case 3:
		Bright::Instance()->ChangeBright(255);
		DrawString(0, 0, "Now Loading ...", 0xffffff);
		ScreenFlip();
		ClearDrawScreen();
		stage->LoadBGM();
		Bright::Instance()->ChangeBright(0);
		return new SceneStageStart(stage);
	}

	DrawModiGraph(0, 0, WINDOW_WIDTH, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, WINDOW_HEIGHT, images[0], FALSE);
	int ofs = ++backScroll % 48;
	for (int x = 0; x < WINDOW_WIDTH / 48 + 1; ++x) {
		for (int y = 0; y < WINDOW_HEIGHT / 48 + 1; ++y) {
			DrawGraph(x * 48 - ofs, y * 48 - ofs, images[1], TRUE);
		}
	}
	switch (curY) {
	case 0: DrawGraph(435, 0, images[5], TRUE); break;
	case 1: DrawGraph(296, 0, images[6], TRUE); break;
	}

	DrawGraph(0, 55, images[2], TRUE);
	DrawGraph(40 + 17, 40 + 17, images[3], TRUE);
	DrawGraph(40, 40, images[4], TRUE);
	fonts[0]->DrawFontString(40 + 60, 40 + 27, 0xffffff, "ステージセレクト");

	DrawGraph(410, 280, images[8], TRUE);
	switch (curY) {
	case 0:
		fonts[1]->DrawFontString(420, 290, 0xffffff, "操作説明です。");
		break;
	case 1:fonts[1]->DrawFontString(420, 290, 0xffffff, "ハイスコア :");
		fonts[1]->DrawFontString(420, 310, 0xffffff, "    %d", Save::Instance()->GetHighScore(curY));
		fonts[1]->DrawFontString(420, 330, 0xffffff, "クリアタイム :");
		fonts[1]->DrawFontString(420, 350, 0xffffff, "    %s", stage->GetTimerToString(Save::Instance()->GetClearTime(curY)));
		break;
	}

	DWORD col;
	int sx = 80, sy = 120, dist = 45;
	for (int i = 0; i < STAGE_MAX; i++) {
		if (i == 2) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawGraph(sx - 8, sy - 8 + dist*i, images[7], TRUE);
		col = (curY == i) ? 0xffffff : 0xaaaaaa;
		if (curFlag[i]) {
			fonts[0]->DrawFontString(sx, sy + i*dist, col, "STAGE%d", i);
		} else {
			fonts[0]->DrawFontString(sx, sy + i*dist, 0x666666, "---");
		}
	}

	return this;
}

// ポーズ
ScenePause::ScenePause(std::shared_ptr<Stage> s) : stage(s)
{
	curFlag.resize(3, true);
	images.push_back(Image::Instance()->Load("pause")->at(0)); //[0]
	images.push_back(Image::Instance()->Load("cross")->at(0)); //[1]
	images.push_back(Image::Instance()->Load("num")->at(stage->GetStock())); //[2]
	Sound::Instance()->Play("ポーズ");
	Sound::Instance()->ChangeVolume(0.25);
}

Scene * ScenePause::Update()
{
	Bright::Instance()->ChangeBright(150);
	MapChip::Instance()->Draw(*stage->GetCamera());
	GameEntity::DrawAll();
	Bright::Instance()->ChangeBright(255);

	MoveCursorY(3);

	switch (curY) {
	case 0:
		if (input->Get(INPUT_Z) == 1) {
			Sound::Instance()->ChangeVolume(4.0);
			Sound::Instance()->Play("ポーズ");
			return new SceneGameMain(stage);
		}
		break;
	case 1:
		if (input->Get(INPUT_Z) == 1) {
			Sound::Instance()->ChangeVolume(4.0);
			Sound::Instance()->StopAll();
			Sound::Instance()->Play("決定");
			return new SceneTitle();
		}
		break;
	case 2:
		if (input->Get(INPUT_Z) == 1) {
			Sound::Instance()->Play("決定");
			return nullptr;
		}
		break;
	}

	if (input->Get(INPUT_P) == 1) {
		Sound::Instance()->ChangeVolume(4.0);
		Sound::Instance()->Play("ポーズ");
		return new SceneGameMain(stage);
	}

	const char* pause_menu[] = {
		{ "ゲームへ戻る" },
		{ "タイトルへ戻る" },
		{ "終了" },
	};

	DWORD col;
	int sx = 200, sy = 170, dist = 32;
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	DrawGraph(sx, sy, images[0], TRUE);
	DrawGraph(WINDOW_WIDTH - 64, WINDOW_HEIGHT - 30, images[1], TRUE);
	DrawGraph(WINDOW_WIDTH - 64 + 24, WINDOW_HEIGHT - 30 - 16, images[2], TRUE);

	for (int i = 0; i < 3; ++i) {
		col = (curY == i) ? 0xffffff : 0xaaaaaa;
		fonts[1]->DrawFontString(sx + 40, sy + 28 + dist*i, col, pause_menu[i]);
	}

	return this;
}

#include "MapChip.h"
#include "Camera.h"
#include "Image.h"
#include "Stage.h"
#include <DxLib.h>

MapChip::MapChip()
{
	map.clear();
}

MapChip* MapChip::Instance()
{
	static MapChip _Instance;
	return &_Instance;
}

byte& MapChip::Get(int x, int y)
{
	if (!InRangeEq(0, x, mmx - 1) || !InRangeEq(0, y, mmy - 1)) {
		return map[0];
	}
	return map[x*mmy + y];
}

byte& MapChip::MapDrawNum(int layer, int x, int y)
{
	assert(InRangeEq(0, layer, 2));

	if (!InRangeEq(0, x, mmx - 1) || !InRangeEq(0, y, mmy - 1)) {
		return draw[layer][0];
	}
	return draw[layer][x*mmy + y];
}

void MapChip::Init()
{
}

void MapChip::Draw(const Camera& c)
{
	DrawBackGround(c);

	Rect r = c.GetVisibleRect(mmx, mmy);

	for (int x = r.left; x < r.right; x++) {
		for (int y = r.top; y < r.bottom; y++) {
#ifdef _DEBUG
			//DrawBoxScreen(x*MW, y*MH, x*MW+MW, y*MH+MH, 0x222222, FALSE);
#endif
			Vector2 p = c.TransCoord({ x*MW, y*MH });
			if (img != nullptr) 
				DrawGraph((int)p.x, (int)p.y, img->at(MapDrawNum(0, x, y)), TRUE);
			if (backImg != nullptr) 
				DrawGraph((int)p.x, (int)p.y, backImg->at(MapDrawNum(1, x, y)), TRUE);

#ifdef _DEBUG
			/*
			if (CKey::Instance()->Get(KEY_INPUT_F1) == 0) {

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				if (MapChip(x, y) & HIT_SUPPORT) {
					DrawBoxScreen(x*MW, y*MH, x*MW + MW, y*MH + MH, 0x333333, FALSE);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

				int color = GetColor(0, 0, 255);
				if (MapChip(x, y) & HIT_HALF) color = GetColor(0, 128, 255);

				if (MapChip(x, y) & HIT_TOP) {
					DrawLineScreen(x*MW, y*MH, x*MW + MW - 1, y*MH, color);
				}
				if (MapChip(x, y) & HIT_BOTTOM) {
					DrawLineScreen(x*MW, y*MH + MH - 1, x*MW + MW - 1, y*MH + MH - 1, color);
				}
				if (MapChip(x, y) & HIT_LEFT) {
					DrawLineScreen(x*MW, y*MH, x*MW, y*MH + MH - 1, color);
				}
				if (MapChip(x, y) & HIT_RIGHT) {
					DrawLineScreen(x*MW + MW - 1, y*MH, x*MW + MW - 1, y*MH + MH - 1, color);
				}

				if (MapChip(x, y) & HIT_SUPPORT) color = 0x333333;
				if (MapChip(x, y) & HIT_RSLOPE) {
					DrawLineScreen(x*MW, y*MH + MH - 1, x*MW + MW - 1, y*MH, color);
				}
				if (MapChip(x, y) & HIT_LSLOPE) {
					DrawLineScreen(x*MW, y*MH, x*MW + MW - 1, y*MH + MH - 1, color);
				}

			}*/
#endif

		}
	}
}

void MapChip::DrawBackGround(const Camera& c) {
	int w, h;
	int back = Image::Instance()->Load("back")->at(0);
	GetGraphSize(back, &w, &h);

	float scx = (float)(w - WINDOW_WIDTH) / (mmx * MW);
	float scy = (float)(h - WINDOW_HEIGHT) / (mmy * MH);
	Vector2 p = c.TransCoordBackGround({ scx, scy });
	DrawGraphF(p.x, p.y, back, FALSE);
}

byte MapChip::GetMap(float x, float y)
{
	return Get((int)(x / MW), (int)(y / MH));
}

byte MapChip::Hit(CorrectVal* pos)
{
	ColPoint p[1] = { 0, 0, HIT_ALL };
	return Hit(pos, p, 1);
}

byte MapChip::Hit(CorrectVal* center, const Vector2& pre, ColPoint* p, int pointNum)
{
	byte ret = 0;
	float dx = center->p.x - pre.x, dy = center->p.y - pre.y;
	int cnt = (int)(max(fabs(dy), fabs(dx)) / 4) + 1;
	CorrectVal dp = *center;
	for (int i = 0; i < cnt; i++) {
		ret = Hit(&dp, p, pointNum);
		if (ret != 0) {
			*center = dp;
			return ret;
		}
		dp.p.x -= dx / cnt;
		dp.p.y -= dy / cnt;
	}
	return ret;

}

byte MapChip::Hit(CorrectVal* center, ColPoint* p, int pointNum)
{
	byte ret = 0;
	for (int i = 0; i < pointNum; i++) {
		float x = center->p.x + p[i].x, y = center->p.y + p[i].y;
		byte map = GetMap(x, y);
		byte hit = map & p[i].type;
		if (map == 0) continue;

		//マップチップの左上座標
		int map_x = (int)x / MW*MW, map_y = (int)y / MH*MH;

		//フロートブロック
		if (map & HIT_HALF) {
			if (center->v.y < 0) continue;
			if ((hit & HIT_TOP) && (y > map_y + MH / 2))continue;
			if (map & HIT_SUPPORT) {
				if ((map & HIT_RSLOPE) && ((y > map_y + MH / 4) || (x > map_x + MW / 4)))continue;
				if ((map & HIT_LSLOPE) && ((y > map_y + MH / 4) || (x < map_x + MW / 4)))continue;
			}
			if (hit & HIT_RSLOPE) {
				if ((y > map_y + MH / 2 + 4) && (x > map_x + MW / 2 + 4)) continue;
			}
			if (hit & HIT_LSLOPE) {
				if ((y > map_y + MH / 2 + 4) && (x < map_x + MW / 2 - 4)) continue;
			}
		}

		//通常ブロック
		if (hit & HIT_TOP) {
			center->p.y = (float)map_y - p[i].y;
			center->jump = 2;
			center->v.y = 0.f;
			ret |= HIT_TOP;
		}
		if (hit & HIT_BOTTOM) {
			center->p.y = (float)map_y - p[i].y + MH;
			center->v.y = 0.f;
			ret |= HIT_BOTTOM;
		}
		if (hit & HIT_LEFT) {
			center->p.x = (float)map_x - p[i].x;
			ret |= HIT_LEFT;
		}
		if (hit & HIT_RIGHT) {
			center->p.x = (float)map_x - p[i].x + MW;
			ret |= HIT_RIGHT;
		}

		if (!(hit & HIT_SLOPE)) continue;

		//斜面ブロックのy軸高さ
		int slope_y = ((int)x % MW) / (MW / MH);

		//斜面補助ブロック
		if (map & HIT_SUPPORT) {
			if (map & HIT_RSLOPE) center->p.y = (float)map_y - slope_y - p[i].y - 1;
			if (map & HIT_LSLOPE) center->p.y = (float)map_y - MH + slope_y - p[i].y;
			ret |= (map & HIT_RSLOPE) ? HIT_RSLOPE : HIT_LSLOPE;
			center->jump = 2;
		}
		//斜面ブロック
		if (hit & HIT_SLOPE) {
			if (map & HIT_SUPPORT) continue;

			if (hit & HIT_RSLOPE && y < map_y + MH - slope_y - 1) {
				continue;
			}
			if (hit & HIT_LSLOPE && y < map_y + slope_y) {
				continue;
			}
		}
		//右上がり
		if (hit & HIT_RSLOPE) {
			if (y > map_y + MH - slope_y - 1 - 1) {
				center->p.y = (float)map_y + MH - slope_y - p[i].y - 1;
				center->jump = 2;
				center->v.y = 0.f;
				ret |= HIT_RSLOPE;
			}
		}
		//左上がり
		if (hit & HIT_LSLOPE) {
			if (y > map_y + slope_y - 1) {
				center->p.y = (float)map_y + slope_y - p[i].y;
				center->jump = 2;
				center->v.y = 0.f;
				ret |= HIT_LSLOPE;
			}
		}
	}
	return ret;
}


void MapChip::Set(const Stage& stage, unsigned long m[][MMY])
{
	auto p = stage.GetMapMaxXY();
	mmx = p.first;
	mmy = p.second;

	map.clear();
	map.resize(mmx*mmy);
	for (int i = 0; i < 3; i++) draw[i].resize(mmx*mmy);

	std::string name = "stage" + std::to_string(stage.GetStageNum());
	img = Image::Instance()->Load(name);
	backImg = Image::Instance()->Load(name+"b");

	for (int x = 0; x < mmx; x++) {
		for (int y = 0; y < mmy; y++) {
			byte type = (byte)(m[x][y] & 0xff);
			Get(x, y) = type;
			MapDrawNum(0, x, y) = (byte)((m[x][y] & 0x0000ff00) >> 8);
			MapDrawNum(1, x, y) = (byte)((m[x][y] & 0x00ff0000) >> 16);
			MapDrawNum(2, x, y) = (byte)((m[x][y] & 0xff000000) >> 24);
		}
	}
}
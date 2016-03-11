#pragma once

#include "Vector2.h"
#include <string>
#include <queue>
#include <memory>

class DebugObject;
struct Rect;
struct Vector2;

// デバッグ描画クラス
class DebugDraw
{
public:
	static void InitFont();
	static void Update();

	static void Vector2Line(Vector2 s, Vector2 e, unsigned color);
	static void RectBox(const Rect& r, unsigned color, bool fill);
	static void RectBox(const Rect& r, Vector2 ofs, unsigned color, bool fill);
	static void RectBox3D(const Rect& r, unsigned color, bool fill);
	static void String(int x, int y, unsigned color, const char* format, ...);
	static void String(Vector2 pos, unsigned color, const char* format, ...);
	static void String3D(Vector2 pos, unsigned color, const char* format, ...);

private:
	static int font;
	static std::queue< std::shared_ptr<DebugObject> > dobj;
};

#define DEBUG_DRAW
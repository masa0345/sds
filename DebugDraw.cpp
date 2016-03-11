#include "DebugDraw.h"
#include "Rect.h"
#include <DxLib.h>


// ƒfƒoƒbƒO•`‰æƒIƒuƒWƒFƒNƒg
class DebugObject
{
public:
	DebugObject(unsigned color);
	virtual void Draw() = 0;

protected:
	unsigned color;
};

// ‹éŒ`•`‰æ
class DebugRect : public DebugObject
{
public:
	DebugRect(const Rect& r, unsigned color, bool fill);
	virtual void Draw() override;

private:
	Rect r;
	bool fill;
};


// ƒ‰ƒCƒ“•`‰æ
class DebugLine : public DebugObject
{
public:
	DebugLine(Vector2 s, Vector2 e, unsigned color);
	virtual void Draw() override;

private:
	Vector2 s, e;
};

// •¶Žš—ñ•`‰æ
class DebugString : public DebugObject
{
public:
	DebugString(int x, int y, unsigned color, int font, std::string str);
	virtual void Draw() override;

private:
	int x, y;
	int font;
	std::string str;
};

DebugObject::DebugObject(unsigned color) : color(color) {}

DebugRect::DebugRect(const Rect& r, unsigned color, bool fill) :
	DebugObject(color), r(r), fill(fill) {}

// ‹éŒ`•`‰æ
void DebugRect::Draw()
{
	DrawBox(r.left, r.top, r.right, r.bottom, color, fill);
}


DebugLine::DebugLine(Vector2 s, Vector2 e, unsigned color) :
	DebugObject(color), s(s), e(e) {}

// ƒ‰ƒCƒ“•`‰æ
void DebugLine::Draw()
{
	DrawLine(
		static_cast<int>(s.x), static_cast<int>(s.y),
		static_cast<int>(e.x), static_cast<int>(e.y), color);
}

DebugString::DebugString(int x, int y, unsigned color, int font, std::string str) :
	DebugObject(color), x(x), y(y), font(font), str(str) {}

// •¶Žš—ñ•`‰æ
void DebugString::Draw()
{
	DrawFormatStringToHandle(x, y, color, font, str.c_str());
}


int DebugDraw::font = -1;
std::queue< std::shared_ptr<DebugObject> > DebugDraw::dobj;

void DebugDraw::InitFont()
{
	font = CreateFontToHandle("‚l‚r ƒSƒVƒbƒN", 16, 0, DX_FONTTYPE_EDGE);
}

void DebugDraw::Update()
{
	SetWriteZBuffer3D(FALSE);
	SetDrawBright(254, 254, 254);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 200);
	while (!dobj.empty()) {
		dobj.front()->Draw();
		dobj.pop();
	}
}

void DebugDraw::Vector2Line(Vector2 s, Vector2 e, unsigned color)
{
	dobj.push(std::make_shared<DebugLine>(s, e, color));
}

void DebugDraw::RectBox(const Rect& r, unsigned color, bool fill)
{
	dobj.push(std::make_shared<DebugRect>(r, color, fill));
}

void DebugDraw::RectBox(const Rect& r, Vector2 ofs, unsigned color, bool fill)
{
	dobj.push(std::make_shared<DebugRect>(r+ofs, color, fill));
}

void DebugDraw::RectBox3D(const Rect& r, unsigned color, bool fill)
{
	VECTOR p1 = ConvWorldPosToScreenPos(VGet((float)r.left, (float)r.top, 0.f));
	VECTOR p2 = ConvWorldPosToScreenPos(VGet((float)r.right, (float)r.bottom, 0.f));
	Rect rect((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
	dobj.push(std::make_shared<DebugRect>(rect, color, fill));
}

void DebugDraw::String(int x, int y, unsigned color, const char* format, ...)
{
	va_list arg;
	char str[256] = "";

	va_start(arg, format);
	vsprintf_s(str, format, arg);
	va_end(arg);

	dobj.push(std::make_shared<DebugString>(x, y, color, font, str));
}
void DebugDraw::String(Vector2 pos, unsigned color, const char* format, ...)
{
	va_list arg;
	char str[256] = "";

	va_start(arg, format);
	vsprintf_s(str, format, arg);
	va_end(arg);

	dobj.push(std::make_shared<DebugString>((int)pos.x, (int)pos.y, color, font, str));
}
void DebugDraw::String3D(Vector2 pos, unsigned color, const char* format, ...)
{
	va_list arg;
	char str[256] = "";

	va_start(arg, format);
	vsprintf_s(str, format, arg);
	va_end(arg);

	VECTOR p = ConvWorldPosToScreenPos(VGet(pos.x, pos.y, 0.f));
	dobj.push(std::make_shared<DebugString>((int)p.x, (int)p.y, color, font, str));
}

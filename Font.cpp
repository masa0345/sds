#include "Font.h"
#include <DxLib.h>

std::vector<std::string> Font::fonts;

Font::Font(std::string FontName, int FontSize, int FontThick, std::string FileName, int FontType)
{
	fileName = FileName;
	fontName = FontName;
	fontSize = FontSize;
	fontThick = FontThick;
	pbFont = NULL;

	LoadFontData();

	hFont = CreateFontToHandle(fontName.c_str(), FontSize, FontThick, FontType);
}

Font::~Font()
{
	DeleteFontData();
}

//フォントを読み込む
void Font::LoadFontData()
{
	if (fileName.c_str() == NULL) return;

	//既に読み込み済みのフォントデータなら戻る
	if (!fonts.empty() && 
		std::find(fonts.begin(), fonts.end(), fontName) != fonts.end()) {
		return;
	}

	int fp = FileRead_open(fileName.c_str());
	if (fp == NULL) {
		MessageBox(NULL, "フォントの読込に失敗しました", "error", MB_OK);
		fontName = "ＭＳ ゴシック";
		fontThick = 0;
		return;
	}
	int fileSize = (int)FileRead_size(fileName.c_str());
	pbFont = new char[fileSize];
	FileRead_read(pbFont, fileSize, fp);
	FileRead_close(fp);

	DWORD fontNum = 0;
	if (AddFontMemResourceEx(pbFont, fileSize, NULL, &fontNum) == 0) {
		MessageBox(NULL, "フォントの読込に失敗しました", "error", MB_OK);
		fontName = "ＭＳ ゴシック";
		fontThick = 0;
		return;
	}
	fonts.push_back(fontName);
}

//フォントを削除する
void Font::DeleteFontData()
{
	if (pbFont != NULL) delete[] pbFont;
}

//DrawFormatStringのフォント版
void Font::DrawFontString(int x, int y, unsigned dwCol, const char* format, ...)
{
	va_list arg;
	char str[256] = "";

	va_start(arg, format);
	vsprintf_s(str, format, arg);
	va_end(arg);

	DrawFormatStringToHandle(x, y, dwCol, hFont, str);
}

int Font::GetSize()
{
	return fontSize;
}

int Font::GetThick()
{
	return fontThick;
}

int Font::GetStringWidth(const char* str)
{
	return GetDrawFormatStringWidthToHandle(hFont, str);
}
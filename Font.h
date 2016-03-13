#pragma once

#include <vector>
#include <string>

class Font
{
public:
	Font(std::string font, int size, int thick, std::string fname, int type = 0x02);
	~Font();

	void	DrawFontString(int x, int y, unsigned dwCol, const char* format, ...);

	int		GetSize();			//フォントの大きさを得る
	int		GetThick();			//フォントの太さを得る
	int		GetStringWidth(const char* str);	//文字列の幅を得る 

private:
	std::string	fileName;			//フォントファイルの名前
	std::string	fontName;			//フォント名
	int		fontThick;		//フォントの太さ
	int		fontSize;			//フォントの大きさ
	void*	pbFont;			//確保したメモリへのポインタ
	int		hFont;			//フォントハンドル

	void	LoadFontData();		//フォントデータを読み込む
	void	DeleteFontData();	//フォントデータを削除する

	static std::vector<std::string>	fonts;	//読み込み済みのフォント名
};
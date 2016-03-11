#include "Image.h"
#include <DxLib.h>

Image::Image()
{
}

Image* Image::Instance()
{
	static Image _Instance;
	return &_Instance;
}

void Image::my_exit(int fp)
{
	if (fp != NULL) FileRead_close(fp);
	MessageBox(GetMainWindowHandle(), "画像読み込みエラー", "error", MB_OK);
	exit(1);
}

//初期化　ファイル読み込み
void Image::Init(const char* fname)
{

	int fp = FileRead_open(fname);
	if (fp == NULL) my_exit();

	while (FileRead_eof(fp) == 0) {

		if (FileRead_getc(fp) == '/') {
			while (FileRead_getc(fp) != '\n');
			continue;
		}
		else {
			FileRead_seek(fp, -1, SEEK_CUR);
		}

		std::vector<int> vect;
		int g, gx, gy, tx, ty;
		char name[32], path[128];

		FileRead_scanf(fp, "%[^,],%d,%d,%s\n", name, &tx, &ty, path);

		//画像ロード
		g = LoadGraph(path);
		if (g == -1 || ty == 0 || tx == 0) my_exit(fp);

		//マップチップ読み込み
		if (strstr(name, "stage") != NULL) {
			GetGraphSize(g, &gx, &gy);
			for (int i = 0; i < gx / tx; i++)
				for (int j = 0; j < gy / ty; j++)
					vect.push_back(DerivationGraph(i * tx, j * ty, tx, ty, g));
			DeleteGraph(g);
			goto REGISTER;
		}

		if (ty < 0 || tx < 0) {
			vect.push_back(g);
		}
		else {
			//１つのサイズが tx * ty になるように分割
			GetGraphSize(g, &gx, &gy);
			for (int i = 0; i < gy / ty; i++)
				for (int j = 0; j < gx / tx; j++)
					vect.push_back(DerivationGraph(j * tx, i * ty, tx, ty, g));
			DeleteGraph(g);
		}

REGISTER:
		//mapに登録
		if ((int)images.count(name) == 0) {
			images.insert(make_pair(name, vect));
		}
		else {
			for (auto it : vect) images[name].push_back(it);
		}

	}

	FileRead_close(fp);
}

//画像をロード　グラフィックハンドルへのポインタを返す
LPHIMG Image::Load(std::string name)
{
	auto it = images.find(name);
	if (it == images.end()) return nullptr;

	return &(*it).second;
}
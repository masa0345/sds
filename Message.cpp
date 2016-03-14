#include "Message.h"
#include "Font.h"
#include "common.h"
#include "Image.h"
#include "Input.h"
#include <DxLib.h>


Message::Message(std::shared_ptr<JoypadInput> input, const char* filename)
	: input(input), fname(filename), 
	charNum(0), cnt(0), state(0), fileTell(0), msgFlag(false), wait(false)
{
	fname = "data/csv/" + fname + ".csv";
	buf.push_back("");

	x = 180;
	y = 364;
	speed = 2;
	width = 400;
	height = 3;

	frameX = 20;
	frameY = 320;
	frameW = WINDOW_WIDTH - 48;
	frameH = 140;

	setlocale(LC_CTYPE, "jpn");

	msgFont = new Font("梅PゴシックC5", 18, 4, "data/font/ume-pgc5.ttf");
	spkFont = new Font("梅PゴシックC5", 16, 4, "data/font/ume-pgc5.ttf");
}

Message::~Message()
{
	if (msgFont) delete msgFont;
	if (spkFont) delete spkFont;
}

bool Message::Display()
{
	int d;
	switch (state) {
	case 0:
		if (cnt++ == 12) {
			cnt = 0;
			state++;
		}
		d = (int)(frameH / 2 * sin(PI / 2 / 12.0*cnt));
		DrawBox(frameX, frameY + frameH / 2 - d, 
			frameX + frameW, frameY + frameH / 2 + d, 0xffffff, FALSE);
		
		break;
	case 1:
		input->SetNoInputFlag(false);
		if (!msgFlag) {
			if (!LoadMessage()) {
				//メッセージ終了
				input->SetNoInputFlag(true);
				return false;
			}
			NewPage();
		}
		msgFlag = DrawMessage();
		input->SetNoInputFlag(true);
		break;
	}
	return true;
}

int Message::GetState() const
{
	return state;
}

//メッセージを読み込む
bool Message::LoadMessage()
{
	int fp = FileRead_open(fname.c_str());
	if (fp == NULL) {
		MessageBox(GetMainWindowHandle(), "メッセージエラー", "error", MB_OK);
		return false;
	}

	char spk[64] = "", ico[32] = "", msg[512] = "";
	FileRead_seek(fp, fileTell, SEEK_SET);
	if (FileRead_eof(fp)) {
		FileRead_close(fp);
		return false;
	}

	FileRead_scanf(fp, "%[^,],%[^,],%s\n", spk, ico, msg);
	fileTell = (int)FileRead_tell(fp);

	FileRead_close(fp);

	if (strcmp(spk, "") != 0)strSpeaker = spk;
	if (strcmp(ico, "") != 0)strIcon = ico;
	strMessage = msg;

	return true;
}

//メッセージ描画
bool Message::DrawMessage()
{
	//枠
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	DrawGraph(frameX, frameY, Image::Instance()->Load("message")->at(0), TRUE);

	if (strIcon != "NULL")
		DrawGraph(frameX + 12, frameY + 10, Image::Instance()->Load(strIcon)->at(0), FALSE);

	//文字
	//ボタンを押したとき
	if (input->Get(INPUT_Z) == 1) {
		if (wait) {
			//待機中なら待機解除
			wait = false;
			if (charNum == (int)strMessage.size()) return false;
			NewLine();
		} else {
			//そうでなければ待機状態まで飛ばす
			while (!wait) PutMessage();
		}
	}
	if (cnt % speed == 0) PutMessage();

	int i = 0;
	for (auto it = buf.begin(); it != buf.end(); it++, i++) {
		msgFont->DrawFontString(x, y + i * 32, 0xffffff, (*it).c_str());
	}
	spkFont->DrawFontString(x, y - 32, 0xffffff, strSpeaker.c_str());

	return true;
}

void Message::PutMessage()
{
	if (charNum == (int)strMessage.size() || wait) {
		wait = true;
		return;
	}

	int byte = mblen(&strMessage[charNum], MB_CUR_MAX);

	switch (strMessage[charNum]) {
	case '\\':
		wait = true;
		charNum++;
		break;
	default:
		if (msgFont->GetStringWidth(buf.back().c_str()) >= width) {
			NewLine();
		}
		for (int i = 0; i < byte; i++) {
			buf.back() += strMessage[charNum++];
		}
		break;
	}
}

//改行
void Message::NewLine()
{
	buf.push_back("");
	if ((int)buf.size() > height) {
		buf.pop_front();
	}
}

//改ページ
void Message::NewPage()
{
	buf.clear();
	buf.push_back("");
	charNum = 0;
}

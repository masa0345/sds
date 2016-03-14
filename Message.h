#pragma once

#include <list>
#include <string>
#include <memory>

class Font;
class JoypadInput;

class Message
{
public:
	Message(std::shared_ptr<JoypadInput> input, const char* filename);
	~Message();
	bool Display();
	int GetState() const;

private:
	std::list<std::string>	buf;
	std::string			strMessage;
	std::string			strSpeaker;
	std::string			strIcon;

	Font*	msgFont;
	Font*	spkFont;

	int		x, y;			//文字列の右上座標
	int		width, height;	//文字列の縦幅と列数
	int		frameX, frameY;	//枠の座標
	int		frameW, frameH;	//枠の大きさ

	bool	msgFlag;			//メッセージ表示中フラグ
	int		charNum;			//メッセージの総文字数
	bool	wait;				//待機中フラグ
	int		cnt, state;
	int		speed;

	std::string	fname;		//メッセージのファイル名
	int		fileTell;		//読み込み済みのファイルポインタの位置

	std::shared_ptr<JoypadInput> input;

	//void	Init();
	bool	LoadMessage();

	bool	DrawMessage();
	void	PutMessage();
	void	NewLine();
	void	NewPage();
};
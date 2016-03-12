#pragma once

#include <string>
#include <unordered_map>

struct SoundData
{
	int		hSnd;		//�T�E���h�n���h��
	int		LoopPos;	//���[�v�ʒu
	std::string	fname;		//�t�@�C���p�X
	int		playType;	//�Đ��`��
	bool	isSE;		//SE��BGM
};

struct FADE
{
	bool flag;
	std::string name;
	int time;
	int count;
	double vol;
};

class Sound
{
private:
	Sound();
	Sound(const Sound&);
	Sound& operator=(const Sound&);
public:
	static Sound* Instance();

private:
	std::unordered_map<std::string, SoundData> mapSnd;
	int		BGMVol, SEVol;
	FADE	fade;

	void	LoadSE(char* fname);
	void	LoadBGMData(char* fname);
	void	FadeOut();

public:
	void	Init();
	void	LoadBGM(std::string name);
	void	Play(std::string name, int flag = 1);
	void	Stop(std::string name);
	void	StopBGM();
	void	StopAll();
	void	FadeOut(std::string name, int time = 180);

	int		GetVolume(bool isSE);
	void	ChangeVolume(double rate);
	void	ChangeSEVolume(int vol);
	void	ChangeBGMVolume(int vol);
	bool	IsPlaying(std::string name);

	void	UpdateState();
};
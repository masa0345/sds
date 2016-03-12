#include "Sound.h"
#include <DxLib.h>
#include <cassert>

Sound::Sound() : BGMVol(255), SEVol(255)
{
}

Sound* Sound::Instance()
{
	static Sound _Instance;
	return &_Instance;
}

//SE��ǂݍ���
void Sound::LoadSE(char* fname)
{
	int fp = FileRead_open(fname);
	if (fp == NULL) return;

	while (FileRead_eof(fp) == 0) {

		if (FileRead_getc(fp) == '/') {
			while (FileRead_getc(fp) != '\n');
		}
		else {
			FileRead_seek(fp, -1, SEEK_CUR);
		}

		char name[32], path[128];

		FileRead_scanf(fp, "%[^,],%s\n", name, path);

		int hSnd = LoadSoundMem(path);

		SoundData sd = { hSnd, -1, path, DX_PLAYTYPE_BACK, true };

		//map�ɓo�^
		mapSnd.insert({ name, sd });
	}

	FileRead_close(fp);
}

//BGM�̏���ǂݍ��ށ@LoadSoundMem�͂��Ȃ�
void Sound::LoadBGMData(char* fname)
{
	int fp = FileRead_open(fname);
	if (fp == NULL) return;

	while (FileRead_eof(fp) == 0) {

		if (FileRead_getc(fp) == '/') {
			while (FileRead_getc(fp) != '\n');
		}
		else {
			FileRead_seek(fp, -1, SEEK_CUR);
		}

		int LPos = 0;
		char name[32], path[128];

		FileRead_scanf(fp, "%[^,],%d,%s\n", name, &LPos, path);

		SoundData sd = { -1, LPos, path, DX_PLAYTYPE_LOOP, false };

		//map�ɓo�^
		mapSnd.insert({ name, sd });
	}

	FileRead_close(fp);
}

//BGM��ǂݍ���
void Sound::LoadBGM(std::string name)
{
	//�ǂݍ��ݍς݂Ȃ�return
	if (mapSnd[name].hSnd != -1) return;
	mapSnd[name].hSnd = LoadSoundMem(mapSnd[name].fname.c_str());
	if (mapSnd[name].hSnd == -1) return;
	if (mapSnd[name].LoopPos > 0)
		SetLoopPosSoundMem(mapSnd[name].LoopPos, mapSnd[name].hSnd);

}

//�������@SE�A���ʂȂ�
void Sound::Init()
{
	fade.flag = false;
	LoadSE("data/sound/se.csv");
	LoadBGMData("data/sound/bgm.csv");
}

//�Đ�
void Sound::Play(std::string name, int flag)
{
	if (mapSnd[name].hSnd == -1) return;
	ChangeVolumeSoundMem(mapSnd[name].isSE ? SEVol : BGMVol, mapSnd[name].hSnd);

	//����
	/*
	if(CheckSoundMem(mapSnd[name].hSnd)){
	int DupSnd = DuplicateSoundMem( mapSnd[name].hSnd );
	PlaySoundMem(DupSnd, mapSnd[name].playType, flag);
	}*/

	PlaySoundMem(mapSnd[name].hSnd, mapSnd[name].playType, flag);
}

bool Sound::IsPlaying(std::string name)
{
	if (mapSnd[name].hSnd == -1) return false;
	return CheckSoundMem(mapSnd[name].hSnd) == 1;
}

//��~
void Sound::Stop(std::string name)
{
	if (mapSnd[name].hSnd == -1) return;
	StopSoundMem(mapSnd[name].hSnd);
}

//���ݍĐ�����BGM�݂̂����ׂĒ�~
void Sound::StopBGM()
{
	for (auto it : mapSnd) {
		if (it.second.isSE || it.second.hSnd == -1) continue;
		if (CheckSoundMem(it.second.hSnd)) StopSoundMem(it.second.hSnd);
	}
}

//�S�Ē�~
void Sound::StopAll()
{
	for (auto it : mapSnd) {
		if (it.second.hSnd == -1) continue;
		if (CheckSoundMem(it.second.hSnd)) StopSoundMem(it.second.hSnd);
	}
}

//�t�F�[�h�A�E�g
void Sound::FadeOut(std::string name, int time)
{
	if (fade.flag) return;

	if (time <= 0) time = 1;
	fade.flag = true;
	fade.time = time;
	fade.vol = BGMVol;
	fade.count = 0;

	if (name == "default") {
		for (auto it : mapSnd) {
			if (it.second.isSE || it.second.hSnd == -1) continue;
			if (CheckSoundMem(it.second.hSnd)) {
				fade.name = it.first;
				return;
			}
		}
	}
	else {
		fade.name = name;
	}
}
void Sound::FadeOut()
{
	if (!fade.flag) return;
	fade.vol -= (double)BGMVol / fade.time;
	if (fade.count++ == fade.time) {
		fade.vol = 0;
		fade.flag = false;
		Stop(fade.name);
		return;
	}
	if (fade.vol < 0.0) fade.vol = 0.0;
	ChangeVolumeSoundMem((int)fade.vol, mapSnd[fade.name].hSnd);

#ifdef _DEBUG
	printfDx("fade.volume = %f.2", fade.vol);
#endif
}

//���ʂ�ύX
void Sound::ChangeVolume(double rate)
{
	assert(rate > 0);

	//ChangeSEVolume((int)(SEVol * rate));
	ChangeBGMVolume((int)(BGMVol * rate));
}
void Sound::ChangeSEVolume(int vol)
{
	SEVol = vol;
	for (auto it : mapSnd) {
		if (!it.second.isSE) continue;
		if (CheckSoundMem(it.second.hSnd)) ChangeVolumeSoundMem(SEVol, it.second.hSnd);
	}
}
void Sound::ChangeBGMVolume(int vol)
{
	BGMVol = vol;
	for (auto it : mapSnd) {
		if (it.second.isSE) continue;
		if (CheckSoundMem(it.second.hSnd)) ChangeVolumeSoundMem(BGMVol, it.second.hSnd);
	}
}

//���݂̉��ʂ𓾂�
int Sound::GetVolume(bool isSE)
{
	return isSE ? SEVol : BGMVol;
}

//�X�V����
void Sound::UpdateState()
{
	FadeOut();
}
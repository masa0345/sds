#include "Save.h"
#include "Stage.h"
#include <DxLib.h>
#include <WinCrypt.h>

namespace {
	struct StageScore
	{
		bool clear;
		int clearTime;
		int highScore;
	};

	struct SaveData
	{
		StageScore stageData[STAGE_MAX];
		byte pcbData[16];
	} save;

	//MD5ハッシュを計算する
	bool GetMD5Hash(const void* pData, DWORD dwLen, BYTE pcbHashData[16])
	{
		bool		ret;
		HCRYPTPROV	hCryptProv;
		HCRYPTHASH	hHash;
		BYTE		pbHash[0x7f];
		DWORD		dwHashLen;

		::ZeroMemory(pcbHashData, 16);
		if (pData == NULL || dwLen == 0)
			return	false;

		dwHashLen = 16;
		hHash = NULL;
		hCryptProv = NULL;
		ret = false;
		if (::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
		{
			if (::CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
			{
				if (::CryptHashData(hHash, (BYTE*)pData, dwLen, 0))
				{
					if (::CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))
					{
						::memcpy_s(pcbHashData, dwHashLen, pbHash, dwHashLen);
						ret = true;
					}
				}
			}
		}

		if (hHash)
			::CryptDestroyHash(hHash);
		if (hCryptProv)
			::CryptReleaseContext(hCryptProv, 0);

		if (ret == false)
			::ZeroMemory(pcbHashData, dwHashLen);

		return	ret;
	}
}

Save::Save()
{
}

Save* Save::Instance()
{
	static Save _Instance;
	return &_Instance;
}

void Save::LoadDefaultData()
{
	for (int i = 0; i < STAGE_MAX; ++i) {
		save.stageData[i].clear = false;
		save.stageData[i].clearTime = -1;
		save.stageData[i].highScore = 0;
	}
}

bool Save::LoadSaveData()
{
	FILE* fp;
	hasSaveData = false;
	if (fopen_s(&fp, "save.dat", "rb") != 0) {
		LoadDefaultData();
		return false;
	}

	fread(&save, sizeof(SaveData), 1, fp);
	fclose(fp);

	BYTE prevData[16];

	for (int i = 0; i < 16; i++) {
		prevData[i] = save.pcbData[i];
	}

	if (GetMD5Hash(&save, sizeof(SaveData), save.pcbData) == false) {
		MessageBox(GetMainWindowHandle(), "セーブデータ取得に失敗しました", "HashError", MB_OK);
		LoadDefaultData();
		return false;
	}

	for (int i = 0; i < 16; i++) {
		if (prevData[i] != save.pcbData[i]) {
			MessageBox(GetMainWindowHandle(), "セーブデータ取得に失敗しました", "SaveDataError", MB_OK);
			LoadDefaultData();
			return false;
		}
	}
	hasSaveData = true;
	return true;
}

void Save::SaveScore()
{
	if (GetMD5Hash(&save, sizeof(SaveData), save.pcbData) == false) {
		return;
	}

	FILE* fp;
	if (fopen_s(&fp, "save.dat", "wb") != NULL) {
		return;
	}

	fwrite(&save, sizeof(SaveData), 1, fp);
	fclose(fp);
}

int Save::SaveStageResult(std::shared_ptr<Stage> stage)
{
	hasSaveData = true;
	if (stage->GetClear()) {
		int num = stage->GetStageNum();
		int score = stage->GetScore();
		int time = stage->GetTimer();
		save.stageData[num].clear = stage->GetClear();
		if (save.stageData[num].highScore < score) {
			save.stageData[num].highScore = score;
			save.stageData[num].clearTime = time;
			return 1;
		}
	}
	return 0;
}

bool Save::GetClearFlag(int stageNum)
{
	return save.stageData[stageNum].clear;
}

int Save::GetClearTime(int stageNum)
{
	return save.stageData[stageNum].clearTime;
}

int Save::GetHighScore(int stageNum)
{
	return save.stageData[stageNum].highScore;
}

bool Save::HasSaveData()
{
	return hasSaveData;
}
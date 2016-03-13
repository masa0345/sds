#pragma once

#include "common.h"
#include <memory>

class Stage;

class Save
{
private:
	Save();
	Save(const Save&);
	Save& operator=(const Save&);

public:
	static Save* Instance();

private:
	bool	hasSaveData;

public:
	void	LoadDefaultData();	//初期状態のデータを読み込む
	bool	LoadSaveData();	//save.datを読み込む
	void	SaveScore();		//データを保存する
	int		SaveStageResult(std::shared_ptr<Stage> s);	//ステージクリア時のデータを保存

	bool	GetClearFlag(int stageNum);
	int		GetClearTime(int stageNum);
	int		GetHighScore(int stageNum);
	bool	HasSaveData();
};
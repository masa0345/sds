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
	void	LoadDefaultData();	//������Ԃ̃f�[�^��ǂݍ���
	bool	LoadSaveData();	//save.dat��ǂݍ���
	void	SaveScore();		//�f�[�^��ۑ�����
	int		SaveStageResult(std::shared_ptr<Stage> s);	//�X�e�[�W�N���A���̃f�[�^��ۑ�

	bool	GetClearFlag(int stageNum);
	int		GetClearTime(int stageNum);
	int		GetHighScore(int stageNum);
	bool	HasSaveData();
};
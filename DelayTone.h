#pragma once
#include <windows.h>
#include "general.h"
class CDelayTone
{
private:
	CDelayTone(void);
	int delay;
	bool changed;
	static CDelayTone* instance;
	CRITICAL_SECTION	lock;
public:
	~CDelayTone(void);
	static CDelayTone* GetInstance();
	void SetDelay(int d);
	int GetDelay();
	bool GetChanged(){return changed;}
};

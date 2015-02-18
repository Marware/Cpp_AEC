//#include "StdAfx.h"
#include "DelayTone.h"

CDelayTone* CDelayTone::instance = NULL;

CDelayTone::CDelayTone(void)
{
	delay = 0;
	changed = false;
	InitializeCriticalSection(&lock);
}

CDelayTone::~CDelayTone(void)
{
	DeleteCriticalSection(&lock);
	instance = NULL;
}

 CDelayTone* CDelayTone::GetInstance()
{
	if(instance == NULL)
	{
		instance = new CDelayTone();
	}
	return instance;
}

void CDelayTone::SetDelay(int d)
{
	// Get critical section
	EnterCriticalSection(&lock);
	delay = d;
	changed = true;
	// Release critical section.
	LeaveCriticalSection(&lock);
}

int CDelayTone::GetDelay()
{

	// Get critical section
	EnterCriticalSection(&lock);
	changed = false;
	// Release critical section
	LeaveCriticalSection(&lock);
	return delay;
}

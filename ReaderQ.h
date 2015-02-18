#pragma once
#include "general.h"
class CWriterQ;
class CReaderQ
{
public:
	 CReaderQ(void);
	~CReaderQ(void);
	void RemoveFromQueue(uint8* buffer,uint32 size);
	void Connect(CWriterQ* param);
	uint32 GetFilledSize(void);
	uint8 GetQuantumSize(void);
	void EmptyQueue(void);
private:
	CWriterQ* pWriterQ;
	uint32 readIndex;
};

#include <windows.h>
#include "ReaderQ.h"
#include "WriterQ.h"
#include "general.h"

CReaderQ::CReaderQ(void)
{
	pWriterQ = NULL;
}

CReaderQ::~CReaderQ(void)
{
}

void CReaderQ::Connect(CWriterQ* param)
{
	pWriterQ = param;
	readIndex = 0;
}


uint32 CReaderQ::GetFilledSize(void)
{
	uint32 writeIndex;
	uint32 queueSize;
	if(pWriterQ)
	{
		writeIndex = pWriterQ->GetCurIndex();
		queueSize  = pWriterQ->GetQueueSize();
		if(writeIndex >= readIndex )
			return writeIndex - readIndex;
		return queueSize - readIndex + writeIndex;
	}
	return 0;
}

uint8 CReaderQ::GetQuantumSize(void)
{
	if(pWriterQ)
		return pWriterQ->GetQuantumSize();
	return 0;
}

void CReaderQ::EmptyQueue(void)
{
	if(pWriterQ)
		readIndex = pWriterQ->GetCurIndex();
}



void CReaderQ::RemoveFromQueue(uint8* buffer,uint32 size)
{
	if(pWriterQ)
	{
		pWriterQ->RemoveFromQueue(buffer,readIndex,size);
		readIndex	+=	size							;
		readIndex	%=	pWriterQ->GetQueueSize()		;
	}
}

#include "WriterQ.h"
#include <windows.h>

CWriterQ::CWriterQ( uint8  seconds	   ,
					uint32 sps		   ,
					uint8  quantumSize)
{
	Quantum_Size = quantumSize							;
	QueueSize	 = seconds*sps							;
	writeIndex   =			0							;
	pointer		 = new uint8 [QueueSize*Quantum_Size]	;
}

CWriterQ::~CWriterQ(void)
{
	delete [] pointer;
}

void CWriterQ::Add2Queue(uint8* buffer,uint32 size)
{
	uint32 remaining;
	if(writeIndex + size < QueueSize)
	{
		memcpy(pointer+(Quantum_Size*writeIndex),buffer,size*Quantum_Size);
	}
	else
	{
		remaining = QueueSize - writeIndex;
		memcpy(pointer+(Quantum_Size*writeIndex),buffer,remaining*Quantum_Size);
		memcpy(pointer,buffer+(remaining*Quantum_Size),(size-remaining)*Quantum_Size);
	}
	writeIndex += size		;
	writeIndex %= QueueSize	;
}

void CWriterQ::RemoveFromQueue(uint8* buffer,uint32 index,uint32 size)
{
	uint32 remaining;
	if( index + size < QueueSize )
	{
		memcpy(buffer,pointer+(index*Quantum_Size),size*Quantum_Size);
	}
	else
	{
		remaining = QueueSize - index;
		memcpy(buffer,pointer+(index*Quantum_Size),remaining*Quantum_Size);
		memcpy(buffer+(remaining*Quantum_Size),pointer,(size-remaining)*Quantum_Size);
	}
}

#pragma once
#include "general.h"

class CWriterQ
{
public:
	 CWriterQ(	uint8  seconds	   ,
				uint32 sps		   ,// samples per second
				uint8  quantumSize 
			 );
	void	Add2Queue(uint8* buffer,uint32 size);
	void	RemoveFromQueue(uint8* buffer,uint32 index,uint32 size);
	uint32  GetCurIndex()   {return writeIndex;		}
	uint32  GetQueueSize()  {return QueueSize;		}
	uint8   GetQuantumSize(){return Quantum_Size;	}
	~CWriterQ(void);
private:
	uint8*		pointer;
	uint8		Quantum_Size;
	uint32		writeIndex;
	uint8		seconds;
	uint32		samplesPerSeconds;
	uint32		QueueSize;
};

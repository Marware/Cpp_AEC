#include <windows.h>
#include <mmsystem.h>
#define MAX_INPUT_BUFFERS   2
#include "WriterQ.h"
/*
This class is a simple wave in device which
captures sound data from input microphones.
*/
class CWaveIn
{
public:
			CWaveIn			(void);
			~CWaveIn		(void);
	int		GetDeviceCount	(void);
	int		GetDeviceCaps	(int deviceNo,PWAVEINCAPS pWaveParams);
	int		OpenDevice		(	 WORD	 deviceNo,
								DWORD	 samplesPerSecond,
								 WORD	 nChannels,
								 WORD    wBitsPerSample
							);
	int		StartCapture	(float ratio);
	int		StopCapture		(void);
	int		ProcessHeader	(WAVEHDR * pHdr);
	CWriterQ* pWriterQ;
private:
	WAVEFORMATEX			m_stWFEX					 ;
	HWAVEIN					m_hWaveIn					 ;
	WAVEHDR					m_stWHDR[MAX_INPUT_BUFFERS]	 ;
	int						PrepareBuffers(float)		 ;
	int						UnPrepareBuffers(void)		 ;
	uint32					write_size					 ;
	int						wBytesPerSample				 ;
};

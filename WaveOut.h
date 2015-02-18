#pragma		once
#include    <windows.h>
#include	<mmsystem.h>
#include	"ReaderQ.h"
#include	"speex/speex_preprocess.h"
#include	"speex/speex_echo.h"



#define		MAX_OUTPUT_BUFFERS   2

class CWaveOut
{
public:
	CWaveOut(void);
public:
	~CWaveOut(void);
public:
	int GetDeviceCount(void);
public:
	int GetDeviceCaps(int deviceNo,PWAVEOUTCAPS pWaveParams);
	int ProcessHeader(WAVEHDR * pHdr);
	int StartPlay(float);
	int StopPlay(void);
	CReaderQ* pReaderQ;
public:
	int	OpenDevice		(		WORD	 deviceNo,
								DWORD	 samplesPerSecond,
								WORD	 nChannels,
								WORD     wBitsPerSample
						);
private:
	WAVEFORMATEX			m_stWFEX					 ;
	HWAVEOUT				m_hWaveOut					 ;
	WAVEHDR					m_stWHDR[MAX_OUTPUT_BUFFERS] ;

	int		PrepareBuffers(float)	;
	int		UnprepareBuffers(void)	;
	uint32	read_size				;
	int		wBytesPerSample			;
	uint8*	buffer					;
	SpeexPreprocessState *st		;
	SpeexEchoState *echo_state		;
	short*  echo_buffer				;
	short*	input_buffer			;
};

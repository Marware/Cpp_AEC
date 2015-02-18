#include "WaveOut.h"
#include "general.h"
#include "DelayTone.h"
#include <math.h>

#pragma comment(lib,"winmm.lib")

void CALLBACK waveOutFunc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
		case WOM_CLOSE:
			break;

		case WOM_DONE:
			{
				CWaveOut *pDlg=(CWaveOut*)dwInstance;
				pDlg->ProcessHeader((WAVEHDR *)dwParam1);
			}
			break;

		case WOM_OPEN:
			break;

		default:
			break;
	}
}

CWaveOut::CWaveOut(void)
{
	read_size	= 0		;
	wBytesPerSample = 0 ;
	m_hWaveOut	= NULL	;
	pReaderQ	= NULL	;
	st			= NULL	;
	echo_state  = NULL	;
	echo_buffer	= NULL	;
	input_buffer= NULL	;

	ZeroMemory(&m_stWFEX,sizeof(WAVEFORMATEX));
	ZeroMemory( m_stWHDR,MAX_OUTPUT_BUFFERS*sizeof(WAVEHDR));
}

CWaveOut::~CWaveOut(void)
{
	StopPlay();
	if(pReaderQ)
		delete pReaderQ;
	pReaderQ = NULL;
	if(st)
	{
		 speex_preprocess_state_destroy(st);
		 st = NULL;
	}
	if(echo_state)
	{
		speex_echo_state_destroy(echo_state);		
		echo_state = NULL;
	}
	if(echo_buffer)
	{
		delete echo_buffer;
		echo_buffer = NULL;
	}
	if(input_buffer)
	{
		delete input_buffer;
		input_buffer = NULL;
	}
}

int CWaveOut::GetDeviceCount(void)
{
	return waveOutGetNumDevs();
}

int CWaveOut::GetDeviceCaps(int deviceNo,PWAVEOUTCAPS pWaveParams)
{
	return waveOutGetDevCaps(deviceNo,pWaveParams,sizeof(WAVEOUTCAPS));
}

int CWaveOut::OpenDevice( WORD	 deviceNo,
						  DWORD	 samplesPerSecond,
						  WORD	 nChannels,
						  WORD   wBitsPerSample
					    )
{

	if(m_hWaveOut)// Device is open now.
		return FAILURE;

	MMRESULT mRes			 =	0					;
	m_stWFEX.nSamplesPerSec  =	samplesPerSecond	;
	m_stWFEX.nChannels	     =	nChannels			;
	m_stWFEX.wBitsPerSample  =	wBitsPerSample		;
	m_stWFEX.wFormatTag		 =	WAVE_FORMAT_PCM		;
	m_stWFEX.nBlockAlign	 =	m_stWFEX.nChannels*m_stWFEX.wBitsPerSample/8;
	m_stWFEX.nAvgBytesPerSec =	m_stWFEX.nSamplesPerSec*m_stWFEX.nBlockAlign;
	m_stWFEX.cbSize			 =	sizeof(WAVEFORMATEX);
	pReaderQ				 =  new CReaderQ()		;


	mRes = waveOutOpen(
						&m_hWaveOut,
						deviceNo,
						&m_stWFEX,
						(DWORD_PTR)waveOutFunc,
						(DWORD_PTR)this,
						CALLBACK_FUNCTION
					  );
	return mRes;
}

int CWaveOut::ProcessHeader(WAVEHDR * pHdr)
{
	MMRESULT mRes	=	0	;
	int vad;

	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{
		memset(input_buffer,0,read_size*wBytesPerSample);
		while(pReaderQ->GetFilledSize() >= read_size)
		{
			pReaderQ->RemoveFromQueue((uint8*)input_buffer,read_size);
		}
		speex_echo_cancellation(echo_state, input_buffer, echo_buffer, (spx_int16_t *)pHdr->lpData);
		vad = speex_preprocess_run(st, (spx_int16_t *)pHdr->lpData);

		mRes=waveOutWrite(m_hWaveOut,pHdr,sizeof(WAVEHDR));
		memcpy(echo_buffer,pHdr->lpData,read_size*2);
	}
	return SUCCESS;
}

int CWaveOut::StartPlay(float ratio)
{
	MMRESULT res=0;
	PrepareBuffers(ratio);
	for(int i=0; i < MAX_OUTPUT_BUFFERS ; i++)
	{
		res=waveOutWrite(m_hWaveOut,&m_stWHDR[i],sizeof(WAVEHDR));
		if ( res != MMSYSERR_NOERROR )
			return FAILURE;
	}
	return SUCCESS;
}

int CWaveOut::StopPlay(void)
{
	MMRESULT mRes=0;
	if(m_hWaveOut)
	{
		UnprepareBuffers();
		mRes=waveOutClose(m_hWaveOut);

		m_hWaveOut = NULL;
		return SUCCESS;
	}
	return FAILURE;
}

int CWaveOut::PrepareBuffers(float ratio)
{
	int			i		;
	float		f		;
	MMRESULT	mRes=0	;
	int			nT1=0	;
	double		lg		;
	int			c_size	;

	for(nT1=0;nT1<MAX_OUTPUT_BUFFERS;++nT1)
	{
		c_size = (SIZE_T)m_stWFEX.nAvgBytesPerSec*ratio;

		lg		= log10((double)c_size)/log10((double)2);
		lg		= ceil(lg);
		c_size	= pow(2,lg);


		m_stWHDR[nT1].lpData=(LPSTR)HeapAlloc(GetProcessHeap(),8,c_size);
		m_stWHDR[nT1].dwBufferLength=c_size	;
		m_stWHDR[nT1].dwUser		=				nT1						;
		mRes=waveOutPrepareHeader(m_hWaveOut,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			return FAILURE;
		}
	}
	echo_state = speex_echo_state_init(c_size/2, m_stWFEX.nSamplesPerSec/10);
	st = speex_preprocess_state_init(c_size/2, m_stWFEX.nSamplesPerSec);
	i=1;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);
    i=0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);
    i=8000;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
    i=0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    f=.0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=.0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_ECHO_STATE,echo_state);

	wBytesPerSample = m_stWFEX.wBitsPerSample/8;
	read_size = (SIZE_T)c_size/wBytesPerSample;

	echo_buffer		= new short[read_size]		;
	memset(echo_buffer,0,read_size*sizeof(short));
	input_buffer	= new short[read_size]		;
	memset(input_buffer,0,read_size*sizeof(short));


	return SUCCESS;
}

int CWaveOut::UnprepareBuffers(void)
{
	MMRESULT mRes=0;
	int nT1=0;

	if(m_hWaveOut)
	{
		mRes=waveOutPause(m_hWaveOut);		
//		Sleep(300);
		for(nT1=0;nT1<MAX_OUTPUT_BUFFERS;++nT1)
		{
			if(m_stWHDR[nT1].lpData)
			{
				mRes=waveOutUnprepareHeader(m_hWaveOut,&m_stWHDR[nT1],sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(),0,m_stWHDR[nT1].lpData);
				ZeroMemory(&m_stWHDR[nT1],sizeof(WAVEHDR));
			}
		}
	}
	return SUCCESS;
}

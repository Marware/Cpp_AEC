#include "WaveIn.h"
#include "general.h"
#include "DelayTone.h"
#include <math.h>

void CALLBACK waveInFunc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
		case WIM_CLOSE:
			break;

		case WIM_DATA:
			{
				CWaveIn *pDlg=(CWaveIn*)dwInstance;
				pDlg->ProcessHeader((WAVEHDR *)dwParam1);
			}
			break;

		case WIM_OPEN:
			break;

		default:
			break;
	}
}

CWaveIn::CWaveIn(void)
{
		write_size		= 0		;
		wBytesPerSample = 0		;
		m_hWaveIn		= NULL	;
		pWriterQ		= NULL	;
		ZeroMemory(&m_stWFEX,sizeof(WAVEFORMATEX));
		ZeroMemory(m_stWHDR,MAX_INPUT_BUFFERS*sizeof(WAVEHDR));
}


CWaveIn::~CWaveIn(void)
{
	StopCapture();
	if(pWriterQ)
		delete pWriterQ;
	pWriterQ  = NULL;
}

int CWaveIn::GetDeviceCount(void)
{
	return waveInGetNumDevs();
}


int CWaveIn::GetDeviceCaps(int deviceNo,PWAVEINCAPS pWaveParams)
{
	return waveInGetDevCaps(deviceNo,pWaveParams,sizeof(WAVEINCAPS));
}

int CWaveIn::OpenDevice( WORD	 deviceNo,
						 DWORD	 samplesPerSecond,
						 WORD	 nChannels,
						 WORD    wBitsPerSample
					   )
{
	if(m_hWaveIn)// Device is open now.
		return FAILURE;
	if(pWriterQ ==NULL)
	{
		pWriterQ = new CWriterQ(25,samplesPerSecond,wBitsPerSample/8);
	}
	MMRESULT mRes				=	0					;
	m_stWFEX.nSamplesPerSec		=	samplesPerSecond	;
	m_stWFEX.nChannels			=	nChannels			;
	m_stWFEX.wBitsPerSample		=	wBitsPerSample		;
	m_stWFEX.wFormatTag			=	WAVE_FORMAT_PCM		;
	m_stWFEX.nBlockAlign		=	m_stWFEX.nChannels*m_stWFEX.wBitsPerSample/8;
	m_stWFEX.nAvgBytesPerSec	=	m_stWFEX.nSamplesPerSec*m_stWFEX.nBlockAlign;
	m_stWFEX.cbSize				=	sizeof(WAVEFORMATEX);
	mRes=waveInOpen(&m_hWaveIn,
					deviceNo,
					&m_stWFEX,
					(DWORD_PTR)waveInFunc,
					(DWORD_PTR)this,
					CALLBACK_FUNCTION);
	return mRes;
}

int CWaveIn::ProcessHeader(WAVEHDR * pHdr)
{
//	CDelayTone* pDelayTone = CDelayTone::GetInstance();
	MMRESULT mRes=0;

//	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{
		pWriterQ->Add2Queue((uint8*)pHdr->lpData,write_size);
		mRes=waveInAddBuffer(m_hWaveIn,pHdr,sizeof(WAVEHDR));
	}
	return SUCCESS;
}

int CWaveIn::PrepareBuffers(float ratio)
{
	MMRESULT mRes=0;
	int nT1=0;
	double lg;
	int c_size;

	for(nT1=0;nT1<MAX_INPUT_BUFFERS;++nT1)
	{
		c_size	= (SIZE_T)m_stWFEX.nAvgBytesPerSec*ratio;
		lg		= log10((double)c_size)/log10((double)2);
		lg		= ceil(lg);
		c_size	= pow(2,lg);

		m_stWHDR[nT1].lpData=(LPSTR)HeapAlloc(GetProcessHeap(),8,(SIZE_T)c_size);
		m_stWHDR[nT1].dwBufferLength		=(DWORD)c_size						;
		wBytesPerSample						= m_stWFEX.wBitsPerSample/8			;
		write_size							= c_size/wBytesPerSample			;


		m_stWHDR[nT1].dwUser		= nT1									;
		mRes=waveInPrepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR))	;
		if(mRes!=0)
		{
			return FAILURE;
		}
		mRes=waveInAddBuffer(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}

int CWaveIn::UnPrepareBuffers()
{
	MMRESULT mRes=0;
	int nT1=0;

	if(m_hWaveIn)
	{
		mRes=waveInStop(m_hWaveIn);
//		Sleep(300);
		for(nT1=0;nT1<MAX_INPUT_BUFFERS;++nT1)
		{
			if(m_stWHDR[nT1].lpData)
			{
				mRes=waveInUnprepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(),0,m_stWHDR[nT1].lpData);
				ZeroMemory(&m_stWHDR[nT1],sizeof(WAVEHDR));
			}
		}
	}
	return SUCCESS;
}


int CWaveIn::StartCapture(float bufferSizeRatio)
{
	MMRESULT mRes=0;
	if(m_hWaveIn == NULL)
		return FAILURE;
	if(PrepareBuffers(bufferSizeRatio)==FAILURE)// failure in preparing buffers.
		return FAILURE;
	mRes=waveInStart(m_hWaveIn);
	return mRes;
}

int CWaveIn::StopCapture(void)
{
	MMRESULT mRes=0;
	if(m_hWaveIn)
	{
		UnPrepareBuffers();
		mRes=waveInClose(m_hWaveIn);
		m_hWaveIn = NULL;
		return SUCCESS;
	}
	return FAILURE;
}

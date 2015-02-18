// AECDlg.cpp : implementation file
//

#include <windows.h>
#include "general.h"
#include "AECDlg.h"
#include "WaveIn.h"
#include "WaveOut.h"
#include "delayTone.h"
#include <QDebug>

#define PROGRAM_MAJOR_VERSION	 1
#define PROGRAM_MINOR_VERSION	 0


int		__argc	;
char**	__argv	;


// CAECDlg message handlers

bool CAECDlg::aecStart()
{
    pWaveIn = new CWaveIn();
	pWaveOut= new CWaveOut();

    started= false;
    aecRun();
    return true;  // return TRUE  unless you set the focus to a control
}

int CAECDlg::OpenInputDevice(void)
{
    MMRESULT	mRes =	0	;

    mRes = pWaveIn->OpenDevice(0,22050,1,16);
	if(mRes!=MMSYSERR_NOERROR)
	{
		return FAILURE;
	}
	return SUCCESS;
}

int CAECDlg::OpenOutputDevice(void)
{
    MMRESULT	mRes=	0	;
    mRes = pWaveOut->OpenDevice(0,22050,1,16);
	if(mRes!=MMSYSERR_NOERROR)
	{
		return FAILURE;
    }
	return SUCCESS; 
}

void CAECDlg::aecRun()
{
    if(!started)
    {
        if(OpenInputDevice()==FAILURE)
        {
            qWarning("Problem opening input device, returning...","Error");
            return;
        }
        if(pWaveIn->StartCapture(CHUNK_RATIO)!= SUCCESS)
        {
            qWarning("Problem starting input device, returning...","Error");
            return;
        }

        if(OpenOutputDevice()==FAILURE)
        {
            qWarning("Problem opening output device, returning...","Error");
            return;
        }
        if(pWaveOut->StartPlay(CHUNK_RATIO)!= SUCCESS)
        {
            qWarning("Problem starting input device, returning...","Error");
            return;
        }
        pWaveOut->pReaderQ->Connect(pWaveIn->pWriterQ);
        started = true;
    }
    else
    {
        qWarning("Program is active now","Warning");
    }
}

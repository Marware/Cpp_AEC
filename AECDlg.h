// AECDlg.h : header file
//

class CWaveIn;
class CWaveOut;
class CAECDlg
{
// Construction
public:
    bool aecStart();
    void aecRun();

private:
	bool		delay_changed;
	bool		started;
	CWaveIn*	pWaveIn;
	CWaveOut*	pWaveOut;
	int			OpenInputDevice(void);
	int			OpenOutputDevice(void);
};

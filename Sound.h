#ifndef INCLUDE_SOUND_H
#define INCLUDE_SOUND_H


#include "bass.h"

class Sound
{
public:
	Sound()
	{
		m_inited = false;
		m_chan = 0;
		if (!(m_inited = init()))
			printf("faild play music");
	}

	~Sound()
	{
		term();
	}

	bool Load(const char* soundfile)
	{
		if (!m_inited)
			return false;

		BASS_MusicFree(m_chan);
		BASS_StreamFree(m_chan);
		if (!(m_chan = BASS_StreamCreateFile(FALSE, soundfile,0,0,0))
		&&  !(m_chan  = BASS_MusicLoad(FALSE,soundfile,0,0, BASS_MUSIC_RAMP,1))) 
		{
			return false;
		}	
		return true;
	}

	void Play()
	{
		if (!m_inited)
			return;

		BASS_ChannelPlay(m_chan,FALSE);
	}
	void Stop()
	{
		if (!m_inited)
			return;

		BASS_ChannelStop(m_chan);
	}
	void Pause()
	{
		if (!m_inited)
			return;

		BASS_ChannelPause(m_chan);
	}

	double GetPosTime()
	{
		if(!m_inited)
			return 0.0;
		const QWORD pos = BASS_ChannelGetPosition(m_chan, BASS_POS_BYTE);
		const double pos_time = BASS_ChannelBytes2Seconds(m_chan, pos);
		return pos_time;
	}
	float* GetFFT()
	{
		static float fft[1024];
		BASS_ChannelGetData(m_chan,fft,BASS_DATA_FFT2048); // get the FFT data
		return fft;
	}

private:
	DWORD m_chan;
	bool m_inited;

	bool init()
	{
		// check the correct BASS was loaded
		if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
			return false;
		}
		if (!BASS_Init(-1,44100,0,0,NULL)) 
			return false;
	
		return true;
	}
	void term()
	{
		BASS_Free();
	}
};

#endif // INCLUDE_SOUND_H
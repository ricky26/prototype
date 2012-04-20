#include "prototype/audio.h"
#include "prototype/driver_internal.h"
#include <xaudio2.h>

using namespace netlib;

namespace prototype
{
	static void wfmt(WAVEFORMATEX *_ex, audio_format const& _fmt)
	{
		memset(_ex, 0, sizeof(*_ex));
		_ex->cbSize = sizeof(*_ex);
		_ex->nChannels = _fmt.channels();
		_ex->nSamplesPerSec = _fmt.rate();
		_ex->wBitsPerSample = _fmt.bits();
		_ex->nBlockAlign = (_fmt.channels()*_fmt.bits())/8;
		_ex->wFormatTag = WAVE_FORMAT_PCM;
		_ex->nAvgBytesPerSec = (_fmt.bits()*_fmt.channels()*_fmt.rate())/8;
	}

	class xaudio_channel: public channel
	{
	public:
		IXAudio2SourceVoice *voice;

		xaudio_channel(IXAudio2SourceVoice *_v) : voice(_v) {}

		void start() override
		{
			voice->Start();
		}

		void stop() override
		{
			voice->Stop();
		}

		bool upload(void *_buffer, size_t _sz) override
		{
			XAUDIO2_BUFFER bfr = {0};
			bfr.AudioBytes = (UINT32)_sz;
			bfr.pAudioData = (BYTE*)_buffer;

			return SUCCEEDED(voice->SubmitSourceBuffer(&bfr));
		}

	};

	class xaudio_soundscape: public soundscape
	{
	public:
		IXAudio2 *xaudio;
		IXAudio2MasteringVoice *mvoice;

		xaudio_soundscape(handle<audio_driver> _drv, IXAudio2 *_x, IXAudio2MasteringVoice *_v)
			: soundscape(_drv), xaudio(_x), mvoice(_v)
		{
		}

		handle<channel> create_channel(audio_format const& _f) override
		{
			IXAudio2SourceVoice *vc;
			WAVEFORMATEX fmt;
			wfmt(&fmt, _f);

			if(FAILED(xaudio->CreateSourceVoice(&vc, &fmt)))
				return nullptr;

			return new xaudio_channel(vc);
		}
	};

	class xaudio_driver: public audio_driver
	{
	public:
		xaudio_driver(): audio_driver("xaudio")
		{
		}

		handle<soundscape> create_soundscape() override
		{
			IXAudio2 *ptr;
			if(FAILED(XAudio2Create(&ptr)))
				return nullptr;

			IXAudio2MasteringVoice *vptr;
			if(FAILED(ptr->CreateMasteringVoice(&vptr)))
				ptr->Release();

			return new xaudio_soundscape(this, ptr, vptr);
		}
	};

	static xaudio_driver xaudio_drv;
	static static_driver_registration gXAudioReg(&xaudio_drv);
}

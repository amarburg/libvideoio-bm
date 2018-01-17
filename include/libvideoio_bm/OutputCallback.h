#pragma once

#include <DeckLinkAPI.h>

#include "SDICameraControl.h"

namespace libvideoio_bm {

class OutputCallback: public IDeckLinkVideoOutputCallback
{
public:
	OutputCallback( IDeckLinkOutput *deckLinkOutput, IDeckLinkDisplayMode *mode )
    : _deckLinkOutput( deckLinkOutput ),
      _mode( mode ),
      _totalFramesScheduled(0),
      _blankFrame( CreateBlueFrame(deckLinkOutput, true ))
	{
      _deckLinkOutput->AddRef();
      _mode->AddRef();
	}

	virtual ~OutputCallback(void)
	{
    if( _deckLinkOutput ) _deckLinkOutput->Release();
	}

	HRESULT	STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result)
	{
		// When a video frame completes, reschedule is again...

    // These are magic values for 1080i50   See SDK manual page 213
    const uint32_t kFrameDuration = 1000;
    const uint32_t kTimeScale = 25000;

		_deckLinkOutput->ScheduleVideoFrame(completedFrame, _totalFramesScheduled*kFrameDuration, kFrameDuration, kTimeScale);
		_totalFramesScheduled++;

		return S_OK;
	}

	HRESULT	STDMETHODCALLTYPE ScheduledPlaybackHasStopped(void)
	{
		return S_OK;
	}

	// IUnknown needs only a dummy implementation
	HRESULT	STDMETHODCALLTYPE QueryInterface (REFIID iid, LPVOID *ppv)
	{
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

private:

IDeckLinkOutput *_deckLinkOutput;
IDeckLinkDisplayMode *_mode;

  unsigned int _totalFramesScheduled;

  IDeckLinkMutableVideoFrame *_blankFrame;

};

}

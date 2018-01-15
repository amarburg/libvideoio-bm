#pragma once

#include <DeckLinkAPI.h>

namespace libvideoio_bm {

class OutputCallback: public IDeckLinkVideoOutputCallback
{
public:
	OutputCallback(const std::shared_ptr<IDeckLinkOutput> &deckLinkOutput)
    : _deckLinkOutput( deckLinkOutput ),
      _totalFramesScheduled(0)
	{
	}

	virtual ~OutputCallback(void)
	{
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

	std::shared_ptr<IDeckLinkOutput>  _deckLinkOutput;
  unsigned int _totalFramesScheduled;

};

}

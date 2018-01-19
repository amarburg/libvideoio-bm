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
			_bmsdiBuffer( bmAllocBuffer() ),
      _totalFramesScheduled(0),
      _blankFrame( CreateBlueFrame(deckLinkOutput, true ))
	{
      _deckLinkOutput->AddRef();
      _mode->AddRef();

			_mode->GetFrameRate( &_timeValue, &_timeScale );

			// Get timing information from mode

			// Pre-roll a few blank frames
	const int prerollFrames = 3;
	for( int i = 0; i < prerollFrames ; ++i ) {
		scheduleFrame(_blankFrame);
	}

	}

	virtual ~OutputCallback(void)
	{
    if( _deckLinkOutput ) _deckLinkOutput->Release();
	}


	void scheduleFrame( IDeckLinkVideoFrame *frame, uint8_t count = 1 )
	{
		_deckLinkOutput->ScheduleVideoFrame(_blankFrame, _totalFramesScheduled*_timeValue, _timeValue*count, _timeScale );
		_totalFramesScheduled += count;
	}

	HRESULT	STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result)
	{
		// For simplicity, this will do just one buffer per frame for now
		BMSDIBuffer *newCmd = nullptr;
		while( _queue.try_and_pop(newCmd) ) {
			LOG(INFO) << "Got a SDI protocol command to send!";

			if( ! bmAppendBuffer( _bmsdiBuffer, newCmd ) ) {
				// Failure means there's no room in the buffer
				break;
			}

			free(newCmd);
		}

		if( _bmsdiBuffer->len > 0 ) {
			scheduleFrame( AddSDICameraControlFrame( _deckLinkOutput, _blankFrame, _bmsdiBuffer ) );

			bmResetBuffer( _bmsdiBuffer );

			// If the last command is still hanging around, add it to the buffer
			if( newCmd ) {
				bmAppendBuffer( _bmsdiBuffer, newCmd );
	 			free(newCmd);
			}
		} else {

			// When a video frame completes, reschedule is again...
			scheduleFrame( completedFrame );
		}

		// Can I release the completeFrame?

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

	active_object::shared_queue< BMSDIBuffer * > &queue() { return _queue; }


private:

  active_object::shared_queue< BMSDIBuffer * > _queue;


	IDeckLinkOutput *_deckLinkOutput;
	IDeckLinkDisplayMode *_mode;

	// Cached values
BMDTimeValue _timeValue;
BMDTimeScale _timeScale;

	BMSDIBuffer *_bmsdiBuffer;

  unsigned int _totalFramesScheduled;

  IDeckLinkMutableVideoFrame *_blankFrame;

};

}

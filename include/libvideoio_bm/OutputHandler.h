#pragma once

#include <DeckLinkAPI.h>
#include <active_object/active.h>

#include "SDICameraControl.h"

namespace libvideoio_bm {

	class OutputHandler: public IDeckLinkVideoOutputCallback
	{
	public:
		OutputHandler( IDeckLinkOutput *deckLinkOutput, IDeckLinkDisplayMode *mode );
		virtual ~OutputHandler(void);

		void scheduleFrame( IDeckLinkVideoFrame *frame, uint8_t count = 1 );

		HRESULT	STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result);

		HRESULT	STDMETHODCALLTYPE ScheduledPlaybackHasStopped(void) {	return S_OK; }

		// Dummy implementations
		HRESULT	STDMETHODCALLTYPE QueryInterface (REFIID iid, LPVOID *ppv){ return E_NOINTERFACE; }
		ULONG STDMETHODCALLTYPE AddRef() { return 1; }
		ULONG STDMETHODCALLTYPE Release() { return 1; }

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

#pragma once

#include <queue>

#include <opencv2/core/core.hpp>

#include "active_object/shared_queue.h"
#include <DeckLinkAPI.h>
#include "ThreadSynchronizer.h"


namespace libvideoio_bm {

  class InputCallback : public IDeckLinkInputCallback
  {
  public:
    InputCallback(  const std::shared_ptr<IDeckLinkInput> &input, 
            const std::shared_ptr<IDeckLinkOutput> &output,
            unsigned int maxFrames = -1 );

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE  Release(void);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);

    active_object::shared_queue< cv::Mat > &queue() { return _queue; }

    // ThreadSynchronizer &imageReady() { return _imageReady; }
    // cv::Mat popImage();

  private:

    bool _stop;

    int32_t _refCount;

    unsigned long _frameCount, _maxFrames;

    std::shared_ptr<IDeckLinkInput> _deckLinkInput;
    std::shared_ptr<IDeckLinkOutput> _deckLinkOutput;

    //IDeckLinkVideoConversion *_deckLinkConversion;

    active_object::shared_queue< cv::Mat > _queue;

  };

}

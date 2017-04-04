#pragma once

#include <queue>

#include <opencv2/core/core.hpp>

#include <DeckLinkAPI.h>
#include "ThreadSynchronizer.h"


namespace libvideoio_bm {

  class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
  {
  public:
    DeckLinkCaptureDelegate( unsigned int maxFrames = -1 );

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE  Release(void);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);

    ThreadSynchronizer &imageReady() { return _imageReady; }
    cv::Mat popImage();

  private:
    int32_t _refCount;

    unsigned int _frameCount, _maxFrames;


        ThreadSynchronizer _imageReady;
        std::queue< cv::Mat > _imageQueue;

  };

}

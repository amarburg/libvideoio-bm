#pragma once

//#include <queue>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "active_object/active.h"
#include "active_object/shared_queue.h"
#include <DeckLinkAPI.h>
#include "ThreadSynchronizer.h"
#include "libvideoio/ImageSize.h"


namespace libvideoio_bm {

  class InputCallback : public IDeckLinkInputCallback
  {
  public:
    InputCallback(  IDeckLinkInput *input,
                    IDeckLinkOutput *output,
                    IDeckLinkDisplayMode *mode );

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE  Release(void);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);

    active_object::shared_queue< cv::Mat > &queue() { return _queue; }

    // ThreadSynchronizer &imageReady() { return _imageReady; }
    // cv::Mat popImage();

    void stopStreams();

    libvideoio::ImageSize imageSize() const;

  protected:

    void registerCallback();

  private:

    bool _stop;

    int32_t _refCount;

    unsigned long _frameCount;

    IDeckLinkInput *_deckLinkInput;
    IDeckLinkOutput *_deckLinkOutput;
    IDeckLinkDisplayMode *_mode;

    //IDeckLinkVideoConversion *_deckLinkConversion;

    active_object::shared_queue< cv::Mat > _queue;

    std::unique_ptr<active_object::Active> _thread;
  };

}

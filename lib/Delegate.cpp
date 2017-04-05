
#include <g3log/g3log.hpp>

#include "libvideoio_bm/Delegate.h"

namespace libvideoio_bm {

  const int maxDequeDepth = 10;

  class MyOutputImage : public IDeckLinkVideoFrame {
  public:
    MyOutputImage( long w, long h, long rb, BMDPixelFormat format,
                  BMDFrameFlags flags = bmdVideoInputFlagDefault )
      : _refCount(1), _width( w ), _height( h ), _rowBytes( rb ),
        _format( format ), _flags( flags ),
        _buffer( new unsigned char[ h * rb ] )
        {;}

        ~MyOutputImage()
        {
          delete _buffer;
        }

        // IUnknown virtual functions
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv)
        { return E_NOINTERFACE; }

        ULONG AddRef(void)
        {
          return __sync_add_and_fetch(&_refCount, 1);
        }

        ULONG Release(void)
        {
          int32_t newRefValue = __sync_sub_and_fetch(&_refCount, 1);
          if (newRefValue == 0)
          {
            delete this;
            return 0;
          }
          return newRefValue;
        }

        // IDeckLinkVideoFrame
    virtual long GetWidth (void)
    { return _width; }

    virtual long GetHeight (void)
     { return _height; }

    virtual long GetRowBytes (void)
    { return _rowBytes; }

    virtual BMDPixelFormat GetPixelFormat(void)
     { return _format; }

    virtual BMDFrameFlags GetFlags (void)
    { return _flags; }

    virtual HRESULT GetBytes (/* out */ void **buffer)
    {
      *buffer = _buffer;
    }

    virtual HRESULT GetTimecode (/* in */ BMDTimecodeFormat format, /* out */ IDeckLinkTimecode **timecode)
    { return S_OK; }

    virtual HRESULT GetAncillaryData (/* out */ IDeckLinkVideoFrameAncillary **ancillary)
    { return S_OK; }


    unsigned char *BufferBytes()
    { return _buffer; }
  protected:

    int32_t _refCount;

    long _width, _height, _rowBytes;

  BMDPixelFormat _format;
  BMDFrameFlags _flags;

    unsigned char *_buffer;


  };

  DeckLinkCaptureDelegate::DeckLinkCaptureDelegate( IDeckLinkInput* input, unsigned int maxFrames )
  : _refCount(1), _maxFrames( maxFrames ), _frameCount(0), _deckLinkInput(input),
    _deckLinkConversion( CreateVideoConversionInstance() )
  {
  }

  ULONG DeckLinkCaptureDelegate::AddRef(void)
  {
    return __sync_add_and_fetch(&_refCount, 1);
  }

  ULONG DeckLinkCaptureDelegate::Release(void)
  {
    int32_t newRefValue = __sync_sub_and_fetch(&_refCount, 1);
    if (newRefValue == 0)
    {
      delete this;
      return 0;
    }
    return newRefValue;
  }

  HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame,
    IDeckLinkAudioInputPacket* audioFrame)
    {
      // IDeckLinkVideoFrame *rightEyeFrame = nullptr;
//      IDeckLinkVideoFrame3DExtensions *threeDExtensions = nullptr;
//      void *audioFrameBytes;

      // Handle Video Frame
      if (videoFrame)
      {
        // If 3D mode is enabled we retreive the 3D extensions interface which gives.
        // us access to the right eye frame by calling GetFrameForRightEye() .
        // if ( (videoFrame->QueryInterface(IID_IDeckLinkVideoFrame3DExtensions, (void **) &threeDExtensions) != S_OK) ||
        //                                   (threeDExtensions->GetFrameForRightEye(&rightEyeFrame) != S_OK))
        // {
        //   rightEyeFrame = nullptr;
        // }
        //
        // if (threeDExtensions)
        // threeDExtensions->Release();

        if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
        {
          LOGF(WARNING,"Frame received (#%lu) - No input signal detected", _frameCount);
        }
        else
        {
          // const char *timecodeString = nullptr;
          // if (g_config.m_timecodeFormat != 0)
          // {
          //   IDeckLinkTimecode *timecode;
          //   if (videoFrame->GetTimecode(g_config.m_timecodeFormat, &timecode) == S_OK)
          //   {
          //     timecode->GetString(&timecodeString);
          //   }
          // }

          LOGF(INFO, "Frame received (#%lu) %li bytes, %lu x %lu",
          _frameCount,
          // timecodeString != nullptr ? timecodeString : "No timecode",
          videoFrame->GetRowBytes() * videoFrame->GetHeight(),
          videoFrame->GetWidth(), videoFrame->GetHeight() );
          //
          // if (timecodeString)
          // free((void*)timecodeString);

          // if (g_videoOutputFile != -1)
          // {
          // void *frameBytes;
          // videoFrame->GetBytes(&frameBytes);

          auto dstFrame = new MyOutputImage( videoFrame->GetWidth(), videoFrame->GetHeight(),
                                            videoFrame->GetWidth()*4,
                                           bmdFormat8BitBGRA  );

          if( _deckLinkConversion->ConvertFrame( videoFrame, dstFrame ) != S_OK ) {
            LOG(WARNING) << "Unable to do conversion";
          }

          // See:
          // https://developer.apple.com/library/content/technotes/tn2162/_index.html#//apple_ref/doc/uid/DTS40013070-CH1-TNTAG8-V210__4_2_2_COMPRESSION_TYPE
          // const unsigned int uwidth = videoFrame->GetWidth();
          // const unsigned int uheight = videoFrame->GetHeight();
          //
          // int i = 0,j = 0, r = 0, g = 0, b = 0;
          // typedef unsigned char BYTE;
          // cv::Mat out(cv::Size(uwidth, uheight), CV_8UC3);
          //
          // unsigned char* pData = (unsigned char *)frameBytes;
          //
          // for(i=0, j=0; i < uwidth * uheight*3 ; i+=6, j+=4)
          // {
          //    unsigned char u = pData[j];
          //    unsigned char y = pData[j+1];
          //    unsigned char v = pData[j+2];
          //
          //    b = 1.0*y + 8 + 1.402*(v-128);
          //    g = 1.0*y - 0.34413*(u-128) - 0.71414*(v-128);
          //    r = 1.0*y + 1.772*(u-128);
          //
          //    if(r>255) r =255;
          //    if(g>255) g =255;
          //    if(b>255) b =255;
          //    if(r<0)   r =0;
          //    if(g<0)   g =0;
          //    if(b<0)   b =0;
          //
          //    out.data[i] = (BYTE)(r*220/256);
          //    out.data[i+1] = (BYTE)(g*220/256);
          //    out.data[i+2] =(BYTE)(b*220/256);
          // }


          // Decode to Mat?

          cv::Mat out( cv::Size(dstFrame->GetWidth(), dstFrame->GetHeight()),
                                CV_8UC4, dstFrame->BufferBytes(), videoFrame->GetRowBytes() );


          if( _queue.size() < maxDequeDepth ) {
            _queue.push( out );
          } else {
            LOG(WARNING) << "Image queue full";
          }

          //   write(g_videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
          //
          //   if (rightEyeFrame)
          //   {
          //     rightEyeFrame->GetBytes(&frameBytes);
          //     write(g_videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
          //   }

        }
      }

      //   if (rightEyeFrame)
      //   rightEyeFrame->Release();
      //   _frameCount++;
      // }

      // Handle Audio Frame
      // if (audioFrame)
      // {
      //   if (g_audioOutputFile != -1)
      //   {
      //     audioFrame->GetBytes(&audioFrameBytes);
      //     write(g_audioOutputFile, audioFrameBytes, audioFrame->GetSampleFrameCount() * g_config.m_audioChannels * (g_config.m_audioSampleDepth / 8));
      //   }
      // }

      // if ( _maxFrames > 0 && videoFrame && _frameCount >= _maxFrames)
      // {
      //   g_do_exit = true;
      //   pthread_cond_signal(&g_sleepCond);
      // }
      return S_OK;
    }

    HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags formatFlags)
    {
      LOG(INFO) << "Received Video Input Format Changed";

      // This only gets called if bmdVideoInputEnableFormatDetection was set
      // when enabling video input

      HRESULT result;
      char*   displayModeName = nullptr;
      BMDPixelFormat  pixelFormat = bmdFormat10BitYUV;

      if (formatFlags & bmdDetectedVideoInputRGB444)
      pixelFormat = bmdFormat10BitRGB;

      mode->GetName((const char**)&displayModeName);
      LOGF(INFO,"Video format changed to %s %s", displayModeName, formatFlags & bmdDetectedVideoInputRGB444 ? "RGB" : "YUV");

      if (displayModeName) free(displayModeName);

      if(_deckLinkInput)
      {
        _deckLinkInput->StopStreams();

        BMDVideoInputFlags m_inputFlags = bmdVideoInputFlagDefault | bmdVideoInputEnableFormatDetection;

        result = _deckLinkInput->EnableVideoInput(mode->GetDisplayMode(), pixelFormat, m_inputFlags);
        if (result != S_OK)
        {
          LOG(WARNING) << "Failed to switch video mode";
          return result;
        }

        _deckLinkInput->StartStreams();
      }

      return S_OK;
    }

    // cv::Mat DeckLinkCaptureDelegate::popImage() {
    //   ThreadSynchronizer::LockGuard lock(_imageReady.mutex());
    //
    //   if( _imageQueue.size() == 0 ) return cv::Mat();
    //
    //   cv::Mat out( _imageQueue.front() );
    //
    //   _imageQueue.pop();
    //   return out;
    // }

  }

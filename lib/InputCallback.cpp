
#include <iostream>
#include <thread>

#include <g3log/g3log.hpp>


#include "libvideoio_bm/InputCallback.h"

namespace libvideoio_bm {

  using libvideoio::ImageSize;

  const int maxDequeDepth = 10;



  InputCallback::InputCallback( IDeckLinkInput *input,
                                IDeckLinkOutput *output,
                                IDeckLinkDisplayMode *mode )
  : _frameCount(0),
    _deckLinkInput(input),
    _deckLinkOutput(output),
    _mode(mode)
  {
    _deckLinkInput->SetCallback(this);
  }

  ULONG InputCallback::AddRef(void)
  {
    return __sync_add_and_fetch(&_refCount, 1);
  }

  ULONG InputCallback::Release(void)
  {
    int32_t newRefValue = __sync_sub_and_fetch(&_refCount, 1);
    if (newRefValue == 0)
    {
      delete this;
      return 0;
    }
    return newRefValue;
  }


  // Callbacks are called in a private thread....
  HRESULT InputCallback::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame,
                                              IDeckLinkAudioInputPacket* audioFrame)
    {
       IDeckLinkVideoFrame *rightEyeFrame = nullptr;
       IDeckLinkVideoFrame3DExtensions *threeDExtensions = nullptr;
//      void *audioFrameBytes;

      // Handle Video Frame
      if (videoFrame)
      {
        // If 3D mode is enabled we retreive the 3D extensions interface which gives.
        // us access to the right eye frame by calling GetFrameForRightEye() .
        if ( (videoFrame->QueryInterface(IID_IDeckLinkVideoFrame3DExtensions, (void **) &threeDExtensions) != S_OK) ||
                                          (threeDExtensions->GetFrameForRightEye(&rightEyeFrame) != S_OK))
        {
          rightEyeFrame = nullptr;
        }

        if (threeDExtensions) threeDExtensions->Release();

        if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
        {
          LOGF(WARNING,"(%d) Frame received (#%lu) - No input signal detected", std::this_thread::get_id(), _frameCount);
        }
        else
        {
          _frameCount++;

          // const char *timecodeString = nullptr;
          // if (g_config.m_timecodeFormat != 0)
          // {
          //   IDeckLinkTimecode *timecode;
          //   if (videoFrame->GetTimecode(g_config.m_timecodeFormat, &timecode) == S_OK)
          //   {
          //     timecode->GetString(&timecodeString);
          //   }
          // }

          LOGF(INFO, "(%u) Frame received (#%lu) %li bytes, %lu x %lu",
                      std::this_thread::get_id(),
                      _frameCount,
                      // timecodeString != nullptr ? timecodeString : "No timecode",
                      videoFrame->GetRowBytes() * videoFrame->GetHeight(),
                      videoFrame->GetWidth(), videoFrame->GetHeight() );

          // The AddRef will ensure the frame is valid after the end of the callback.
          videoFrame->AddRef();
          std::thread t = processInThread( videoFrame );
          t.detach();

        // if (timecodeString)
          // free((void*)timecodeString);

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

    HRESULT InputCallback::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags formatFlags)
    {
      LOG(INFO) << "(" << std::this_thread::get_id() << ") Received Video Input Format Changed";

      // This only gets called if bmdVideoInputEnableFormatDetection was set
      // when enabling video input

      HRESULT result;
      char*   displayModeName = nullptr;
      BMDPixelFormat  pixelFormat = bmdFormat10BitYUV;

      if (formatFlags & bmdDetectedVideoInputRGB444) pixelFormat = bmdFormat10BitRGB;

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

      _mode = mode;

      return S_OK;
    }

  ImageSize InputCallback::imageSize(void) const {
      if( !_mode ) return ImageSize(0,0);
      return ImageSize( _mode->GetWidth(), _mode->GetHeight() );
  }


  void InputCallback::stopStreams() {

    LOG(INFO) << "(" << std::this_thread::get_id() << ") Stopping DeckLinkInput streams";
    if (_deckLinkInput->StopStreams() != S_OK) {
      LOG(WARNING) << "Failed to stop input streams";
    }
    LOG(INFO) << "    ...done";

    //_thread.doDone();
  }

  bool InputCallback::process(  IDeckLinkVideoFrame *videoFrame )
  {
    LOG(INFO) << "Start process in thread " << std::this_thread::get_id();

      cv::Mat out;

      switch (videoFrame->GetPixelFormat()) {
      case bmdFormat8BitYUV:
      {
          void* data;
          if ( videoFrame->GetBytes(&data) != S_OK )
              return false;
          cv::Mat mat = cv::Mat(videoFrame->GetHeight(), videoFrame->GetWidth(), CV_8UC2, data,
              videoFrame->GetRowBytes());
          cv::cvtColor(mat, out, cv::COLOR_YUV2BGR ); //_UYVY);
          return true;
      }
      case bmdFormat8BitBGRA:
      {
          void* data;
          if ( videoFrame->GetBytes(&data) != S_OK )
              return false;

          cv::Mat mat = cv::Mat(videoFrame->GetHeight(), videoFrame->GetWidth(), CV_8UC4, data);
          cv::cvtColor(mat, out, cv::COLOR_BGRA2BGR);
          return true;
      }
      default:
      {
          //LOG(INFO) << "Converting through Blackmagic VideoConversionInstance";
          IDeckLinkMutableVideoFrame*     dstFrame = NULL;

          //CvMatDeckLinkVideoFrame cvMatWrapper(videoFrame->GetHeight(), videoFrame->GetWidth());
          HRESULT result = _deckLinkOutput->CreateVideoFrame( videoFrame->GetWidth(), videoFrame->GetHeight(),
                                      videoFrame->GetWidth() * 4, bmdFormat8BitBGRA, bmdFrameFlagDefault, &dstFrame);
            if (result != S_OK)
            {
                    LOG(WARNING) << "Failed to create destination video frame";
                    return false;
            }


          IDeckLinkVideoConversion *converter =  CreateVideoConversionInstance();

          //LOG(WARNING) << "Converting " << std::hex << videoFrame->GetPixelFormat() << " to " << dstFrame->GetPixelFormat();
          result =  converter->ConvertFrame(videoFrame, dstFrame);

          if (result != S_OK ) {
             LOG(WARNING) << "Failed to do conversion " << std::hex << result;
            return false;
          }

          void *buffer = nullptr;
          if( dstFrame->GetBytes( &buffer ) != S_OK ) {
            LOG(WARNING) << "Unable to get bytes from dstFrame";
            return false;
          }
          cv::Mat srcMat( cv::Size(dstFrame->GetWidth(), dstFrame->GetHeight()), CV_8UC4, buffer, dstFrame->GetRowBytes() );
          //cv::cvtColor(srcMat, out, cv::COLOR_BGRA2BGR);
          cv::resize( srcMat, out, cv::Size(), 0.25, 0.25  );

          dstFrame->Release();
          videoFrame->Release();
        //  return true;
      }
    }

      // auto dstFrame = new MyOutputImage( videoFrame->GetWidth(), videoFrame->GetHeight(),
      //                                   videoFrame->GetWidth()*4,
      //                                  bmdFormat8BitBGRA  );
      //
      // if( _deckLinkConversion->ConvertFrame( videoFrame, dstFrame ) != S_OK ) {
      // }

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

      // cv::Mat out( cv::Size(dstFrame->GetWidth(), dstFrame->GetHeight()),
      //                       CV_8UC4, dstFrame->BufferBytes(), videoFrame->GetRowBytes() );


      if( queue().size() < maxDequeDepth ) {
        queue().push( out );
      } else {
        LOG(WARNING) << "Image queue full, unable to queue more images";
      }

      //   write(g_videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
      //
      //   if (rightEyeFrame)
      //   {
      //     rightEyeFrame->GetBytes(&frameBytes);
      //     write(g_videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
      //   }

      LOG(INFO) << "Finishing process in thread " << std::this_thread::get_id();

  }





}

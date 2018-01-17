#pragma once


#include <Identical3DFrames.h>

namespace libvideoio_bm {

    Identical3DFrames::Identical3DFrames( IDeckLinkMutableVideoFrame *data )
    : _data(data)
    {
      _data->AddRef();
    }

    Identical3DFrames::~Identical3DFrames()
    {
      if( _data ) _data->Release();
    }


    //class IUnknown
    virtual HRESULT STDMETHODCALLTYPE Identical3DFrames::QueryInterface(REFIID iid, LPVOID *ppv) {
      if( iid ==  IID_IDeckLinkVideoFrame3DExtensions ) {
          *ppv = this;
          return S_OK;
      }

      return E_NOINTERFACE;
    }

    ULONG Identical3DFrames::AddRef(void)
    {
      return __sync_add_and_fetch(&_refCount, 1);
    }

    ULONG Identical3DFrames::Release(void)
    {
      int32_t newRefValue = __sync_sub_and_fetch(&_refCount, 1);
      if (newRefValue == 0)
      {
        delete this;
        return 0;
      }
      return newRefValue;
    }

    //class IDeckLinkVideoFrame
    virtual long GetWidth (void)
    { return _data->GetWidth(); }

    virtual long GetHeight (void)
    { return _data->GetHeight(); }

    virtual long GetRowBytes (void)
    { return _data->GetRowBytes(); }

    virtual BMDPixelFormat GetPixelFormat (void)
    { return _data->GetPixelFormat(); }

    virtual BMDFrameFlags GetFlags (void)
    { return _data->GetFlags(); }

    virtual HRESULT GetBytes (void **buffer)
    { return _data->GetBytes(buffer); }

    virtual HRESULT GetTimecode (/* in */ BMDTimecodeFormat format, /* out */ IDeckLinkTimecode **timecode)
    { return _data->GetTimecode( format, timecode); }

    virtual HRESULT GetAncillaryData (/* out */ IDeckLinkVideoFrameAncillary **ancillary)
    { return _data->GetAncillaryData(ancillary); }

    //class IDeckLinkMutableVideoFrame
    virtual HRESULT SetFlags (/* in */ BMDFrameFlags newFlags)
    { return _data->SetFlags( newFlags); }

    virtual HRESULT SetTimecode (/* in */ BMDTimecodeFormat format, /* in */ IDeckLinkTimecode *timecode)
    { return _data->SetTimecode( format, timecode ); }

    virtual HRESULT SetTimecodeFromComponents (/* in */ BMDTimecodeFormat format, /* in */ uint8_t hours, /* in */ uint8_t minutes, /* in */ uint8_t seconds, /* in */ uint8_t frames, /* in */ BMDTimecodeFlags flags)
    { return _data->SetTimecodeFromComponents( format, hours, minutes, seconds, frames, flags ); }

    virtual HRESULT SetAncillaryData (/* in */ IDeckLinkVideoFrameAncillary *ancillary)
    { return _data->SetANcillaryData( ancillary ); }

    virtual HRESULT SetTimecodeUserBits (/* in */ BMDTimecodeFormat format, /* in */ BMDTimecodeUserBits userBits)
    { return _data->SetTimecodeUserBits( format, userBits ); }

    // class IDeckLinkVideoFrame3DExtensions
    virtual BMDVideo3DPackingFormat Get3DPackingFormat (void)
    {
      return bmdVideo3DPackingRightOnly;
    }

    virtual HRESULT GetFrameForRightEye (/* out */ IDeckLinkVideoFrame* *rightEyeFrame)
    {
      *rightEyeFrame = this;
      return S_OK;
    }


  protected:

    IDeckLinkMutableVideoFrame *_data;

  };


}

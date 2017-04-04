
#include <string>

#include "libvideoio_bm/DeckLinkSource.h"

namespace libvideoio_bm {

  using std::string;

  DeckLinkSource::DeckLinkSource()
  : DataSource(),
  _initialized( false ),
  _deckLinkInput( nullptr )
  {
    initialize();
  }

  DeckLinkSource::~DeckLinkSource()
  {

  }

  void DeckLinkSource::initialize()
  {
    IDeckLinkIterator *deckLinkIterator = NULL;
    IDeckLink *deckLink = NULL;
    HRESULT result;

    // Get the DeckLink device
    deckLinkIterator = CreateDeckLinkIteratorInstance();
    if (!deckLinkIterator)
    {
      LOG(WARNING) << "This application requires the DeckLink drivers installed.";
      return;
    }

    // idx = g_config.m_deckLinkIndex;
    //
    // while ((result = deckLinkIterator->Next(&deckLink)) == S_OK)
    // {
    //         if (idx == 0)
    //                 break;
    //         --idx;
    //
    //         deckLink->Release();
    // }

    // Use first device
    if( (result = deckLinkIterator->Next(&deckLink)) != S_OK) {
      LOG(WARNING) << "Couldn't get information on the first DeckLink object.";
      return;
    }

    char *modelName, *displayName;
    if( deckLink->GetModelName( (const char **)&modelName ) != S_OK ) {
      LOG(WARNING) << "Unable to query model name.";
      return;
    }

    if( deckLink->GetDisplayName( (const char **)&displayName ) != S_OK ) {
      LOG(WARNING) << "Unable to query display name.";
      return;
    }

    LOG(INFO) << "Using model \"" << modelName << "\" with display name \"" << displayName << "\"";

    free(modelName);
    free(displayName);

    // Get the input (capture) interface of the DeckLink device
    result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&_deckLinkInput);
    if (result != S_OK) {
      LOG(WARNING) << "Couldn't get input for Decklink";
      return;
    }

    IDeckLinkAttributes* deckLinkAttributes = NULL;
    bool formatDetectionSupported;
    //      // Get the display mode
    //       if (g_config.m_displayModeIndex == -1)
    //       {
    // Check the card supports format detection
    result = deckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&deckLinkAttributes);
    if (result == S_OK)
    {
      result = deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &formatDetectionSupported);
      if (result != S_OK || !formatDetectionSupported)
      {
        LOG(WARNING) << "Format detection is not supported on this device";
        return;
      }
    }

    //g_config.m_inputFlags |= bmdVideoInputEnableFormatDetection;

    // Format detection still needs a valid mode to start with
    //idx = 0;
    //         }
    //         else
    //         {
    //                 idx = g_config.m_displayModeIndex;
    //         }


    IDeckLinkDisplayModeIterator* displayModeIterator = NULL;
    IDeckLinkDisplayMode *displayMode = NULL, *displayModeItr = NULL;

    result = _deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
    if (result != S_OK) {
      LOG(WARNING) << "Unable to get DisplayModeIterator";
      return;
    }

    // Use first displayMode for now
    while( displayModeIterator->Next( &displayModeItr ) == S_OK ) {


      char *displayModeName = nullptr;
      if( displayModeItr->GetName( (const char **)&displayModeName) != S_OK ) {
        LOG(WARNING) << "Unable to get name of DisplayMode";
        return;
      }

      BMDTimeValue timeValue = 0;
      BMDTimeScale timeScale = 0;

      if( displayModeItr->GetFrameRate( &timeValue, &timeScale ) != S_OK ) {
        LOG(WARNING) << "Unable to get DisplayMode frame rate";
        return;
      }

      float frameRate = (timeScale != 0) ? float(timeValue)/timeScale : float(timeValue);

      LOG(INFO) << "Using display mode \"" << displayModeName << "\"    " <<
      displayModeItr->GetWidth() << " x " << displayModeItr->GetHeight() <<
      ", " << frameRate;

      string modeName( displayModeName );

      if( modeName == "1080p59.94" ) {
        displayMode = displayModeItr;
      }

      free( displayModeName );

      // Check for the desired displayModeName

      // LOG(WARNING) << "Unable to get first DisplayMode";
      // return;
    }

    if( displayMode == nullptr  ) {
      LOG(WARNING) << "Didn't select a display mode";
      return;
    }



    //  // Check display mode is supported with given options
    //  result = g_deckLinkInput->DoesSupportVideoMode(displayMode->GetDisplayMode(), g_config.m_pixelFormat, bmdVideoInputFlagDefault, &displayModeSupported, NULL);
    //  if (result != S_OK)
    //          goto bail;
    //
    //  if (displayModeSupported == bmdDisplayModeNotSupported)
    //  {
    //          fprintf(stderr, "The display mode %s is not supported with the selected pixel format\n", displayModeName);
    //          goto bail;
    //  }
    //
    //  if (g_config.m_inputFlags & bmdVideoInputDualStream3D)
    //  {
    //          if (!(displayMode->GetFlags() & bmdDisplayModeSupports3D))
    //          {
    //                  fprintf(stderr, "The display mode %s is not supported with 3D\n", displayModeName);
    //                  goto bail;
    //          }
    //  }
    //
    //  // Print the selected configuration
    //  g_config.DisplayConfiguration();
    //
    //  // Configure the capture callback
    // delegate = new DeckLinkCaptureDelegate();
    // g_deckLinkInput->SetCallback(delegate);
    //
    // // Start capturing
    //  result = g_deckLinkInput->EnableVideoInput(displayMode->GetDisplayMode(), g_config.m_pixelFormat, g_config.m_inputFlags);
    //  if (result != S_OK)
    //  {
    //          fprintf(stderr, "Failed to enable video input. Is another application using the card?\n");
    //          goto bail;
    //  }
    //
    //  result = g_deckLinkInput->EnableAudioInput(bmdAudioSampleRate48kHz, g_config.m_audioSampleDepth, g_config.m_audioChannels);
    //  if (result != S_OK)
    //          goto bail;
    //
    //  result = g_deckLinkInput->StartStreams();
    //  if (result != S_OK)
    //          goto bail;

    _initialized = true;

  }

  bool DeckLinkSource::grab( void )
  {
    // Ifdesired,enumeratethesupportedcapturevideomodesbycalling IDeckLinkInput::GetDisplayModeIterator. For each reported capture mode, call IDeckLinkInput::DoesSupportVideoMode to check if the combination of the video mode and pixel format is supported.
    //  IDeckLinkInput::EnableVideoInput
    //  IDeckLinkInput::EnableAudioInput
    //  IDeckLinkInput::SetCallback
    //  IDeckLinkInput::StartStreams
    //  Whilestreamsarerunning:
    // - receive calls to IDeckLinkInputCallback::VideoInputFrameArrived
    // with video frame and corresponding audio packet
    // IDeckLinkInput::StopStreams

    //     if( _cam->grab( sl::zed::STANDARD, false, false, false ) ) {
    // //    if( _cam->grab( _mode, _computeDepth, _computeDepth, false ) ) {
    //       LOG( WARNING ) << "Error from Zed::grab";
    //       return false;
    //     }
    //
    //     return true;
  }

  int DeckLinkSource::getImage( int i, cv::Mat &mat )
  {

  }

  ImageSize DeckLinkSource::imageSize( void ) const
  {

  }

}

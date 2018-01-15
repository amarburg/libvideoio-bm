
#include <string>

#include "libvideoio_bm/DeckLinkSource.h"

namespace libvideoio_bm {

  using std::string;

  DeckLinkSource::DeckLinkSource()
  : DataSource(),
  _initialized( false ),
  _deckLink( nullptr ),
  _deckLinkInput( nullptr ),
  _deckLinkOutput( nullptr ),
  _inputCallback( nullptr ),
  _outputCallback(nullptr)
  {
    //initialize();
  }

  DeckLinkSource::~DeckLinkSource()
  {
    if( _deckLinkOutput ) {
      // Disable the video input interface
      _deckLinkOutput->DisableVideoOutput();
    }

    if( _deckLinkInput ) {
      _deckLinkInput->StopStreams();
    }

  }

  // Thread entry point
  void DeckLinkSource::operator()() {
    initialize();
    startStreams();
    doneSync.wait();
    stopStreams();
  }

  void DeckLinkSource::initialize()
  {
    // IDeckLink *deckLink = NULL;
    HRESULT result;

    // Hardcode some parameters for now
    BMDVideoInputFlags m_inputFlags = bmdVideoInputFlagDefault;
    BMDPixelFormat m_pixelFormat = bmdFormat10BitYUV;
    //BMDTimecodeFormat m_timecodeFormat;

    if( !_deckLink ) {
      if( !findDeckLink() ) {
        LOG(WARNING) << "Couldn't find Decklink card";
        return;
      }
    }

    CHECK( (bool)_deckLink ) << "_deckLink not set";

    char *modelName, *displayName;
    if( _deckLink->GetModelName( (const char **)&modelName ) != S_OK ) {
      LOG(WARNING) << "Unable to query model name.";
      return;
    }

    if( _deckLink->GetDisplayName( (const char **)&displayName ) != S_OK ) {
      LOG(WARNING) << "Unable to query display name.";
      return;
    }

    LOG(INFO) << "Using model \"" << modelName << "\" with display name \"" << displayName << "\"";

    free(modelName);
    free(displayName);

    // Get the input (capture) interface of the DeckLink device
    result = _deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&_deckLinkInput);
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
    result = _deckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&deckLinkAttributes);
    if (result == S_OK)
    {
      result = deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &formatDetectionSupported);
      if (result != S_OK || !formatDetectionSupported)
      {
        LOG(WARNING) << "Format detection is not supported on this device";
        return;
      } else {
        LOG(INFO) << "Enabling automatic format detection on input card.";
        m_inputFlags |= bmdVideoInputEnableFormatDetection;
      }
    }


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

      LOG(INFO) << "Card supports display mode \"" << displayModeName << "\"    " <<
      displayModeItr->GetWidth() << " x " << displayModeItr->GetHeight() <<
      ", " << 1.0/frameRate << " FPS";

      string modeName( displayModeName );

      if( modeName == "1080i60" ) {
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



    // Check display mode is supported with given options
    BMDDisplayModeSupport displayModeSupported;
    result = _deckLinkInput->DoesSupportVideoMode(displayMode->GetDisplayMode(),
    m_pixelFormat,
    bmdVideoInputFlagDefault,
    &displayModeSupported, NULL);

    if (result != S_OK) {
      LOG(WARNING) << "Error checking if DeckLinkInput supports this mode";
      return;
    }

    if (displayModeSupported == bmdDisplayModeNotSupported)
    {
      LOG(WARNING) <<  "The display mode is not supported with the selected pixel format";
      return;
    }

    //  if (g_config.m_inputFlags & bmdVideoInputDualStream3D)
    //  {
    //          if (!(displayMode->GetFlags() & bmdDisplayModeSupports3D))
    //          {
    //                  LOG(WARNING) << "The display mode " << " is not supported with 3D\n", displayModeName);
    //                  goto bail;
    //          }
    //  }

    //  // Print the selected configuration
    //  g_config.DisplayConfiguration();
    //
    //  // Configure the capture callback
    _inputCallback.reset( new InputCallback( _deckLinkInput, _deckLinkOutput ) );
    _deckLinkInput->SetCallback(_inputCallback.get());

    //
    // Start capturing
    result = _deckLinkInput->EnableVideoInput(displayMode->GetDisplayMode(),
    m_pixelFormat,
    bmdVideoInputEnableFormatDetection);
    if (result != S_OK)
    {
      LOG(WARNING) << "Failed to enable video input. Is another application using the card?";
      return;
    }

    _initialized = true;
    initializedSync.notify();
  }

  void DeckLinkSource::startStreams( void )
  {
    //
    //  result = g_deckLinkInput->EnableAudioInput(bmdAudioSampleRate48kHz, g_config.m_audioSampleDepth, g_config.m_audioChannels);
    //  if (result != S_OK)
    //          goto bail;
    //

    auto result = _deckLinkInput->StartStreams();
    if (result != S_OK) {
      LOG(WARNING) << "Failed to start streams";
      return;
    }

  }

  void DeckLinkSource::stopStreams( void )
  {
    auto result = _deckLinkInput->StopStreams();
    if (result != S_OK) {
      LOG(WARNING) << "Failed to stop streams";
      return;
    }
  }

  bool DeckLinkSource::grab( void )
  {
    if( _inputCallback ) {
      if( _inputCallback->queue().wait_for_pop(_grabbedImage, std::chrono::milliseconds(100) ) == false ) {
        LOG(WARNING) << "Timeout waiting for image";
        return false;
      }
      LOG(INFO) << "Grabbing image";

      // _grabbedImage = _inputCallback->popImage();

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

      return true;
    }

    return false;
  }

  bool DeckLinkSource::findDeckLink() {

    HRESULT result;

    // Get the DeckLink device
    IDeckLinkIterator *deckLinkIterator = CreateDeckLinkIteratorInstance();
    if (!deckLinkIterator)
    {
      LOG(WARNING) << "This application requires the DeckLink drivers installed.";
      return false;
    }

    //idx = g_config.m_deckLinkIndex;
    //
    int i = 0;
    IDeckLink *deckLink = nullptr;
    while ((result = deckLinkIterator->Next(&deckLink)) == S_OK)
    {

      char *modelName, *displayName;
      if( deckLink->GetModelName( (const char **)&modelName ) != S_OK ) {
        LOG(WARNING) << "Unable to query model name.";
        return false;
      }

      if( deckLink->GetDisplayName( (const char **)&displayName ) != S_OK ) {
        LOG(WARNING) << "Unable to query display name.";
        return false;
      }

      LOG(INFO) << "Card " << i << " \"" << modelName << "\" with display name \"" << displayName << "\"";

      free(modelName);
      free(displayName);

      deckLink->Release();

      ++i;
    }

    free(deckLinkIterator);

    deckLinkIterator = CreateDeckLinkIteratorInstance();
    // Just use the first device for now

    if( (result = deckLinkIterator->Next(&deckLink)) != S_OK) {
      LOG(WARNING) << "Couldn't get information on the first DeckLink object.";
      return false;
    }
    _deckLink.reset(deckLink);

    return true;
  }

  bool DeckLinkSource::createVideoOutput()
  {
    // Video mode parameters
    const BMDDisplayMode      kDisplayMode = bmdModeHD1080i50;
    const BMDVideoOutputFlags kOutputFlag  = bmdVideoOutputVANC;
    const BMDPixelFormat      kPixelFormat = bmdFormat10BitYUV;

    HRESULT result;

    // Obtain the output interface for the DeckLink device
    IDeckLinkOutput *deckLinkOutput = nullptr;
    result = _deckLink->QueryInterface(IID_IDeckLinkOutput, (void**)&deckLinkOutput);
    if(result != S_OK)
    {
      LOGF(WARNING, "Could not obtain the IDeckLinkInput interface - result = %08x\n", result);
      return false;
    }

    _deckLinkOutput.reset( deckLinkOutput );
    _outputCallback.reset( new OutputCallback( _deckLinkOutput ));

    // Set the callback object to the DeckLink device's output interface
    result = _deckLinkOutput->SetScheduledFrameCompletionCallback(_outputCallback.get());
    if(result != S_OK)
    {
      LOGF(WARNING, "Could not set callback - result = %08x\n", result);
      return false;
    }

    // Enable video output
    result = _deckLinkOutput->EnableVideoOutput(kDisplayMode, kOutputFlag);
    if(result != S_OK)
    {
      LOGF(WARNING, "Could not enable video output - result = %08x\n", result);
      return false;
    }

    return true;
  }

  bool DeckLinkSource::sendSDICameraControl()
  {
    if( !_deckLinkOutput ) {
      if (!createVideoOutput()) return false;
    }

    CHECK(_deckLinkOutput);

    std::unique_ptr<IDeckLinkMutableVideoFrame> videoFrameBlue( CreateFrame(_deckLinkOutput) );

    // These are magic values for 1080i50   See SDK manual page 213
    const uint32_t kFrameDuration = 1000;
    const uint32_t kTimeScale = 25000;

    auto result = deckLinkOutput->ScheduleVideoFrame(*videoFrameBlue, 0, kFrameDuration, kTimeScale);
    if(result != S_OK)
    {
      LOG(WARNING) << "Could not schedule video frame - result = " << std::hex << result;
      return false;
    }

    //
    result = deckLinkOutput->StartScheduledPlayback(0, kTimeScale, 1.0);
    if(result != S_OK)
    {
      LOG(WARNING) << "Could not schedule video frame - result = " << std::hex << result;
      return false;
    }

    // And stop after one frame
    BMDTimeValue stopTime;
    result = deckLinkOutput->StopScheduledPlayback(kFrameDuration, &stopTime, kTimeScale);


    return true;
  }




  int DeckLinkSource::getImage( int i, cv::Mat &mat )
  {
    switch(i) {
      case 0:
      mat = _grabbedImage;
      return 1;
      break;
      default:
      return 0;
    }
  }

  ImageSize DeckLinkSource::imageSize( void ) const
  {

  }

}

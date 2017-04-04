
#include "libvideoio_bm/DeckLinkSource.h"

namespace libvideoio_bm {

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
      LOG(FATAL) << "This application requires the DeckLink drivers installed.";
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
    if ((result = deckLinkIterator->Next(&deckLink)) != S_OK) {
      LOG(FATAL) << "Couldn't get information on the first DeckLink object.";
      return;
    }

    // Get the input (capture) interface of the DeckLink device
    result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&_deckLinkInput);
    if (result != S_OK) {
      LOG(FATAL) << "Couldn't get input for Decklink";
      return;
    }

    //      // Get the display mode
    //       if (g_config.m_displayModeIndex == -1)
    //       {
    //               // Check the card supports format detection
    //               result = deckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&deckLinkAttributes);
    //               if (result == S_OK)
    //               {
    //                       result = deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &formatDetectionSupported);
    //                       if (result != S_OK || !formatDetectionSupported)
    //                       {
    //                               fprintf(stderr, "Format detection is not supported on this device\n");
    //                               goto bail;
    //                       }
    //               }
    //
    //               g_config.m_inputFlags |= bmdVideoInputEnableFormatDetection;
    //
    //                 // Format detection still needs a valid mode to start with
    //                 idx = 0;
    //         }
    //         else
    //         {
    //                 idx = g_config.m_displayModeIndex;
    //         }
    //
    //         result = g_deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
    //         if (result != S_OK)
    //                 goto bail;
    //
    //         while ((result = displayModeIterator->Next(&displayMode)) == S_OK)
    //         {
    //                 if (idx == 0)
    //                         break;
    //                 --idx;
    //
    //                 displayMode->Release();
    //         }
    //
    //         if (result != S_OK || displayMode == NULL)
    //         {
    //                 fprintf(stderr, "Unable to get display mode %d\n", g_config.m_displayModeIndex);
    //                 goto bail;
    //         }
    //
    //         // Get display mode name
    //         result = displayMode->GetName((const char**)&displayModeName);
    //         if (result != S_OK)
    //
    //         {
    //          displayModeName = (char *)malloc(32);
    //          snprintf(displayModeName, 32, "[index %d]", g_config.m_displayModeIndex);
    //  }
    //
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

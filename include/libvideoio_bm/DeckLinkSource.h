#pragma once

#include <atomic>


#include <DeckLinkAPI.h>

#include "libvideoio/DataSource.h"

#include "libbmsdi/bmsdi.h"

#include "InputCallback.h"
#include "OutputCallback.h"
#include "SDICameraControl.h"


namespace libvideoio_bm {

  using libvideoio::DataSource;
  using libvideoio::ImageSize;


class DeckLinkSource : public libvideoio::DataSource {
public:

	DeckLinkSource();
  ~DeckLinkSource();

  // These can be called expicitly before initialize(),
  // Otherwise it will assume defaults.
  bool setDeckLink( int cardno = 0 );
  bool createVideoInput( const BMDDisplayMode desiredMode = bmdModeHD1080p2997, bool do3D = false );
  bool createVideoOutput( const BMDDisplayMode desiredMode = bmdModeHD1080p2997, bool do3D = false );

  bool initialize();
  bool initialized() const { return _initialized; }

  // These start and stop the input streams
  bool startStreams();
  void stopStreams();


  bool queueSDIBuffer( BMSDIBuffer *buffer );

  virtual int numFrames( void ) const { return -1; }

  // // Delete copy operators
  // DeckLinkSource( const DeckLinkSource & ) = delete;
  // DeckLinkSource &operator=( const DeckLinkSource & ) = delete;

  // Pull images from _inputCallback
  virtual bool grab( void );

  virtual int getImage( int i, cv::Mat &mat );

  virtual ImageSize imageSize( void ) const;

protected:

  cv::Mat _grabbedImage;

  bool _initialized;

  // For now assume an object uses just one Decklink board
  // Stupid COM model precludes use of auto ptrs
  IDeckLink *_deckLink;
  IDeckLinkInput *_deckLinkInput;
  IDeckLinkOutput *_deckLinkOutput;

  BMDTimeScale _outputTimeScale;
  BMDTimeValue _outputTimeValue;

  InputCallback *_inputCallback;
  OutputCallback *_outputCallback;

};

}

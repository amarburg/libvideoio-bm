#pragma once

#include <atomic>

#include "libvideoio/DataSource.h"
#include "Delegate.h"

#include <DeckLinkAPI.h>

namespace libvideoio_bm {

  using libvideoio::DataSource;
  using libvideoio::ImageSize;


class DeckLinkSource : public libvideoio::DataSource {
public:

	DeckLinkSource( );
  ~DeckLinkSource();

  // Thread entry point
  void operator()();

  void initialize();
  bool initialized() const { return _initialized; }

  virtual int numFrames( void ) const { return -1; }

  // // Delete copy operators
  // DeckLinkSource( const DeckLinkSource & ) = delete;
  // DeckLinkSource &operator=( const DeckLinkSource & ) = delete;

  virtual bool grab( void );

  virtual int getImage( int i, cv::Mat &mat );

  virtual ImageSize imageSize( void ) const;

  void start();
  void stop();

  ThreadSynchronizer doneSync;
  ThreadSynchronizer initializedSync;

protected:


  cv::Mat _grabbedImage;

  bool _initialized;

  IDeckLinkInput* _deckLinkInput;
  IDeckLinkOutput* _deckLinkOutput;


  DeckLinkCaptureDelegate *_delegate;


};

}

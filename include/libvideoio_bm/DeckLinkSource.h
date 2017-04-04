#pragma once


#include <DeckLinkAPI.h>

#include "libvideoio/DataSource.h"

namespace libvideoio_bm {

  using libvideoio::DataSource;
  using libvideoio::ImageSize;


class DeckLinkSource : public libvideoio::DataSource {
public:

	DeckLinkSource( );
  ~DeckLinkSource();

  void initialize();

  bool initialized() const { return _initialized; }

  // Delete copy operators
  DeckLinkSource( const DeckLinkSource & ) = delete;
  DeckLinkSource &operator=( const DeckLinkSource & ) = delete;

  virtual bool grab( void );

  virtual int getImage( int i, cv::Mat &mat );

  virtual ImageSize imageSize( void ) const;

protected:

  bool _initialized;

  IDeckLinkInput* _deckLinkInput;


};

}

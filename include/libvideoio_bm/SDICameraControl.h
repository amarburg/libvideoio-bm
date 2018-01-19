#pragma once


#include <DeckLinkAPI.h>

namespace libvideoio_bm {

  IDeckLinkMutableVideoFrame* AddSDICameraControlFrame( IDeckLinkOutput *deckLinkOutput, IDeckLinkMutableVideoFrame* frame, BMSDIBuffer *buffer );
  IDeckLinkMutableVideoFrame* CreateBlueFrame( IDeckLinkOutput *deckLinkOutput, bool do3D );


}

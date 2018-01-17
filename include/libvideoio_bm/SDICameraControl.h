#pragma once


#include <DeckLinkAPI.h>

namespace libvideoio_bm {

  IDeckLinkMutableVideoFrame* AddSDICameraControlFrame( IDeckLinkOutput *deckLinkOutput, IDeckLinkMutableVideoFrame* frame );
  IDeckLinkMutableVideoFrame* CreateBlueFrame( IDeckLinkOutput *deckLinkOutput, bool do3D );


}

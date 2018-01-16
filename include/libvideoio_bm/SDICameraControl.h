#pragma once


#include <DeckLinkAPI.h>

namespace libvideoio_bm {


  IDeckLinkMutableVideoFrame* CreateSDICameraControlFrame( IDeckLinkOutput *deckLinkOutput);

}

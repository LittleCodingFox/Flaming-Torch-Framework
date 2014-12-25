#if USE_GRAPHICS
/*!
*	RendererDisplayMode
*	\note only 32 bpp displays are collected since lower than that is incredibly rare these days
*/
class RendererDisplayMode
{
public:
	//!Width of the display
	uint32 Width;
	//!Height of the display
	uint32 Height;
	//!Bits per Pixel
	uint32 Bpp;
};
#endif

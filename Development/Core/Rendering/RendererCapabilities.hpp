#if USE_GRAPHICS
/*!
*	Renderer Capabilities container
*/
class RendererCapabilities
{
public:
	uint32 AntialiasLevel; //!<Anti-Aliasing Level
	uint32 DepthBits; //!<Depth Bits
	uint32 StencilBits; //!<Stencil Bits

	RendererCapabilities() : AntialiasLevel(0), DepthBits(32), StencilBits(0) {};
};
#endif

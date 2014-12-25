#if USE_GRAPHICS
/*!
*	Renderer Frame Statistics
*/
class RendererFrameStats
{
public:
	//!Total amount of draw calls this frame
	uint32 DrawCalls;
	
	//!Total amount of vertices drawn this frame
	uint32 VertexCount;
	
	//!Total amount of texture changes this frame
	uint32 TextureChanges;

	//!Total amount of matrix changes this frame
	uint32 MatrixChanges;

	//!Total amount of clipping changes this frame
	uint32 ClippingChanges;

	//!Total amount of state changes this frame
	uint32 StateChanges;

	//!Total amount of resources loaded
	uint32 TotalResources;

	//!Total amount of RAM used by resources (MB)
	f32 TotalResourceUsage;

	//!Skipped Draw Calls
	uint32 SkippedDrawCalls;

	//!Renderer Name
	std::string RendererName;

	//!Renderer Version
	std::string RendererVersion;

	//!Renderer Custom Message
	std::string RendererCustomMessage;

	RendererFrameStats() : DrawCalls(0), VertexCount(0), TextureChanges(0), MatrixChanges(0), ClippingChanges(0),
		StateChanges(0), TotalResources(0), TotalResourceUsage(0), SkippedDrawCalls(0) {};

	/*!
	*	Clears the statistics
	*/
	void Clear()
	{
		DrawCalls = VertexCount = TextureChanges = MatrixChanges = ClippingChanges =
			StateChanges = TotalResources = SkippedDrawCalls = 0;

		TotalResourceUsage = 0.0f;
	};
};
#endif

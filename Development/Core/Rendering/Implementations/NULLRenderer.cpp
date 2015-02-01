#include "FlamingCore.hpp"
#if USE_GRAPHICS
namespace FlamingTorch
{
	bool NULLRendererImplementation::Create(void *WindowHandle, RendererCapabilities ExpectedCaps)
	{
		return true;
	}

	bool NULLRendererImplementation::Create(const std::string &Title, uint32 Width, uint32 Height, uint32 Style, RendererCapabilities ExpectedCaps)
	{
		return true;
	}

	const RendererCapabilities &NULLRendererImplementation::Capabilities() const
	{
		static RendererCapabilities caps;

		return caps;
	}

	RendererDisplayMode NULLRendererImplementation::DesktopDisplayMode()
	{
		return RendererDisplayMode();
	}

	std::vector<RendererDisplayMode> NULLRendererImplementation::DisplayModes()
	{
		return std::vector<RendererDisplayMode>();
	}

	Vector2 NULLRendererImplementation::Size() const
	{
		return Vector2(1, 1);
	}

	VertexBufferHandle NULLRendererImplementation::CreateVertexBuffer()
	{
		return 1;
	}

	bool NULLRendererImplementation::IsVertexBufferHandleValid(VertexBufferHandle Handle)
	{
		return 1;
	}

	FrameBufferHandle NULLRendererImplementation::CreateFrameBuffer(const FrameBufferCreationInfo &Info)
	{
		return 1;
	}

	void NULLRendererImplementation::BindFrameBuffer(FrameBufferHandle Handle) {}

	bool NULLRendererImplementation::IsFrameBufferValid(FrameBufferHandle Handle)
	{
		return true;
	}

	void NULLRendererImplementation::DestroyFrameBuffer(FrameBufferHandle Handle) {}

	void NULLRendererImplementation::SetVertexBufferData(VertexBufferHandle Handle, uint8 DetailsMode, VertexElementDescriptor *Elements, uint32 ElementCount, const void *Data, uint32 DataByteSize) {}

	void NULLRendererImplementation::DestroyVertexBuffer(VertexBufferHandle Handle) {}

	void NULLRendererImplementation::RenderVertices(uint32 VertexMode, VertexBufferHandle Buffer, uint32 Start, uint32 End) {}

	void NULLRendererImplementation::SetClipRect(const Rect &ClippingRect) {}

	void NULLRendererImplementation::Clear(uint32 Buffers) {}

	void NULLRendererImplementation::Display() {}

	const RendererFrameStats &NULLRendererImplementation::FrameStats() const
	{
		static RendererFrameStats stats;

		return stats;
	}

	void NULLRendererImplementation::SetWorldMatrix(const Matrix4x4 &WorldMatrix) {}

	void NULLRendererImplementation::SetProjectionMatrix(const Matrix4x4 &ProjectionMatrix) {}

	void NULLRendererImplementation::SetViewport(f32 x, f32 y, f32 Width, f32 Height) {}

	TextureHandle NULLRendererImplementation::CreateTexture()
	{
		return 1;
	}

	bool NULLRendererImplementation::IsTextureHandleValid(TextureHandle Handle)
	{
		return true;
	}

	void NULLRendererImplementation::DestroyTexture(TextureHandle Handle) {}

	void NULLRendererImplementation::SetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 Width, uint32 Height) {}

	bool NULLRendererImplementation::GetTextureData(TextureHandle Handle, uint8 *Pixels, uint32 BufferByteCount) { return true; }

	void NULLRendererImplementation::SetTextureWrapMode(TextureHandle Handle, uint32 WrapMode) {}

	void NULLRendererImplementation::SetTextureFiltering(TextureHandle Handle, uint32 Filtering) {}

	void NULLRendererImplementation::SetBlendingMode(uint32 BlendingMode) {}

	void NULLRendererImplementation::BindTexture(TextureHandle Handle) {}

	bool NULLRendererImplementation::CaptureScreen(uint8 *Pixels, uint32 BufferByteCount)
	{
		return true;
	}

	bool NULLRendererImplementation::PollEvent(RendererEvent &Out)
	{
		return false;
	}

	void *NULLRendererImplementation::WindowHandle() const
	{
		return NULL;
	}

	void NULLRendererImplementation::SetMousePosition(const Vector2 &Position) {}

	void NULLRendererImplementation::SetFrameRate(uint32 FPS) {}

	void NULLRendererImplementation::ReportSkippedDrawCall() {}
}
#endif

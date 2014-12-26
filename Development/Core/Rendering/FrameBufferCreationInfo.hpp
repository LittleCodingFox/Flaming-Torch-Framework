#if USE_GRAPHICS
class FrameBufferCreationInfo
{
public:
	std::vector<DisposablePointer<Texture>> ColorBuffers;

	Vector2 Size;
};
#endif

#if USE_GRAPHICS
class ScenePass
{
	friend class Renderer;
private:
	FrameBufferHandle FrameBuffer;
	std::string Name;
public:
	
	ScenePass();
	~ScenePass();

	inline const std::string GetName()
	{
		return Name;
	}

	void Bind(Renderer *TheRenderer);
	void Unbind(Renderer *TheRenderer);
	void Draw(Renderer *TheRenderer);
};
#endif

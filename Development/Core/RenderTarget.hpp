/*!
	Rendering Buffer Types
*/
namespace RenderBufferTypes
{
	enum
	{
		Color = 0, //!<Color Buffer
		Depth, //!<Depth Buffer
		Stencil //!<Stencil Buffer
	};
};

/*!
	Color Buffer Attachments
*/
namespace ColorBufferAttachments
{
	enum
	{
		Color = 0, //!<Color
		Depth, //!<Depth
		Stencil //!<Stencil
	};
};

/*!
	Rendering Target
	Allows for rendering a frame of content to one or more textures
*/
class RenderTarget
{
private:
	uint32 FBOHandle;
	std::vector<uint32 > RenderBuffers;
	std::vector<Texture *> Attachments;
	uint32 WidthValue, HeightValue;
public:
	RenderTarget();
	~RenderTarget();
	
	/*!
		Destroy the internal data of this Render Target
	*/
	void Destroy();
	/*!
		Try to initialize this Render Target
		\return Whether the Render Target was initialized
	*/
	bool Initialize();
	/*!
		Add a Rendering Buffer
		\param Width the Width of the Rendering Buffer
		\param Height the Height of the Rendering Buffer
		\param Type one of RenderBufferTypes::*
		\sa RenderBufferTypes
	*/
	void AddRenderBuffer(uint32 Width, uint32 Height, uint32 Type);
	/*!
		Add a Color Buffer
		\param Texture the Texture to attach
		\param Attachment one of ColorBufferAttachments::*
		\return Whether we successfully attached this buffer
		\sa ColorBufferAttachments
	*/
	bool AddColorBuffer(Texture *Texture, uint32 Attachment = ColorBufferAttachments::Color);

	/*!
		\return the Width of the Render Target
	*/
	uint32 Width();

	/*!
		\return the Height of the Render Target
	*/
	uint32 Height();

	/*!
		Activates this Render Target so all rendering will go to it
	*/
	void Activate();

	/*!
		Deactivates this Render Target
	*/
	void Deactivate();
};

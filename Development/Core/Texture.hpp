#pragma once

namespace TextureFiltering
{
	enum TextureFiltering
	{
		Nearest,
		Linear,
		Nearest_Mipmap,
		Linear_Mipmap
	};
};

namespace ColorType
{
	enum ColorType
	{
		RGB8 = 0,
		RGBA8
	};
};

namespace TextureEncoderType
{
	enum TextureEncoderType
	{
		PNG,
		WebP,
		FTI //Flaming Torch Image
	};
};

/*!
*	Texture Encoder Settings
*/
struct TextureEncoderInfo
{
	/*!
	*	Target Width for resizing
	*	\note Only used for webP
	*/
	uint32 TargetWidth;

	/*!
	*	Target Height for resizing
	*	\note Only used for webP
	*/
	uint32 TargetHeight;

	/*!
	*	Target quality [0, 100]
	*	\note Only used for webP
	*/
	uint32 Quality;
	
	/*!
	*	Encoder ID
	*/
	uint32 Encoder;
	
	/*!
	*	Whether encoding should be lossless
	*	\note Only used for webP
	*/
	bool Lossless;

	TextureEncoderInfo() : TargetWidth(0), TargetHeight(0), Quality(100), Encoder(TextureEncoderType::PNG), Lossless(true) {};
};

namespace TextureWrapMode
{
	enum TextureWrapMode
	{
		Clamp,
		ClampToBorder,
		ClampToEdge,
		Repeat
	};
};

/*!
*	Texture Buffer
*/
class TextureBuffer
{
private:
	uint32 WidthValue, HeightValue, ColorTypeValue;
public:
	/*!
	*	The Pixel Data of this TextureBuffer
	*/
	std::vector<uint8> Data;

	TextureBuffer() : WidthValue(0), HeightValue(0), ColorTypeValue(ColorType::RGBA8) {};

	/*!
	*	Loads this TextureBuffer from data
	*	\param Pixels the Pixels to read
	*	\param Width the width of the image
	*	\param Height the height of the image
	*	\note Pixels should be large enough to accommodate Width x Height x 4 bytes
	*	\note Pixels is expected to be RGBA8 Data
	*/
	bool FromData(const uint8 *Pixels, uint32 Width, uint32 Height);

	/*!
	*	Loads this TextureBuffer from a Stream
	*	\param Stream the Stream to read from
	*/
	bool FromStream(Stream *Stream);

	/*!
	*	Loads this TextureBuffer as an empty Image
	*	\param Width the width of the image
	*	\param Height the height of the image
	*	\param BaseColor the Base Color of the image
	*	\note Think of BaseColor like the Background Color that is filling the entire image
	*/
	void CreateEmpty(uint32 Width, uint32 Height, const Vector4 &BaseColor = Vector4());

	/*!
	*	Saves this TextureBuffer into a Stream
	*	\param Out the stream to write to
	*	\param Info the Encoder Settings to encode the image with
	*/
	bool Save(Stream *Out, const TextureEncoderInfo &Info);
	
	/*!
	*	Flips this TextureBuffer Horizontally
	*/
	void FlipX();
	
	/*!
	*	Flips this TextureBuffer Vertically
	*/
	void FlipY();

	/*!
	*	Makes a copy of this TextureBuffer
	*/
	DisposablePointer<TextureBuffer> Clone();
	
	/*!
	*	Overlays a TextureBuffer with this TextureBuffer
	*	\param Other the other TextureBuffer
	*	\note the Other TextureBuffer should have the same size and color type as this TextureBuffer
	*/
	bool Overlay(TextureBuffer *Other);

	/*!
	*	Multiplies a TextureBuffer with this TextureBuffer
	*	\param Other the other TextureBuffer
	*	\note the Other TextureBuffer should have the same size and color type as this TextureBuffer
	*/
	bool Multiply(TextureBuffer *Other);

	/*!
	*	Alpha-blends a TextureBuffer with this TextureBuffer
	*	\param x the x position where to blend
	*	\param y the y position where to blend
	*	\param Other the other TextureBuffer
	*	\note the Other TextureBuffer should have the same color type as this TextureBuffer
	*	\note the Other TextureBuffer doesn't have to have the same size as this one
	*/
	bool Blend(uint32 x, uint32 y, TextureBuffer *Other);

	/*!
	*	The Width of this TextureBuffer
	*/
	uint32 Width() const;

	/*!
	*	The Height of this TextureBuffer
	*/
	uint32 Height() const;

	/*!
	*	The Color Type of this TextureBuffer
	*/
	uint32 ColorType() const;
};

class TexturePacker;

class TexturePackerIndex
{
public:
	int32 Index, WrapMode, Filtering;

	DisposablePointer<TexturePacker> Owner;

	TexturePackerIndex() : Index(-1), Filtering(TextureFiltering::Nearest), WrapMode(TextureWrapMode::Clamp) {};

	uint32 Width() const;
	uint32 Height() const;
	TextureHandle Handle() const;
	uint32 ColorType() const;
};

class Renderer;

/*!
*	Texture Class
*/
class Texture
{
private:
	Texture(const Texture &o);
	TextureHandle HandleValue;
	DisposablePointer<TextureBuffer> Buffer;

	uint32 WidthValue, HeightValue, ColorTypeValue, TextureFilter, TextureWrap;

	struct TextureState
	{
		uint32 FilterMode, WrapMode;
	};

	std::vector<TextureState> StateStack;

	TexturePackerIndex Index;

#if USE_GRAPHICS
	DisposablePointer<Renderer> Owner;
#endif

public:
	/*!
	*	Whether Textures keep their data when loading
	*/
	static bool KeepData;

	Texture();
	~Texture();
	bool operator==(const Texture &o) const;
	
	/*!
	*	Destroys this texture
	*/
	void Destroy();
	
	/*!
	*	Loads this texture from a buffer
	*	\param Buffer
	*/
	bool FromBuffer(DisposablePointer<TextureBuffer> Buffer);
	
	/*!
	*	Loads this texture from data
	*	\param Pixels the Pixels to read
	*	\param Width the width of the image
	*	\param Height the height of the image
	*	\note Pixels should be large enough to accommodate Width x Height x 4 bytes
	*	\note Pixels is expected to be RGBA8 Data
	*/
	bool FromData(const uint8 *Pixels, uint32 Width, uint32 Height);

	/*!
	*	Loads this texture from a file
	*	\param FileName is the filename to load
	*/
	bool FromFile(const std::string &FileName);
	
	/*!
	*	Loads this texture from a Stream
	*	\param Stream the stream to load
	*/
	bool FromStream(Stream *Stream);
	
	/*!
	*	Loads this texture from a file in the package filesystem
	*	\param Directory the directory of the file
	*	\param Name the name of the file
	*/
	bool FromPackage(const std::string &Directory, const std::string &Name);
	
	/*!
	*	Creates an empty texture
	*	\param Width the width of the texture
	*	\param Height the height of the texture
	*	\param RGBA whether the texture should be transparent and use RGBA pixels
	*/
	bool CreateEmptyTexture(uint32 Width, uint32 Height, bool RGBA = true);

	/*!
	*	Sets the texture's filtering
	*	\param Filter the filtering mode
	*/
	void SetTextureFiltering(int32 Filter);
	
	/*!
	*	Updates the data inside this texture
	*	\param Pixels the Pixels to read
	*	\param Width the width of the image
	*	\param Height the height of the image
	*	\note Pixels should be large enough to accommodate Width x Height x 4 bytes
	*	\note Pixels is expected to be RGBA8 Data
	*/
	void UpdateData(const uint8 *Pixels, uint32 Width, uint32 Height);

	/*!
	*	Gets a Pixel at a specific point of this image
	*	\param x the x position of the pixel
	*	\param y the y position of the pixel
	*	\return the Pixel as a Vector4
	*/
	Vector4 GetPixel(uint32 x, uint32 y);

#if USE_GRAPHICS
	/*!
	*	Creates a texture from the screen's contents if available
	*/
	bool FromScreen();
#endif

	/*!
	*	Binds this texture for rendering
	*/
	void Bind();

	/*!
	*	Pushes the Texture States so you can restore them later
	*/
	void PushTextureStates();

	/*!
	*	Pops the Texture States, restoring one of the old states
	*/
	void PopTextureStates();
	
	/*!
	*	Sets the wrapping mode for the texture
	*	\param WrapMode the Wrapping Mode we want to use
	*/
	void SetWrapMode(uint32 WrapMode);

	/*!
	*	Sets the Texture Packing Index of this texture
	*/
	void SetIndex(TexturePackerIndex Index);
	
	/*!
	*	Gets the Texture Packing Index of this texture
	*/
	const TexturePackerIndex &GetIndex() const;

	/*!
	*	Width of this texture
	*/
	uint32 Width() const;

	/*!
	*	Height of this texture
	*/
	uint32 Height() const;

	/*!
	*	Color Type of this texture
	*/
	uint32 ColorType() const;
	
	/*!
	*	Texture Filtering Mode
	*/
	uint32 FilterMode() const;
	
	/*!
	*	Texture Wrapping Mode
	*/
	uint32 WrapMode() const;
	
	/*!
	*	Texture Handle
	*/
	TextureHandle Handle() const;

	/*!
	*	Get the Buffer associated with this texture
	*/
	DisposablePointer<TextureBuffer> GetData() const;
	
	/*!
	*	Texture Size
	*/
	Vector2 Size() const;

	/*!
	*	Blur this texture
	*	\param Radius is the radius to blur with
	*	\param Strength is the iterations to perform
	*/
	void Blur(uint32 Radius, uint32 Strength = 1);
};

/*!
*	Texture Packer Class
*/
class TexturePacker
{
public:
	class SortedTexture
	{
	public:
		int32 Index;
		uint32 x, y, Width, Height;
		DisposablePointer<Texture> TextureInstance;

		SortedTexture() : Index(-1), x(0), y(0), Width(0), Height(0) {};
	};

	/*!
	*	Sorted Texture Indices
	*/
	std::vector<SortedTexture> Indices;

	/*!
	*	Main Texture
	*/
	DisposablePointer<Texture> MainTexture;

	/*!
	*	Gets the packing-ready texture from an index
	*	\param Index the index of the texture to get
	*/
	DisposablePointer<Texture> GetTexture(uint32 Index);

	/*!
	*	\return the total amount of indices in this Texture Packer
	*/
	inline uint32 IndexCount()
	{
		return Indices.size();
	};

	/*!
	*	Creates a TexturePacker from multiple textures
	*	\param Textures the Textures to load from
	*	\param MaxWidth the maximum width of the texture
	*	\param MaxHeight the maximum height of the texture
	*/
	static DisposablePointer<TexturePacker> FromTextures(const std::vector<DisposablePointer<Texture> > &Textures,
		uint32 MaxWidth, uint32 MaxHeight);

	/*!
	*	Creates a TexturePacker from a prepacked Texture
	*	\param MainTexture the prepacked Texture
	*	\param Config the prepacked texture configuration
	*/
	static DisposablePointer<TexturePacker> FromConfig(DisposablePointer<Texture> MainTexture, const GenericConfig &Config);
};

/*!
*	Texture Group
*	Automatically repacks textures into a single TexturePacker
*/
class TextureGroup
{
private:
	DisposablePointer<TexturePacker> PackedTexture;
	uint32 MaxWidth, MaxHeight;
	std::vector<DisposablePointer<Texture> > InstanceTextures, StoredTextures;

	void UpdateInstanceTextures();
public:
	TextureGroup(uint32 MaxWidth, uint32 MaxHeight);

	/*!
	*	Adds a texture
	*	\param t the texture to add to this group
	*	\return a texture index or -1
	*/
	int32 Add(DisposablePointer<Texture> t);

	/*!
	*	Gets a texture
	*	\param Index the texture index
	*/
	DisposablePointer<Texture> Get(int32 Index);
};

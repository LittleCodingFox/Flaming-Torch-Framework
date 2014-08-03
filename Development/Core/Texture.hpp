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
	enum
	{
		RGB8 = 0,
		RGBA8
	};
};

namespace TextureEncoderType
{
	enum
	{
		PNG,
		WebP,
		FTI //Flaming Torch Image
	};
};

struct TextureEncoderInfo
{
	uint32 TargetWidth, TargetHeight; //Only used for WebP
	uint32 Quality; //Only used for webP
	uint32 Encoder; //One of TextureEncoderType::PNG/WebP/FTI
	bool Lossless;

	TextureEncoderInfo() : TargetWidth(0), TargetHeight(0), Quality(100), Encoder(TextureEncoderType::PNG), Lossless(true) {};
};

namespace TextureWrapMode
{
	enum
	{
		Clamp,
		ClampToBorder,
		ClampToEdge,
		Repeat
	};
};

class FLAMING_API TextureBuffer
{
private:
	uint32 WidthValue, HeightValue, ColorTypeValue;
public:
	std::vector<uint8> Data;

	TextureBuffer() : WidthValue(0), HeightValue(0), ColorTypeValue(ColorType::RGBA8) {};

	bool FromData(const uint8 *Pixels, uint32 Width, uint32 Height);
	bool FromStream(Stream *Stream);
	void CreateEmpty(uint32 Width, uint32 Height, const Vector4 &BaseColor = Vector4());
	bool Save(Stream *Out, const TextureEncoderInfo &Info);
	void FlipX();
	void FlipY();
	SuperSmartPointer<TextureBuffer> Clone();
	bool Overlay(TextureBuffer *Other);
	bool Multiply(TextureBuffer *Other);
	bool Blend(TextureBuffer *Other);

	uint32 Width() const;
	uint32 Height() const;
	uint32 ColorType() const;
};

class TexturePacker;

class TexturePackerIndex
{
public:
	int32 Index, WrapMode, Filtering;

	SuperSmartPointer<TexturePacker> Owner;

	TexturePackerIndex() : Index(-1), Filtering(TextureFiltering::Nearest), WrapMode(TextureWrapMode::Clamp) {};

	uint32 Width() const;
	uint32 Height() const;
	uint32 GLID() const;
	uint32 ColorType() const;
};

class FLAMING_API Texture
{
private:
	Texture(const Texture &o);
	int32 GLID;
	SuperSmartPointer<TextureBuffer> Buffer;

	uint32 WidthValue, HeightValue, ColorTypeValue, TextureFilter, TextureWrap;

	struct TextureState
	{
		uint32 FilterMode, WrapMode;
	};

	std::vector<TextureState> StateStack;

	TexturePackerIndex Index;

public:
	static bool KeepData;

	Texture();
	~Texture();
	bool operator==(const Texture &o) const;
	void Destroy();
	bool FromBuffer(SuperSmartPointer<TextureBuffer> Buffer);
	//Expects RGBA8
	bool FromData(const uint8 *Pixels, uint32 Width, uint32 Height);
	bool FromFile(const std::string &FileName);
	bool FromStream(Stream *Stream);
	bool FromPackage(const std::string &Directory, const std::string &Name);
	bool CreateEmptyTexture(uint32 Width, uint32 Height, bool RGBA = true);
	void SetTextureFiltering(int32 Filter);
	void UpdateData(const uint8 *Pixels, uint32 Width, uint32 Height);

	Vector4 GetPixel(uint32 x, uint32 y);

#if USE_GRAPHICS
	bool FromScreen();
#endif

	void Bind();
	void PushTextureStates();
	void PopTextureStates();
	void SetWrapMode(uint32 WrapMode);
	void SetIndex(TexturePackerIndex Index);
	const TexturePackerIndex &GetIndex() const;

	uint32 Width() const;
	uint32 Height() const;
	uint32 ColorType() const;
	//Texture Filtering Mode
	uint32 FilterMode() const;
	//Texture Wrapping Mode
	uint32 WrapMode() const;
	int32 ID() const;
	SuperSmartPointer<TextureBuffer> GetData() const;
	Vector2 Size() const;

	void Blur(uint32 Radius, uint32 Strength = 1);
};

class TexturePacker
{
public:
	class SortedTexture
	{
	public:
		int32 Index;
		uint32 x, y, Width, Height;
		SuperSmartPointer<Texture> TextureInstance;

		SortedTexture() : Index(-1), x(0), y(0), Width(0), Height(0) {};
	};

	std::vector<SortedTexture> Indices;

	SuperSmartPointer<Texture> MainTexture;

	SuperSmartPointer<Texture> GetTexture(uint32 Index);

	static SuperSmartPointer<TexturePacker> FromTextures(const std::vector<SuperSmartPointer<Texture> > &Textures, uint32 MaxWidth, uint32 MaxHeight);
};

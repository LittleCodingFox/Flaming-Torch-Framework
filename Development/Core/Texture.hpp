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
	void CreateEmpty(uint32 Width, uint32 Height);
	bool Save(Stream *Out, const TextureEncoderInfo &Info);

	uint32 Width();
	uint32 Height();
	uint32 ColorType();
};

class FLAMING_API Texture
{
private:
	Texture(const Texture &o);
	Texture &operator=(const Texture &o);
	int32 GLID;
	SuperSmartPointer<TextureBuffer> Buffer;

	uint32 WidthValue, HeightValue, ColorTypeValue, TextureFilter, TextureWrap;

	struct TextureState
	{
		uint32 FilterMode, WrapMode;
	};

	std::vector<TextureState> StateStack;

public:
	static bool KeepData;

	Texture();
	~Texture();
	void Destroy();
	//Expects RGBA8
	bool FromData(const uint8 *Pixels, uint32 Width, uint32 Height);
	bool FromFile(const std::string &FileName);
	bool FromStream(Stream *Stream);
	bool FromPackage(const std::string &Directory, const std::string &Name);
	bool CreateEmptyTexture(uint32 Width, uint32 Height, bool RGBA = true);
	void SetTextureFiltering(int32 Filter);
	void UpdateData(const uint8 *Pixels, uint32 Width, uint32 Height);

#if USE_GRAPHICS
	bool FromScreen();
#endif

	void Bind();
	void PushTextureStates();
	void PopTextureStates();
	void SetWrapMode(uint32 WrapMode);

	uint32 Width();
	uint32 Height();
	uint32 ColorType();
	//Texture Filtering Mode
	uint32 FilterMode();
	//Texture Wrapping Mode
	uint32 WrapMode();
	int32 ID();
	SuperSmartPointer<TextureBuffer> GetData();
	Vector2 Size();

	void Blur(uint32 Radius, uint32 Strength = 1);
};

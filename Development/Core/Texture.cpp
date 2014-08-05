#include "FlamingCore.hpp"
#include <png.h>
#include "webp/encode.h"
#include "webp/decode.h"
namespace FlamingTorch
{
#	define TAG "Texture"
#	define TAGBUFFER "TextureBuffer"

	bool Texture::KeepData = true;

	struct FTIHeader
	{
		uint32 ID; //'F' 'T' 'I' 'H'
		uint32 Version;
		uint32 Width;
		uint32 Height;
		uint8 ColorType;
	};

	bool LoadFTI(Stream *In, uint32 &Width, uint32 &Height, uint32 &ColorTypeValue, std::vector<uint8> &Data)
	{
		FTIHeader HDR;

		SFLASSERT(In->Read2<FTIHeader>(&HDR));

		if(HDR.ID != CoreUtils::MakeIntFromBytes('F', 'T', 'I', 'H'))
		{
			Log::Instance.LogErr(TAG, "While reading a FTI Image: Invalid ID for this file!");

			return false;
		};

		if(HDR.Version != CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR))
		{
			Log::Instance.LogErr(TAG, "While reading a FTI Image: Invalid Version for this file!");
			
			return false;
		};

		Width = HDR.Width;
		Height = HDR.Height;
		ColorTypeValue = HDR.ColorType;

		Data.resize(Width * Height * (ColorTypeValue == ColorType::RGB8 ? 3 : 4));

		return In->Read2<uint8>(&Data[0], Data.size());
	};

	bool WriteFTI(Stream *Out, uint32 &Width, uint32 &Height, uint32 &ColorTypeValue, const std::vector<uint8> &Data)
	{
		FTIHeader HDR;
		HDR.ID = CoreUtils::MakeIntFromBytes('F', 'T', 'I', 'H');
		HDR.Version = CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR);
		HDR.Width = Width;
		HDR.Height = Height;
		HDR.ColorType = (uint8)ColorTypeValue;

		SFLASSERT(Out->Write2<FTIHeader>(&HDR));

		return Data.size() && Out->Write2<uint8>(&Data[0], Data.size());
	};

	bool LoadWebP(Stream *In, uint32 &Width, uint32 &Height, std::vector<uint8> &Data)
	{
		PROFILE("TextureBuffer::LoadWebP", StatTypes::Rendering);

		WebPDecoderConfig config;
		WebPDecBuffer* const output_buffer = &config.output;
		WebPBitstreamFeatures* const bitstream = &config.input;

		if(!WebPInitDecoderConfig(&config))
		{
			Log::Instance.LogErr(TAGBUFFER, "@LoadWebP: Failed to load a webP stream due to invalid decoder");

			return false;
		};

		VP8StatusCode status = VP8_STATUS_OK;
		size_t data_size = 0;

		size_t Length = (size_t)(In->Length() - In->Position());
		std::vector<uint8> FeatureData(Length);

		SFLASSERT(In->Read2<uint8>(&FeatureData[0], Length));

		status = WebPGetFeatures(&FeatureData[0], Length, bitstream);

		SFLASSERT(status == VP8_STATUS_OK);

		if(bitstream->has_animation)
		{
			WebPFreeDecBuffer(output_buffer);

			Log::Instance.LogErr(TAGBUFFER, "@LoadWebP: Unable to decode animated webP");

			return false;
		};

		status = WebPDecode(&FeatureData[0], Length, &config);

		Width = output_buffer->width;
		Height = output_buffer->height;

		uint8 *RGB = output_buffer->u.RGBA.rgba;
		Data.resize(Width * Height * 4);

		uint8 ColorSpace = output_buffer->colorspace;

		//Not getting any RGBAs for some reason
		if(ColorSpace == MODE_RGB)
		{
			for(uint32 i = 0, index = 0; i < Width * Height * 4; i += 4, index += 3)
			{
				Data[i] = RGB[index];
				Data[i + 1] = RGB[index + 1];
				Data[i + 2] = RGB[index + 2];
				Data[i + 3] = 255;
			};
		}
		else
		{
			memcpy(&Data[0], RGB, sizeof(uint8) * Data.size());
		};

		WebPFreeDecBuffer(output_buffer);

		return true;
	};

	static int WebPStreamWriter(const uint8_t* data, size_t data_size, const WebPPicture* const pic)
	{
		Stream *Out = (Stream*)pic->custom_ptr;

		return data_size ? (Out->Write2<uint8_t>(data, data_size)) : 1;
	}

	bool WriteWebP(Stream *Out, uint32 Width, uint32 Height, const std::vector<uint8> &Data, bool Lossless, uint32 Quality = 100, uint32 TargetWidth = 0, uint32 TargetHeight = 0)
	{
		PROFILE("TextureBuffer::WriteWebP", StatTypes::Rendering);

		WebPConfig config;
		WebPMemoryWriter memory_writer;
		WebPPicture picture;
		WebPMemoryWriterInit(&memory_writer);

		if(!WebPPictureInit(&picture) || !WebPConfigInit(&config))
		{
			Log::Instance.LogErr(TAGBUFFER, "@WriteWebP: Failed to write a webP stream: Failed to init picture/config");
			
			return false;
		};

		config.quality = (f32)Quality;
		config.lossless = Lossless ? 1 : 0;
		config.method = 0;
		picture.width = Width;
		picture.height = Height;
		picture.use_argb = 1;
		picture.writer = WebPStreamWriter;
		picture.custom_ptr = Out;

		if(!WebPValidateConfig(&config) || !WebPPictureAlloc(&picture))
		{
			Log::Instance.LogErr(TAGBUFFER,
				"@WriteWebP: Failed to write a webP stream: Failed to init validate config or alloc picture");
			
			return false;
		};

		if(!WebPPictureImportRGBA(&picture, &Data[0], Data.size() / Height))
		{
			WebPPictureFree(&picture);
			Log::Instance.LogErr(TAGBUFFER, "@WriteWebP: Failed to write a webP stream: Failed to import RGBA");

			return false;
		};

		if(TargetWidth != 0 && TargetHeight != 0)
		{
			if(!WebPPictureRescale(&picture, TargetWidth, TargetHeight))
			{
				WebPPictureFree(&picture);

				Log::Instance.LogErr(TAGBUFFER,
					"@WriteWebP: Failed to write a webP stream: Failed to rescale picture from %dx%d to %dx%d",
					Width, Height, TargetWidth, TargetHeight);

				return false;
			};
		};

		if(!WebPEncode(&config, &picture))
		{
			Log::Instance.LogErr(TAGBUFFER, "@WriteWebP: Failed to encode the final picture");
			
			return false;
		};

		return true;
	};

	void PNGReadFromStream(png_structp ptr, png_bytep dest, png_size_t len)
	{
		Stream *t = (Stream *)png_get_io_ptr(ptr);
		t->Read(dest, sizeof(uint8), len);
	};

	void PNGWriteToStream(png_structp ptr, png_bytep dest, png_size_t len)
	{
		Stream *t = (Stream *)png_get_io_ptr(ptr);
		t->Write(dest, sizeof(uint8), len);
	};

	void PNGDummyFlush(png_structp ptr) {};

	bool LoadPNG(png_structp png_ptr, png_infop info_ptr, uint32 &Width, uint32 &Height, std::vector<uint8> &Data)
	{
		PROFILE("TextureBuffer::LoadPNG", StatTypes::Rendering);

		int BitDepth = 0, ColorType = 0, Interlace = 0, Compression = 0, Filter = 0;

		png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)&Width, (png_uint_32 *)&Height, &BitDepth, &ColorType,
			&Interlace, &Compression, &Filter);

		//If the depth is > 8, strip to 8
		if(BitDepth > 8)
			png_set_strip_16(png_ptr);

		//If our color type is palette, convert to RGB
		if(ColorType == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_palette_to_rgb(png_ptr);

			//Then convert paletted alpha too
			if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			{
				png_set_tRNS_to_alpha(png_ptr);
				ColorType = PNG_COLOR_TYPE_RGBA;
			}
			else
			{
				ColorType = PNG_COLOR_TYPE_RGB;
			};
		}
		//If we have grayscale, check whether grayscale-alpha and convert it to RGB
		else if(ColorType == PNG_COLOR_TYPE_GA || ColorType == PNG_COLOR_TYPE_GRAY)
		{
			png_set_gray_to_rgb(png_ptr);
			ColorType = ColorType == PNG_COLOR_TYPE_GA ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
		};

		Data.resize(Width * Height * sizeof(uint8[4]));

		uint8 *DataPtr = &Data[0];

		switch(ColorType)
		{
		case PNG_COLOR_TYPE_RGBA:
			{
				int32 Size = Width * sizeof(uint8[4]);

				for (uint32 i = 0; i < Height; i++, DataPtr += Size) {
					png_read_row(png_ptr, DataPtr, NULL);
				};

				png_destroy_read_struct(&png_ptr, &info_ptr,  NULL);
				png_destroy_info_struct(png_ptr, &info_ptr);

				return true;
			};

			break;

		case PNG_COLOR_TYPE_RGB:
			{
				int32 Size = Width * sizeof(uint8[3]);
				int32 SizeA = Width * sizeof(uint8[4]);

				std::vector<uint8> Row(Size);

				for (uint32 i = 0; i < Height; i++, DataPtr += SizeA) {
					png_read_row(png_ptr, &Row[0], NULL);

					for(int32 j = 0, index = 0; j < Size; j += 3, index += 4)
					{
						DataPtr[index] = Row[j];
						DataPtr[index + 1] = Row[j + 1];
						DataPtr[index + 2] = Row[j + 2];
						DataPtr[index + 3] = 255;
					};
				};

				png_destroy_read_struct(&png_ptr, &info_ptr,  NULL);
				png_destroy_info_struct(png_ptr, &info_ptr);

				return true;
			};

			break;

		default:
			png_destroy_read_struct(&png_ptr, &info_ptr,  NULL);
			png_destroy_info_struct(png_ptr, &info_ptr);

			Log::Instance.LogErr(TAGBUFFER, "@LoadPNG: Failed to read a PNG stream: Invalid Color Mode 0x%08x", ColorType);
			

			return false;
		};
	};

	bool WritePNG(Stream *Out, uint32 Width, uint32 Height, const std::vector<uint8> &Data)
	{
		PROFILE("TextureBuffer::WritePNG", StatTypes::Rendering);

		//Create write and info structures
		png_structp pngstruct = NULL;
		png_infop pnginfo = NULL;
		pngstruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if(!pngstruct)
		{
			Log::Instance.LogErr(TAGBUFFER, "@WritePNG: Failed to write a PNG stream: Failed to init PNG");
			
			return false;
		};

		pnginfo = png_create_info_struct(pngstruct);

		if(!pnginfo)
		{
			png_destroy_write_struct(&pngstruct, NULL);

			Log::Instance.LogErr(TAGBUFFER, "@WritePNG: Failed to write a PNG stream: Failed to init PNG");

			return false;
		};

		png_set_write_fn(pngstruct, Out, PNGWriteToStream, PNGDummyFlush);

		png_color_8_struct SignatureBits;

		//Set the image info
		png_set_IHDR(pngstruct, pnginfo, 
			Width, Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,  PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		//Set pixel format
		SignatureBits.red = 8;
		SignatureBits.green = 8;
		SignatureBits.blue = 8;
		SignatureBits.alpha = 8;
		png_set_sBIT(pngstruct, pnginfo, &SignatureBits);	

		//Make a list of all the rows to be saved
		std::vector<uint8 *> RowPtrs(Height);
		uint32 Size = Width * sizeof(uint8[4]);

		for(uint32 y = 0, index = 0; y < Height; y++, index += Size)
		{
			RowPtrs[y] = (uint8*)&Data[index];
		};

		//Send the rows and write the PNG
		png_set_rows(pngstruct, pnginfo, &RowPtrs[0]);

		png_write_png(pngstruct, pnginfo, PNG_TRANSFORM_IDENTITY, NULL);

		png_destroy_write_struct(&pngstruct, &pnginfo);
		png_destroy_info_struct(pngstruct, &pnginfo);

		return true;
	};

	bool TextureBuffer::FromStream(Stream *Stream)
	{
		uint64 Position = Stream->Position();

		png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

		if (!png_ptr)
		{
			Log::Instance.LogErr("TextureBuffer", "Unable to init PNG");

			return false;
		};

		png_infop info_ptr = png_create_info_struct(png_ptr);

		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr,
				(png_infopp)NULL, (png_infopp)NULL);

			Log::Instance.LogErr("TextureBuffer", "Unable to init PNG");

			return false;
		};

		png_set_read_fn(png_ptr, (png_voidp)Stream, PNGReadFromStream);

		uint8 Signature[4];
		
		if(!Stream->Read2<uint8>(Signature, 4))
		{
			Log::Instance.LogInfo("TextureBuffer", "Unable to read texture signature");

			return false;
		};

		Stream->Seek(Position);

		//Will autoconvert to RGBA
		ColorTypeValue = ColorType::RGBA8;

		if(png_check_sig(Signature, 4))
		{
			png_read_info(png_ptr, info_ptr);

			if(!LoadPNG(png_ptr, info_ptr, WidthValue, HeightValue, Data))
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				png_destroy_info_struct(png_ptr, &info_ptr);

				Log::Instance.LogErr("TextureBuffer", "Unable to read Stream '0x%08x' as a PNG.", Stream);

				return false;
			};
		}
		else
		{
			Stream->Seek(Position);

			if(!LoadWebP(Stream, WidthValue, HeightValue, Data))
			{
				Stream->Seek(Position);

				if(!LoadFTI(Stream, WidthValue, HeightValue, ColorTypeValue, Data))
				{
					Log::Instance.LogWarn("TextureBuffer", "Unable to read Stream '0x%08x' as a webP or FTI.", Stream);

					return false;
				}
			};
		};

		return true;
	};

	void TextureBuffer::CreateEmpty(uint32 Width, uint32 Height, const Vector4 &BaseColor)
	{
		WidthValue = Width;
		HeightValue = Height;

		uint8 Components[4] = {
			(uint8)(BaseColor.x * 255),
			(uint8)(BaseColor.y * 255),
			(uint8)(BaseColor.z * 255),
			(uint8)(BaseColor.w * 255)
		};

		Data.resize(Width * Height * 4);

		//memset(&Data[0], *(uint32 *)Components, Data.size());

		for(uint32 i = 0; i < Data.size(); i+=4)
		{
			Data[i] = Components[0];
			Data[i + 1] = Components[1];
			Data[i + 2] = Components[2];
			Data[i + 3] = Components[3];
		};
	};

	SuperSmartPointer<TextureBuffer> TextureBuffer::Clone()
	{
		SuperSmartPointer<TextureBuffer> Out(new TextureBuffer());

		Out->ColorTypeValue = ColorTypeValue;
		Out->WidthValue = WidthValue;
		Out->HeightValue = HeightValue;
		Out->Data = Data;

		return Out;
	};

	inline uint8 SingleOverlay(uint8 Base, uint8 Blend)
	{
		//(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))

		//return Base < 127 ? (2 * Base * Blend) >> 8 : (255 - 2 * (255 - Base) * (255 - Blend)) >> 8;

		f32 BaseFloat = Base / 255.f, BlendFloat = Blend / 255.f;

		return (uint8)((BaseFloat < 0.5f ? (2.0f * BaseFloat * BlendFloat) : (1.0f - 2.0f * (1.0f - BaseFloat) * (1.0f - BlendFloat))) * 255.f);
	};

	bool TextureBuffer::Overlay(TextureBuffer *Other)
	{
		FLASSERT(Other != NULL, "Invalid Buffer to overlay into");

		if(Other->WidthValue != WidthValue || Other->HeightValue != HeightValue || Other->ColorTypeValue != ColorTypeValue)
			return false;

		uint32 SkipComponent = ColorTypeValue == ColorType::RGB8 ? 3 : 4;
		uint32 RowBytes = WidthValue * SkipComponent;

		for(uint32 y = 0, yindex = 0; y < HeightValue; y++, yindex += RowBytes)
		{
			for(uint32 x = 0, xindex = 0; x < WidthValue; x++, xindex += SkipComponent)
			{
				uint8 *MyPixels = &Data[xindex + yindex], *TheirPixels = &Other->Data[xindex + yindex];

				MyPixels[0] = SingleOverlay(MyPixels[0], TheirPixels[0]);
				MyPixels[1] = SingleOverlay(MyPixels[1], TheirPixels[1]);
				MyPixels[2] = SingleOverlay(MyPixels[2], TheirPixels[2]);
			};
		};

		return true;
	};

	bool TextureBuffer::Blend(TextureBuffer *Other)
	{
		FLASSERT(Other != NULL, "Invalid Buffer to blend into");

		if(Other->WidthValue != WidthValue || Other->HeightValue != HeightValue || Other->ColorTypeValue != ColorTypeValue)
			return false;

		uint32 SkipComponent = ColorTypeValue == ColorType::RGB8 ? 3 : 4;
		uint32 RowBytes = WidthValue * SkipComponent;

		for(uint32 y = 0, yindex = 0; y < HeightValue; y++, yindex += RowBytes)
		{
			for(uint32 x = 0, xindex = 0; x < WidthValue; x++, xindex += SkipComponent)
			{
				uint8 *MyPixels = &Data[xindex + yindex], *TheirPixels = &Other->Data[xindex + yindex];

				if(SkipComponent == 4)
				{
					if(MyPixels[3] == TheirPixels[3] && MyPixels[3] == 0)
						continue;

					MyPixels[0] = ((MyPixels[0] * TheirPixels[3]) >> 8) + ((TheirPixels[0] * (255 - TheirPixels[3])) >> 8);
					MyPixels[1] = ((MyPixels[1] * TheirPixels[3]) >> 8) + ((TheirPixels[1] * (255 - TheirPixels[3])) >> 8);
					MyPixels[2] = ((MyPixels[2] * TheirPixels[3]) >> 8) + ((TheirPixels[2] * (255 - TheirPixels[3])) >> 8);
					MyPixels[3] = ((MyPixels[3] * TheirPixels[3]) >> 8) + ((TheirPixels[3] * (255 - TheirPixels[3])) >> 8);
				}
				else
				{
					MyPixels[0] = (MyPixels[0] * TheirPixels[0]) >> 8;
					MyPixels[1] = (MyPixels[1] * TheirPixels[1]) >> 8;
					MyPixels[2] = (MyPixels[2] * TheirPixels[2]) >> 8;
				};
			};
		};

		return true;
	};

	bool TextureBuffer::Multiply(TextureBuffer *Other)
	{
		FLASSERT(Other != NULL, "Invalid Buffer to multiply into");

		if(Other->WidthValue != WidthValue || Other->HeightValue != HeightValue || Other->ColorTypeValue != ColorTypeValue)
			return false;

		uint32 SkipComponent = ColorTypeValue == ColorType::RGB8 ? 3 : 4;
		uint32 RowBytes = WidthValue * SkipComponent;

		for(uint32 y = 0, yindex = 0; y < HeightValue; y++, yindex += RowBytes)
		{
			for(uint32 x = 0, xindex = 0; x < WidthValue; x++, xindex += SkipComponent)
			{
				uint8 *MyPixels = &Data[xindex + yindex], *TheirPixels = &Other->Data[xindex + yindex];

				MyPixels[0] = (MyPixels[0] * TheirPixels[0]) >> 8;
				MyPixels[1] = (MyPixels[1] * TheirPixels[1]) >> 8;
				MyPixels[2] = (MyPixels[2] * TheirPixels[2]) >> 8;
			};
		};

		return true;
	};

	bool TextureBuffer::FromData(const uint8 *Pixels, uint32 Width, uint32 Height)
	{
		FLASSERT(Pixels, "Invalid Pixels!");
		FLASSERT(Width > 0 && Height > 0, "Invalid Width or Height!");

		if(!Pixels || Width == 0 || Height == 0)
		{
			Log::Instance.LogErr(TAGBUFFER, "@FromData: Invalid Pixels: 0x%08x; Width: %d; Height: %d;", Pixels, Width, Height);
			
			return false;
		};

		WidthValue = Width;
		HeightValue = Height;

		Data.resize(Width * Height * 4);
		memcpy(&Data[0], Pixels, Width * Height * 4);

		return true;
	};

	void TextureBuffer::FlipX()
	{
		uint32 RowSize = WidthValue * 4;

		for(uint32 y = 0, StartIndex = 0; y < HeightValue; y++, StartIndex += RowSize)
		{
			for(uint32 x = 0, CurrentX = 0, Start = 0, End = 0; x < WidthValue / 2; x++, CurrentX += 4)
			{
				Start = StartIndex + CurrentX;
				End = StartIndex + RowSize - CurrentX;

				std::swap(Data[Start], Data[End - 4]);
				std::swap(Data[Start + 1], Data[End - 3]);
				std::swap(Data[Start + 2], Data[End - 2]);
				std::swap(Data[Start + 3], Data[End - 1]);
			};
		};
	};

	void TextureBuffer::FlipY()
	{
		uint32 RowSize = WidthValue * 4;
		std::vector<uint8> TempRow(RowSize);

		for(uint32 y = 0, StartIndex = 0, EndIndex = (HeightValue - 1) * RowSize; y < HeightValue / 2; y++, StartIndex += RowSize,
			EndIndex -= RowSize)
		{
			memcpy(&TempRow[0], &Data[EndIndex], RowSize);
			memcpy(&Data[EndIndex], &Data[StartIndex], RowSize);
			memcpy(&Data[StartIndex], &TempRow[0], RowSize);
		};
	};

	uint32 TextureBuffer::Width() const
	{
		return WidthValue;
	};

	uint32 TextureBuffer::Height() const
	{
		return HeightValue;
	};

	uint32 TextureBuffer::ColorType() const
	{
		return ColorTypeValue;
	};

	bool TextureBuffer::Save(Stream *Out, const TextureEncoderInfo &Info)
	{
		FLASSERT(WidthValue > 0 && HeightValue > 0, "Invalid Image Height");
		FLASSERT(Data.size() > 0, "Invalid Image Data");

		switch(Info.Encoder)
		{
		case TextureEncoderType::PNG:
			return WritePNG(Out, WidthValue, HeightValue, Data);

			break;
		case TextureEncoderType::WebP:
			return WriteWebP(Out, WidthValue, HeightValue, Data, Info.Lossless, Info.Quality, Info.TargetWidth, Info.TargetHeight);

			break;
		case TextureEncoderType::FTI:
			return WriteFTI(Out, WidthValue, HeightValue, ColorTypeValue, Data);

			break;
		};

		return false;
	};

	Texture::Texture() : GLID(0), WidthValue(0), HeightValue(0), ColorTypeValue(ColorType::RGBA8),
		TextureFilter(TextureFiltering::Nearest), TextureWrap(TextureWrapMode::Clamp)
	{
	};

	Texture::~Texture()
	{
		Destroy();
		Buffer.Dispose();
	};

	const TexturePackerIndex &Texture::GetIndex() const
	{
		return Index;
	};

	bool Texture::operator==(const Texture &o) const
	{
		return &o == this;
	};

	SuperSmartPointer<TextureBuffer> Texture::GetData() const
	{
		return Buffer;
	};

	Vector2 Texture::Size() const
	{
		return Vector2((f32)Width(), (f32)Height());
	};

	void Texture::SetIndex(TexturePackerIndex Index)
	{
		this->Index = Index;
	};

	void Texture::Destroy()
	{
		Index.Index = -1;

#if USE_GRAPHICS
		if(GLID)
		{
			glDeleteTextures(1, (GLuint *)&GLID);
			GLID = 0;
		};
#endif
	};

	bool Texture::FromBuffer(SuperSmartPointer<TextureBuffer> Buffer)
	{
		return FromData(&Buffer->Data[0], Buffer->Width(), Buffer->Height());
	};

	bool Texture::FromStream(Stream *Stream)
	{
		Index.Index = -1;

		PROFILE("Texture::FromStream", StatTypes::Rendering);

		Destroy();

		Buffer.Reset(new TextureBuffer());

		if(!Buffer->FromStream(Stream))
		{
			Log::Instance.LogErr(TAGBUFFER, "@FromSteam: Failed to load a buffer from the stream");
			
			return false;
		};

		UpdateData(&Buffer->Data[0], Buffer->Width(), Buffer->Height());

		GLCHECK();

		SetTextureFiltering(TextureFilter);
		SetWrapMode(TextureWrap);

		GLCHECK();

		return true;
	};

	bool Texture::FromFile(const std::string &FileName)
	{
		Index.Index = -1;

		PROFILE("Texture::FromFile", StatTypes::Rendering);

		Destroy();

		FileStream In;

		if(!In.Open(FileName, StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG, "Unable to open '%s' for reading.", FileName.c_str());

			return false;
		};

		if(!FromStream(&In))
		{
			Log::Instance.LogInfo(TAG, "Unable to read '%s' as a PNG.", FileName.c_str());

			return false;
		};

		return true;
	};

	bool Texture::FromData(const uint8 *Pixels, uint32 Width, uint32 Height)
	{
		Index.Index = -1;

		Destroy();

#if USE_GRAPHICS
		if(glIsTexture(GLID))
		{
			glDeleteTextures(1, (const GLuint *)&GLID);
			GLCHECK();
		};

		glGenTextures(1, (GLuint *)&GLID);
		GLCHECK();

		glBindTexture(GL_TEXTURE_2D, GLID);

		GLCHECK();
#endif

		UpdateData(Pixels, Width, Height);

		GLCHECK();

		SetTextureFiltering(TextureFilter);
		SetWrapMode(TextureWrap);

		GLCHECK();

		return true;
	};

	bool Texture::CreateEmptyTexture(uint32 Width, uint32 Height, bool RGBA)
	{
		Index.Index = -1;

		Destroy();
		Buffer.Dispose();
		WidthValue = Width;
		HeightValue = Height;
		ColorTypeValue = RGBA ? ColorType::RGBA8 : ColorType::RGB8;

#if USE_GRAPHICS
		glGenTextures(1, (GLuint *)&GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);

		if(RendererManager::Instance.ActiveRenderer()->GetBoolFeature(RendererFeature::MayUseNonPowerOfTwoTextures))
		{
			glTexImage2D(GL_TEXTURE_2D, 0, ColorTypeValue == 1 ? GL_RGBA8 : GL_RGB8, Width, Height, 0,
				ColorTypeValue == 1 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);
		}
		else
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, ColorTypeValue == 1 ? GL_RGBA8 : GL_RGB8, Width, Height, 
				ColorTypeValue == 1 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);
		};

		GLCHECK();

		SetTextureFiltering(TextureFiltering::Linear_Mipmap);
		SetWrapMode(TextureWrap);

		GLCHECK();
#endif

		return true;
	};

	void Texture::UpdateData(const uint8 *Pixels, uint32 Width, uint32 Height)
	{
		Index.Index = -1;

		if(!Buffer.Get() || &Buffer->Data[0] != Pixels)
		{
			Buffer.Reset(new TextureBuffer());
			Buffer->FromData(Pixels, Width, Height);
		};

#if USE_GRAPHICS
		if(!glIsTexture(GLID))
		{
			glGenTextures(1, (GLuint *)&GLID);
			glBindTexture(GL_TEXTURE_2D, GLID);

			GLCHECK();
		};

		Bind();

		if(RendererManager::Instance.ActiveRenderer()->GetBoolFeature(RendererFeature::MayUseNonPowerOfTwoTextures))
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
		}
		else
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
		};

		GLCHECK();

		SetTextureFiltering(TextureFilter);
		SetWrapMode(TextureWrap);

		GLCHECK();

		if(!KeepData)
		{
			WidthValue = Buffer->Width();
			HeightValue = Buffer->Height();
			ColorTypeValue = Buffer->ColorType();

			Buffer.Dispose();
		};
#endif
	};

	int32 Texture::ID() const
	{
		if(Index.Index != -1)
			return Index.GLID();

		return GLID;
	};

	uint32 Texture::Width() const
	{
		return Index.Index != -1 ? Index.Width() : (Buffer.Get() ? Buffer->Width() : WidthValue);
	};

	uint32 Texture::Height() const
	{
		return Index.Index != -1 ? Index.Height() : (Buffer.Get() ? Buffer->Height() : HeightValue);
	};
	
	uint32 Texture::ColorType() const
	{
		return Index.Index != -1 ? Index.ColorType() : (Buffer.Get() ? Buffer->ColorType() : ColorTypeValue);
	};
	
	uint32 Texture::FilterMode() const
	{
		return Index.Index != -1 ? Index.Filtering : TextureFilter;
	};

	uint32 Texture::WrapMode() const
	{
		return Index.Index != -1 ? Index.WrapMode : TextureWrap;
	};

	void Texture::Bind()
	{
#if USE_GRAPHICS
		if(!glIsTexture(ID()))
			return;

		glBindTexture(GL_TEXTURE_2D, ID());

		GLCHECK();
#endif
	};

	void Texture::SetWrapMode(uint32 WrapMode)
	{
		if(Index.Index != -1)
		{
			Index.WrapMode = WrapMode;

			return;
		};

#if USE_GRAPHICS
		TextureWrap = WrapMode;

		Bind();

		switch(WrapMode)
		{
		case TextureWrapMode::Repeat:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			break;

		case TextureWrapMode::Clamp:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			break;

		case TextureWrapMode::ClampToBorder:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			break;

		case TextureWrapMode::ClampToEdge:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			break;
		};
#endif
	};

	void Texture::PushTextureStates()
	{
		TextureState Frame;
		Frame.FilterMode = FilterMode();
		Frame.WrapMode = WrapMode();

		StateStack.push_back(Frame);
	};

	void Texture::PopTextureStates()
	{
		if(StateStack.size())
		{
			std::vector<TextureState>::reverse_iterator it = StateStack.rbegin();

			if(it != StateStack.rend())
			{
				Bind();

				SetTextureFiltering(it->FilterMode);
				SetWrapMode(it->WrapMode);

				StateStack.erase(it.base());
			};
		};
	};

	void Texture::SetTextureFiltering(int32 Filter)
	{
		if(Index.Index != -1)
		{
			Index.Filtering = Filter;

			return;
		};

#if USE_GRAPHICS
		TextureFilter = Filter;

		glBindTexture(GL_TEXTURE_2D, GLID);

		if(Filter == TextureFiltering::Nearest || Filter == TextureFiltering::Linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest ?
GL_NEAREST : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest ?
GL_NEAREST : GL_LINEAR);
		}
		else if(Filter == TextureFiltering::Nearest_Mipmap || Filter == TextureFiltering::Linear_Mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == TextureFiltering::Nearest_Mipmap ?
GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == TextureFiltering::Nearest_Mipmap ?
GL_NEAREST : GL_LINEAR);

			GLCHECK();

			if(RendererManager::Instance.ActiveRenderer()->GetBoolFeature(RendererFeature::CanAutoGenerateMipMaps))
			{
				glGenerateMipmap(GL_TEXTURE_2D);

				GLCHECK();
			}
			else
			{
				if(RendererManager::Instance.ActiveRenderer()->GlewIsAvailable && glewIsSupported("GL_VERSION_1_4"))
					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 

				GLCHECK();
			};

			if(RendererManager::Instance.ActiveRenderer()->GetBoolFeature(RendererFeature::AnisotropicEnabled))
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
					RendererManager::Instance.ActiveRenderer()->GetFloatFeature(RendererFeature::CurrentAnisotropicLevel));
			};

			GLCHECK();
		};

		//Maybe temp, not sure
		if(RendererManager::Instance.ActiveRenderer()->GlewIsAvailable && glewIsSupported("GL_VERSION_1_2"))
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		};

		GLCHECK();
#endif
	};

	bool Texture::FromPackage(const std::string &Directory, const std::string &Name)
	{
		Index.Index = -1;

		std::string ActualDirectory = Directory;

		if(ActualDirectory.length() == 0)
			ActualDirectory = "/";

		if(ActualDirectory[0] != '/')
			ActualDirectory = "/" + ActualDirectory;

		if(ActualDirectory[ActualDirectory.length() - 1] != '/')
			ActualDirectory += "/";

		SuperSmartPointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(MakeStringID(ActualDirectory),
			MakeStringID(Name));

		if(PackageStream.Get() == NULL)
		{
			Log::Instance.LogErr(TAG, "Package Stream for '%s%s' was not found", ActualDirectory.c_str(),
				Name.c_str());

			return false;
		};

		return FromStream(PackageStream.Get());
	};

#if USE_GRAPHICS
	bool Texture::FromScreen()
	{
		Index.Index = -1;

		RendererManager::Renderer *Renderer = RendererManager::Instance.ActiveRenderer();

		if(Renderer == NULL)
		{
			Log::Instance.LogDebug(TAG, "@FromScreen: Invalid Renderer");

			return false;
		};

		sf::Image Picture = Renderer->Window.capture();

		return FromData(Picture.getPixelsPtr(), Picture.getSize().x, Picture.getSize().y);
	};
#endif

	Vector4 Texture::GetPixel(uint32 x, uint32 y)
	{
		if(Buffer.Get() == NULL || x >= Buffer->Width() || y >= Buffer->Height())
			return Vector4();

		uint8 *Piece = &Buffer->Data[x * 4 + y * Buffer->Width() * 4];

		return Vector4(Piece[0] / 255.f, Piece[1] / 255.f, Piece[2] / 255.f, Piece[3] / 255.f);
	};

	void Texture::Blur(uint32 Radius, uint32 Strength)
	{
		if(Index.Index != -1)
			return;

		PROFILE("Texture::Blur", StatTypes::Rendering);

#if USE_GRAPHICS
		bool RequiredBufferPlacement = false;

		if(Buffer.Get() == NULL && WidthValue != 0)
		{
			RequiredBufferPlacement = true;

			GLCHECK();

			Bind();

			GLCHECK();

			Buffer.Reset(new TextureBuffer());

			static std::vector<uint8> Data;
			Data.resize(WidthValue * HeightValue * 4);

			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA8, GL_UNSIGNED_BYTE, &Data[0]);

			GLCHECK();

			glBindTexture(GL_TEXTURE_2D, 0);

			Buffer->FromData(&Data[0], WidthValue, HeightValue);
		};
#endif

		if(!Buffer.Get())
		{
			Log::Instance.LogErr(TAG, "@Blur: Failed to blur due to invalid Buffer");

			return;
		};

		static std::vector<uint8> PixelBuffer;
		
		if(PixelBuffer.size() != Width() * Height() * 4)
		{
			PixelBuffer.resize(Width() * Height() * 4);
		};

		int32 ActualWidth = Width(), ActualHeight = Height();
		int32 ActualWidthBytes = ActualWidth * 4, ActualHeightBytes = ActualHeight * 4;
		int32 RadiusX = Radius * 4;
		int32 RadiusY = Radius * ActualWidthBytes;

		int32 Directions[] = {
			//-1, -1
			-RadiusX - RadiusY,
			//0, -1
			-RadiusY,
			//1, -1
			RadiusX - RadiusY,
			//-1, 0
			-RadiusX,
			//0, 0
			0,
			//1, 0
			RadiusX,
			//-1, 1
			-RadiusX + RadiusY,
			//0, 1
			RadiusY,
			//1, 1
			RadiusX + RadiusY
		};

		uint32 DirectionCount = sizeof(Directions) / sizeof(Directions[0]);
		uint32 Additions = 0;
		uint8 *BufferPtr = &Buffer->Data[0];

		for(uint32 i = 0; i < Strength; i++)
		{
			for(int32 y = 0, PixelBufferIndex = 0, ypos = 0; y < ActualHeight; y++, ypos += ActualWidthBytes)
			{
				for(int32 x = 0, xpos = 0; x < ActualWidth; x++, PixelBufferIndex+=4, xpos += 4)
				{
					uint32 Pixel[3] = {
						0, 0, 0
					};

					Additions = 0;

					for(uint32 DIndex = 0; DIndex < DirectionCount; DIndex++)
					{
						if(Directions[DIndex] + xpos + ypos < 0 || Directions[DIndex] + xpos + ypos >= (int32)PixelBuffer.size())
							continue;

						uint32 Index = Directions[DIndex] + xpos + ypos;

						Pixel[0] += BufferPtr[Index];
						Pixel[1] += BufferPtr[Index + 1];
						Pixel[2] += BufferPtr[Index + 2];

						Additions++;
					};

					if(Additions != 0 && (Pixel[0] != 0 || Pixel[1] != 0 || Pixel[2] != 0))
					{
						PixelBuffer[PixelBufferIndex] = (uint8)(Pixel[0] / Additions);
						PixelBuffer[PixelBufferIndex + 1] = (uint8)(Pixel[1] / Additions);
						PixelBuffer[PixelBufferIndex + 2] = (uint8)(Pixel[2] / Additions);
						PixelBuffer[PixelBufferIndex + 3] = BufferPtr[PixelBufferIndex + 3];
					}
					else
					{
						PixelBuffer[PixelBufferIndex] = BufferPtr[PixelBufferIndex];
						PixelBuffer[PixelBufferIndex + 1] = BufferPtr[PixelBufferIndex + 1];
						PixelBuffer[PixelBufferIndex + 2] = BufferPtr[PixelBufferIndex + 2];
						PixelBuffer[PixelBufferIndex + 3] = BufferPtr[PixelBufferIndex + 3];
					};
				};
			};

			memcpy(&BufferPtr[0], &PixelBuffer[0], ActualWidth * ActualHeight * sizeof(uint8[4]));

			PROFILE_PROGRESS((uint32)(100 * (i / (f32)Strength)));
		};

		PROFILE_PROGRESS(101);

#if USE_GRAPHICS
		UpdateData(&Buffer->Data[0], Width(), Height());

		if(RequiredBufferPlacement)
			Buffer.Dispose();
#endif
	};

	uint32 TexturePackerIndex::Width() const
	{
		return Owner.Get() && (int32)Owner->Indices.size() > Index ? Owner->Indices[Index].Width : 0;
	};

	uint32 TexturePackerIndex::Height() const
	{
		return Owner.Get() && (int32)Owner->Indices.size() > Index ? Owner->Indices[Index].Height : 0;
	};

	uint32 TexturePackerIndex::GLID() const
	{
		return Owner.Get() ? Owner->MainTexture->ID() : 0;
	};

	uint32 TexturePackerIndex::ColorType() const
	{
		return Owner.Get() ? Owner->MainTexture->ColorType() : 0;
	};

	bool SortSprites(const TexturePacker::SortedTexture &a, const TexturePacker::SortedTexture &b)
	{
		int32 aSize = a.Height * 1024 + a.Width;
		int32 bSize = b.Height * 1024 + b.Width;
 
		return bSize > aSize;
	};

	bool SortSpriteIndices(const TexturePacker::SortedTexture &a, const TexturePacker::SortedTexture &b)
	{
		return a.Index < b.Index;
	};

	int32 GuessSpriteWidth(const std::vector<TexturePacker::SortedTexture> &Sprites)
	{
		std::vector<int32> Widths;

		for(uint32 i = 0; i < Sprites.size(); i++)
		{
			Widths.push_back(Sprites[i].Width);
		};

		std::sort(Widths.begin(), Widths.end());

		int32 MaxWidth = Widths[Widths.size() - 1];
		int32 MedianWidth = Widths[Widths.size() / 2];

		int32 Width = MedianWidth * (int32)(MathUtils::Round(sqrtf((f32)Sprites.size())));

		return (int32)MathUtils::Max((f32)Width, (f32)MaxWidth);
	};

	int32 FindIntersectingSprite(const std::vector<TexturePacker::SortedTexture> &Sprites, int32 Index, int32 x, int32 y)
	{
		int32 Width = Sprites[Index].Width;
		int32 Height = Sprites[Index].Height;

		for(int32 i = 0; i < Index; i++)
		{
			if(Sprites[i].x >= (uint32)(x + Width) || Sprites[i].x + (uint32)Sprites[i].Width <= (uint32)x || Sprites[i].y >= (uint32)(y + Height) ||
				Sprites[i].y + Sprites[i].Height <= (uint32)y)
				continue;

			return i;
		};

		return -1;
	};

	void PositionSprite(const std::vector<TexturePacker::SortedTexture> &Sprites, TexturePacker::SortedTexture &Target, int32 TargetIndex, int32 TextureWidth)
	{
		int32 x = 0, y = 0;

		for(;;)
		{
			int32 IntersectionIndex = FindIntersectingSprite(Sprites, TargetIndex, x, y);

			if(IntersectionIndex < 0)
			{
				Target.x = x;
				Target.y = y;

				return;
			};

			x = Sprites[IntersectionIndex].x + Sprites[IntersectionIndex].Width;

			if(x + Target.Width > (uint32)TextureWidth)
			{
				x = 0;
				y++;
			};
		};
	};

	SuperSmartPointer<TexturePacker> TexturePacker::FromTextures(const std::vector<SuperSmartPointer<Texture> > &Textures, uint32 MaxWidth, uint32 MaxHeight)
	{
		SuperSmartPointer<TexturePacker> Out(new TexturePacker());

		for(uint32 i = 0; i < Textures.size(); i++)
		{
			SortedTexture Texture;
			Texture.Index = i;
			Texture.Width = Textures[i]->Width() + 2;
			Texture.Height = Textures[i]->Height() + 2;

			Out->Indices.push_back(Texture);
		};

		std::sort(Out->Indices.begin(), Out->Indices.end(), SortSprites);

		int32 Width = GuessSpriteWidth(Out->Indices), Height = 0, Size = 0;

		if(Width > (int32)MaxWidth)
			Width = (int32)MaxWidth;

		for(uint32 i = 0; i < Textures.size(); i++)
		{
			PositionSprite(Out->Indices, Out->Indices[i], i, Width);
			
			int32 OldHeight = Height;

			Height = (int32)MathUtils::Max((f32)Height, (f32)(Out->Indices[i].y + Out->Indices[i].Height));

			if(Height > (int32)MaxHeight)
			{
				Height = OldHeight;

				Out->Indices.resize(i);

				break;
			};

			Size += Out->Indices[i].Width * Out->Indices[i].Height;
		};

		std::sort(Out->Indices.begin(), Out->Indices.end(), SortSpriteIndices);

		Out->MainTexture.Reset(new Texture());
		TextureBuffer Temp;
		Temp.CreateEmpty(Width, Height);

		memset(&Temp.Data[0], 255, Temp.Data.size());

		uint32 RowSize = Width * 4;

		for(uint32 i = 0; i < Out->Indices.size(); i++)
		{
			uint32 MyRowSize = (Out->Indices[i].Width - 2) * 4;
			uint32 xpos = (Out->Indices[i].x + 1) * 4;

			for(uint32 y = 0, ypos = (Out->Indices[i].y + 1) * RowSize, ypostarget = 0; y < Out->Indices[i].Height - 2; y++, ypos += RowSize, ypostarget += MyRowSize)
			{
				memcpy(&Temp.Data[xpos + ypos], &Textures[Out->Indices[i].Index]->GetData()->Data[ypostarget], MyRowSize);
			};

			Out->Indices[i].TextureInstance.Reset(new Texture());

			TexturePackerIndex Index;
			Index.Index = Out->Indices[i].Index;
			Index.Owner = Out;

			Out->Indices[i].TextureInstance->SetIndex(Index);
		};

		if(!Out->MainTexture->FromData(&Temp.Data[0], Width, Height))
			return SuperSmartPointer<TexturePacker>();

		return Out;
	};

	SuperSmartPointer<Texture> TexturePacker::GetTexture(uint32 Index)
	{
		return Index < Indices.size() ? Indices[Index].TextureInstance : SuperSmartPointer<Texture>();
	};
};

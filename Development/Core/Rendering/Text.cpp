#include "FlamingCore.hpp"
#if USE_GRAPHICS
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H
namespace FlamingTorch
{
	struct Span
	{
		Span() { }
		Span(int32 _x, int32 _y, int32 _width, int32 _coverage)
			: x(_x), y(_y), width(_width), coverage(_coverage) { }

		int32 x, y, width, coverage;
	};

	typedef std::vector<Span> Spans;

	void RasterCallback(const int32 y, const int32 count, const FT_Span * const spans, void * const user)
	{
		Spans *sptr = (Spans *)user;

		for (int i = 0; i < count; i++)
		{
			sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
		}
	}

	StringID MakeGlyphString(uint32 character, const Vector4 &textColor, const Vector4 &textColor2, f32 borderSize, const Vector4 &borderColor, uint32 textSize)
	{
		MemoryStream stream;

		stream.Write2(&character, 1);
		stream.Write2(&textColor, 1);
		stream.Write2(&textColor2, 1);
		stream.Write2(&borderSize, 1);
		stream.Write2(&borderColor, 1);
		stream.Write2(&textSize, 1);

		return CRC32::Instance.CRC((const uint8 *)stream.Get(), (uint32)stream.Length());
	}

	TextFont::TextFont() : currentSize(0), library(NULL), face(NULL)
	{
	}

	TextFont::~TextFont()
	{
		Clear();
	}

	bool TextFont::FromStream(Stream *stream)
	{
		Clear();

		containedData.resize((uint32)stream->Length());

		if (!stream->Seek(0) || !stream->Read2(&containedData[0], (uint32)stream->Length()))
		{
			return false;
		}

		if (FT_Init_FreeType(&library) != 0)
		{
			return false;
		}

		if (FT_New_Memory_Face(library, &containedData[0], containedData.size(), 0, &face) != 0)
		{
			return false;
		}

		if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
		{
			FT_Done_Face(face);

			return false;
		}

		return true;
	}

	uint32 TextFont::LineSpacing(const TextParams &params)
	{
		if (!face)
			return 0;

		SetSize(params.fontSizeValue);

		return face->size->metrics.height >> 6;
	}

	int32 TextFont::Kerning(uint32 from, uint32 to, const TextParams &params)
	{
		if (face == NULL || !FT_HAS_KERNING(face))
			return 0;

		SetSize(params.fontSizeValue);

		FT_Vector kerning;
		FT_UInt fromindex = FT_Get_Char_Index(face, from);
		FT_UInt toindex = FT_Get_Char_Index(face, to);

		if(FT_Get_Kerning(face, fromindex, toindex, FT_KERNING_DEFAULT, &kerning) != FT_Err_Ok)
			return 0;

		return kerning.x >> 6;
	}

	Glyph TextFont::LoadGlyph(uint32 character, const TextParams &params)
	{
		StringID ID = MakeGlyphString(character, params.textColorValue, params.secondaryTextColorValue, params.borderSizeValue,
			params.borderColorValue, params.fontSizeValue);

		GlyphMap::iterator it = glyphs.find(ID);

		if (it != glyphs.end())
			return it->second;

		Glyph out;
		FT_Glyph glyphDesc;

		SetSize(params.fontSizeValue);

		if (FT_Load_Char(face, character, FT_LOAD_TARGET_LIGHT | FT_LOAD_FORCE_AUTOHINT) != FT_Err_Ok)
			return Glyph();

		if (FT_Get_Glyph(face->glyph, &glyphDesc))
			return Glyph();

		Spans spans, outlineSpans;

		if (params.borderSizeValue > 0 && glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE)
		{
			FT_Raster_Params rasterParams;

			memset(&rasterParams, 0, sizeof(rasterParams));
			rasterParams.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
			rasterParams.gray_spans = RasterCallback;
			rasterParams.user = &spans;

			FT_Outline_Render(library, &face->glyph->outline, &rasterParams);

			FT_Stroker stroker;
			FT_Stroker_New(library, &stroker);
			FT_Stroker_Set(stroker, (int32)(params.borderSizeValue * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

			FT_Glyph_StrokeBorder(&glyphDesc, stroker, 0, 1);

			FT_Outline *o = &reinterpret_cast<FT_OutlineGlyph>(glyphDesc)->outline;

			rasterParams.user = &outlineSpans;

			FT_Outline_Render(library, o, &rasterParams);

			FT_Stroker_Done(stroker);
		}

		FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyphDesc;
		FT_Bitmap& bitmap = bitmapGlyph->bitmap;

		out.advance = glyphDesc->advance.x >> 16;

		uint32 width = (uint32)bitmap.width;
		uint32 height = (uint32)bitmap.rows;

		if (width > 0 && height > 0)
		{
			out.bounds.Left = (f32)bitmapGlyph->left;
			out.bounds.Top = (f32)bitmapGlyph->top;
			out.bounds.Right = (f32)width + out.bounds.Left;
			out.bounds.Bottom = (f32)height + out.bounds.Top;

			std::vector<uint8> pixelBuffer;

			pixelBuffer.resize(width * height * 4, 0);

			const uint8* pixels = bitmap.buffer;

			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				for (uint32 y = 0; y < height; ++y)
				{
					for (uint32 x = 0; x < width; ++x)
					{
						uint32 index = (x + y * width) * 4 + 3;

						pixelBuffer[index] = ((pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;
					}

					pixels += bitmap.pitch;
				}
			}
			else
			{
				for (uint32 y = 0; y < height; ++y)
				{
					for (uint32 x = 0; x < width; ++x)
					{
						uint32 index = (x + y * width) * 4 + 3;

						pixelBuffer[index] = pixels[x];
					}

					pixels += bitmap.pitch;
				}
			}

			Vector4 byteColor = params.textColorValue * 255;
			Vector4 secondaryByteColor = params.secondaryTextColorValue * 255;

			Vector3 diff(MathUtils::Clamp(params.secondaryTextColorValue.x - params.textColorValue.x) * 255,
				MathUtils::Clamp(params.secondaryTextColorValue.y - params.textColorValue.y) * 255, 
				MathUtils::Clamp(params.secondaryTextColorValue.z - params.textColorValue.z) * 255);

			for (uint32 y = 0; y < height; ++y)
			{
				for (uint32 x = 0; x < width; ++x)
				{
					f32 percent = y / (f32)(height - 1);

					uint32 index = (x + y * width) * 4;

					if (params.textColorValue == params.secondaryTextColorValue)
					{
						pixelBuffer[index] = (uint8)byteColor.x;
						pixelBuffer[index + 1] = (uint8)byteColor.y;
						pixelBuffer[index + 2] = (uint8)byteColor.z;
					}
					else
					{
						pixelBuffer[index] = (uint8)(byteColor.x + diff.x * percent);
						pixelBuffer[index + 1] = (uint8)(byteColor.y + diff.y * percent);
						pixelBuffer[index + 2] = (uint8)(byteColor.z + diff.z * percent);
					}
				}
			}

			Vector2 min, max;

			for (uint32 i = 0; i < spans.size(); i++)
			{
				if (i == 0)
				{
					min.x = (f32)spans[i].x;
					min.y = (f32)spans[i].y;
					max.x = (f32)spans[i].x;
					max.y = (f32)spans[i].y;
				}

				if (spans[i].x < min.x)
					min.x = (f32)spans[i].x;

				if (spans[i].x > max.x)
					max.x = (f32)spans[i].x;

				if (spans[i].y < min.y)
					min.y = (f32)spans[i].y;

				if (spans[i].y > max.y)
					max.y = (f32)spans[i].y;

				if (spans[i].x + spans[i].width - 1 < min.x)
					min.x = (f32)(spans[i].x + spans[i].width - 1);

				if (spans[i].x + spans[i].width - 1 > max.x)
					max.x = (f32)(spans[i].x + spans[i].width - 1);
			}

			for (size_t i = 0; i < outlineSpans.size(); i++)
			{
				if (outlineSpans[i].x < min.x)
					min.x = (f32)outlineSpans[i].x;

				if (outlineSpans[i].x > max.x)
					max.x = (f32)outlineSpans[i].x;

				if (outlineSpans[i].y < min.y)
					min.y = (f32)outlineSpans[i].y;

				if (outlineSpans[i].y > max.y)
					max.y = (f32)outlineSpans[i].y;

				if (outlineSpans[i].x + outlineSpans[i].width - 1 < min.x)
					min.x = (f32)(outlineSpans[i].x + outlineSpans[i].width - 1);

				if (outlineSpans[i].x + outlineSpans[i].width - 1 > max.x)
					max.x = (f32)(outlineSpans[i].x + outlineSpans[i].width - 1);
			}

			Vector3 byteBorderColor = params.borderColorValue.ToVector3() * 255;

			for (uint32 i = 0; i < outlineSpans.size(); i++)
			{
				for (int32 w = 0; w < outlineSpans[i].width; w++)
				{
					uint32 index = (uint32)((height - 1 - (outlineSpans[i].y - min.y)) * width + (outlineSpans[i].x - min.x) + w) * 4;

					pixelBuffer[index] = (uint8)byteBorderColor.x;
					pixelBuffer[index + 1] = (uint8)byteBorderColor.y;
					pixelBuffer[index + 2] = (uint8)byteBorderColor.z;
					pixelBuffer[index + 3] = (uint8)std::min(255, outlineSpans[i].coverage);
				}
			}

			for (uint32 i = 0; i < spans.size(); i++)
			{
				for (uint32 w = 0; w < (uint32)spans[i].width; w++)
				{
					uint32 y = (uint32)(height - 1 - (spans[i].y - min.y));
					f32 percent = y / (f32)(height - 1);

					uint32 index = (uint32)((height - 1 - (spans[i].y - min.y)) * width + (spans[i].x - min.x) + w) * 4;

					pixelBuffer[index] = (uint8)(pixelBuffer[index] + (((byteColor.x + (int32)(diff.x * percent)) - pixelBuffer[index]) * spans[i].coverage / 255.f));
					pixelBuffer[index + 1] = (uint8)(pixelBuffer[index + 1] + (((byteColor.y + (int32)(diff.y * percent)) - pixelBuffer[index + 1]) * spans[i].coverage / 255.f));
					pixelBuffer[index + 2] = (uint8)(pixelBuffer[index + 2] + (((byteColor.z + (int32)(diff.z * percent)) - pixelBuffer[index + 2]) * spans[i].coverage / 255.f));
					pixelBuffer[index + 3] = (uint8)std::min(255, (int32)(pixelBuffer[index + 3]) + spans[i].coverage);
				}
			}

			out.pixels = TextureBuffer::CreateFromData(&pixelBuffer[0], width, height);
		}

		FT_Done_Glyph(glyphDesc);

		glyphs[ID] = out;

		return out;
	}

	void TextFont::Clear()
	{
		if (face)
		{
			FT_Done_Face(face);

			face = NULL;
		}

		if (library)
		{
			FT_Done_FreeType(library);

			library = NULL;
		}

		containedData.resize(0);
	}

	void TextFont::SetSize(uint32 size)
	{
		if (!library || !face)
			return;

		FT_Error result = FT_Set_Pixel_Sizes(face, 0, size);

		if (result == FT_Err_Ok)
			currentSize = size;
	}

	Glyph::Glyph() : advance(0)
	{
	}

#define COPYOFFSET(var, type)\
	memcpy(&Buffer[Offset], &var, sizeof(type));\
	\
	Offset += sizeof(type);

	StringID MakeTextResourceString(uint32 character, const TextParams &parameters)
	{
		const uint32 size = sizeof(uint32) + sizeof(TextParams) - sizeof(f32) - sizeof(Vector2);
		static uint8 Buffer[size];

		uint32 Offset = 0;

		COPYOFFSET(character, uint32);
		COPYOFFSET(parameters.borderColorValue, Vector4);
		COPYOFFSET(parameters.borderSizeValue, f32);
		COPYOFFSET(parameters.fontSizeValue, uint32);
		COPYOFFSET(*parameters.fontValue.Get(), intptr_t);
		COPYOFFSET(parameters.secondaryTextColorValue, Vector4);
		COPYOFFSET(parameters.textColorValue, Vector4);

		return CRC32::Instance.CRC(Buffer, size);
	}

	void TextRenderer::ClearUnusedResources()
	{
		for (TextResourceMap::iterator it = textResources.begin(); it != textResources.end(); it++)
		{
			if (it->second.references == 0)
			{
				it->second.sourceTexture.Dispose();

				textResources.erase(it);

				it = textResources.begin();

				if (it == textResources.end())
					break;
			}
		}

		for (TextResourceMap::iterator it = textResources.begin(); it != textResources.end(); it++)
		{
			it->second.references = 0;
		}
	}

	void TextRenderer::GetText(const std::string &text, const TextParams &parameters)
	{
		for (uint32 i = 0; i < text.size(); i++)
		{
			if (text[i] == ' ' || text[i] == '\n' || text[i] == '\r')
				continue;

			StringID ID = MakeTextResourceString(text[i], parameters);
			TextResourceMap::iterator it = textResources.find(ID);

			if (it != textResources.end())
			{
				it->second.references++;
			}
			else
			{
				TextResourceInfo resource;

				resource.character = text[i];
				resource.textParameters = parameters;
				resource.references = 1;

				resource.info = const_cast<TextFont *>(parameters.fontValue.Get())->LoadGlyph(text[i], parameters);

				if (resource.info.pixels.Get())
				{
					resource.sourceTexture = Texture::CreateFromBuffer(resource.info.pixels);
					resource.instanceTexture = resourcesGroup->Get(resourcesGroup->Add(resource.sourceTexture));
				}

				textResources[ID] = resource;
			}
		}
	}

	void TextRenderer::DrawText(const std::string &text, const TextParams &params)
	{
		static Sprite sprite;

		TextParams actualParams = params.fontValue ? params : TextParams(params).Font(RenderTextUtils::DefaultFont);

		DisposablePointer<TextFont> font = actualParams.fontValue;

		if (font.Get() == nullptr)
			return;

		f32 lineSpace = (f32)font->LineSpacing(actualParams);
		f32 spaceSize = (f32)font->LoadGlyph(' ', actualParams).advance;

		Vector2 position = Vector2(params.positionValue.x, params.positionValue.y + params.fontSizeValue), initialPosition = position;

		GetText(text, actualParams);

		std::vector<std::string> lines(StringUtils::Split(StringUtils::Strip(text, '\r'), '\n'));

		for (uint32 i = 0; i < lines.size(); i++)
		{
			for (uint32 j = 0; j < lines[i].length(); j++)
			{
				switch (lines[i][j])
				{
				case ' ':
					position.x = position.x + spaceSize;

					break;

				default:
				{
					StringID ID = MakeTextResourceString(lines[i][j], actualParams);
					TextResourceMap::iterator it = textResources.find(ID);

					if (j > 0)
						position.x += font->Kerning(lines[i][j - 1], lines[i][j], actualParams);

					if (it != textResources.end())
					{
						sprite.Options.Position(position + Vector2(it->second.info.bounds.Left, -it->second.info.bounds.Top))
							.Color(Vector4(1, 1, 1, actualParams.textColorValue.w));
						sprite.SpriteTexture = it->second.instanceTexture;

						sprite.Draw();

#if DEBUG_DRAWTEXT
						if (sprite.SpriteTexture.Get())
						{
							sprite.Options.Scale(Vector2(sprite.SpriteTexture->Size())).Color(Vector4(1, 1, 0, 1)).Wireframe(true);
							sprite.SpriteTexture = DisposablePointer<Texture>();

							sprite.Draw(Owner);

							sprite.Options.Scale(Vector2(1, 1)).Color(Vector4(1, 1, 1, 1)).Wireframe(false);
						}
#endif

						position.x = position.x + it->second.info.advance;
					}
					else
					{
						position.x = position.x + spaceSize;
					}
				}

				break;
				}
			}

			position.x = initialPosition.x;
			position.y += lineSpace;
		}
	}
}
#endif

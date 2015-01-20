#include "FlamingCore.hpp"
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
			: x(_x), y(_y), Width(_width), Coverage(_coverage) { }

		int32 x, y, Width, Coverage;
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

	StringID MakeGlyphString(uint32 Character, const Vector4 &TextColor, const Vector4 &TextColor2, f32 BorderSize, const Vector4 &BorderColor, uint32 TextSize)
	{
		MemoryStream Stream;

		Stream.Write2(&Character, 1);
		Stream.Write2(&TextColor, 1);
		Stream.Write2(&TextColor2, 1);
		Stream.Write2(&BorderSize, 1);
		Stream.Write2(&BorderColor, 1);
		Stream.Write2(&TextSize, 1);

		return CRC32::Instance.CRC((const uint8 *)Stream.Get(), (uint32)Stream.Length());
	}

	Font::Font() : CurrentSize(0), Library(NULL), Face(NULL)
	{
	}

	Font::~Font()
	{
		Clear();
	}

	bool Font::FromStream(Stream *TheStream)
	{
		Clear();

		ContainedData.resize((uint32)TheStream->Length());

		if (!TheStream->Seek(0) || !TheStream->Read2(&ContainedData[0], (uint32)TheStream->Length()))
		{
			return false;
		}

		if (FT_Init_FreeType(&Library) != 0)
		{
			return false;
		}

		if (FT_New_Memory_Face(Library, &ContainedData[0], ContainedData.size(), 0, &Face) != 0)
		{
			return false;
		}

		if (FT_Select_Charmap(Face, FT_ENCODING_UNICODE) != 0)
		{
			FT_Done_Face(Face);

			return false;
		}

		return true;
	}

	uint32 Font::LineSpacing(const TextParams &Params)
	{
		if (!Face)
			return 0;

		SetSize(Params.FontSizeValue);

		return Face->size->metrics.height >> 6;
	}

	uint32 Font::Kerning(uint32 From, uint32 To, const TextParams &Params)
	{
		if (Face == NULL || !FT_HAS_KERNING(Face))
			return 0;

		SetSize(Params.FontSizeValue);

		FT_Vector Kerning;
		FT_UInt FromIndex = FT_Get_Char_Index(Face, From);
		FT_UInt ToIndex = FT_Get_Char_Index(Face, To);

		if(FT_Get_Kerning(Face, FromIndex, ToIndex, FT_KERNING_DEFAULT, &Kerning) != FT_Err_Ok)
			return 0;

		return Kerning.x >> 6;
	}

	Glyph Font::LoadGlyph(uint32 Character, const TextParams &Params)
	{
		StringID ID = MakeGlyphString(Character, Params.TextColorValue, Params.SecondaryTextColorValue, Params.BorderSizeValue, Params.BorderColorValue, Params.FontSizeValue);

		GlyphMap::iterator it = Glyphs.find(ID);

		if (it != Glyphs.end())
			return it->second;

		Glyph Out;
		FT_Glyph GlyphDesc;

		SetSize(Params.FontSizeValue);

		if (FT_Load_Char(Face, Character, FT_LOAD_TARGET_LIGHT | FT_LOAD_FORCE_AUTOHINT) != FT_Err_Ok)
			return Glyph();

		if (FT_Get_Glyph(Face->glyph, &GlyphDesc))
			return Glyph();

		Spans spans, OutlineSpans;

		if (Params.BorderSizeValue > 0 && GlyphDesc->format == FT_GLYPH_FORMAT_OUTLINE)
		{
			FT_Raster_Params RasterParams;

			memset(&RasterParams, 0, sizeof(RasterParams));
			RasterParams.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
			RasterParams.gray_spans = RasterCallback;
			RasterParams.user = &spans;

			FT_Outline_Render(Library, &Face->glyph->outline, &RasterParams);

			FT_Stroker Stroker;
			FT_Stroker_New(Library, &Stroker);
			FT_Stroker_Set(Stroker, (int32)(Params.BorderSizeValue * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

			FT_Glyph_StrokeBorder(&GlyphDesc, Stroker, 0, 1);

			FT_Outline *o = &reinterpret_cast<FT_OutlineGlyph>(GlyphDesc)->outline;

			RasterParams.user = &OutlineSpans;

			FT_Outline_Render(Library, o, &RasterParams);

			FT_Stroker_Done(Stroker);
		}

		FT_Glyph_To_Bitmap(&GlyphDesc, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)GlyphDesc;
		FT_Bitmap& bitmap = bitmapGlyph->bitmap;

		Out.Advance = GlyphDesc->advance.x >> 16;

		uint32 Width = (uint32)bitmap.width;
		uint32 Height = (uint32)bitmap.rows;

		if (Width > 0 && Height > 0)
		{
			Out.Bounds.Left = (f32)bitmapGlyph->left;
			Out.Bounds.Top = (f32)bitmapGlyph->top;
			Out.Bounds.Right = (f32)Width + Out.Bounds.Left;
			Out.Bounds.Bottom = (f32)Height + Out.Bounds.Top;

			std::vector<uint8> PixelBuffer;

			PixelBuffer.resize(Width * Height * 4, 0);

			const uint8* Pixels = bitmap.buffer;

			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				for (uint32 y = 0; y < Height; ++y)
				{
					for (uint32 x = 0; x < Width; ++x)
					{
						uint32 Index = (x + y * Width) * 4 + 3;

						PixelBuffer[Index] = ((Pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;
					}

					Pixels += bitmap.pitch;
				}
			}
			else
			{
				for (uint32 y = 0; y < Height; ++y)
				{
					for (uint32 x = 0; x < Width; ++x)
					{
						uint32 Index = (x + y * Width) * 4 + 3;

						PixelBuffer[Index] = Pixels[x];
					}

					Pixels += bitmap.pitch;
				}
			}

			Vector4 ByteColor = Params.TextColorValue * 255;
			Vector4 SecondaryByteColor = Params.SecondaryTextColorValue * 255;

			Vector3 Diff(MathUtils::Clamp(Params.SecondaryTextColorValue.x - Params.TextColorValue.x) * 255,
				MathUtils::Clamp(Params.SecondaryTextColorValue.y - Params.TextColorValue.y) * 255, 
				MathUtils::Clamp(Params.SecondaryTextColorValue.z - Params.TextColorValue.z) * 255);

			for (uint32 y = 0; y < Height; ++y)
			{
				for (uint32 x = 0; x < Width; ++x)
				{
					f32 Percent = y / (f32)(Height - 1);

					uint32 Index = (x + y * Width) * 4;

					if (Params.TextColorValue == Params.SecondaryTextColorValue)
					{
						PixelBuffer[Index] = (uint8)ByteColor.x;
						PixelBuffer[Index + 1] = (uint8)ByteColor.y;
						PixelBuffer[Index + 2] = (uint8)ByteColor.z;
					}
					else
					{
						PixelBuffer[Index] = (uint8)(ByteColor.x + Diff.x * Percent);
						PixelBuffer[Index + 1] = (uint8)(ByteColor.y + Diff.y * Percent);
						PixelBuffer[Index + 2] = (uint8)(ByteColor.z + Diff.z * Percent);
					}
				}
			}

			Vector2 Min, Max;

			for (uint32 i = 0; i < spans.size(); i++)
			{
				if (i == 0)
				{
					Min.x = (f32)spans[i].x;
					Min.y = (f32)spans[i].y;
					Max.x = (f32)spans[i].x;
					Max.y = (f32)spans[i].y;
				}

				if (spans[i].x < Min.x)
					Min.x = (f32)spans[i].x;

				if (spans[i].x > Max.x)
					Max.x = (f32)spans[i].x;

				if (spans[i].y < Min.y)
					Min.y = (f32)spans[i].y;

				if (spans[i].y > Max.y)
					Max.y = (f32)spans[i].y;

				if (spans[i].x + spans[i].Width - 1 < Min.x)
					Min.x = (f32)(spans[i].x + spans[i].Width - 1);

				if (spans[i].x + spans[i].Width - 1 > Max.x)
					Max.x = (f32)(spans[i].x + spans[i].Width - 1);
			}

			for (size_t i = 0; i < OutlineSpans.size(); i++)
			{
				if (OutlineSpans[i].x < Min.x)
					Min.x = (f32)OutlineSpans[i].x;

				if (OutlineSpans[i].x > Max.x)
					Max.x = (f32)OutlineSpans[i].x;

				if (OutlineSpans[i].y < Min.y)
					Min.y = (f32)OutlineSpans[i].y;

				if (OutlineSpans[i].y > Max.y)
					Max.y = (f32)OutlineSpans[i].y;

				if (OutlineSpans[i].x + OutlineSpans[i].Width - 1 < Min.x)
					Min.x = (f32)(OutlineSpans[i].x + OutlineSpans[i].Width - 1);

				if (OutlineSpans[i].x + OutlineSpans[i].Width - 1 > Max.x)
					Max.x = (f32)(OutlineSpans[i].x + OutlineSpans[i].Width - 1);
			}

			Vector3 ByteBorderColor = Params.BorderColorValue.ToVector3() * 255;

			for (uint32 i = 0; i < OutlineSpans.size(); i++)
			{
				for (int32 w = 0; w < OutlineSpans[i].Width; w++)
				{
					uint32 Index = (uint32)((Height - 1 - (OutlineSpans[i].y - Min.y)) * Width + (OutlineSpans[i].x - Min.x) + w) * 4;

					PixelBuffer[Index] = (uint8)ByteBorderColor.x;
					PixelBuffer[Index + 1] = (uint8)ByteBorderColor.y;
					PixelBuffer[Index + 2] = (uint8)ByteBorderColor.z;
					PixelBuffer[Index + 3] = (uint8)std::min(255, OutlineSpans[i].Coverage);
				}
			}

			for (uint32 i = 0; i < spans.size(); i++)
			{
				for (uint32 w = 0; w < (uint32)spans[i].Width; w++)
				{
					uint32 y = (uint32)(Height - 1 - (spans[i].y - Min.y));
					f32 Percent = y / (f32)(Height - 1);

					uint32 Index = (uint32)((Height - 1 - (spans[i].y - Min.y)) * Width + (spans[i].x - Min.x) + w) * 4;

					PixelBuffer[Index] = (uint8)(PixelBuffer[Index] + (((ByteColor.x + (int32)(Diff.x * Percent)) - PixelBuffer[Index]) * spans[i].Coverage / 255.f));
					PixelBuffer[Index + 1] = (uint8)(PixelBuffer[Index + 1] + (((ByteColor.y + (int32)(Diff.y * Percent)) - PixelBuffer[Index + 1]) * spans[i].Coverage / 255.f));
					PixelBuffer[Index + 2] = (uint8)(PixelBuffer[Index + 2] + (((ByteColor.z + (int32)(Diff.z * Percent)) - PixelBuffer[Index + 2]) * spans[i].Coverage / 255.f));
					PixelBuffer[Index + 3] = (uint8)std::min(255, (int32)(PixelBuffer[Index + 3]) + spans[i].Coverage);
				}
			}

			Out.Pixels = TextureBuffer::CreateFromData(&PixelBuffer[0], Width, Height);
		}

		FT_Done_Glyph(GlyphDesc);

		Glyphs[ID] = Out;

		return Out;
	}

	void Font::Clear()
	{
		if (Face)
		{
			FT_Done_Face(Face);

			Face = NULL;
		}

		if (Library)
		{
			FT_Done_FreeType(Library);

			Library = NULL;
		}

		ContainedData.resize(0);
	}

	void Font::SetSize(uint32 Size)
	{
		if (!Library || !Face)
			return;

		FT_Error Result = FT_Set_Pixel_Sizes(Face, 0, Size);

		if (Result == FT_Err_Ok)
			CurrentSize = Size;
	}

	Glyph::Glyph() : Advance(0)
	{
	}
}

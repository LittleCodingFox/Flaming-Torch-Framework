#pragma once

class Glyph
{
public:
	uint32 Advance;
	Rect Bounds;
	DisposablePointer<TextureBuffer> Pixels;

	Glyph();
};

class Font
{
private:
	typedef std::map<StringID, Glyph> GlyphMap;
	GlyphMap Glyphs;
	std::vector<uint8> ContainedData;

	FT_Library Library;
	FT_Face Face;
	uint32 CurrentSize, LineSpacingValue;
public:

	Font();
	~Font();
	bool FromStream(Stream *TheStream);
	void Clear();
	void SetSize(uint32 Size);
	uint32 LineSpacing(const TextParams &Params);
	int32 Kerning(uint32 From, uint32 To, const TextParams &Params);
	Glyph LoadGlyph(uint32 Character, const TextParams &Params);
};

#pragma once

class Font;

/*!
*	Text rendering parameters
*/
class TextParams
{
public:
	Vector4 TextColorValue, SecondaryTextColorValue, BorderColorValue;
	Vector2 PositionValue;
	f32 BorderSizeValue, RotationValue;
	DisposablePointer<Font> FontValue;
	uint32 FontSizeValue;

	TextParams() : BorderSizeValue(0), TextColorValue(1, 1, 1, 1), SecondaryTextColorValue(1, 1, 1, 1), BorderColorValue(0, 0, 0, 1),
		FontValue(0), FontSizeValue(12), RotationValue(0) {}
	TextParams(const TextParams &o) : BorderSizeValue(o.BorderSizeValue), TextColorValue(o.TextColorValue),
		SecondaryTextColorValue(o.SecondaryTextColorValue), BorderColorValue(o.BorderColorValue), PositionValue(o.PositionValue),
		FontValue(o.FontValue), FontSizeValue(o.FontSizeValue), RotationValue(o.RotationValue) {}

	TextParams &operator=(const TextParams &o)
	{
		BorderSizeValue = o.BorderSizeValue;
		TextColorValue = o.TextColorValue;
		SecondaryTextColorValue = o.SecondaryTextColorValue;
		BorderColorValue = o.BorderColorValue;
		PositionValue = o.PositionValue;
		FontValue = o.FontValue;
		FontSizeValue = o.FontSizeValue;
		RotationValue = o.RotationValue;

		return *this;
	}

	/*!
	*	Sets the text's rotation
	*	\param Rotation the rotation in Radians
	*/
	TextParams &Rotate(f32 Rotation)
	{
		RotationValue = Rotation;

		return *this;
	}

	/*!
	*	Sets the text font
	*	\param Font the text's font
	*/
	TextParams &Font(DisposablePointer<Font> TheFont)
	{
		FontValue = TheFont;

		return *this;
	}

	/*!
	*	Sets the text font size
	*	\param FontSize the text's font size in pixels
	*/
	TextParams &FontSize(uint32 FontSize)
	{
		FontSizeValue = FontSize;

		return *this;
	}

	/*!
	*	Sets the text color
	*	\param Color the color to use
	*	\note Changes the secondary color too to prevent issues with using this
	*	\sa SecondaryColor
	*/
	TextParams &Color(const Vector4 &Color)
	{
		TextColorValue = SecondaryTextColorValue = Color;

		return *this;
	}

	/*!
	*	Sets the secondary text color
	*	\param SecondaryColor the secondary color to use
	*	\sa Color
	*/
	TextParams &SecondaryColor(const Vector4 &SecondaryColor)
	{
		SecondaryTextColorValue = SecondaryColor;

		return *this;
	}

	/*!
	*	Sets the border color for the text
	*	\param BorderColor the border color to use (default value is 0, 0, 0, 1)
	*	\sa BorderSize
	*/
	TextParams &BorderColor(const Vector4 &BorderColor)
	{
		BorderColorValue = BorderColor;

		return *this;
	}

	/*!
	*	Sets the border size for the text
	*	\param BorderSize the size of the border, in pixels (default value is 0)
	*	\sa BorderColor
	*/
	TextParams &BorderSize(f32 BorderSize)
	{
		BorderSizeValue = BorderSize;

		return *this;
	}

	/*!
	*	Sets the text position
	*	\param Position the text's position
	*/
	TextParams &Position(const Vector2 &Position)
	{
		PositionValue = Position;

		return *this;
	}
};


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

class TextRenderer
{
	friend class RendererManager;
	friend class Renderer;
private:
	struct TextResourceInfo
	{
		uint32 Character;
		TextParams TextParameters;
		Glyph Info;

		uint32 References;

		DisposablePointer<Texture> SourceTexture, InstanceTexture;

		TextResourceInfo() : References(0), Character(0) {}
	};

	typedef std::map<StringID, TextResourceInfo> TextResourceMap;

	TextResourceMap TextResources;

	DisposablePointer<TextureGroup> ResourcesGroup;

	Renderer *Owner;

	void ClearUnusedResources();
	void GetText(const std::string &Text, const TextParams &Parameters);
public:

	/*!
	*	\param Text the text string to draw
	*	\param Params the text parameters
	*/
	void DrawText(const std::string &Text, const TextParams &Params);
};

#pragma once

class Font;

/*!
*	Text rendering parameters
*/
class TextParams
{
public:
	Vector4 textColorValue, secondaryTextColorValue, borderColorValue;
	Vector2 positionValue;
	f32 borderSizeValue, rotationValue;
	DisposablePointer<Font> fontValue;
	uint32 fontSizeValue;

	TextParams() : borderSizeValue(0), textColorValue(1, 1, 1, 1), secondaryTextColorValue(1, 1, 1, 1), borderColorValue(0, 0, 0, 1),
		fontValue(0), fontSizeValue(12), rotationValue(0) {}
	TextParams(const TextParams &o) : borderSizeValue(o.borderSizeValue), textColorValue(o.textColorValue),
		secondaryTextColorValue(o.secondaryTextColorValue), borderColorValue(o.borderColorValue), positionValue(o.positionValue),
		fontValue(o.fontValue), fontSizeValue(o.fontSizeValue), rotationValue(o.rotationValue) {}

	TextParams &operator=(const TextParams &o)
	{
		borderSizeValue = o.borderSizeValue;
		textColorValue = o.textColorValue;
		secondaryTextColorValue = o.secondaryTextColorValue;
		borderColorValue = o.borderColorValue;
		positionValue = o.positionValue;
		fontValue = o.fontValue;
		fontSizeValue = o.fontSizeValue;
		rotationValue = o.rotationValue;

		return *this;
	}

	/*!
	*	Sets the text's rotation
	*	\param Rotation the rotation in Radians
	*/
	TextParams &rotate(f32 rotation)
	{
		rotationValue = rotation;

		return *this;
	}

	/*!
	*	Sets the text font
	*	\param TheFont the text's font
	*/
	TextParams &font(DisposablePointer<Font> font)
	{
		fontValue = font;

		return *this;
	}

	/*!
	*	Sets the text font size
	*	\param FontSize the text's font size in pixels
	*/
	TextParams &fontSize(uint32 fontSize)
	{
		fontSizeValue = fontSize;

		return *this;
	}

	/*!
	*	Sets the text color
	*	\param Color the color to use
	*	\note Changes the secondary color too to prevent issues with using this
	*	\sa SecondaryColor
	*/
	TextParams &color(const Vector4 &color)
	{
		textColorValue = secondaryTextColorValue = color;

		return *this;
	}

	/*!
	*	Sets the secondary text color
	*	\param SecondaryColor the secondary color to use
	*	\sa Color
	*/
	TextParams &secondaryColor(const Vector4 &secondaryColor)
	{
		secondaryTextColorValue = secondaryColor;

		return *this;
	}

	/*!
	*	Sets the border color for the text
	*	\param BorderColor the border color to use (default value is 0, 0, 0, 1)
	*	\sa BorderSize
	*/
	TextParams &borderColor(const Vector4 &borderColor)
	{
		borderColorValue = borderColor;

		return *this;
	}

	/*!
	*	Sets the border size for the text
	*	\param BorderSize the size of the border, in pixels (default value is 0)
	*	\sa BorderColor
	*/
	TextParams &borderSize(f32 borderSize)
	{
		borderSizeValue = borderSize;

		return *this;
	}

	/*!
	*	Sets the text position
	*	\param Position the text's position
	*/
	TextParams &position(const Vector2 &position)
	{
		positionValue = position;

		return *this;
	}
};


class Glyph
{
public:
	uint32 advance;
	Rect bounds;
	DisposablePointer<TextureBuffer> pixels;

	Glyph();
};

class Font
{
private:
	typedef std::map<StringID, Glyph> GlyphMap;
	GlyphMap glyphs;
	std::vector<uint8> containedData;

	FT_Library library;
	FT_Face face;
	uint32 currentSize, lineSpacingValue;
public:

	Font();
	~Font();
	bool FromStream(Stream *stream);
	void Clear();
	void SetSize(uint32 size);
	uint32 LineSpacing(const TextParams &params);
	int32 Kerning(uint32 from, uint32 to, const TextParams &params);
	Glyph LoadGlyph(uint32 character, const TextParams &params);
};

class TextRenderer
{
	friend class Renderer;
private:
	struct TextResourceInfo
	{
		uint32 character;
		TextParams textParameters;
		Glyph info;

		uint32 references;

		DisposablePointer<Texture> sourceTexture, instanceTexture;

		TextResourceInfo() : references(0), character(0) {}
	};

	typedef std::map<StringID, TextResourceInfo> TextResourceMap;

	TextResourceMap textResources;

	DisposablePointer<TextureGroup> resourcesGroup;

	void ClearUnusedResources();
	void GetText(const std::string &text, const TextParams &parameters);
public:

	/*!
	*	\param Text the text string to draw
	*	\param Params the text parameters
	*/
	void DrawText(const std::string &text, const TextParams &params);
};

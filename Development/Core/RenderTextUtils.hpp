#pragma once

/*!
*	Text rendering parameters
*/
class FLAMING_API TextParams
{
public:
	Vector4 TextColorValue, SecondaryTextColorValue, BorderColorValue;
	Vector2 PositionValue;
	f32 BorderSizeValue, RotationValue;
	sf::Font *FontValue;
	uint32 FontSizeValue;
	uint32 StyleValue;

	TextParams() : BorderSizeValue(0), TextColorValue(1, 1, 1, 1), SecondaryTextColorValue(1, 1, 1, 1), BorderColorValue(0, 0, 0, 1),
		FontValue(NULL), FontSizeValue(12), StyleValue(sf::Text::Regular), RotationValue(0) {}
	TextParams(const TextParams &o) : BorderSizeValue(o.BorderSizeValue), TextColorValue(o.TextColorValue),
		SecondaryTextColorValue(o.SecondaryTextColorValue), BorderColorValue(o.BorderColorValue), PositionValue(o.PositionValue),
		FontValue(o.FontValue), FontSizeValue(o.FontSizeValue), StyleValue(o.StyleValue), RotationValue(o.RotationValue) {};

	TextParams &operator=(const TextParams &o)
	{
		BorderSizeValue = o.BorderSizeValue;
		TextColorValue = o.TextColorValue;
		SecondaryTextColorValue = o.SecondaryTextColorValue;
		BorderColorValue = o.BorderColorValue;
		PositionValue = o.PositionValue;
		FontValue = o.FontValue;
		FontSizeValue = o.FontSizeValue;
		StyleValue = o.StyleValue;
		RotationValue = o.RotationValue;

		return *this;
	};

	/*!
	*	Sets the text's rotation
	*	\param Rotation the rotation in Radians
	*/
	TextParams &Rotate(f32 Rotation)
	{
		RotationValue = Rotation;

		return *this;
	};

	/*!
	*	Sets the font style
	*	\param Style one or more of sf::Style::*
	*/
	TextParams &Style(uint32 Style)
	{
		StyleValue = Style;

		return *this;
	};

	/*!
	*	Sets the text font
	*	\param Font the text's font
	*/
	TextParams &Font(sf::Font *Font)
	{
		FontValue = Font;

		return *this;
	};

	/*!
	*	Sets the text font size
	*	\param FontSize the text's font size in pixels
	*/
	TextParams &FontSize(uint32 FontSize)
	{
		FontSizeValue = FontSize;

		return *this;
	};

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
	};

	/*!
	*	Sets the secondary text color
	*	\param SecondaryColor the secondary color to use
	*	\sa Color
	*/
	TextParams &SecondaryColor(const Vector4 &SecondaryColor)
	{
		SecondaryTextColorValue = SecondaryColor;

		return *this;
	};

	/*!
	*	Sets the border color for the text
	*	\param BorderColor the border color to use (default value is 0, 0, 0, 1)
	*	\sa BorderSize
	*/
	TextParams &BorderColor(const Vector4 &BorderColor)
	{
		BorderColorValue = BorderColor;

		return *this;
	};

	/*!
	*	Sets the border size for the text
	*	\param BorderSize the size of the border, in pixels (default value is 0)
	*	\sa BorderColor
	*/
	TextParams &BorderSize(f32 BorderSize)
	{
		BorderSizeValue = BorderSize;

		return *this;
	};

	/*!
	*	Sets the text position
	*	\param Position the text's position
	*/
	TextParams &Position(const Vector2 &Position)
	{
		PositionValue = Position;

		return *this;
	};
};

class FLAMING_API RenderTextUtils
{
public:
	static SuperSmartPointer<sf::Font> DefaultFont;

	//Loads the default font
	//Can be called several times, it'll only load it once
	static bool LoadDefaultFont(const char *FileName);
	static Rect MeasureTextSimple(const sf::String &String, TextParams Params);
	static void FitTextAroundLength(const sf::String &String, TextParams Params,
		const f32 &LengthInPixels, int32 *OutFontSize);
	static void RenderText(RendererManager::Renderer *TheRenderer, const sf::String &String, TextParams Params);
	static std::vector<sf::String> FitTextOnRect(const sf::String &String, TextParams Params, const Vector2 &Size);
	static Rect MeasureTextLines(sf::String *Lines, uint32 LineCount, TextParams Params);
};

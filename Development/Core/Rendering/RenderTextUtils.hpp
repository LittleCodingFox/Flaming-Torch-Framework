#pragma once
#if USE_GRAPHICS

class Renderer;
class TextFont;

class RenderTextUtils
{
public:
	static DisposablePointer<TextFont> DefaultFont;

	//Loads the default font
	//Can be called several times, it'll only load it once
	static bool LoadDefaultFont(const std::string &FileName);
	static Rect MeasureTextSimple(const std::string &String, TextParams Params);
	static void FitTextAroundLength(const std::string &String, TextParams Params,
		const f32 &LengthInPixels, int32 *OutFontSize);
	static void RenderText(const std::string &String, TextParams Params);
	static std::vector<std::string> FitTextOnRect(const std::string &String, TextParams Params, const Vector2 &Size);
	static Rect MeasureTextLines(std::string *Lines, uint32 LineCount, TextParams Params);
};
#endif

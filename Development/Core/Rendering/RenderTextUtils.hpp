#pragma once
#if USE_GRAPHICS

class Renderer;
class Font;

class RenderTextUtils
{
public:
	static DisposablePointer<Font> DefaultFont;

	//Loads the default font
	//Can be called several times, it'll only load it once
	static bool LoadDefaultFont(Renderer *TheRenderer, const std::string &FileName);
	static Rect MeasureTextSimple(Renderer *TheRenderer, const std::string &String, TextParams Params);
	static void FitTextAroundLength(Renderer *TheRenderer, const std::string &String, TextParams Params,
		const f32 &LengthInPixels, int32 *OutFontSize);
	static void RenderText(Renderer *TheRenderer, const std::string &String, TextParams Params);
	static std::vector<std::string> FitTextOnRect(Renderer *TheRenderer, const std::string &String, TextParams Params, const Vector2 &Size);
	static Rect MeasureTextLines(Renderer *TheRenderer, std::string *Lines, uint32 LineCount, TextParams Params);
};
#endif

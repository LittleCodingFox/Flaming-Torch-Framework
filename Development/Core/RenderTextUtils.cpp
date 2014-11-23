#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	FontHandle RenderTextUtils::DefaultFont = 0;

	bool RenderTextUtils::LoadDefaultFont(Renderer *TheRenderer, const std::string &FileName)
	{
		PROFILE("RenderTextUtils LoadDefaultFont", StatTypes::Rendering);

		if(DefaultFont)
			return true;

		DefaultFont = ResourceManager::Instance.GetFontFromPackage(TheRenderer, "/", FileName);

		if(!DefaultFont)
		{
			DefaultFont = ResourceManager::Instance.GetFont(TheRenderer, FileName);

			if(!DefaultFont)
				return false;
		};

		return true;
	};

	Rect RenderTextUtils::MeasureTextSimple(Renderer *TheRenderer, const std::string &Str, TextParams Params)
	{
		PROFILE("RenderTextUtils MeasureTextSimple", StatTypes::Rendering);

		return TheRenderer->MeasureText(Str, Params.Font(Params.FontValue ? Params.FontValue : DefaultFont));
	};

	void RenderTextUtils::FitTextAroundLength(Renderer *TheRenderer, const std::string &Str, TextParams Params, const f32 &LengthInPixels, int32 *OutFontSize)
	{
		PROFILE("RenderTextUtils FitTextAroundLength", StatTypes::Rendering);

		FLASSERT(OutFontSize, "Invalid Font Size Pointer");
		FLASSERT(LengthInPixels > 0, "Invalid Length");

		*OutFontSize = Params.FontSizeValue;

		Vector2 MeasuredText;

		while(MeasureTextSimple(TheRenderer, Str, Params.FontSize(*OutFontSize)).ToFullSize().x > LengthInPixels)
		{
			(*OutFontSize)--;
		};
	};

	void RenderTextUtils::RenderText(Renderer *TheRenderer, const std::string &String, TextParams Params)
	{
		PROFILE("RenderTextUtils RenderText", StatTypes::Rendering);

		TheRenderer->RenderText(String, Params.Font(Params.FontValue ? Params.FontValue : DefaultFont));
	};

	Rect RenderTextUtils::MeasureTextLines(Renderer *TheRenderer, std::string *Lines, uint32 LineCount, TextParams Params)
	{
		Rect Out;
		f32 AdditionalBottom = 0;

		static Rect Temp;

		for(uint32 i = 0, y = 0; i < LineCount; i++, y += Params.FontSizeValue)
		{
			Temp = MeasureTextSimple(TheRenderer, Lines[i], Params);

			if(i == 0)
			{
				Out = Temp;
			};

			//Compensate for extra space due to lower letters like y and p
			if(Temp.Bottom > Params.FontSizeValue)
				AdditionalBottom += Temp.Bottom - Params.FontSizeValue;

			Temp.Top += y;
			Temp.Bottom += y;

			if(Temp.Left < Out.Left)
			{
				Out.Left = Temp.Left;
			};

			if(Temp.Top < Out.Top)
			{
				Out.Top = Temp.Top;
			};

			if(Temp.Right > Out.Right)
			{
				Out.Right = Temp.Right;
			};

			if(Temp.Bottom > Out.Bottom)
			{
				Out.Bottom = Temp.Bottom;
			};
		};

		Out.Bottom += AdditionalBottom;

		return Out;
	};

	std::vector<std::string> RenderTextUtils::FitTextOnRect(Renderer *TheRenderer, const std::string &String, TextParams Params, const Vector2 &Size)
	{
		PROFILE("RenderTextUtils FitTextOnRect", StatTypes::Rendering);

		std::vector<std::string> Lines = StringUtils::Split(StringUtils::Replace(String, "\r", ""), '\n'), Words, OutLines;

		std::string str = String;

		PROFILE_PROGRESS(10);

		f32 CurrentY = 0;
		f32 CurrentLineSize = Size.x;
		std::stringstream Stream, PreviousStream;

		for(uint32 i = 0; i < Lines.size(); i++)
		{
			Words = StringUtils::Split(Lines[i], ' ');

			for (uint32 j = 0; j < Words.size(); j++)
			{
				Stream << (j > 0 ? " " : "") << Words[j];

				Rect MeasuredRect = RenderTextUtils::MeasureTextSimple(TheRenderer, Stream.str(), Params);
				Vector2 MeasuredSize = MeasuredRect.ToFullSize();

				if(MeasuredSize.x > Size.x)
				{
					if(j > 0 && PreviousStream.str().length())
					{
						OutLines.push_back(PreviousStream.str());
						PreviousStream.str("");
						Stream.str(Words[j]);
					}
					else
					{
						return OutLines;
					};
				};

				PreviousStream << (j > 0 ? " " : "") << Words[j];
			};

			if (Stream.str().length())
			{
				OutLines.push_back(Stream.str());

				Stream.str("");
				PreviousStream.str("");
			};
		};

		return OutLines;
	};
#endif
};

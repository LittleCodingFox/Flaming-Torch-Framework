#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	DisposablePointer<TextFont> RenderTextUtils::DefaultFont;

	bool RenderTextUtils::LoadDefaultFont(const std::string &FileName)
	{
		PROFILE("RenderTextUtils LoadDefaultFont", StatTypes::Rendering);

		if(DefaultFont)
			return true;

		DefaultFont = g_ResourceManager.GetFont("/" + FileName);

		if(!DefaultFont)
		{
			return false;
		}

		return true;
	}

	Rect RenderTextUtils::MeasureTextSimple(const std::string &Str, TextParams Params)
	{
		PROFILE("RenderTextUtils MeasureTextSimple", StatTypes::Rendering);

		DisposablePointer<TextFont> TheFont = Params.fontValue.Get() ? Params.fontValue : DefaultFont;

		if (!TheFont.Get())
			return Rect();

		f32 LineSpacing = (f32)TheFont->LineSpacing(Params);
		f32 SpaceSize = (f32)TheFont->LoadGlyph(' ', Params).advance;

		Vector2 Position(0, (f32)Params.fontSizeValue), Min, Max;

		std::vector<std::string> Lines(StringUtils::Split(StringUtils::Strip(Str, '\r'), '\n'));

		for (uint32 i = 0; i < Lines.size(); i++)
		{
			f32 LineHeight = LineSpacing;

			for (uint32 j = 0; j < Lines[i].length(); j++)
			{
				Glyph TheGlyph = TheFont->LoadGlyph(Lines[i][j], Params);

				if (i == 0 && j == 0)
				{
					Min.x = TheGlyph.bounds.Left;
					Min.y = TheGlyph.bounds.Top;
					Max.x = TheGlyph.bounds.Right;
					Max.y = Position.y - TheGlyph.bounds.Bottom;
				}

				switch (Lines[i][j])
				{
				case ' ':
					Position.x = Position.x + SpaceSize;

					break;

				default:
					{
						if (j > 0)
							Position.x += TheFont->Kerning(Lines[i][j - 1], Lines[i][j], Params);

						Position.x = Position.x + TheGlyph.advance;
					}

					break;
				}
			}

			if (Position.x < Min.x)
				Min.x = Position.x;

			if (Position.x > Max.x)
				Max.x = Position.x;

			if (Position.y < Min.y)
				Min.y = Position.y;

			if (Position.y > Max.y)
				Max.y = Position.y;

			Position.x = 0;
			Position.y += LineSpacing;
		}

		return Rect(Min.x, Max.x, Min.y, Max.y);
	}

	void RenderTextUtils::FitTextAroundLength(const std::string &Str, TextParams Params, const f32 &LengthInPixels, int32 *OutFontSize)
	{
		PROFILE("RenderTextUtils FitTextAroundLength", StatTypes::Rendering);

		FLASSERT(OutFontSize, "Invalid Font Size Pointer");
		FLASSERT(LengthInPixels > 0, "Invalid Length");

		*OutFontSize = Params.fontSizeValue;

		Vector2 MeasuredText;

		while(MeasureTextSimple(Str, Params.FontSize(*OutFontSize)).Right > LengthInPixels)
		{
			(*OutFontSize)--;
		}
	}

	void RenderTextUtils::RenderText(const std::string &String, TextParams Params)
	{
		PROFILE("RenderTextUtils RenderText", StatTypes::Rendering);

		g_Renderer.RenderText.DrawText(String, Params);
	}

	Rect RenderTextUtils::MeasureTextLines(std::string *Lines, uint32 LineCount, TextParams Params)
	{
		Rect Out;
		f32 AdditionalBottom = 0;

		static Rect Temp;

		for(uint32 i = 0, y = 0; i < LineCount; i++, y += Params.fontSizeValue)
		{
			Temp = MeasureTextSimple(Lines[i], Params);

			if(i == 0)
			{
				Out = Temp;
			}

			//Compensate for extra space due to lower letters like y and p
			if(Temp.Bottom > Params.fontSizeValue)
				AdditionalBottom += Temp.Bottom - Params.fontSizeValue;

			Temp.Top += y;
			Temp.Bottom += y;

			if(Temp.Left < Out.Left)
			{
				Out.Left = Temp.Left;
			}

			if(Temp.Top < Out.Top)
			{
				Out.Top = Temp.Top;
			}

			if(Temp.Right > Out.Right)
			{
				Out.Right = Temp.Right;
			}

			if(Temp.Bottom > Out.Bottom)
			{
				Out.Bottom = Temp.Bottom;
			}
		}

		Out.Bottom += AdditionalBottom;

		return Out;
	}

	std::vector<std::string> RenderTextUtils::FitTextOnRect(const std::string &String, TextParams Params, const Vector2 &Size)
	{
		PROFILE("RenderTextUtils FitTextOnRect", StatTypes::Rendering);

		std::vector<std::string> Lines = StringUtils::Split(StringUtils::Replace(String, "\r", ""), '\n'), Words, OutLines;

		std::string str = String;

		PROFILE_PROGRESS(10);

		f32 CurrentY = 0;
		f32 CurrentLineSize = Size.x;
		std::stringstream Stream, PreviousStream;

		Rect MeasuredRect;
		Vector2 MeasuredSize;

		for(uint32 i = 0; i < Lines.size(); i++)
		{
			Words = StringUtils::Split(Lines[i], ' ');

			for (uint32 j = 0; j < Words.size(); j++)
			{
				Stream << (Stream.str().length() ? " " : "") << Words[j];
				
				MeasuredRect = RenderTextUtils::MeasureTextSimple(Stream.str(), Params);
				MeasuredSize = Vector2(MeasuredRect.Right, MeasuredRect.Bottom);

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
					}
				}

				PreviousStream << (j > 0 ? " " : "") << Words[j];
			}

			if (Stream.str().length())
			{
				if (CurrentY + MeasuredSize.y > Size.y)
					return OutLines;

				OutLines.push_back(Stream.str());

				Stream.str("");
				PreviousStream.str("");

				CurrentY += MathUtils::Max(MeasuredSize.y, (f32)Params.fontSizeValue);
			}
			else if(CurrentY + Params.fontSizeValue <= Size.y)
			{
				OutLines.push_back("");

				CurrentY += Params.fontSizeValue;
			}
		}

		return OutLines;
	}
#endif
}

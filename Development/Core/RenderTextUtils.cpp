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

		return TheRenderer->MeasureText(Params.FontValue ? Params.FontValue : DefaultFont, Str, Params);
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

		TheRenderer->RenderText(Params.FontValue ? Params.FontValue : DefaultFont, String, Params);
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
	
	/*!
	*	Formatting logic:
	*	#1: Split Words and Lines
	*	#2: For each word, keep adding it together until you exceed either size.x or size.y
	*	#3: Check if we can recover by verifying if the old sentence works.
	*		If it does, add a new line and continue if the size.y is less than the text height.
	*	\note: Cannot use FullSize due to it adding top and left
	*/
	std::vector<std::string> RenderTextUtils::FitTextOnRect(Renderer *TheRenderer, const std::string &String, TextParams Params, const Vector2 &Size)
	{
		PROFILE("RenderTextUtils FitTextOnRect", StatTypes::Rendering);

		std::vector<std::string> Lines;

		std::string str = String;

		std::vector<std::string> Fragments;

		//Extract Fragments

		uint32 Offset = 0, PreviousOffset = 0;

		for(;;)
		{
			PreviousOffset = Offset;

			int32 MatchSpace = str.find(L' ', PreviousOffset), MatchNewLine = str.find(L'\n', PreviousOffset);
			int32 Match = std::string::npos;

			if(MatchSpace != -1)
			{
				Match = MatchSpace;
			};

			if(MatchNewLine != -1 && (MatchSpace == -1 || MatchNewLine < MatchSpace))
			{
				Match = MatchNewLine;
			};

			if(Match == std::string::npos)
			{
				if(Offset < str.length())
				{
					Fragments.push_back(str.substr(Offset));
				};

				break;
			};

			if(Match - PreviousOffset > 0)
			{
				Fragments.push_back(str.substr(PreviousOffset, Match - PreviousOffset));

				if(Match == MatchNewLine)
					Fragments.push_back("\n");
			}
			else if(Match == MatchNewLine) //Add empty newlines
			{
				Fragments.push_back("\n");
			};

			Offset = Match + 1;
		};

		PROFILE_PROGRESS(10);

		f32 CurrentY = 0;
		f32 CurrentLineSize = Size.x;
		std::stringstream stream;

		for(uint32 i = 0; i < Fragments.size(); i++)
		{
			if(Fragments[i].at(0) == L'\n')
			{
				Lines.push_back(stream.str());

				f32 Height = RenderTextUtils::MeasureTextSimple(TheRenderer, stream.str(), Params).Size().y;

				if(Height < (f32)Params.FontSizeValue)
					Height = (f32)Params.FontSizeValue;

				CurrentY += Height;
				CurrentLineSize = Size.x;

				stream.str("");

				continue;
			};

			Rect CurrentRect = RenderTextUtils::MeasureTextSimple(TheRenderer, Fragments[i], Params);
			Vector2 CurrentSize = CurrentRect.Size() + Vector2(CurrentRect.Left, 0);

			//Impossible to proceed
			if(CurrentSize.y + CurrentY > Size.y || CurrentSize.x > Size.x)
				break;

			if(CurrentLineSize < CurrentSize.x)
			{
				Lines.push_back(stream.str());

				f32 Height = RenderTextUtils::MeasureTextSimple(TheRenderer, stream.str(), Params).Size().y;

				if(Height < (f32)Params.FontSizeValue)
					Height = (f32)Params.FontSizeValue;

				CurrentY += Height;
				CurrentLineSize = Size.x;

				stream.str("");
			};

			stream << (stream.str().length() ? " " : "") << Fragments[i];

			CurrentLineSize -= CurrentSize.x;
		};

		if(stream.str().length())
		{
			Lines.push_back(stream.str());
		};

		PROFILE_PROGRESS(100);

		return Lines;
	};
#endif
};

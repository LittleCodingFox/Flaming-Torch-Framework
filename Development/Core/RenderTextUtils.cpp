#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	SuperSmartPointer<sf::Font> RenderTextUtils::DefaultFont;

	bool RenderTextUtils::LoadDefaultFont(const char *FileName)
	{
		PROFILE("RenderTextUtils LoadDefaultFont", StatTypes::Rendering);

		if(DefaultFont.Get())
			return true;

		DefaultFont.Reset(new sf::Font);

		if(!DefaultFont->loadFromFile(FileName))
		{
			DefaultFont = ResourceManager::Instance.GetFontFromPackage("/", FileName);

			if(!DefaultFont)
				return false;
		};

		return true;
	};

	Rect RenderTextUtils::MeasureTextSimple(const sf::String &Str, TextParams Params)
	{
		PROFILE("RenderTextUtils MeasureTextSimple", StatTypes::Rendering);

		//Workaround for newline strings
		if(Str.getSize() == 0)
			return Rect(0, 0, 0, (f32)Params.FontSizeValue);

		sf::Font *TheFont = Params.FontValue ? Params.FontValue : DefaultFont.Get();

		if(!TheFont)
			return Rect();

		sf::Text Text;
		Text.setFont(*TheFont);
		Text.setCharacterSize(Params.FontSizeValue);
		Text.setString(Str);
		Text.setStyle(Params.StyleValue);

		sf::FloatRect InRect = Text.getLocalBounds();

		return Rect(InRect.left, InRect.width, InRect.top, InRect.height);
	};

	void RenderTextUtils::FitTextAroundLength(const sf::String &Str, TextParams Params, const f32 &LengthInPixels, int32 *OutFontSize)
	{
		PROFILE("RenderTextUtils FitTextAroundLength", StatTypes::Rendering);

		FLASSERT(OutFontSize, "Invalid Font Size Pointer");
		FLASSERT(LengthInPixels > 0, "Invalid Length");

		*OutFontSize = Params.FontSizeValue;

		Vector2 MeasuredText;

		while(MeasureTextSimple(Str, Params.FontSize(*OutFontSize)).ToFullSize().x > LengthInPixels)
		{
			(*OutFontSize)--;
		};
	};

	void RenderTextUtils::RenderText(RendererManager::Renderer *TheRenderer, const sf::String &String, TextParams Params)
	{
		PROFILE("RenderTextUtils RenderText", StatTypes::Rendering);

		if(String.getSize() == 0)
			return;

		Vector2 ActualPosition = Params.PositionValue;

		sf::Font *TheFont = Params.FontValue ? Params.FontValue : DefaultFont.Get();
		
		GLCHECK();

		sf::Color Border((uint8)(Params.BorderColorValue.x * 255),
			(uint8)(Params.BorderColorValue.y * 255),
			(uint8)(Params.BorderColorValue.z * 255),
			(uint8)(Params.BorderColorValue.w * 255));

		sf::Color ActualTextColor((uint8)(Params.TextColorValue.x * 255),
			(uint8)(Params.TextColorValue.y * 255),
			(uint8)(Params.TextColorValue.z * 255),
			(uint8)(Params.TextColorValue.w * 255));

		sf::Color ActualTextColor2((uint8)(Params.SecondaryTextColorValue.x * 255),
			(uint8)(Params.SecondaryTextColorValue.y * 255),
			(uint8)(Params.SecondaryTextColorValue.z * 255),
			(uint8)(Params.SecondaryTextColorValue.w * 255));

		static sf::Text Text;

		Text.setFont(*TheFont);
		Text.setCharacterSize(Params.FontSizeValue);
		Text.setColor(ActualTextColor);
		Text.setColor2(ActualTextColor2);
		Text.setBorderColor(Border);
		Text.setBorderSize(Params.BorderSizeValue);
		Text.setString(String);
		Text.setStyle(Params.StyleValue);

		const sf::VertexArray &Vertices = Text.getVertices();

		uint32 VertexCount = Vertices.getVertexCount() / 4 * 6;
		const sf::Texture *FontTexture = &TheFont->getTexture(Text.getCharacterSize(), Text.getColor(), Text.getColor2(), Text.getBorderSize(), Text.getBorderColor());
		Vector2 FontTextureSize((f32)FontTexture->getSize().x, (f32)FontTexture->getSize().y);

		static std::vector<Vector2> Positions(VertexCount), TexCoords(VertexCount);
		static std::vector<Vector4> Colors(VertexCount);

		Positions.resize(VertexCount);
		TexCoords.resize(VertexCount);
		Colors.resize(VertexCount);

		for(uint32 i = 0, index = 0; i < VertexCount; i+=6, index+=4)
		{
			Positions[i] = Positions[i + 5] = ActualPosition + Vector2(Vertices[index].position.x, Vertices[index].position.y);
			Positions[i + 1] = ActualPosition + Vector2(Vertices[index + 3].position.x, Vertices[index + 3].position.y);
			Positions[i + 2] = Positions[i + 3] = ActualPosition + Vector2(Vertices[index + 2].position.x, Vertices[index + 2].position.y);
			Positions[i + 4] = ActualPosition + Vector2(Vertices[index + 1].position.x, Vertices[index + 1].position.y);

			TexCoords[i] = TexCoords[i + 5] = Vector2(Vertices[index].texCoords.x, Vertices[index].texCoords.y) / FontTextureSize;
			TexCoords[i + 1] = Vector2(Vertices[index + 3].texCoords.x, Vertices[index + 3].texCoords.y) / FontTextureSize;
			TexCoords[i + 2] = TexCoords[i + 3] = Vector2(Vertices[index + 2].texCoords.x, Vertices[index + 2].texCoords.y) / FontTextureSize;
			TexCoords[i + 4] = Vector2(Vertices[index + 1].texCoords.x, Vertices[index + 1].texCoords.y) / FontTextureSize;

			Colors[i] = Colors[i + 5] = Vector4(Vertices[index].color.r / 255.f, Vertices[index].color.g / 255.f, Vertices[index].color.b / 255.f,
				Vertices[index].color.a / 255.f);
			Colors[i + 1] = Vector4(Vertices[index + 3].color.r / 255.f, Vertices[index + 3].color.g / 255.f, Vertices[index + 3].color.b / 255.f,
				Vertices[index + 3].color.a / 255.f);;
			Colors[i + 2] = Colors[i + 3] = Vector4(Vertices[index + 2].color.r / 255.f, Vertices[index + 2].color.g / 255.f, Vertices[index + 2].color.b / 255.f,
				Vertices[index + 2].color.a / 255.f);
			Colors[i + 4] = Vector4(Vertices[index + 1].color.r / 255.f, Vertices[index + 1].color.g / 255.f, Vertices[index + 1].color.b / 255.f,
				Vertices[index + 1].color.a / 255.f);
		};

		if(!Positions.size())
			return;

		TheRenderer->BindTexture(NULL);

		sf::Texture::bind(FontTexture);

		TheRenderer->EnableState(GL_VERTEX_ARRAY);
		TheRenderer->EnableState(GL_TEXTURE_COORD_ARRAY);
		TheRenderer->EnableState(GL_COLOR_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, &Positions[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords[0]);
		glColorPointer(4, GL_FLOAT, 0, &Colors[0]);

		glDrawArrays(GL_TRIANGLES, 0, Positions.size());

		glBindTexture(GL_TEXTURE_2D, 0);
	};

	Rect RenderTextUtils::MeasureTextLines(sf::String *Lines, uint32 LineCount, TextParams Params)
	{
		Rect Out;
		f32 AdditionalBottom = 0;

		static Rect Temp;

		for(uint32 i = 0, y = 0; i < LineCount; i++, y += Params.FontSizeValue)
		{
			Temp = MeasureTextSimple(Lines[i], Params);

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
		Formatting logic:
		#1: Split Words and Lines
		#2: For each word, keep adding it together until you exceed either size.x or size.y
		#3: Check if we can recover by verifying if the old sentence works.
			If it does, add a new line and continue if the size.y is less than the text height.
	*/
	std::vector<sf::String> RenderTextUtils::FitTextOnRect(const sf::String &String, TextParams Params, const Vector2 &Size)
	{
		PROFILE("RenderTextUtils FitTextOnRect", StatTypes::Rendering);

		std::vector<sf::String> Lines;

		std::wstring str = String.toWideString();

		std::vector<std::wstring> Fragments;

		//Extract Fragments

		uint32 Offset = 0, PreviousOffset = 0;

		for(;;)
		{
			PreviousOffset = Offset;

			int32 MatchSpace = str.find(L' ', PreviousOffset), MatchNewLine = str.find(L'\n', PreviousOffset);
			int32 Match = std::wstring::npos;

			if(MatchSpace != -1)
			{
				Match = MatchSpace;
			};

			if(MatchNewLine != -1 && (MatchSpace == -1 || MatchNewLine < MatchSpace))
			{
				Match = MatchNewLine;
			};

			if(Match == std::wstring::npos)
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
					Fragments.push_back(L"\n");
			}
			else if(Match == MatchNewLine) //Add empty newlines
			{
				Fragments.push_back(L"\n");
			};

			Offset = Match + 1;
		};

		PROFILE_PROGRESS(10);

		f32 CurrentY = 0;
		f32 CurrentLineSize = Size.x;
		std::wstringstream stream;

		for(uint32 i = 0; i < Fragments.size(); i++)
		{
			if(Fragments[i].at(0) == L'\n')
			{
				Lines.push_back(stream.str());

				f32 Height = RenderTextUtils::MeasureTextSimple(stream.str(), Params).ToFullSize().y;

				if(Height < (f32)Params.FontSizeValue)
					Height = (f32)Params.FontSizeValue;

				CurrentY += Height;
				CurrentLineSize = Size.x;

				stream.str(L"");

				continue;
			};

			Vector2 CurrentSize = RenderTextUtils::MeasureTextSimple((stream.str().length() ? L" " : L"") + Fragments[i], Params).ToFullSize();

			//Impossible to proceed
			if(CurrentSize.y + CurrentY > Size.y || CurrentSize.x > Size.x)
				break;

			if(CurrentLineSize < CurrentSize.x)
			{
				Lines.push_back(stream.str());

				f32 Height = RenderTextUtils::MeasureTextSimple(stream.str(), Params).ToFullSize().y;

				if(Height < (f32)Params.FontSizeValue)
					Height = (f32)Params.FontSizeValue;

				CurrentY += Height;
				CurrentLineSize = Size.x;

				stream.str(L"");
			};

			stream << (stream.str().length() ? L" " : L"") << Fragments[i];

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

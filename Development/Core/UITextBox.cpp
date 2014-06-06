#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UITextBox::UITextBox(UIManager *Manager) : UIPanel("UITextBox", Manager), CursorPosition(0),
		TextOffset(0)
	{
		OnMouseJustPressed.Connect(this, &UITextBox::OnMouseJustPressedTextBox);
		OnKeyJustPressed.Connect(this, &UITextBox::OnKeyJustPressedTextBox);
		OnCharacterEntered.Connect(this, &UITextBox::OnCharacterEnteredTextBox);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UITextBox::OnMouseJustPressedTextBox(UIPanel *This, const InputCenter::MouseButtonInfo &o)
	{
		if(this == Manager->GetFocusedElement() && Text.getSize() > 0 && o.Name == sf::Mouse::Left)
		{
			//Since we're already pressed, we must be inside it, so we just need to care about the X axis

			Vector2 ActualPosition = PositionValue + Vector2((f32)Padding, 0) + GetParentPosition();

			std::wstring WideString = Text.toWideString();

			if(IsPasswordValue)
			{
				std::wstringstream str;

				for(uint32 i = 0; i < WideString.size(); i++)
					str << "*";

				WideString = str.str();
			};

			uint32 X = 0;

			Rect TextSize = RenderTextUtils::MeasureTextSimple(WideString.substr(TextOffset), TextParameters);

			for(uint32 i = 0; i < WideString.length() - TextOffset; i++)
			{
				Vector2 Size = RenderTextUtils::MeasureTextSimple(WideString.substr(TextOffset, i + 1), TextParameters).ToFullSize();

				if(ActualPosition.x + Size.x + TextSize.Left > RendererManager::Instance.Input.MousePosition.x)
					break;

				X++;
			};

			CursorPosition = X;
		};
	};

	void UITextBox::OnKeyJustPressedTextBox(UIPanel *This, const InputCenter::KeyInfo &o)
	{
		if(this == Manager->GetFocusedElement())
		{
			RendererManager::Instance.Input.ConsumeInput();

			if(o.Name == sf::Keyboard::Left)
			{
				if(CursorPosition == 0 && TextOffset > 0)
				{
					TextOffset--;
				}
				else if(CursorPosition == 0 && TextOffset == 0)
				{
					return;
				}
				else
				{
					CursorPosition--;
				};
			}
			else if(o.Name == sf::Keyboard::Right)
			{
				if(CursorPosition + TextOffset + 1 > Text.getSize())
					return;

				std::wstring ActualText = Text.toWideString();

				std::wstring str = ActualText.substr(TextOffset, CursorPosition + 1);

				Vector2 TextSize = RenderTextUtils::MeasureTextSimple(str, TextParameters).ToFullSize();

				if(TextSize.x >= SizeValue.x)
				{
					TextOffset++;
				}
				else
				{
					CursorPosition++;
				};
			};
		};
	};

	void UITextBox::OnCharacterEnteredTextBox(UIPanel *This)
	{
		if(this == Manager->GetFocusedElement())
		{
			RendererManager::Instance.Input.ConsumeInput();

			std::wstring ActualText = Text.toWideString();

			if(RendererManager::Instance.Input.Character == 8) //Hardcoded Backspace
			{
				if(ActualText.length() == 0)
					return;

				ActualText.erase(TextOffset + CursorPosition - 1);

				if(TextOffset > 0)
				{
					TextOffset--;
				}
				else if(CursorPosition > 0)
				{
					CursorPosition--;
				};
			}
			else
			{
				ActualText = ActualText.substr(0, TextOffset + CursorPosition) + RendererManager::Instance.Input.Character +
					ActualText.substr(TextOffset + CursorPosition);

				CursorPosition++;

				Vector2 TextSize = RenderTextUtils::MeasureTextSimple(ActualText.substr(TextOffset), TextParameters).ToFullSize();

				while(TextSize.x >= SizeValue.x)
				{
					CursorPosition--;
					TextOffset++;

					TextSize = RenderTextUtils::MeasureTextSimple(ActualText.substr(TextOffset), TextParameters).ToFullSize();
				};
			};

			Text = ActualText;
		};
	};

	void UITextBox::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("TextBox", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		BackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		BackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("TextBox", "TextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
			&TextureRect.Top, &TextureRect.Bottom);

		TextParameters.FontSizeValue = Manager->GetDefaultFontSize();

		std::string PaddingValue = Skin->GetString("TextBox", "Padding");

		sscanf(PaddingValue.c_str(), "%u", &Padding);

		SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
	};

	void UITextBox::PerformLayout()
	{
		if(SizeValue.y + SelectBoxExtraSize.y / 2 < TextParameters.FontSizeValue + 5)
		{
			SizeValue.y = (f32)TextParameters.FontSizeValue + 5 - SelectBoxExtraSize.y / 2;
		};
	};

	void UITextBox::Update(const Vector2 &ParentPosition)
	{
		FLASSERT(Children.size() == 0, "UITextBoxes should not have children!");

		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

#define TEXTBOX_BORDER_SIZE 1

	void UITextBox::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "UITextBoxes should not have children!");

		Vector2 ActualPosition = ParentPosition + PositionValue;
		Vector2 ActualSize = SizeValue + Vector2(Padding * 2.0f, 0) + Vector2(0, SelectBoxExtraSize.y / 2);

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Sprite TheSprite;
		TheSprite.SpriteTexture = BackgroundTexture;
		TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(SizeValue).Color(Vector4(1, 1, 1, GetParentAlpha()));

		TheSprite.Draw(Renderer);

		std::wstring WideString = Text.toWideString();

		if(IsPasswordValue)
		{
			std::wstringstream str;

			for(uint32 i = 0; i < WideString.size(); i++)
				str << "*";

			WideString = str.str();
		};

		Rect TextSize = RenderTextUtils::MeasureTextSimple(WideString.substr(TextOffset), TextParameters);

		Vector2 ActualTextSize = TextSize.Position() + TextSize.Size();

		FLASSERT(SizeValue.y + SelectBoxExtraSize.y / 2 > ActualTextSize.y, "Invalid TextBox sizes!");

		//Not centering, and using SizeValue otherwise text pops
		Vector2 Offset = Vector2(-TextSize.Left + Padding, ((SizeValue.y - SelectBoxExtraSize.y / 4) * 0.25f));

		uint32 Count = 0;

		{
			for(uint32 i = 0; i < WideString.size() - TextOffset; i++, Count++)
			{
				Vector2 Size = RenderTextUtils::MeasureTextSimple(WideString.substr(TextOffset, i + 1), TextParameters).ToFullSize();

				if(Size.x >= SizeValue.x)
					break;
			};
		};

		RenderTextUtils::RenderText(Renderer, WideString.substr(TextOffset, Count),
			TextParams(TextParameters).Position(ActualPosition + Offset)
					.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
					.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));

		if(this == Manager->GetFocusedElement())
		{
			//Draw cursor
			f32 X = 0;

			for(uint32 i = 0; i < CursorPosition; i++)
			{
				Vector2 Size = RenderTextUtils::MeasureTextSimple(WideString.substr(TextOffset, i + 1), TextParameters).ToFullSize();

				if(Size.x >= SizeValue.x + Offset.x)
					break;

				X = Size.x;
			};

			Renderer->BindTexture(NULL);
			Renderer->EnableState(GL_VERTEX_ARRAY);
			Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
			Renderer->DisableState(GL_COLOR_ARRAY);

			glColor4f(0, 0, 0, AlphaValue);

			Vector2 BasePosition = ActualPosition + Vector2(X + Padding, -SelectBoxExtraSize.y / 4);

			Vector2 LineVertices[2] = {
				BasePosition + Vector2(0, SizeValue.y * 0.05f),
				BasePosition + Vector2(0, (SizeValue.y + SelectBoxExtraSize.y / 2) * 0.95f)
			};

			glVertexPointer(2, GL_FLOAT, 0, LineVertices);

			glLineWidth(TEXTBOX_BORDER_SIZE);

			glDrawArrays(GL_LINES, 0, 2);

			glLineWidth(1);
		};

		glColor4f(1, 1, 1, 1);

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

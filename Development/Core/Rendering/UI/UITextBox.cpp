#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UITextBox::UITextBox(UIManager *Manager) : UISprite(Manager), MultiLine(false), IsPasswordValue(false), CursorPosition(0), FontSize(UIELEMENT_DEFAULT_FONT_SIZE), Padding(0), TextOffset(0)
	{
		NativeTypeValue = "UITextBox";

		OnConstructed();

		OnEvent.Connect<UITextBox, &UITextBox::PrivOnEvent>(this);
	};

	void UITextBox::SetSkin(DisposablePointer<GenericConfig> Skin)
	{
		if (!Skin.Get())
			return;

		Path BackgroundPath = Path(Skin->GetString("TextBox", "BackgroundTexture", "/UIThemes/PolyCode/textfield.png"));

		Background = ManagerValue->GetUITexture(BackgroundPath);

		Path BackgroundMultilinePath = Path(Skin->GetString("TextBox", "BackgroundTextureMultiline", "/UIThemes/PolyCode/textfieldMulti.png"));

		BackgroundMultiline = ManagerValue->GetUITexture(BackgroundMultilinePath);

		if (1 != sscanf(Skin->GetString("TextBox", "FontSize", "12").c_str(), "%u", &FontSize))
		{
			FontSize = 12;
		};

		if (1 != sscanf(Skin->GetString("TextBox", "Padding", "12").c_str(), "%u", &Padding))
		{
			Padding = 0;
		};

		Rect NinePatchTextureRect;

		if (4 == sscanf(Skin->GetString("TextBox", "TextureRect", "").c_str(), "%f,%f,%f,%f", &NinePatchTextureRect.Left,
			&NinePatchTextureRect.Right, &NinePatchTextureRect.Top, &NinePatchTextureRect.Bottom))
		{
			TheSprite.Options.NinePatch(true, NinePatchTextureRect);
		}
		else
		{
			TheSprite.Options.NinePatch(false, NinePatchTextureRect);
		};
	};

	const Vector2 &UITextBox::Size() const
	{
		static Vector2 Out;

		Out = UISprite::Size() + Vector2(Padding, Padding);

		return Out;
	};

	void UITextBox::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);
	};

	void UITextBox::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if (!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		if (MultiLine)
		{
			TheSprite.SpriteTexture = BackgroundMultiline;
		}
		else
		{
			TheSprite.SpriteTexture = Background;
		};

		UISprite::Draw(ParentPosition, Renderer);

		std::string ActualString = Text;
		Vector2 Offset(TheSprite.Options.NinePatchRectValue.Left + Padding, TheSprite.Options.NinePatchRectValue.Top);

		if (ActualString.length())
		{
			if (IsPasswordValue)
			{
				std::stringstream str;

				for (unsigned long i = 0; i < ActualString.size(); i++)
					str << "*";

				ActualString = str.str();
			};

			Rect TextSize = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), ActualString.substr(TextOffset),
				TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

			Vector2 ActualTextSize = TextSize.Position() + TextSize.Size();

			unsigned long Count = 0;

			{
				for (unsigned long i = 0; i < ActualString.size() - TextOffset; i++, Count++)
				{
					Rect TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(),
						ActualString.substr(TextOffset, i + 1), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

					Vector2 Size(TextRect.Right, TextRect.Bottom);

					if (Size.x >= SizeValue.x)
						break;
				};
			};

			ManagerValue->DrawText(ActualString.substr(TextOffset, Count), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize)
				.Color(Vector4(0, 0, 0, 1)).Position(ActualPosition + Offset));
		};

		if (this == ManagerValue->GetFocusedElement())
		{
			//Draw cursor
			float X = 0;

			for (unsigned long i = 0; i < CursorPosition; i++)
			{
				Rect TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(),
					ActualString.substr(TextOffset, i + 1), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

				Vector2 Size(TextRect.Right, TextRect.Bottom);

				if (Size.x >= SizeValue.x + Offset.x)
					break;

				X = Size.x;
			};

			Sprite TheSprite;
			TheSprite.Options.Position(ActualPosition + Vector2(X + Offset.x, Offset.y)).Scale(Vector2(2, FontSize)).Color(Vector4(0, 0, 0, 1));

			TheSprite.Draw(Renderer);
		};
	};

	void UITextBox::PrivOnEvent(uint32 EventType, std::vector<void *> Arguments)
	{
		switch (EventType)
		{
		case UIEventType::MouseJustPressed:
			{
				const InputCenter::MouseButtonInfo &o = *static_cast<const InputCenter::MouseButtonInfo *>(Arguments[0]);

				if (this == ManagerValue->GetFocusedElement() && Text.length() > 0 && o.Name == sf::Mouse::Left)
				{
					//Since we're already pressed, we must be inside it, so we just need to care about the X axis

					RendererManager::Instance.Input.ConsumeInput();

					Vector2 ActualPosition = PositionValue + Vector2((float)Padding, 0) + ParentPosition();

					std::string TempText = Text;

					if (IsPasswordValue)
					{
						std::stringstream str;

						for (unsigned long i = 0; i < TempText.size(); i++)
							str << "*";

						TempText = str.str();
					};

					unsigned long X = 0;

					Rect TextSize = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), TempText.substr(TextOffset), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

					for (unsigned long i = 0; i < TempText.length() - TextOffset; i++)
					{
						TextSize = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), TempText.substr(TextOffset, i + 1),
							TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

						Vector2 Size = Vector2(TextSize.Right, TextSize.Bottom);

						if (ActualPosition.x + Size.x + TextSize.Left > RendererManager::Instance.Input.MousePosition.x)
							break;

						X++;
					};

					CursorPosition = X;
				};
			};

			break;

		case UIEventType::KeyJustPressed:
			if (this == ManagerValue->GetFocusedElement())
			{
				RendererManager::Instance.Input.ConsumeInput();

				const InputCenter::KeyInfo &o = *static_cast<const InputCenter::KeyInfo *>(Arguments[0]);

				if (o.Name == InputKey::Left)
				{
					if (CursorPosition == 0 && TextOffset > 0)
					{
						TextOffset--;
					}
					else if (CursorPosition == 0 && TextOffset == 0)
					{
						return;
					}
					else
					{
						CursorPosition--;
					};
				}
				else if (o.Name == InputKey::Right)
				{
					if (CursorPosition + TextOffset + 1 > Text.length())
						return;

					std::string ActualText = Text;

					std::string str = ActualText.substr(TextOffset, CursorPosition + 1);

					Rect TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), str, TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));
					Vector2 TextSize(TextRect.Right, TextRect.Bottom);

					if(TextSize.x >= SizeValue.x)
					{
						TextOffset++;
					}
					else
					{
						CursorPosition++;
					};
				}
				else if (o.Name == InputKey::Home)
				{
					CursorPosition = TextOffset = 0;
				}
				else if (o.Name == InputKey::End)
				{
					std::string ActualText = Text.substr(TextOffset);

					uint32 MaxCharacterCount = 0;

					for (uint32 i = 0; i < ActualText.length(); i++)
					{
						Rect TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), ActualText.substr(0, i), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));
						Vector2 TextSize(TextRect.Right, TextRect.Bottom);

						if (TextSize.x >= SizeValue.x)
						{
							MaxCharacterCount = i;
							CursorPosition = i;

							break;
						};
					};

					if (MaxCharacterCount == 0)
					{
						MaxCharacterCount = ActualText.length();
						CursorPosition = ActualText.length();
					};

					if (Text.length() && CursorPosition)
					{
						TextOffset = Text.length() - CursorPosition;
					};

					return;
				};
			};

			break;

		case UIEventType::CharacterEntered:
			if (this == ManagerValue->GetFocusedElement())
			{
				RendererManager::Instance.Input.ConsumeInput();

				std::string ActualText = Text;

				if (RendererManager::Instance.Input.Character == 8) //Hardcoded Backspace
				{
					if (ActualText.length() == 0)
						return;

					ActualText.erase(TextOffset + CursorPosition - 1, 1);

					if (TextOffset > 0)
					{
						TextOffset--;
					}
					else if (CursorPosition > 0)
					{
						CursorPosition--;
					};
				}
				else
				{
					Rect TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), ActualText.substr(TextOffset), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));
					Vector2 TextSize(TextRect.Right, TextRect.Bottom);

					ActualText = ActualText.substr(0, TextOffset + CursorPosition) + (char)RendererManager::Instance.Input.Character +
						ActualText.substr(TextOffset + CursorPosition);
					
					TextRect = RenderTextUtils::MeasureTextSimple(ManagerValue->GetOwner(), ActualText.substr(TextOffset), TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

					f32 Difference = TextRect.Right - TextSize.x;

					CursorPosition++;

					if (TextRect.Right >= SizeValue.x)
					{
						TextGlyphInfo Info = ManagerValue->GetOwner()->GetTextGlyph(RendererManager::Instance.Input.Character, TextParams().Font(ManagerValue->DefaultFontHandle).FontSize(FontSize));

						f32 Size = MathUtils::Min(Info.Pixels->Width(), Info.Advance);

						for (f32 i = 0; i < Difference; i += Size)
						{
							CursorPosition--;
							TextOffset++;
						};
					};
				};

				Text = ActualText;
			};

			break;
		};
	};

#endif
};

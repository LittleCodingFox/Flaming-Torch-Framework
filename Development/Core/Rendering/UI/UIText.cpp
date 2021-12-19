#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIText::UIText(UIManager *Manager) : UIElement("UIText", Manager), TextAlignment(UITextAlignment::Left)
	{
		OnConstructed();

		TextParameters = TextParameters.FontSize(UIELEMENT_DEFAULT_FONT_SIZE);
	}

	void UIText::SetSkin(DisposablePointer<GenericConfig> Skin)
	{
		if (!Skin.Get())
			return;

		SkinValue = Skin;

		TextParameters
			.Font(ManagerValue->DefaultFont)
			.FontSize(ManagerValue->DefaultTextFontSize)
			.Color(ManagerValue->DefaultTextColor)
			.SecondaryColor(ManagerValue->DefaultTextSecondaryColor);
	}

	void UIText::SetText(const std::string &String)
	{
		TextValue = String;
		std::vector<std::string> OutputStrings = RenderTextUtils::FitTextOnRect(TextValue, TextParameters, SizeValue);

		Strings.resize(0);

		for(uint32 i = 0; i < OutputStrings.size(); i++)
		{
			StringInfo Info;
			Info.TheString = OutputStrings[i];
			Info.Size = RenderTextUtils::MeasureTextSimple(OutputStrings[i], TextParameters);

			Strings.push_back(Info);
		}
	}

	const std::string &UIText::Text() const
	{
		return TextValue;
	}

	Vector2 UIText::TextSize() const
	{
		Vector2 Size;

		for(uint32 i = 0; i < Strings.size(); i++)
		{
			Size.y += (Strings[i].Size.Bottom < TextParameters.fontSizeValue ? TextParameters.fontSizeValue : Strings[i].Size.Bottom);

			if (Size.x < Strings[i].Size.Right)
				Size.x = Strings[i].Size.Right;
		}

		return Size;
	}

	void UIText::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);
	}

	void UIText::Draw(const Vector2 &ParentPosition)
	{
		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		if(!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > g_Renderer.Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > g_Renderer.Size().y))
			return;

		UIElement::Draw(ParentPosition);

		f32 YOffset = 0;

		if(TextAlignment & UITextAlignment::VCenter)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += MathUtils::Max(Strings[i].Size.Bottom, (f32)TextParameters.fontSizeValue);
			}

			YOffset = MathUtils::Clamp((SizeValue.y - YOffset) / 2, 0, SizeValue.y);
		}
		else if(TextAlignment & UITextAlignment::Bottom)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += MathUtils::Max(Strings[i].Size.Bottom, (f32)TextParameters.fontSizeValue);
			}

			YOffset = SizeValue.y - YOffset;
		}

		f32 TextYOffset = YOffset;

		for (uint32 i = 0; i < Strings.size(); TextYOffset += (Strings[i].Size.Size().y < TextParameters.fontSizeValue ? TextParameters.fontSizeValue : Strings[i].Size.Size().y), i++)
		{
			if(TextYOffset + MathUtils::Max(Strings[i].Size.Bottom, (f32)TextParameters.fontSizeValue) > SizeValue.y)
				break;

			Vector2 ChildrenPosition;

			if(TextAlignment & UITextAlignment::Center)
			{
				ChildrenPosition = Vector2((SizeValue.x - Strings[i].Size.Right) / 2, (f32)TextYOffset);
			}
			else if(TextAlignment & UITextAlignment::Right)
			{
				ChildrenPosition = Vector2(SizeValue.x - Strings[i].Size.Right, (f32)TextYOffset);
			}
			else
			{
				ChildrenPosition = Vector2(0, (f32)TextYOffset);
			}

			Vector2 FinalPosition = ActualPosition + ChildrenPosition;

			RenderTextUtils::RenderText(Strings[i].TheString, TextParams(TextParameters)
				.Position(FinalPosition)
				.Color(TextParameters.textColorValue)
				.SecondaryColor(TextParameters.secondaryTextColorValue));
		}

		DrawUIFocusZone(ParentPosition);
		DrawUIRect(ParentPosition);
	}
#endif
}
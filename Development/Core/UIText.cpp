#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIText::UIText(UIManager *Manager) : UIPanel("UIText", Manager), TextAlignment(UITextAlignment::Left)
	{
		OnConstructed();

		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIText::OnSkinChange()
	{
	};

	void UIText::SetText(const sf::String &String, bool AutoExpandHeight)
	{
		Text = String;
		Strings = RenderTextUtils::FitTextOnRect(Text, TextParameters, AutoExpandHeight ? Vector2(SizeValue.x, 9999999) : SizeValue);

		if(AutoExpandHeight)
		{
			SizeValue.y = SizeValue.y > Strings.size() * (TextParameters.FontSizeValue + 4) ? SizeValue.y : Strings.size() * (TextParameters.FontSizeValue + 4);
		};
	};

	const sf::String &UIText::GetText()
	{
		return Text;
	};

	Vector2 UIText::GetTextSize()
	{
		Vector2 Size;

		for(uint32 i = 0; i < Strings.size(); i++)
		{
			Vector2 TextSize = RenderTextUtils::MeasureTextSimple(Strings[i], TextParameters).ToFullSize();
			Size.y += TextSize.y;

			if(Size.x < TextSize.x)
				Size.x = TextSize.x;
		};

		return Size;
	};

	void UIText::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);
	};

	void UIText::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		uint32 YOffset = 0;

		if(TextAlignment & UITextAlignment::VCenter)
		{
			YOffset = (uint32)(SizeValue.y - (Strings.size() * (TextParameters.FontSizeValue + 4))) / 2;
		};

		for(uint32 i = 0, TextYOffset = YOffset; i < Strings.size(); i++, TextYOffset += TextParameters.FontSizeValue + 4)
		{
			if(TextAlignment & UITextAlignment::Center)
			{
				RenderTextUtils::RenderText(Renderer, Strings[i], TextParameters.Position(ActualPosition + Vector2((SizeValue.x -
					RenderTextUtils::MeasureTextSimple(Strings[i], TextParameters).ToFullSize().x) / 2, (f32)TextYOffset))
					.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha())).SecondaryColor(TextParameters.SecondaryTextColorValue *
					Vector4(1, 1, 1, GetParentAlpha())));
			}
			else if(TextAlignment & UITextAlignment::Right)
			{
				RenderTextUtils::RenderText(Renderer, Strings[i], TextParams(TextParameters).Position(ActualPosition + Vector2(SizeValue.x -
					RenderTextUtils::MeasureTextSimple(Strings[i], TextParameters).ToFullSize().x, (f32)TextYOffset))
					.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha())).SecondaryColor(TextParameters.SecondaryTextColorValue *
					Vector4(1, 1, 1, GetParentAlpha())));
			}
			else
			{
				RenderTextUtils::RenderText(Renderer, Strings[i], TextParameters.Position(ActualPosition + Vector2(0, (f32)TextYOffset))
					.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha())).SecondaryColor(TextParameters.SecondaryTextColorValue *
					Vector4(1, 1, 1, GetParentAlpha())));
			};
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

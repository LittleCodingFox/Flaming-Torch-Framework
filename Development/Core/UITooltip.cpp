#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UITooltip::UITooltip(UIManager *Manager) : UIPanel("UITooltip", Manager)
	{
		OnConstructed();
		SetMouseInputEnabled(false);
		SetKeyboardInputEnabled(false);

		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UITooltip::PerformLayout()
	{
		if(Source.Get() && Source->TooltipElement())
		{
			SizeValue = Source->TooltipElement()->ChildrenSize();
		}
		else
		{
			std::string Text = (Source ? Source->TooltipText() : OverrideText);

			Vector2 ActualFontSize = RenderTextUtils::MeasureTextSimple(Manager()->GetOwner(), Text, TextParameters).ToFullSize();

			SizeValue = Vector2(ActualFontSize.x + 10, ActualFontSize.y + 10);
		};
	};

	void UITooltip::Update(const Vector2 &ParentPosition)
	{
		FLASSERT(Children.size() == 0, "UITooltips can't have children!");

		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UITooltip::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "UITooltips can't have children!");

		SuperSmartPointer<UIPanel> MouseOverElement = Renderer->UI->GetMouseOverElement();

		if(((Source.Get() == NULL || MouseOverElement != Source) && OverrideText.length() == 0) ||
			(OverrideText.length() && Source.Get() == NULL && MouseOverElement.Get()))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		bool UsingFixedPosition = Source.Get() != NULL && Source->TooltipElement().Get() != NULL && Source->TooltipsFixed();

		if(UsingFixedPosition)
		{
			ActualPosition = Source->ParentPosition() + Source->TooltipsPosition();
		}
		else
		{
			ActualPosition.y -= SizeValue.y;

			if(ActualPosition.x > Renderer->Size().x / 2)
				ActualPosition.x -= SizeValue.x;
		};

		if(AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		if(Source.Get() && Source->TooltipElement())
		{
			Source->TooltipElement()->SetVisible(true);
			Source->TooltipElement()->Draw(ActualPosition, Renderer);
			Source->TooltipElement()->SetVisible(false);
		}
		else
		{
			std::string Text = (Source ? Source->TooltipText() : OverrideText);

			Vector2 ActualFontSize = RenderTextUtils::MeasureTextSimple(Renderer, Text, TextParameters).ToFullSize();

			Sprite TheSprite;
			TheSprite.Options.Position(ActualPosition).Scale(SizeValue).Color(Vector4(0.98f, 0.96f, 0.815f, ParentAlpha()));

			TheSprite.Draw(Renderer);

			RenderTextUtils::RenderText(Renderer, Text, TextParams(TextParameters).Color(Vector4(0, 0, 0, ParentAlpha()))
				.Position(ActualPosition + (SizeValue - ActualFontSize) / 2));
		};

		DrawUIFocusZone(ParentPosition, Renderer);
		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

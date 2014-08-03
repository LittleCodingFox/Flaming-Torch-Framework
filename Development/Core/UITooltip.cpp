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
		if(Source.Get() && Source->GetTooltipElement())
		{
			SizeValue = Source->GetTooltipElement()->GetChildrenSize();
		}
		else
		{
			sf::String Text = (Source ? Source->GetTooltipText() : OverrideText);

			Vector2 ActualFontSize = RenderTextUtils::MeasureTextSimple(Text, TextParameters).ToFullSize();

			SizeValue = Vector2(ActualFontSize.x + 10, ActualFontSize.y + 10);
		};
	};

	void UITooltip::Update(const Vector2 &ParentPosition)
	{
		FLASSERT(Children.size() == 0, "UITooltips can't have children!");

		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UITooltip::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "UITooltips can't have children!");

		SuperSmartPointer<UIPanel> MouseOverElement = Renderer->UI->GetMouseOverElement();

		if(((Source.Get() == NULL || MouseOverElement != Source) && OverrideText.getSize() == 0) ||
			(OverrideText.getSize() && Source.Get() == NULL && MouseOverElement.Get()))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		bool UsingFixedPosition = Source.Get() != NULL && Source->GetTooltipElement().Get() != NULL && Source->TooltipsFixed();

		if(UsingFixedPosition)
		{
			ActualPosition = Source->GetParentPosition() + Source->TooltipsPosition();
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

		if(Source.Get() && Source->GetTooltipElement())
		{
			Source->GetTooltipElement()->SetVisible(true);
			Source->GetTooltipElement()->Draw(ActualPosition, Renderer);
			Source->GetTooltipElement()->SetVisible(false);
		}
		else
		{
			SpriteCache::Instance.Flush(Renderer);

			sf::String Text = (Source ? Source->GetTooltipText() : OverrideText);

			Vector2 ActualFontSize = RenderTextUtils::MeasureTextSimple(Text, TextParameters).ToFullSize();

			Renderer->EnableState(GL_VERTEX_ARRAY);
			Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
			Renderer->DisableState(GL_COLOR_ARRAY);
			Renderer->DisableState(GL_NORMAL_ARRAY);
			Renderer->BindTexture(NULL);

			Vector2 Vertices[6] = {
				ActualPosition,
				ActualPosition + Vector2(0, SizeValue.y),
				ActualPosition + SizeValue,
				ActualPosition + SizeValue,
				ActualPosition + Vector2(SizeValue.x, 0),
				ActualPosition,
			};

			glColor4f(0.98f, 0.96f, 0.815f, GetParentAlpha());

			glVertexPointer(2, GL_FLOAT, 0, Vertices);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glColor4f(1, 1, 1, 1);

			RenderTextUtils::RenderText(Renderer, Text,
				TextParams(TextParameters).Color(Vector4(0, 0, 0, GetParentAlpha())).Position(ActualPosition + (SizeValue - ActualFontSize) / 2));
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

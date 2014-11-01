#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UISprite::UISprite(UIManager *Manager) : UIPanel("UISprite", Manager)
	{
		OnConstructed();
	};

	void UISprite::PerformLayout()
	{
	};

	void UISprite::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		Vector2 ParentSizeHalf = ComposedSize() / 2;
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = Children[i]->ComposedSize() / 2;
			Vector2 ChildrenPosition = Children[i]->Position() - Children[i]->Translation() + Children[i]->Offset();

			Children[i]->Update(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, ParentRotation()) +
				ParentSizeHalf - ChildrenSizeHalf - ChildrenPosition);
		};
	};

	void UISprite::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!Visible() || AlphaValue == 0 || (ActualPosition.x + ComposedSize().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + ComposedSize().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Renderer->StartClipping(Rect(ActualPosition.x - 1, ActualPosition.x + ComposedSize().x + 1, ActualPosition.y - 1, ActualPosition.y + ComposedSize().y + 1));

		UIPanel::Draw(ParentPosition, Renderer);

		Sprite TempSprite = TheSprite;

		if(TempSprite.Options.ScaleValue.x > 0 && TempSprite.Options.ScaleValue.y > 0)
		{
			TempSprite.Options = TempSprite.Options.Position(ActualPosition + TheSprite.Options.PositionValue + ScaledExtraSize() / 2).Color(TheSprite.Options.ColorValue * Vector4(1, 1, 1, ParentAlpha()))
				.Rotation(ParentRotation() + TempSprite.Options.RotationValue).NinePatchScale(ExtraSizeScaleValue);
			TempSprite.Draw(Renderer);
		};

		//We want Debug Rects to show up on top of the element... but we don't want to show it after drawing children...
		if(Manager()->DrawUIRects || Manager()->DrawUIFocusZones)
		{
			Renderer->FinishClipping();

			DrawUIFocusZone(ParentPosition, Renderer);
			DrawUIRect(ParentPosition, Renderer);

			Renderer->StartClipping(Rect(ActualPosition.x, ActualPosition.x + ComposedSize().x, ActualPosition.y, ActualPosition.y + ComposedSize().y));
		};

		Vector2 ParentSizeHalf = ComposedSize() / 2;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = Children[i]->ComposedSize() / 2;
			Vector2 ChildrenPosition = Children[i]->Position() - Children[i]->Translation() + Children[i]->Offset();

			Children[i]->Draw(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, ParentRotation()) + ParentSizeHalf -
				ChildrenSizeHalf - ChildrenPosition, Renderer);
		};

		Renderer->FinishClipping();
	};
#endif
};

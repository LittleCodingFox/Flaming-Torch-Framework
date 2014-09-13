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

		Vector2 ParentSizeHalf = GetComposedSize() / 2;
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = (Children[i]->GetSize() + Children[i]->GetScaledExtraSize()) / 2;
			Vector2 ChildrenPosition = Children[i]->GetPosition() - Children[i]->GetTranslation() + Children[i]->GetOffset();

			Children[i]->Update(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, GetParentRotation()) + ParentSizeHalf - ChildrenSizeHalf - ChildrenPosition);
		};
	};

	void UISprite::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + GetComposedSize().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + GetComposedSize().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Sprite TempSprite = TheSprite;

		if(TempSprite.Options.ScaleValue.x > 0 && TempSprite.Options.ScaleValue.y > 0)
		{
			TempSprite.Options = TempSprite.Options.Position(ActualPosition + TheSprite.Options.PositionValue + GetScaledExtraSize() / 2).Color(TheSprite.Options.ColorValue * Vector4(1, 1, 1, GetParentAlpha()))
				.Rotation(GetParentRotation() + TempSprite.Options.RotationValue).NinePatchScale(ExtraSizeScaleValue);
			TempSprite.Draw(Renderer);
		};

		DrawUIFocusZone(ParentPosition, Renderer);
		DrawUIRect(ParentPosition, Renderer);

		Vector2 ParentSizeHalf = GetComposedSize() / 2;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = Children[i]->GetComposedSize() / 2;
			Vector2 ChildrenPosition = Children[i]->GetPosition() - Children[i]->GetTranslation() + Children[i]->GetOffset();

			Children[i]->Draw(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, GetParentRotation()) + ParentSizeHalf -
				ChildrenSizeHalf - ChildrenPosition, Renderer);
		};
	};
#endif
};

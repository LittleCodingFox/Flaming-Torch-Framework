#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UISprite::UISprite(UIManager *Manager) : UIElement("UISprite", Manager)
	{
		OnConstructed();
	}
	
	const Vector2 &UISprite::Size() const
	{
		static Vector2 Out;

		Out = TheSprite.Options.NinePatchValue ? SizeValue + Vector2(TheSprite.Options.NinePatchRectValue.Left +
			TheSprite.Options.NinePatchRectValue.Right, TheSprite.Options.NinePatchRectValue.Top + TheSprite.Options.NinePatchRectValue.Bottom) *
			TheSprite.Options.NinePatchScaleValue : SizeValue;

		return Out;
	}

	void UISprite::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		for(uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Update(ActualPosition);
		}
	}

	void UISprite::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIElement::Draw(ParentPosition, Renderer);

		Sprite TempSprite = TheSprite;

		if(TempSprite.Options.ScaleValue.x > 0 && TempSprite.Options.ScaleValue.y > 0)
		{
			TempSprite.Options.Position(ActualPosition + TheSprite.Options.PositionValue);

			if(TempSprite.Options.NinePatchValue)
			{
				TempSprite.Options.Position(TempSprite.Options.PositionValue + TheSprite.Options.NinePatchRectValue.Position() * TheSprite.Options.NinePatchScaleValue);
			}

			if(!TempSprite.Options.UsingColorsArray)
			{
				TempSprite.Options.Color(TheSprite.Options.ColorValue);
			}

			TempSprite.Draw(Renderer);
		}

		//We want Debug Rects to show up on top of the element... but we don't want to show it after drawing children...
		if(Manager()->DrawUIRects || Manager()->DrawUIFocusZones)
		{
			DrawUIFocusZone(ParentPosition, Renderer);
			DrawUIRect(ParentPosition, Renderer);
		}

		for(uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Draw(ActualPosition, Renderer);
		}
	}
#endif
}

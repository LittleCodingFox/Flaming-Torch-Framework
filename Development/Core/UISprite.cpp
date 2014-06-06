#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UISprite::PerformLayout()
	{
	};

	void UISprite::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		Vector2 ActualPosition = ParentPosition + PositionValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ActualPosition);
		};
	};

	void UISprite::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Sprite TempSprite = TheSprite;

		if(TempSprite.Options.ScaleValue.x > 0 && TempSprite.Options.ScaleValue.y > 0)
		{
			TempSprite.Options = TempSprite.Options.Position(ActualPosition).Color(TheSprite.Options.ColorValue * Vector4(1, 1, 1, GetParentAlpha()));
			TempSprite.Draw(Renderer);
		};

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Draw(ActualPosition, Renderer);
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

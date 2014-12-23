#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIGroup::UIGroup(UIManager *Manager) : UIElement("UIGroup", Manager)
	{
		OnConstructed();
	};

	void UIGroup::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		for(uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Update(ActualPosition);
		};
	};

	void UIGroup::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		if(!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIElement::Draw(ParentPosition, Renderer);

		//We want Debug Rects to show up on top of the element... but we don't want to show it after drawing children...
		if(Manager()->DrawUIRects || Manager()->DrawUIFocusZones)
		{
			DrawUIFocusZone(ParentPosition, Renderer);
			DrawUIRect(ParentPosition, Renderer);
		};

		for (uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Draw(ActualPosition, Renderer);
		};
	};
#endif
};

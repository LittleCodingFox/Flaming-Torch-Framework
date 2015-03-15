#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIGroup::UIGroup(UIManager *Manager) : UIElement("UIGroup", Manager)
	{
		OnConstructed();
	}

	void UIGroup::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		for(uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Update(ActualPosition);
		}
	}

	void UIGroup::Draw(const Vector2 &ParentPosition)
	{
		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		if(!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > g_Renderer.Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > g_Renderer.Size().y))
			return;

		UIElement::Draw(ParentPosition);

		//We want Debug Rects to show up on top of the element... but we don't want to show it after drawing children...
		if(Manager()->DrawUIRects || Manager()->DrawUIFocusZones)
		{
			DrawUIFocusZone(ParentPosition);
			DrawUIRect(ParentPosition);
		}

		for (uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Draw(ActualPosition);
		}
	}
#endif
}

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIVerticalGroup::UIVerticalGroup(UIManager *Manager) : UIElement("UIVerticalGroup", Manager)
	{
		OnConstructed();
	};

	void UIVerticalGroup::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + Position() + Offset();
		f32 InitialExtent = 0;
		f32 CurrentExtent = 0, CurrentMaxMeasurement = 0;
		Vector2 CurrentPosition;
		uint32 LineStartingElement = 0;

		for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Visible() ? ChildrenValue[i]->Offset().y + ChildrenValue[i]->Size().y : 0, i++)
		{
			if (ChildrenValue[i]->Visible() == false)
				continue;

			if (CurrentExtent + ChildrenValue[i]->Size().y + ChildrenValue[i]->Offset().y > SizeValue.y)
			{
				CurrentExtent = InitialExtent;
				CurrentPosition.x += CurrentMaxMeasurement;
				CurrentMaxMeasurement = 0;
				LineStartingElement = i;
			};

			ChildrenValue[i]->SetPosition(Vector2(CurrentPosition.x, CurrentPosition.y + CurrentExtent));
			CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, ChildrenValue[i]->Size().x + ChildrenValue[i]->Offset().x);
		};

		for(uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			ChildrenValue[i]->Update(ActualPosition);
		};
	};

	void UIVerticalGroup::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
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

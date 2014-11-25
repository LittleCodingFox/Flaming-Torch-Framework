#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIGroup::UIGroup(UIManager *Manager) : UIElement("UIGroup", Manager), LayoutMode(UIGroupLayoutMode::None)
	{
		OnConstructed();
	};

	void UIGroup::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + Position() + Offset();
		f32 InitialExtent = 0;
		f32 CurrentExtent = 0, CurrentMaxMeasurement = 0;
		Vector2 CurrentPosition;
		uint32 LineStartingElement = 0;

		if (LayoutMode & UIGroupLayoutMode::Horizontal)
		{
			for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().x + ChildrenValue[i]->Size().x, i++)
			{
				if ((LayoutMode & UIGroupLayoutMode::AdjustWidth) && (CurrentExtent +
					ChildrenValue[i]->Size().x + ChildrenValue[i]->Offset().x > SizeValue.x))
				{
					CurrentExtent = InitialExtent;
					CurrentPosition.y += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				ChildrenValue[i]->SetPosition(Vector2(CurrentPosition.x + CurrentExtent, CurrentPosition.y));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, ChildrenValue[i]->Size().y + ChildrenValue[i]->Offset().y);
			};
		}
		else if (LayoutMode & UIGroupLayoutMode::Vertical)
		{
			for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().y + ChildrenValue[i]->Size().y, i++)
			{
				if((LayoutMode & UIGroupLayoutMode::AdjustHeight) && (CurrentExtent + ChildrenValue[i]->Size().y + ChildrenValue[i]->Offset().y > SizeValue.y))
				{
					CurrentExtent = InitialExtent;
					CurrentPosition.x += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				ChildrenValue[i]->SetPosition(Vector2(CurrentPosition.x, CurrentPosition.y + CurrentExtent));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, ChildrenValue[i]->Size().x + ChildrenValue[i]->Offset().x);
			};
		}
		//TODO: Make this work combined with Vertical/Horizontal
		else if (LayoutMode & UIGroupLayoutMode::Center)
		{
			f32 Size = 0;

			for (uint32 i = 0; i < ChildrenValue.size(); i++)
			{
				Size += ChildrenValue[i]->Size().x + ChildrenValue[i]->Offset().x;
			};

			f32 Difference = SizeValue.x - Size;

			if (LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().x + ChildrenValue[i]->Size().x, i++)
				{
					ChildrenValue[i]->SetPosition(Vector2(CurrentExtent, CurrentPosition.y));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / ChildrenValue.size()) / 2;
				CurrentExtent = SizeFragment;

				for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().x + ChildrenValue[i]->Size().x + SizeFragment * 2, i++)
				{
					ChildrenValue[i]->SetPosition(Vector2(CurrentExtent, CurrentPosition.y));
				};
			};
		}
		else if (LayoutMode & UIGroupLayoutMode::VerticalCenter)
		{
			f32 Size = 0;

			for (uint32 i = 0; i < ChildrenValue.size(); i++)
			{
				Size += ChildrenValue[i]->Size().y + ChildrenValue[i]->Offset().y;
			};

			f32 Difference = SizeValue.y - Size;

			if (LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().y + ChildrenValue[i]->Size().y, i++)
				{
					ChildrenValue[i]->SetPosition(Vector2(CurrentPosition.x, CurrentExtent));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / ChildrenValue.size()) / 2;
				CurrentExtent = SizeFragment;

				for (uint32 i = 0; i < ChildrenValue.size(); CurrentExtent += ChildrenValue[i]->Offset().y + ChildrenValue[i]->Size().y + SizeFragment * 2, i++)
				{
					ChildrenValue[i]->SetPosition(Vector2(CurrentPosition.x, CurrentExtent));
				};
			};
		};

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

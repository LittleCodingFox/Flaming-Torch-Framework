#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIGroup::UIGroup(UIManager *Manager) : UIPanel("UIGroup", Manager), LayoutMode(UIGroupLayoutMode::None)
	{
		OnConstructed();
	};

	void UIGroup::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		Vector2 ParentSizeHalf = ComposedSize() / 2;
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = Children[i]->ComposedSize() / 2;
			Vector2 ChildrenPosition = Children[i]->Position() - Children[i]->Translation() + Children[i]->Offset();

			Children[i]->Update(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, ParentRotation()) + ParentSizeHalf - ChildrenSizeHalf - ChildrenPosition - 
				ScaledExtraSize() / 2);
		};
	};

	void UIGroup::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!Visible() || AlphaValue == 0 || (ActualPosition.x + ComposedSize().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + ComposedSize().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		DrawUIFocusZone(ParentPosition, Renderer);
		DrawUIRect(ParentPosition, Renderer);

		Vector2 ParentSizeHalf = ComposedSize() / 2;
		Vector2 ParentOffset = Vector2::Rotate(ScaledExtraSize() / 2, ParentRotation());

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = Children[i]->ComposedSize() / 2;
			Vector2 ChildrenPosition = Children[i]->Position() - Children[i]->Translation() + Children[i]->Offset();

			Children[i]->Draw(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, ParentRotation()) + ParentSizeHalf -
				ChildrenSizeHalf - ChildrenPosition - ParentOffset, Renderer);
		};
	};

	void UIGroup::PerformLayout()
	{
		f32 InitialExtent = 0;
		f32 CurrentExtent = 0, CurrentMaxMeasurement = 0;
		f32 DirectionMultiplier = 1;
		f32 FinalHeight = 0, FinalWidth = 0;
		Vector2 CurrentPosition;
		uint32 LineStartingElement = 0;

		if(LayoutMode & UIGroupLayoutMode::InvertDirection)
		{
			DirectionMultiplier = -1;
		};

		if(LayoutMode & UIGroupLayoutMode::Horizontal)
		{
			if(LayoutMode & UIGroupLayoutMode::InvertDirection)
			{
				InitialExtent = CurrentExtent = SizeValue.x - (Children.size() ?
					Children[0]->Offset().x + Children[0]->Size().x + Children[0]->ScaledExtraSize().x : 0);
			};

			for(uint32 i = 0; i < Children.size(); CurrentExtent += DirectionMultiplier *
				(Children[i]->Offset().x + Children[i]->Size().x + Children[i]->ScaledExtraSize().x), i++)
			{
				if((LayoutMode & UIGroupLayoutMode::AdjustWidth) && (DirectionMultiplier == 1 && CurrentExtent +
					Children[i]->ScaledExtraSize().x / 2 + Children[i]->Size().x + Children[i]->Offset().x > SizeValue.x) ||
					(DirectionMultiplier == -1 && CurrentExtent < 0))
				{
					if(LayoutMode & UIGroupLayoutMode::InvertX)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize += Children[j]->Size().x + Children[j]->ScaledExtraSize().x + Children[j]->Offset().x;
						};

						FullSize = SizeValue.x - FullSize;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							Children[j]->SetPosition(Children[j]->Position() + Vector2(FullSize, 0));
						};
					};

					if(LayoutMode & UIGroupLayoutMode::InvertY)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize = MathUtils::Max(Children[j]->Size().y + Children[j]->ScaledExtraSize().y +
								Children[j]->Offset().y, FullSize);
						};

						FinalHeight += FullSize;
					};

					CurrentExtent = InitialExtent;
					CurrentPosition.y += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				Children[i]->SetPosition(Vector2(CurrentPosition.x + CurrentExtent + Children[i]->ScaledExtraSize().x / 2,
					CurrentPosition.y + Children[i]->ScaledExtraSize().y / 2 + Children[i]->Offset().y));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, Children[i]->Size().y + Children[i]->ScaledExtraSize().y);
			};

			if(LayoutMode & UIGroupLayoutMode::InvertX)
			{
				f32 FullSize = 0;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					FullSize += Children[i]->Size().x + Children[i]->ScaledExtraSize().x + Children[i]->Offset().x;
				};

				FullSize = SizeValue.x - FullSize;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->Position() + Vector2(FullSize, 0));
				};
			};

			if(LayoutMode & UIGroupLayoutMode::InvertY)
			{
				if(LineStartingElement == 0)
				{
					for(uint32 i = 0; i < Children.size(); i++)
					{
						FinalHeight = MathUtils::Max(Children[i]->Size().y + Children[i]->ScaledExtraSize().y + Children[i]->Offset().y, FinalHeight);
					};
				}
				else
				{
					f32 FullSize = 0;

					for(uint32 i = LineStartingElement; i < Children.size(); i++)
					{
						FullSize = MathUtils::Max(Children[i]->Size().y + Children[i]->ScaledExtraSize().y +
							Children[i]->Offset().y, FullSize);
					};

					FinalHeight += FullSize;
				};

				FinalHeight = SizeValue.y - FinalHeight;

				for(uint32 i = 0; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->Position() + Vector2(0, FinalHeight));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::Vertical)
		{
			if(LayoutMode & UIGroupLayoutMode::InvertDirection)
			{
				InitialExtent = CurrentExtent = SizeValue.y - (Children.size() ?
					Children[0]->Offset().y + Children[0]->Size().y + Children[0]->ScaledExtraSize().y : 0);
			};

			for(uint32 i = 0; i < Children.size(); CurrentExtent += DirectionMultiplier * (Children[i]->Offset().y + Children[i]->Size().y + Children[i]->ScaledExtraSize().y), i++)
			{
				if((LayoutMode & UIGroupLayoutMode::AdjustHeight) && (DirectionMultiplier == 1 && CurrentExtent +
					Children[i]->ScaledExtraSize().y / 2 + Children[i]->Size().y + Children[i]->Offset().y > SizeValue.y) ||
					(DirectionMultiplier == -1 && CurrentExtent < 0))
				{
					if(LayoutMode & UIGroupLayoutMode::InvertX)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize = MathUtils::Max(Children[j]->Size().x + Children[j]->ScaledExtraSize().x + Children[j]->Offset().x, FullSize);
						};

						FinalWidth += FullSize;
					};

					if(LayoutMode & UIGroupLayoutMode::InvertY)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize += Children[j]->Size().y + Children[j]->ScaledExtraSize().y + Children[j]->Offset().y;
						};

						FullSize = SizeValue.y - FullSize;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							Children[j]->SetPosition(Children[j]->Position() + Vector2(0, FullSize));
						};
					};

					CurrentExtent = InitialExtent;
					CurrentPosition.x += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->ScaledExtraSize().x / 2 + Children[i]->Offset().x,
					CurrentPosition.y + CurrentExtent + Children[i]->ScaledExtraSize().y / 2));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, Children[i]->Size().x + Children[i]->ScaledExtraSize().x);
			};

			if(LayoutMode & UIGroupLayoutMode::InvertX)
			{
				if(LineStartingElement == 0)
				{
					for(uint32 i = 0; i < Children.size(); i++)
					{
						FinalWidth = MathUtils::Max(Children[i]->Size().x + Children[i]->ScaledExtraSize().x + Children[i]->Offset().x, FinalWidth);
					};
				}
				else
				{
					f32 FullSize = 0;

					for(uint32 i = LineStartingElement; i < Children.size(); i++)
					{
						FullSize = MathUtils::Max(Children[i]->Size().x + Children[i]->ScaledExtraSize().x +
							Children[i]->Offset().x, FullSize);
					};

					FinalWidth += FullSize;
				};

				FinalWidth = SizeValue.x - FinalWidth;

				for(uint32 i = 0; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->Position() + Vector2(FinalWidth, 0));
				};
			};

			if(LayoutMode & UIGroupLayoutMode::InvertY)
			{
				f32 FullSize = 0;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					FullSize += Children[i]->Size().y + Children[i]->ScaledExtraSize().y + Children[i]->Offset().y;
				};

				FullSize = SizeValue.y - FullSize;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->Position() + Vector2(0, FullSize));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::Center)
		{
			f32 Size = 0;

			for(uint32 i = 0; i < Children.size(); i++)
			{
				Size += Children[i]->Size().x + Children[i]->ScaledExtraSize().x + Children[i]->Offset().x;
			};

			f32 Difference = SizeValue.x - Size;

			if(LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->Offset().x + Children[i]->Size().x + Children[i]->ScaledExtraSize().x, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentExtent + Children[i]->ScaledExtraSize().x / 2, CurrentPosition.y + Children[i]->ScaledExtraSize().y / 2 + Children[i]->Offset().y));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / Children.size()) / 2;
				CurrentExtent = SizeFragment;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->Offset().x + Children[i]->Size().x + Children[i]->ScaledExtraSize().x + SizeFragment * 2, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentExtent + Children[i]->ScaledExtraSize().x / 2, CurrentPosition.y + Children[i]->ScaledExtraSize().y / 2 + Children[i]->Offset().y));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::VerticalCenter)
		{
			f32 Size = 0;

			for(uint32 i = 0; i < Children.size(); i++)
			{
				Size += Children[i]->Size().y + Children[i]->ScaledExtraSize().y + Children[i]->Offset().y;
			};

			f32 Difference = SizeValue.y - Size;

			if(LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->Offset().y + Children[i]->Size().y + Children[i]->ScaledExtraSize().y, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->ScaledExtraSize().x / 2 + Children[i]->Offset().x, CurrentExtent + Children[i]->ScaledExtraSize().y / 2));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / Children.size()) / 2;
				CurrentExtent = SizeFragment;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->Offset().y + Children[i]->Size().y + Children[i]->ScaledExtraSize().y + SizeFragment * 2, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->ScaledExtraSize().x / 2 + Children[i]->Offset().x, CurrentExtent + Children[i]->ScaledExtraSize().y / 2));
				};
			};
		};
	};
#endif
};

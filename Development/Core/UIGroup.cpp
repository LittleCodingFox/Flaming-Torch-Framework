#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIGroup::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		Vector2 ParentSizeHalf = SizeValue / 2;
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = (Children[i]->GetSize() + Children[i]->GetScaledExtraSize()) / 2;
			Vector2 ChildrenPosition = Children[i]->GetPosition() - Children[i]->GetTranslation() + Children[i]->GetOffset() - Children[i]->GetScaledExtraSize() / 4;

			Children[i]->Update(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, GetParentRotation()) + ParentSizeHalf - ChildrenSizeHalf - ChildrenPosition);
		};
	};

	void UIGroup::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Vector2 ParentSizeHalf = SizeValue / 2;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Vector2 ChildrenSizeHalf = (Children[i]->GetSize() + Children[i]->GetScaledExtraSize()) / 2;
			Vector2 ChildrenPosition = Children[i]->GetPosition() - Children[i]->GetTranslation() + Children[i]->GetOffset() - Children[i]->GetScaledExtraSize() / 4;

			Children[i]->Draw(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, GetParentRotation()) + ParentSizeHalf - ChildrenSizeHalf - ChildrenPosition, Renderer);
		};

		DrawUIRect(ParentPosition, Renderer);
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
					Children[0]->GetOffset().x + Children[0]->GetSize().x + Children[0]->GetScaledExtraSize().x : 0);
			};

			for(uint32 i = 0; i < Children.size(); CurrentExtent += DirectionMultiplier *
				(Children[i]->GetOffset().x + Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x), i++)
			{
				if((LayoutMode & UIGroupLayoutMode::AdjustWidth) && (DirectionMultiplier == 1 && CurrentExtent +
					Children[i]->GetScaledExtraSize().x / 2 + Children[i]->GetSize().x + Children[i]->GetOffset().x > SizeValue.x) ||
					(DirectionMultiplier == -1 && CurrentExtent < 0))
				{
					if(LayoutMode & UIGroupLayoutMode::InvertX)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize += Children[j]->GetSize().x + Children[j]->GetScaledExtraSize().x + Children[j]->GetOffset().x;
						};

						FullSize = SizeValue.x - FullSize;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							Children[j]->SetPosition(Children[j]->GetPosition() + Vector2(FullSize, 0));
						};
					};

					if(LayoutMode & UIGroupLayoutMode::InvertY)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize = MathUtils::Max(Children[j]->GetSize().y + Children[j]->GetScaledExtraSize().y +
								Children[j]->GetOffset().y, FullSize);
						};

						FinalHeight += FullSize;
					};

					CurrentExtent = InitialExtent;
					CurrentPosition.y += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				Children[i]->SetPosition(Vector2(CurrentPosition.x + CurrentExtent + Children[i]->GetScaledExtraSize().x / 2,
					CurrentPosition.y + Children[i]->GetScaledExtraSize().y / 2 + Children[i]->GetOffset().y));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y);
			};

			if(LayoutMode & UIGroupLayoutMode::InvertX)
			{
				f32 FullSize = 0;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					FullSize += Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x + Children[i]->GetOffset().x;
				};

				FullSize = SizeValue.x - FullSize;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->GetPosition() + Vector2(FullSize, 0));
				};
			};

			if(LayoutMode & UIGroupLayoutMode::InvertY)
			{
				if(LineStartingElement == 0)
				{
					for(uint32 i = 0; i < Children.size(); i++)
					{
						FinalHeight = MathUtils::Max(Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y + Children[i]->GetOffset().y, FinalHeight);
					};
				}
				else
				{
					f32 FullSize = 0;

					for(uint32 i = LineStartingElement; i < Children.size(); i++)
					{
						FullSize = MathUtils::Max(Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y +
							Children[i]->GetOffset().y, FullSize);
					};

					FinalHeight += FullSize;
				};

				FinalHeight = SizeValue.y - FinalHeight;

				for(uint32 i = 0; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->GetPosition() + Vector2(0, FinalHeight));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::Vertical)
		{
			if(LayoutMode & UIGroupLayoutMode::InvertDirection)
			{
				InitialExtent = CurrentExtent = SizeValue.y - (Children.size() ?
					Children[0]->GetOffset().y + Children[0]->GetSize().y + Children[0]->GetScaledExtraSize().y : 0);
			};

			for(uint32 i = 0; i < Children.size(); CurrentExtent += DirectionMultiplier * (Children[i]->GetOffset().y + Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y), i++)
			{
				if((LayoutMode & UIGroupLayoutMode::AdjustHeight) && (DirectionMultiplier == 1 && CurrentExtent +
					Children[i]->GetScaledExtraSize().y / 2 + Children[i]->GetSize().y + Children[i]->GetOffset().y > SizeValue.y) ||
					(DirectionMultiplier == -1 && CurrentExtent < 0))
				{
					if(LayoutMode & UIGroupLayoutMode::InvertX)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize = MathUtils::Max(Children[j]->GetSize().x + Children[j]->GetScaledExtraSize().x + Children[j]->GetOffset().x, FullSize);
						};

						FinalWidth += FullSize;
					};

					if(LayoutMode & UIGroupLayoutMode::InvertY)
					{
						f32 FullSize = 0;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							FullSize += Children[j]->GetSize().y + Children[j]->GetScaledExtraSize().y + Children[j]->GetOffset().y;
						};

						FullSize = SizeValue.y - FullSize;

						for(uint32 j = LineStartingElement; j < i; j++)
						{
							Children[j]->SetPosition(Children[j]->GetPosition() + Vector2(0, FullSize));
						};
					};

					CurrentExtent = InitialExtent;
					CurrentPosition.x += CurrentMaxMeasurement;
					CurrentMaxMeasurement = 0;
					LineStartingElement = i;
				};

				Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->GetScaledExtraSize().x / 2 + Children[i]->GetOffset().x,
					CurrentPosition.y + CurrentExtent + Children[i]->GetScaledExtraSize().y / 2));
				CurrentMaxMeasurement = MathUtils::Max(CurrentMaxMeasurement, Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x);
			};

			if(LayoutMode & UIGroupLayoutMode::InvertX)
			{
				if(LineStartingElement == 0)
				{
					for(uint32 i = 0; i < Children.size(); i++)
					{
						FinalWidth = MathUtils::Max(Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x + Children[i]->GetOffset().x, FinalWidth);
					};
				}
				else
				{
					f32 FullSize = 0;

					for(uint32 i = LineStartingElement; i < Children.size(); i++)
					{
						FullSize = MathUtils::Max(Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x +
							Children[i]->GetOffset().x, FullSize);
					};

					FinalWidth += FullSize;
				};

				FinalWidth = SizeValue.x - FinalWidth;

				for(uint32 i = 0; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->GetPosition() + Vector2(FinalWidth, 0));
				};
			};

			if(LayoutMode & UIGroupLayoutMode::InvertY)
			{
				f32 FullSize = 0;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					FullSize += Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y + Children[i]->GetOffset().y;
				};

				FullSize = SizeValue.y - FullSize;

				for(uint32 i = LineStartingElement; i < Children.size(); i++)
				{
					Children[i]->SetPosition(Children[i]->GetPosition() + Vector2(0, FullSize));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::Center)
		{
			f32 Size = 0;

			for(uint32 i = 0; i < Children.size(); i++)
			{
				Size += Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x + Children[i]->GetOffset().x;
			};

			f32 Difference = SizeValue.x - Size;

			if(LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetOffset().x + Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentExtent + Children[i]->GetScaledExtraSize().x / 2, CurrentPosition.y + Children[i]->GetScaledExtraSize().y / 2 + Children[i]->GetOffset().y));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / Children.size()) / 2;
				CurrentExtent = SizeFragment;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetOffset().x + Children[i]->GetSize().x + Children[i]->GetScaledExtraSize().x + SizeFragment * 2, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentExtent + Children[i]->GetScaledExtraSize().x / 2, CurrentPosition.y + Children[i]->GetScaledExtraSize().y / 2 + Children[i]->GetOffset().y));
				};
			};
		}
		else if(LayoutMode & UIGroupLayoutMode::VerticalCenter)
		{
			f32 Size = 0;

			for(uint32 i = 0; i < Children.size(); i++)
			{
				Size += Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y + Children[i]->GetOffset().y;
			};

			f32 Difference = SizeValue.y - Size;

			if(LayoutMode & UIGroupLayoutMode::AdjustCloser)
			{
				CurrentExtent = Difference / 2;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetOffset().y + Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->GetScaledExtraSize().x / 2 + Children[i]->GetOffset().x, CurrentExtent + Children[i]->GetScaledExtraSize().y / 2));
				};
			}
			else
			{
				f32 SizeFragment = (Difference / Children.size()) / 2;
				CurrentExtent = SizeFragment;

				for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetOffset().y + Children[i]->GetSize().y + Children[i]->GetScaledExtraSize().y + SizeFragment * 2, i++)
				{
					Children[i]->SetPosition(Vector2(CurrentPosition.x + Children[i]->GetScaledExtraSize().x / 2 + Children[i]->GetOffset().x, CurrentExtent + Children[i]->GetScaledExtraSize().y / 2));
				};
			};
		};
	};
#endif
};

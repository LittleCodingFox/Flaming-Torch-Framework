#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UISplitPanel::UISplitPanel(UIManager *Owner) : UIPanel("UISplitPanel", Owner), Percentage(0.25f), Horizontal(true), SplitSize(5),
		GrabbingSplitter(false)
	{
	};

	void UISplitPanel::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		Vector2 TargetSize = SizeValue;

		if(Left.Get())
		{
			Left->SetPosition(Vector2());

			if(Horizontal)
			{
				TargetSize.x *= Percentage;
				TargetSize.x -= SplitSize / 2;
			}
			else
			{
				TargetSize.y *= Percentage;
				TargetSize.y -= SplitSize / 2;
			};

			Left->SetSize(TargetSize);
		};

		if(Right.Get())
		{
			TargetSize = SizeValue;

			if(Horizontal)
			{
				TargetSize.x *= 1 - Percentage;
				TargetSize.x -= SplitSize / 2;
			}
			else
			{
				TargetSize.y *= 1 - Percentage;
				TargetSize.y -= SplitSize / 2;
			};

			Right->SetPosition(Horizontal ? Vector2(SizeValue.x - TargetSize.x, 0) : Vector2(0, SizeValue.y - TargetSize.y));

			Right->SetSize(TargetSize);
		};

		if(this == GetManager()->GetFocusedElement().Get())
		{
			if(RendererManager::Instance.Input.MouseButtons[sf::Mouse::Left].JustPressed)
			{
				AxisAlignedBoundingBox AABB;

				if(Horizontal)
				{
					AABB.min = ParentPosition + PositionValue + OffsetValue + Vector2(SizeValue.x * Percentage - SplitSize / 2, 0);
					AABB.max = AABB.min + Vector2((f32)SplitSize, SizeValue.y);
				}
				else
				{
					AABB.min = ParentPosition + PositionValue + OffsetValue + Vector2(0, SizeValue.y * Percentage - SplitSize / 2);
					AABB.max = AABB.min + Vector2(SizeValue.x, (f32)SplitSize);
				};

				if(AABB.IsInside(RendererManager::Instance.Input.MousePosition))
				{
					GrabbingSplitter = true;
				};
			}
			else if(RendererManager::Instance.Input.MouseButtons[sf::Mouse::Left].JustReleased)
			{
				GrabbingSplitter = false;
			}
			else if(GrabbingSplitter)
			{
				Vector2 Difference = RendererManager::Instance.Input.MousePosition - (ParentPosition + PositionValue + OffsetValue);

				Difference.x = MathUtils::Clamp(Difference.x, 0, SizeValue.x);
				Difference.y = MathUtils::Clamp(Difference.y, 0, SizeValue.y);

				if(Horizontal)
				{
					Percentage = Difference.x / SizeValue.x;
				}
				else
				{
					Percentage = Difference.y / SizeValue.y;
				};
			};
		}
		else
		{
			GrabbingSplitter = false;
		};

		if(Left.Get())
		{
			Left->Update(ParentPosition + PositionValue + OffsetValue);
		};

		if(Right.Get())
		{
			Right->Update(ParentPosition + PositionValue + OffsetValue);
		};
	};

	void UISplitPanel::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		if(Left.Get())
		{
			bool EnabledScissor = Renderer->IsStateEnabled(GL_SCISSOR_TEST);

			Renderer->EnableState(GL_SCISSOR_TEST);

			glScissor((GLsizei)ActualPosition.x, (GLsizei)(Renderer->Size().y - ActualPosition.y - Left->GetSize().y),
				(GLsizei)Left->GetSize().x, (GLsizei)Left->GetSize().y);

			Left->Draw(ActualPosition, Renderer);

			if(!EnabledScissor)
				Renderer->DisableState(GL_SCISSOR_TEST);
		};

		if(Right.Get())
		{
			bool EnabledScissor = Renderer->IsStateEnabled(GL_SCISSOR_TEST);

			Renderer->EnableState(GL_SCISSOR_TEST);

			glScissor((GLsizei)(ActualPosition.x + Right->GetPosition().x + OffsetValue.x),
				(GLsizei)(Renderer->Size().y - ActualPosition.y - OffsetValue.y - ((!Horizontal && Left.Get()) ? Left->GetSize().y + Right->GetSize().y : Right->GetSize().y)),
				(GLsizei)Right->GetSize().x, (GLsizei)Right->GetSize().y);

			Right->Draw(ActualPosition, Renderer);

			if(!EnabledScissor)
				Renderer->DisableState(GL_SCISSOR_TEST);
		};

		Sprite SplitSprite;
		SplitSprite.Options.Color(Vector4(0.5f, 0.5f, 0.5f, 1));

		if(Horizontal)
		{
			SplitSprite.Options.Scale(Vector2((f32)SplitSize, SizeValue.y)).Position(ActualPosition + Vector2(SizeValue.x * Percentage - SplitSize / 2, 0));
		}
		else
		{
			SplitSprite.Options.Scale(Vector2(SizeValue.x, (f32)SplitSize)).Position(ActualPosition + Vector2(0, SizeValue.y * Percentage - SplitSize / 2));
		};

		SplitSprite.Draw(Renderer);

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

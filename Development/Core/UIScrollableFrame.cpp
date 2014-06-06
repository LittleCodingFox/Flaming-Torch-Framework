#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIScrollableFrame::OnSkinChange()
	{
	};

	void UIScrollableFrame::MakeScrolls()
	{
		VerticalScroll.Dispose();
		HorizontalScroll.Dispose();

		VerticalScroll.Reset(new UIScrollbar(GetManager(), true));
		HorizontalScroll.Reset(new UIScrollbar(GetManager(), false));

		GetManager()->AddElement(MakeStringID(StringUtils::MakeIntString((uint32)this, true) + "_VERTICALSCROLL"), VerticalScroll);
		GetManager()->AddElement(MakeStringID(StringUtils::MakeIntString((uint32)this, true) + "_HORIZONTALSCROLL"), HorizontalScroll);
	};

	void UIScrollableFrame::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		if(VerticalScroll.Get() == NULL || HorizontalScroll.Get() == NULL)
			MakeScrolls();

		if((VerticalScroll->GetParent() == NULL || HorizontalScroll->GetParent() == NULL) &&
			GetManager()->GetElement(ID) != SuperSmartPointer<UIPanel>())
		{
			AddChild(VerticalScroll);
			AddChild(HorizontalScroll);
		};

		Vector2 ActualPosition = ParentPosition + PositionValue;

		Vector2 ChildrenSize = GetChildrenSize();

		if(ChildrenSize.y > SizeValue.y)
		{
			VerticalScroll->SetSize(Vector2(SCROLLBAR_DRAGGABLE_SIZE, SizeValue.y) - VerticalScroll->GetExtraSize());
			VerticalScroll->SetPosition(Vector2(SizeValue.x - VerticalScroll->GetSize().x, 0) +
				Vector2(-VerticalScroll->GetExtraSize().x / 2.f, VerticalScroll->GetExtraSize().y / 2.f));
			TranslationValue.y = (VerticalScroll->Value() / (f32)VerticalScroll->MaxValue) * (ChildrenSize.y - SizeValue.y);
		};

		VerticalScroll->SetVisible(ChildrenSize.y > SizeValue.y);

		if(ChildrenSize.x > SizeValue.x)
		{
			HorizontalScroll->SetSize(Vector2(SizeValue.x, SCROLLBAR_DRAGGABLE_SIZE) - HorizontalScroll->GetExtraSize());
			HorizontalScroll->SetPosition(Vector2(0, SizeValue.y - HorizontalScroll->GetSize().y) +
				Vector2(HorizontalScroll->GetExtraSize().x / 2.f, -HorizontalScroll->GetExtraSize().y / 2.f));
			TranslationValue.x = (HorizontalScroll->Value() / (f32)HorizontalScroll->MaxValue) * (ChildrenSize.x - SizeValue.x);
		};

		HorizontalScroll->SetVisible(ChildrenSize.x > SizeValue.x);

		if(VerticalScroll->IsVisible() && HorizontalScroll->IsVisible())
		{
			VerticalScroll->SetSize(VerticalScroll->GetSize() - Vector2(0, HorizontalScroll->GetExtraSize().y + HorizontalScroll->GetSize().y));
			HorizontalScroll->SetSize(HorizontalScroll->GetSize() - Vector2(VerticalScroll->GetExtraSize().x + VerticalScroll->GetSize().x, 0));
		};

		if(VerticalScroll->IsVisible())
		{
			UIPanel *MouseOverElement = GetManager()->GetMouseOverElement().Get();

			while(MouseOverElement != NULL && MouseOverElement != this && MouseOverElement->GetParent())
			{
				MouseOverElement = MouseOverElement->GetParent();
			};

			if(MouseOverElement == this)
			{
				int32 Step = RendererManager::Instance.Input.MouseWheel < 0 ? 1 : RendererManager::Instance.Input.MouseWheel > 0 ? -1 : 0;
				uint32 MaxSteps = (VerticalScroll->MaxValue - VerticalScroll->MinValue) / VerticalScroll->ValueStep;
				uint32 StepPower = (uint32)(MaxSteps * 0.025f * (GetManager()->GetOwner()->Size().y / SizeValue.y));

				if(Step < 0)
				{
					if(VerticalScroll->CurrentStep > StepPower)
					{
						VerticalScroll->CurrentStep -= StepPower;
					}
					else if(VerticalScroll->CurrentStep > 0)
					{
						VerticalScroll->CurrentStep = 0;
					};
				}
				else if(Step > 0)
				{
					if(VerticalScroll->CurrentStep + StepPower < MaxSteps)
					{
						VerticalScroll->CurrentStep += StepPower;
					}
					else if(VerticalScroll->CurrentStep < MaxSteps)
					{
						VerticalScroll->CurrentStep = MaxSteps;
					};
				};
			};
		};

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ActualPosition - TranslationValue);
		};
	};

	void UIScrollableFrame::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		if(VerticalScroll.Get() == NULL || HorizontalScroll.Get() == NULL)
			MakeScrolls();

		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Vector2 ChildrenSize = GetChildrenSize();

		bool EnabledScissor = Renderer->IsStateEnabled(GL_SCISSOR_TEST);
		Renderer->EnableState(GL_SCISSOR_TEST);

		Vector4 PreviousScissor;
		glGetFloatv(GL_SCISSOR_BOX, (GLfloat *)&PreviousScissor);

		glScissor((GLsizei)ActualPosition.x, (GLsizei)(Renderer->Size().y - ActualPosition.y - SizeValue.y),
			(GLsizei)(SizeValue.x - (ChildrenSize.y > SizeValue.y ? VerticalScroll->GetSize().x : 0)), (GLsizei)(SizeValue.y -
			(ChildrenSize.x > SizeValue.x ? HorizontalScroll->GetSize().y : 0)));

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(Children[i] == VerticalScroll || Children[i] == HorizontalScroll)
				continue;

			Children[i]->Draw(ActualPosition - TranslationValue, Renderer);
		};

		if(!EnabledScissor)
		{
			Renderer->DisableState(GL_SCISSOR_TEST);
		}
		else
		{
			glScissor((GLsizei)PreviousScissor.x, (GLsizei)PreviousScissor.y, (GLsizei)PreviousScissor.z, (GLsizei)PreviousScissor.w);
		};

		VerticalScroll->Draw(ActualPosition, Renderer);
		HorizontalScroll->Draw(ActualPosition, Renderer);

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

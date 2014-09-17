#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UITextComposer::UITextComposer(UIManager *Manager) : UIPanel("UITextComposer", Manager), LineHeight(0), IgnoreHeightBoundsValue(false)
	{
		OnConstructed();
	};

	void UITextComposer::ClearText()
	{
		for(uint32 i = 0; i < Nodes.size(); i++)
		{
			Nodes[i].Instance.Dispose();
		};

		Nodes.clear();

		LastPosition = Vector2();
	};

	const Vector2 &UITextComposer::GetWritePosition()
	{
		return LastPosition;
	};

	void UITextComposer::SetWritePosition(const Vector2 &WritePosition)
	{
		LastPosition = WritePosition;
	};

	bool UITextComposer::IgnoreHeightBounds()
	{
		return IgnoreHeightBoundsValue;
	};

	void UITextComposer::SetIgnoreHeightBounds(bool Value)
	{
		IgnoreHeightBoundsValue = Value;
	};

	void UITextComposer::AddText(const std::string &Text, const TextParams &Params)
	{
		std::vector<std::string> Lines = StringUtils::Split(Text, '\n');

		if(Lines.size() > 1)
		{
			for(uint32 i = 0; i < Lines.size(); i++)
			{
				AddText(Lines[i], Params);

				LastPosition.x = 0;
				LastPosition.y += LineHeight;

				LineHeight = 0;
			};

			return;
		};

		Vector2 TextSize = RenderTextUtils::MeasureTextSimple(Manager()->GetOwner(), Text, Params).ToFullSize();

		if(!IgnoreHeightBoundsValue && LastPosition.y + Params.PositionValue.y + TextSize.y > SizeValue.y)
			return;

		if(LastPosition.x + Params.PositionValue.x + TextSize.x >= SizeValue.x)
		{
			std::vector<std::string> Temp = RenderTextUtils::FitTextOnRect(Manager()->GetOwner(), Text, Params, SizeValue - Vector2(LastPosition.x, 0));
			static TextParams TempParams = Params;

			AddText(Temp[0], TempParams);

			TempParams.Position(Vector2());

			LastPosition.x = 0;
			LastPosition.y += LineHeight;

			LineHeight = 0;

			AddText(Text.substr(Temp[0].length()), TempParams);

			return;
		};

		LineHeight = (uint32)MathUtils::Max((f32)LineHeight, TextSize.y);

		TextComposerNode Out;
		Out.Text = Text;
		Out.Params = Params;
		Out.Instance.Reset(new UIText(Manager()));

		Manager()->AddElement(MakeStringID("UITEXTCOMPOSER_" + StringUtils::PointerString(this) + "_" +
			StringUtils::MakeIntString((uint32)Nodes.size()) + "_NODE"), Out.Instance);

		Out.Instance->TextParameters = Params;
		Out.Instance->TextParameters.Position(Vector2());

		Out.Instance->SetPosition(LastPosition + Params.PositionValue);
		Out.Instance->SetSize(Vector2(TextSize.x * 1.05f, TextSize.y));
		Out.Instance->SetText(Text);

		AddChild(Out.Instance);

		Nodes.push_back(Out);

		LastPosition.x += Params.PositionValue.x + TextSize.x;
	};

	void UITextComposer::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ActualPosition);
		};
	};

	void UITextComposer::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		PerformLayout();

		UIPanel::Draw(ParentPosition, Renderer);

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(IgnoreHeightBoundsValue)
		{
			Vector2 ChildrenSize = this->ChildrenSize();

			//TODO
			/*
			bool EnabledScissor = Renderer->IsStateEnabled(GL_SCISSOR_TEST);
			Renderer->EnableState(GL_SCISSOR_TEST);

			Vector4 PreviousScissor;
			glGetFloatv(GL_SCISSOR_BOX, (GLfloat *)&PreviousScissor);

			glScissor((GLsizei)ActualPosition.x, (GLsizei)(Renderer->Size().y - ActualPosition.y - SizeValue.y), (GLsizei)SizeValue.x, (GLsizei)SizeValue.y);
			*/

			for(uint32 i = 0; i < Children.size(); i++)
			{
				//TODO: Maybe allow the Translation field here like in scrollbars?
				Children[i]->Draw(ActualPosition, Renderer);
			};

			/*
			if(!EnabledScissor)
			{
				Renderer->DisableState(GL_SCISSOR_TEST);
			}
			else
			{
				glScissor((GLsizei)PreviousScissor.x, (GLsizei)PreviousScissor.y, (GLsizei)PreviousScissor.z, (GLsizei)PreviousScissor.w);
			};
			*/
		}
		else
		{
			for(uint32 i = 0; i < Children.size(); i++)
			{
				Children[i]->Draw(ActualPosition, Renderer);
			};
		};
	};
#endif
};

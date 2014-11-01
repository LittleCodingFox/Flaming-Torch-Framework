#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIText::UIText(UIManager *Manager) : UIPanel("UIText", Manager), TextAlignment(UITextAlignment::Left), ExpandHeight(false)
	{
		OnConstructed();

		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIText::OnSkinChange()
	{
	};

	void UIText::SetText(const std::string &String, bool AutoExpandHeight)
	{
		TextValue = String;
		std::vector<std::string> OutputStrings = RenderTextUtils::FitTextOnRect(Manager()->GetOwner(), TextValue, TextParameters, AutoExpandHeight ? Vector2(SizeValue.x, 9999999) : SizeValue);

		Strings.resize(0);

		f32 y = 0;

		for(uint32 i = 0; i < OutputStrings.size(); i++)
		{
			StringInfo Info;
			Info.TheString = OutputStrings[i];
			Info.Size = RenderTextUtils::MeasureTextSimple(Manager()->GetOwner(), OutputStrings[i], TextParameters);

			y += Info.Size.Bottom;

			Strings.push_back(Info);
		};

		if(AutoExpandHeight)
		{
			SizeValue.y = SizeValue.y > y ? SizeValue.y : y;
		};
	};

	const std::string &UIText::Text()
	{
		return TextValue;
	};

	Vector2 UIText::TextSize()
	{
		Vector2 Size;

		for(uint32 i = 0; i < Strings.size(); i++)
		{
			Size.y += Strings[i].Size.Bottom;

			if(Size.x < Strings[i].Size.Left)
				Size.x = Strings[i].Size.Left;
		};

		return Size;
	};

	void UIText::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);
	};

	void UIText::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!Visible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Renderer->StartClipping(Rect(ActualPosition.x - 1, ActualPosition.x + ComposedSize().x + 1, ActualPosition.y - 1, ActualPosition.y + ComposedSize().y + 1));

		UIPanel::Draw(ParentPosition, Renderer);

		f32 YOffset = 0;

		if(TextAlignment & UITextAlignment::VCenter)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += (Strings[i].Size.Bottom + Strings[i].Size.Top);
			};

			YOffset = MathUtils::Clamp((SizeValue.y - YOffset) / 2, 0, SizeValue.y);
		}
		else if(TextAlignment & UITextAlignment::Bottom)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += (Strings[i].Size.Bottom + Strings[i].Size.Top);
			};

			YOffset = SizeValue.y - YOffset;
		};

		f32 TextYOffset = YOffset;

		Vector2 ParentSizeHalf = SizeValue / 2;

		for(uint32 i = 0; i < Strings.size(); TextYOffset += Strings[i].Size.Bottom, i++)
		{
			Vector2 ChildrenSizeHalf = Strings[i].Size.ToFullSize() / 2;
			Vector2 ChildrenPosition;

			if(TextAlignment & UITextAlignment::Center)
			{
				ChildrenPosition = Vector2((SizeValue.x - (Strings[i].Size.Right - Strings[i].Size.Left)) / 2, (f32)TextYOffset);
			}
			else if(TextAlignment & UITextAlignment::Right)
			{
				ChildrenPosition = Vector2(SizeValue.x - Strings[i].Size.Right, (f32)TextYOffset);
			}
			else
			{
				ChildrenPosition = Vector2(0, (f32)TextYOffset);
			};

			Vector2 FinalPosition = ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, ParentRotation()) + ParentSizeHalf - ChildrenSizeHalf;

			RenderTextUtils::RenderText(Renderer, Strings[i].TheString, TextParams(TextParameters).Position(FinalPosition)
				.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, ParentAlpha())).SecondaryColor(
				TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, ParentAlpha())).Rotate(ParentRotation()));
		};

		Renderer->FinishClipping();

		DrawUIFocusZone(ParentPosition, Renderer);
		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

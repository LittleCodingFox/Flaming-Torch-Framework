#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIText::UIText(UIManager *Manager) : UIElement("UIText", Manager), TextAlignment(UITextAlignment::Left)
	{
		OnConstructed();

		TextParameters = TextParameters.FontSize(UIELEMENT_DEFAULT_FONT_SIZE);
	};

	void UIText::SetText(const std::string &String)
	{
		TextValue = String;
		std::vector<std::string> OutputStrings = RenderTextUtils::FitTextOnRect(Manager()->GetOwner(), TextValue, TextParameters, SizeValue);

		Strings.resize(0);

		for(uint32 i = 0; i < OutputStrings.size(); i++)
		{
			StringInfo Info;
			Info.TheString = OutputStrings[i];
			Info.Size = RenderTextUtils::MeasureTextSimple(Manager()->GetOwner(), OutputStrings[i], TextParameters);

			Strings.push_back(Info);
		};
	};

	const std::string &UIText::Text() const
	{
		return TextValue;
	};

	Vector2 UIText::TextSize() const
	{
		Vector2 Size;

		for(uint32 i = 0; i < Strings.size(); i++)
		{
			Size.y += (Strings[i].Size.Size().y < TextParameters.FontSizeValue ? TextParameters.FontSizeValue : Strings[i].Size.Size().y);

			if (Size.x < Strings[i].Size.Size().x)
				Size.x = Strings[i].Size.Size().x;
		};

		return Size;
	};

	void UIText::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);
	};

	void UIText::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + Position() + Offset();

		if(!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIElement::Draw(ParentPosition, Renderer);

		f32 YOffset = 0;

		if(TextAlignment & UITextAlignment::VCenter)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += MathUtils::Max(Strings[i].Size.Size().y, (f32)TextParameters.FontSizeValue);
			};

			YOffset = MathUtils::Clamp((SizeValue.y - YOffset) / 2, 0, SizeValue.y);
		}
		else if(TextAlignment & UITextAlignment::Bottom)
		{
			for(uint32 i = 0; i < Strings.size(); i++)
			{
				YOffset += MathUtils::Max(Strings[i].Size.Size().y, (f32)TextParameters.FontSizeValue);
			};

			YOffset = SizeValue.y - YOffset;
		};

		f32 TextYOffset = YOffset;

		for (uint32 i = 0; i < Strings.size(); TextYOffset += (Strings[i].Size.Size().y < TextParameters.FontSizeValue ? TextParameters.FontSizeValue : Strings[i].Size.Size().y), i++)
		{
			if(TextYOffset + MathUtils::Max(Strings[i].Size.Size().y, (f32)TextParameters.FontSizeValue) > SizeValue.y)
				break;

			Vector2 ChildrenPosition;

			if(TextAlignment & UITextAlignment::Center)
			{
				ChildrenPosition = Vector2((SizeValue.x - Strings[i].Size.Size().x) / 2, (f32)TextYOffset);
			}
			else if(TextAlignment & UITextAlignment::Right)
			{
				ChildrenPosition = Vector2(SizeValue.x - Strings[i].Size.Size().x, (f32)TextYOffset);
			}
			else
			{
				ChildrenPosition = Vector2(0, (f32)TextYOffset);
			};

			Vector2 FinalPosition = ActualPosition + ChildrenPosition;

			RenderTextUtils::RenderText(Renderer, Strings[i].TheString, TextParams(TextParameters).Position(FinalPosition)
				.Color(TextParameters.TextColorValue).SecondaryColor(TextParameters.SecondaryTextColorValue));
		};

		DrawUIFocusZone(ParentPosition, Renderer);
		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UITextBox::UITextBox(UIManager *Manager) : UISprite(Manager), MultiLine(false), CursorPosition(0), FontSize(UIELEMENT_DEFAULT_FONT_SIZE), Padding(0)
	{
		NativeTypeValue = "UITextBox";

		OnConstructed();

		OnEvent.Connect<UITextBox, &UITextBox::PrivOnEvent>(this);
	};

	void UITextBox::SetSkin(DisposablePointer<GenericConfig> Skin)
	{
		if (!Skin.Get())
			return;

		Path BackgroundPath = Path(Skin->GetString("TextBox", "BackgroundTexture", "/UIThemes/PolyCode/textfield.png"));

		Background = ManagerValue->GetUITexture(BackgroundPath);

		Path BackgroundMultilinePath = Path(Skin->GetString("TextBox", "BackgroundTextureMultiline", "/UIThemes/PolyCode/textfieldMulti.png"));

		BackgroundMultiline = ManagerValue->GetUITexture(BackgroundMultilinePath);

		if (1 != sscanf(Skin->GetString("TextBox", "FontSize", "12").c_str(), "%u", &FontSize))
		{
			FontSize = 12;
		};

		if (1 != sscanf(Skin->GetString("TextBox", "Padding", "12").c_str(), "%u", &Padding))
		{
			Padding = 0;
		};

		Rect NinePatchTextureRect;

		if (4 == sscanf(Skin->GetString("TextBox", "TextureRect", "").c_str(), "%f,%f,%f,%f", &NinePatchTextureRect.Left,
			&NinePatchTextureRect.Right, &NinePatchTextureRect.Top, &NinePatchTextureRect.Bottom))
		{
			TheSprite.Options.NinePatch(true, NinePatchTextureRect);
		}
		else
		{
			TheSprite.Options.NinePatch(false, NinePatchTextureRect);
		};
	};

	const Vector2 &UITextBox::Size() const
	{
		static Vector2 Out;

		Out = UISprite::Size() + Vector2(Padding, Padding);

		return Out;
	};

	void UITextBox::Update(const Vector2 &ParentPosition)
	{
		UIElement::Update(ParentPosition);
	};

	void UITextBox::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if (!Visible() || (ActualPosition.x + Size().x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + Size().y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		if (MultiLine)
		{
			TheSprite.SpriteTexture = BackgroundMultiline;
		}
		else
		{
			TheSprite.SpriteTexture = Background;
		};

		UISprite::Draw(ParentPosition, Renderer);
	};

	void UITextBox::PrivOnEvent(uint32 EventType, std::vector<void *> Arguments)
	{
	};

#endif
};

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIButton::UIButton(UIManager *Manager) : UISprite(Manager), MouseDown(false), MouseOver(false)
	{
		NativeTypeValue = "UIButton";

		OnConstructed();

		OnEvent.Connect<UIButton, &UIButton::PrivOnEvent>(this);
	};

	void UIButton::SetSkin(DisposablePointer<GenericConfig> Skin)
	{
		if (!Skin.Get())
			return;

		Path NormalPath = Path(Skin->GetString("Button", "BackgroundTexture", "/UIThemes/PolyCode/button.png"));

		Normal = ManagerValue->GetUITexture(NormalPath);

		Path DownPath = Path(Skin->GetString("Button", "FocusedTexture", "/UIThemes/PolyCode/button.png"));

		Down = ManagerValue->GetUITexture(DownPath);

		Path OverPath = Path(Skin->GetString("Button", "OverTexture", "/UIThemes/PolyCode/buttonOver.png"));

		Over = ManagerValue->GetUITexture(OverPath);

		if (1 != sscanf(Skin->GetString("Button", "FontSize", "12").c_str(), "%u", &FontSize))
		{
			FontSize = 12;
		};

		FontColor = MathUtils::ColorFromHTML(Skin->GetString("Button", "FontColor", "#FFFFFFFF"));

		if (FontColor.w == 0)
		{
			FontColor = Vector4(1, 1, 1, 1);
		};

		Rect NinePatchTextureRect;

		if (4 == sscanf(Skin->GetString("Button", "TextureRect", "").c_str(), "%f,%f,%f,%f", &NinePatchTextureRect.Left,
			&NinePatchTextureRect.Right, &NinePatchTextureRect.Top, &NinePatchTextureRect.Bottom))
		{
			TheSprite.Options.NinePatch(true, NinePatchTextureRect);
		}
		else
		{
			TheSprite.Options.NinePatch(false, NinePatchTextureRect);
		};

		if (2 != sscanf(Skin->GetString("Button", "LabelOffset", "").c_str(), "%f,%f", &LabelOffset.x, &LabelOffset.y))
		{
			LabelOffset = Vector2();
		};
	};

	void UIButton::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		if (MouseDown && Down.Get())
		{
			TheSprite.SpriteTexture = Down;
		}
		else if (MouseOver && Over.Get())
		{
			TheSprite.SpriteTexture = Over;
		}
		else
		{
			TheSprite.SpriteTexture = Normal;
		};

		UISprite::Draw(ParentPosition, Renderer);

		TextParams Parameters;
		Parameters.Font(ManagerValue->DefaultFontHandle).Color(FontColor).FontSize(FontSize);

		f32 Size = RenderTextUtils::MeasureTextSimple(Renderer, Caption, Parameters).Right;

		RenderTextUtils::RenderText(Renderer, Caption, TextParams(Parameters).Position(ParentPosition + PositionValue + LabelOffset + TheSprite.Options.NinePatchRectValue.Position() + Vector2((SizeValue.x - Size) / 2, 0)));
	};

	void UIButton::SetCaption(const std::string &Caption)
	{
		this->Caption = Caption;
	};

	const std::string &UIButton::GetCaption() const
	{
		return Caption;
	};

	void UIButton::PrivOnEvent(uint32 EventType, std::vector<void *> Arguments)
	{
		switch(EventType)
		{
		case UIEventType::MouseJustPressed:
			{
				const InputCenter::MouseButtonInfo &o = *static_cast<const InputCenter::MouseButtonInfo *>(Arguments[0]);
		
				if (o.Name != InputMouseButton::Left)
					return;

				MouseDown = true;
			};

			break;

		case UIEventType::MouseReleased:
			{
				const InputCenter::MouseButtonInfo &o = *static_cast<const InputCenter::MouseButtonInfo *>(Arguments[0]);

				if (o.Name != InputMouseButton::Left)
					return;

				MouseDown = false;
			};

			break;

		case UIEventType::MouseEntered:
			MouseOver = true;

			break;

		case UIEventType::MouseLeft:
			MouseOver = false;

			break;
		};
	};
#endif
};

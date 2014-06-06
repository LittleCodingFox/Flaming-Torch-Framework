#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIButton::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("Button", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		NormalTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		NormalTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		TexturePath = Skin->GetString("Button", "FocusedTexture");
		TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		FocusedTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		FocusedTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("Button", "TextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
			&TextureRect.Top, &TextureRect.Bottom);

		std::string FontSizeValue = Skin->GetString("Button", "FontSize");

		sscanf(FontSizeValue.c_str(), "%u", &TextParameters.FontSizeValue);

		std::string FontColorValue = Skin->GetString("Button", "FontColor");

		sscanf(FontColorValue.c_str(), "%f,%f,%f,%f", &TextParameters.TextColorValue.x, &TextParameters.TextColorValue.y, &TextParameters.TextColorValue.z, &TextParameters.TextColorValue.w);

		TextParameters.Color(TextParameters.TextColorValue);

		std::string LabelOffsetValue = Skin->GetString("Button", "LabelOffset");

		sscanf(LabelOffsetValue.c_str(), "%f,%f", &LabelOffset.x, &LabelOffset.y);

		SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
	};

	UIButton::UIButton(UIManager *Manager) : UIPanel("UIButton", Manager)
	{
		OnConstructed();
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIButton::PerformLayout()
	{
		Rect Size = RenderTextUtils::MeasureTextSimple(Caption, TextParameters);

		Vector2 ActualSize = Size.ToFullSize();

		if(ActualSize.x > SizeValue.x + SelectBoxExtraSize.x / 4)
		{
			SizeValue.x = ActualSize.x - SelectBoxExtraSize.x / 4;
		};

		if(ActualSize.y > SizeValue.y + SelectBoxExtraSize.y / 4)
		{
			SizeValue.y = ActualSize.y - SelectBoxExtraSize.y / 4;
		};
	};

	void UIButton::Update(const Vector2 &ParentPosition)
	{
		FLASSERT(Children.size() == 0, "UIButtons should not have children!");

		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UIButton::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "UIButtons should not have children!");

		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x || ActualPosition.y + SizeValue.y < 0 ||
			ActualPosition.y > Renderer->Size().y))
			return;

		Rect Size = RenderTextUtils::MeasureTextSimple(Caption, TextParameters);

		Vector2 ActualSize = Size.ToFullSize();

		Sprite TheSprite;
		TheSprite.SpriteTexture = ClickPressed ? FocusedTexture : NormalTexture;
		TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(SizeValue).Color(Vector4(1, 1, 1, GetParentAlpha()));

		TheSprite.Draw(Renderer);

		//Not /2'ing the Y axis because it works better this way for some reason
		Vector2 Offset = Vector2((SizeValue.x - ActualSize.x) / 2 + LabelOffset.x + SelectBoxExtraSize.x / 4, LabelOffset.y - SelectBoxExtraSize.y / 4);

		RenderTextUtils::RenderText(Renderer, Caption, TextParams(TextParameters).Position(ActualPosition + Offset)
			.Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
			.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));

		glColor4f(1, 1, 1, 1);
		
		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

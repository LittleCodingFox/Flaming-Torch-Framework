#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UICheckBox::UICheckBox(UIManager *Manager) : UIPanel("UICheckBox", Manager), Checked(false)
	{
		OnConstructed();
		OnClick.Connect(this, &UICheckBox::CheckMouseClick);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UICheckBox::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("CheckBox", "CheckTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		CheckTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		CheckTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		TexturePath = Skin->GetString("CheckBox", "UnCheckTexture");
		TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		UnCheckTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		UnCheckTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string LabelOffsetValue = Skin->GetString("CheckBox", "LabelOffset");

		sscanf(LabelOffsetValue.c_str(), "%f,%f", &LabelOffset.x, &LabelOffset.y);
	};

	void UICheckBox::CheckMouseClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name == sf::Mouse::Left)
			Checked = !Checked;
	};

	void UICheckBox::PerformLayout()
	{
		SizeValue = Vector2((CheckTexture.Get() ? (f32)CheckTexture->Width() : 0.f) + RenderTextUtils::MeasureTextSimple(Caption, TextParameters)
			.ToFullSize().x + LabelOffset.x, CheckTexture.Get() ? (f32)CheckTexture->Height() : 0.f);
	};

	void UICheckBox::Update(const Vector2 &ParentPosition)
	{
		FLASSERT(Children.size() == 0, "This control does not support children!");

		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UICheckBox::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "This control does not support children!");

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x || ActualPosition.y + SizeValue.y < 0 ||
			ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Sprite TheSprite;
		TheSprite.SpriteTexture = Checked ? CheckTexture : UnCheckTexture;
		TheSprite.Options = SpriteDrawOptions().Position(ActualPosition).Color(Vector4(1, 1, 1, GetParentAlpha()));
		TheSprite.Draw(Renderer);

		RenderTextUtils::RenderText(Renderer, Caption, TextParams(TextParameters).Position(ActualPosition +
			Vector2((f32)TheSprite.SpriteTexture->Width(), 0) + LabelOffset).Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
			.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

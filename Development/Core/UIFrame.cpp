#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIFrame::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("Frame", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		BackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		BackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("Frame", "TextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
			&TextureRect.Top, &TextureRect.Bottom);

		SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
	};

	void UIFrame::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ParentPosition + PositionValue);
		};
	};

	void UIFrame::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		Sprite TheSprite;
		TheSprite.SpriteTexture = BackgroundTexture;
		TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(SizeValue).Color(Vector4(1, 1, 1, GetParentAlpha()));

		TheSprite.Draw(Renderer);

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(Children[i]->IsVisible())
				Children[i]->Draw(ActualPosition, Renderer);
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIMenu::UIMenu(UIManager *Manager) : UIPanel("UIMenu", Manager)
	{
		OnConstructed();
		OnClick.Connect(this, &UIMenu::OnItemClick);
		OnLoseFocus.Connect(this, &UIMenu::CloseSelf);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIMenu::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("Menu", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		BackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		BackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");
		TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		SelectorBackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		SelectorBackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("Menu", "TextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
			&TextureRect.Top, &TextureRect.Bottom);

		std::string PaddingValue = Skin->GetString("Menu", "Padding");

		sscanf(PaddingValue.c_str(), "%f,%f", &Padding.x, &Padding.y);

		std::string SelectorPaddingValue = Skin->GetString("Menu", "SelectorPadding");

		sscanf(SelectorPaddingValue.c_str(), "%u", &SelectorPadding);

		std::string TextOffsetValue = Skin->GetString("Menu", "TextOffset");

		sscanf(TextOffsetValue.c_str(), "%f,%f", &TextOffset.x, &TextOffset.y);

		std::string ItemHeightValue = Skin->GetString("Menu", "ItemHeight");

		sscanf(ItemHeightValue.c_str(), "%u", &ItemHeight);

		SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
	};

	void UIMenu::CloseSelf(UIPanel *Self)
	{
		Future::Instance.Post(Manager, &UIManager::RemoveMenuFuture);
	};

	void UIMenu::PerformLayout()
	{
		SizeValue.y = (f32)Items.size() * ItemHeight + Padding.y * 2.f;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			f32 SizeW = RenderTextUtils::MeasureTextSimple(Items[i].Caption, TextParameters).ToFullSize().x;

			if(SizeW > SizeValue.x)
				SizeValue.x = SizeW;
		};
	};

	void UIMenu::OnItemClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name == sf::Mouse::Left)
		{
			Vector2 ActualPosition = PositionValue + GetParentPosition() + OffsetValue;

			const Vector2 &MousePosition = RendererManager::Instance.Input.MousePosition;

			AxisAlignedBoundingBox AABB;

			for(uint32 i = 0, ypos = 0; i < Items.size(); i++, ypos += ItemHeight)
			{
				AABB.min = ActualPosition + Vector2(Padding.x, Padding.y + (f32)ypos);
				AABB.max = AABB.min + Vector2(SizeValue.x, (f32)ItemHeight);

				if(AABB.IsInside(MousePosition))
				{
					OnItemSelected(&Items[i]);

					break;
				};
			};
		};

		CloseSelf(NULL);
	};

	void UIMenu::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UIMenu::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		FLASSERT(Children.size() == 0, "This control does not support children!");

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue - Vector2(Padding.x, 0);
		Vector2 ActualSize = SizeValue + Vector2(4 + Padding.x, Padding.y);

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x || ActualPosition.y + SizeValue.y < 0 ||
			ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Sprite TheSprite;
		TheSprite.SpriteTexture = BackgroundTexture;
		TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(ActualSize);

		TheSprite.Draw(Renderer);

		const Vector2 &MousePosition = RendererManager::Instance.Input.MousePosition;

		AxisAlignedBoundingBox AABB;
		bool DrewSelector = false;

		for(uint32 i = 0, ypos = 0; i < Items.size(); i++, ypos += ItemHeight)
		{
			AABB.min = ActualPosition + Vector2(Padding.x, Padding.y + (f32)ypos);
			AABB.max = AABB.min + Vector2(SizeValue.x, (f32)ItemHeight);

			if(!DrewSelector && AABB.IsInside(MousePosition))
			{
				DrewSelector = true;

				TheSprite.SpriteTexture = SelectorBackgroundTexture;
				TheSprite.Options.Position(AABB.min.ToVector2()).NinePatch(true, Rect()).Scale(Vector2(SizeValue.x, (f32)ItemHeight + SelectorPadding))
					.Color(Vector4(1, 1, 1, GetParentAlpha()));

				TheSprite.Draw(Renderer);
			};

			Vector2 TextSize = RenderTextUtils::MeasureTextSimple(Items[i].Caption, TextParameters).ToFullSize();

			RenderTextUtils::RenderText(Renderer, Items[i].Caption, TextParams(TextParameters)
				.Position(AABB.min.ToVector2() + Vector2(0, (ItemHeight - TextSize.y) / 2.f)).Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
				.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

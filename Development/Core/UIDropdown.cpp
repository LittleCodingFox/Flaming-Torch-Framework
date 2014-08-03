#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIDropdown::UIDropdown(UIManager *Manager) : UIPanel("UIDropdown", Manager), SelectedIndex(-1)
	{
		OnConstructed();
		OnClick.Connect(this, &UIDropdown::OnItemClickCheck);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIDropdown::OnSkinChange()
	{
		TextParameters.FontSizeValue = GetManager()->GetDefaultFontSize();

		std::string TexturePath = Skin->GetString("Dropdown", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		BackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		BackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		TexturePath = Skin->GetString("Dropdown", "DropdownTexture");
		TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		DropdownTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		DropdownTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("Dropdown", "TextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
			&TextureRect.Top, &TextureRect.Bottom);

		std::string DropdownHeightString = Skin->GetString("Dropdown", "Height");

		sscanf(DropdownHeightString.c_str(), "%f", &DropdownHeight);

		std::string DropdownOffsetString = Skin->GetString("Dropdown", "Offset");

		sscanf(DropdownOffsetString.c_str(), "%f,%f", &DropdownOffset.x, &DropdownOffset.y);

		std::string TextOffsetString = Skin->GetString("Dropdown", "TextOffset");

		sscanf(TextOffsetString.c_str(), "%f,%f", &TextOffset.x, &TextOffset.y);
	};

	void UIDropdown::PerformLayout()
	{
		SizeValue.y = DropdownHeight;
	};

	void UIDropdown::SetSelectedItem(UIMenu::Item *Item)
	{
		SelectedIndex = Item->Index;
		OnItemClick(this);
	};

	void UIDropdown::OnItemClickCheck(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != sf::Mouse::Left)
			return;

		UIMenu *Menu = Self->GetManager()->CreateMenu(GetParentPosition() + PositionValue + OffsetValue + Vector2(0, DropdownHeight));

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Menu->AddItem(Items[i]);
		};

		Menu->SetSize(Vector2(SizeValue.x, Menu->GetSize().y));
		Menu->OnItemSelected.Connect(this, &UIDropdown::SetSelectedItem);
	};

	void UIDropdown::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);
	};

	void UIDropdown::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		static AxisAlignedBoundingBox AABB;

		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		Sprite TheSprite;
		TheSprite.SpriteTexture = BackgroundTexture;
		TheSprite.Options.NinePatch(true, TextureRect).Position(ActualPosition).Scale(Vector2(SizeValue.x, DropdownHeight)).
			Color(Vector4(1, 1, 1, AlphaValue));

		TheSprite.Draw(Renderer);

		std::string ItemName;

		if(SelectedIndex == -1 || SelectedIndex >= (int32)Items.size())
		{
			ItemName = "(None)";
		}
		else
		{
			ItemName = Items[SelectedIndex];
		};

		RenderTextUtils::RenderText(Renderer, ItemName, TextParams(TextParameters).Position(ActualPosition + TextOffset).Color(TextParameters.TextColorValue *
			Vector4(1, 1, 1, GetParentAlpha())).SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));

		TheSprite.SpriteTexture = DropdownTexture;
		TheSprite.Options.NinePatch(false, TextureRect).Position(ActualPosition + Vector2(SizeValue.x - DropdownOffset.x - DropdownTexture->Width(),
			DropdownOffset.y)).Scale(Vector2(1, 1)).Color(Vector4(1, 1, 1, GetParentAlpha()));

		TheSprite.Draw(Renderer);

		DrawUIRect(ParentPosition, Renderer);
	};

#endif
};

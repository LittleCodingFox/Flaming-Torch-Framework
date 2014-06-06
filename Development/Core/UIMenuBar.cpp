#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIMenuBar::UIMenuBar(UIManager *Manager) : UIPanel("UIMenuBar", Manager)
	{
		OnConstructed();
		OnClick.Connect(this, &UIMenuBar::OnItemClick);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIMenuBar::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

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

		SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
	};

	void UIMenuBar::OnItemSelected(UIMenu::Item *Item)
	{
		OnMenuItemSelected(Item->Caption);
	};

	void UIMenuBar::OnItemClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != sf::Mouse::Left)
			return;

		Vector2 ActualPosition = PositionValue + GetParentPosition();
		Vector2 CurrentPosition;
		AxisAlignedBoundingBox AABB;
		bool DrewSelector = false;
		const Vector2 &MousePosition = RendererManager::Instance.Input.MousePosition;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 TextSize = RenderTextUtils::MeasureTextSimple(Items[i].Caption, TextParameters).ToFullSize();

			AABB.min = ActualPosition + CurrentPosition + Vector2(Padding.x, 0);
			AABB.max = AABB.min + Vector2(TextSize.x, 22);

			if(AABB.IsInside(MousePosition) && Items[i].SubItems.size())
			{
				UIMenu *Menu = Manager->CreateMenu(Vector2(AABB.min.x, AABB.max.y));

				for(uint32 j = 0; j < Items[i].SubItems.size(); j++)
				{
					Menu->AddItem(Items[i].SubItems[j]);
				};

				Menu->OnItemSelected.Connect(this, &UIMenuBar::OnItemSelected);
			};

			CurrentPosition.x += TextSize.x + 5;
		};
	};

	void UIMenuBar::PerformLayout()
	{
		SizeValue = Vector2((f32)Manager->GetOwner()->Size().x, 25);
	};

	void UIMenuBar::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();
	};

	void UIMenuBar::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x || ActualPosition.y + SizeValue.y < 0 ||
			ActualPosition.y > Renderer->Size().y))
			return;

		Renderer->BindTexture(NULL);
		Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
		Renderer->DisableState(GL_NORMAL_ARRAY);
		Renderer->EnableState(GL_VERTEX_ARRAY);

		Vector2 Vertices[6] = {
			Vector2(),
			Vector2(0, SizeValue.y),
			SizeValue,
			SizeValue,
			Vector2(SizeValue.x, 0),
			Vector2()
		};

		glColor4f(0, 0, 0, 1);

		glVertexPointer(2, GL_FLOAT, 0, Vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glColor4f(1, 1, 1, 1);

		Vector2 CurrentPosition;

		AxisAlignedBoundingBox AABB;
		bool DrewSelector = false;
		const Vector2 &MousePosition = RendererManager::Instance.Input.MousePosition;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 TextSize = RenderTextUtils::MeasureTextSimple(Items[i].Caption, TextParameters).ToFullSize();

			AABB.min = ActualPosition + CurrentPosition + Vector2(Padding.x, 0);
			AABB.max = AABB.min + Vector2(TextSize.x, 22);

			if(!DrewSelector && AABB.IsInside(MousePosition))
			{
				DrewSelector = true;

				Sprite TheSprite;
				TheSprite.SpriteTexture = SelectorBackgroundTexture;
				TheSprite.Options.Position(AABB.min.ToVector2()).NinePatch(true, TextureRect).Scale(Vector2(TextSize.x, 22.f + SelectorPadding))
					.Color(Vector4(1, 1, 1, GetParentAlpha()));

				TheSprite.Draw(Renderer);
			};

			RenderTextUtils::RenderText(Renderer, Items[i].Caption, TextParams(TextParameters)
				.Position(AABB.min.ToVector2() + Vector2(0, (22 - 12) / 2.f)).Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
				.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha())));

			CurrentPosition.x += TextSize.x + 5;
		};

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

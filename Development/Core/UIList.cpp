#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UIList::UIList(UIManager *Manager) : UIPanel("UIList", Manager)
	{
		OnConstructed();
		OnClick.Connect(this, &UIList::OnItemClickCheck);
		TextParameters = TextParameters.Font(Manager->GetDefaultFont()).Color(Manager->GetDefaultFontColor())
			.SecondaryColor(Manager->GetDefaultSecondaryFontColor()).FontSize(Manager->GetDefaultFontSize());
	};

	void UIList::OnSkinChange()
	{
		TextParameters.FontSizeValue = GetManager()->GetDefaultFontSize();

		std::string TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		SelectorBackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		SelectorBackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);
	};

	void UIList::PerformLayout()
	{
		f32 Height = 0;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 Size = RenderTextUtils::MeasureTextSimple(Items[i], TextParameters).ToFullSize();

			f32 SizeY = (Size.y < TextParameters.FontSizeValue * 1.15f ? TextParameters.FontSizeValue * 1.15f : Size.y);

			Height += SizeY + 2;
		};

		if(SizeValue.y < Height)
			SizeValue.y = Height;
	};

	void UIList::OnItemClickCheck(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != sf::Mouse::Left)
			return;

		Vector2 ActualPosition = GetParentPosition() + PositionValue;
		AxisAlignedBoundingBox AABB;
		f32 Height = 0;
		UIScrollableFrame *Parent = ParentValue.AsDerived<UIScrollableFrame>();

		AABB.min = ActualPosition;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 Size = RenderTextUtils::MeasureTextSimple(Items[i], TextParameters).ToFullSize();

			f32 SizeY = (Size.y < TextParameters.FontSizeValue * 1.15f ? TextParameters.FontSizeValue * 1.15f : Size.y);

			AABB.min.y = ActualPosition.y + Height;
			AABB.max = ActualPosition + Vector2(ParentValue->GetSize().x - (Parent->VerticalScroll->IsVisible() ? SCROLLBAR_DRAGGABLE_SIZE : 0), Height + SizeY);

			Height += SizeY + 2;

			if(AABB.IsInside(RendererManager::Instance.Input.MousePosition))
			{
				OnItemClick(this, i);

				return;
			};
		};
	};

	void UIList::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		Vector2 ActualPosition = ParentPosition + PositionValue;
		AxisAlignedBoundingBox AABB;
		f32 Height = 0;
		UIScrollableFrame *Parent = ParentValue.AsDerived<UIScrollableFrame>();

		AABB.min = ActualPosition;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 Size = RenderTextUtils::MeasureTextSimple(Items[i], TextParameters).ToFullSize();

			f32 SizeY = (Size.y < TextParameters.FontSizeValue * 1.15f ? TextParameters.FontSizeValue * 1.15f : Size.y);

			AABB.min.y = ActualPosition.y + Height;
			AABB.max = ActualPosition + Vector2(SizeValue.x, Height + SizeY);

			Height += SizeY + 2;

			if(AABB.IsInside(RendererManager::Instance.Input.MousePosition))
			{
				OnItemMouseOver(this, i);

				return;
			};
		};
	};

	void UIList::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		if(!ParentValue)
			return;

		static AxisAlignedBoundingBox AABB;
		UIScrollableFrame *Parent = ParentValue.AsDerived<UIScrollableFrame>();

		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		f32 Height = 0;

		AABB.min = ActualPosition;

		for(uint32 i = 0; i < Items.size(); i++)
		{
			Vector2 Size = RenderTextUtils::MeasureTextSimple(Items[i], TextParameters).ToFullSize();

			f32 SizeY = (Size.y < TextParameters.FontSizeValue * 1.15f ? TextParameters.FontSizeValue * 1.15f : Size.y);

			AABB.min.y = ActualPosition.y + Height;
			AABB.max = ActualPosition + Vector2(SizeValue.x, Height + SizeY);

			Height += SizeY + 2;

			if(AABB.IsInside(RendererManager::Instance.Input.MousePosition))
			{
				Sprite TheSprite;
				TheSprite.SpriteTexture = SelectorBackgroundTexture;
				TheSprite.Options.Position(AABB.min.ToVector2()).Scale(Vector2(SizeValue.x, AABB.max.y - AABB.min.y) / SelectorBackgroundTexture->Size())
					.Color(Vector4(1, 1, 1, GetParentAlpha()));

				TheSprite.Draw(Renderer);
			};

			RenderTextUtils::RenderText(Renderer, Items[i], TextParams(TextParameters).Color(TextParameters.TextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
				.SecondaryColor(TextParameters.SecondaryTextColorValue * Vector4(1, 1, 1, GetParentAlpha()))
				.Position(Vector2(AABB.min.x, AABB.min.y)));
		};

		glColor4f(1, 1, 1, 1);

		Vector3 Final = AABB.max - ActualPosition;

		SizeValue = Vector2(Final.x, Final.y);

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

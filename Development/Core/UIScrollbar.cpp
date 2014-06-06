#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIScrollbar::OnSkinChange()
	{
		std::string TexturePath = Skin->GetString("Scrollbar", "BackgroundTexture");
		std::string TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		std::string TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		BackgroundTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		BackgroundTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		std::string NinePatchRectValue = Skin->GetString("Scrollbar", "BackgroundTextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &BackgroundTextureRect.Left, &BackgroundTextureRect.Right,
			&BackgroundTextureRect.Top, &BackgroundTextureRect.Bottom);

		TexturePath = Skin->GetString("Scrollbar", "HandleTexture");
		TextureDirectory = TexturePath.substr(0, TexturePath.rfind('/'));
		TextureName = TexturePath.substr(TexturePath.rfind('/') + 1);

		if(TextureDirectory.length() == 0)
			TextureDirectory = "/";

		HandleTexture = ResourceManager::Instance.GetTextureFromPackage(TextureDirectory, TextureName);

		HandleTexture->SetTextureFiltering(DEFAULT_GUI_TEXTURE_FILTERING);

		NinePatchRectValue = Skin->GetString("Scrollbar", "HandleTextureRect");

		sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &HandleTextureRect.Left, &HandleTextureRect.Right,
			&HandleTextureRect.Top, &HandleTextureRect.Bottom);

		std::string PaddingValue = Skin->GetString("Scrollbar", "Padding");

		sscanf(PaddingValue.c_str(), "%u", &Padding);

		std::string MinSizeValue = Skin->GetString("Scrollbar", "MinSize");

		sscanf(MinSizeValue.c_str(), "%u", &MinSize);

		SelectBoxExtraSize = Vector2(BackgroundTextureRect.Left + BackgroundTextureRect.Right, BackgroundTextureRect.Top + BackgroundTextureRect.Bottom);
	};

	void UIScrollbar::PerformLayout()
	{
	};

	void UIScrollbar::Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		if(this == Manager->GetFocusedElement().Get() && RendererManager::Instance.Input.MouseButtons[sf::Mouse::Left].Pressed)
		{
			Vector2 ActualPosition = ParentPosition + PositionValue;

			uint32 Steps = (MaxValue - MinValue) / ValueStep;

			AxisAlignedBoundingBox AABB;

			for(uint32 i = 0; i <= Steps; i++)
			{
				f32 StepOffset = (Vertical ? (SizeValue.y + SelectBoxExtraSize.y) / Steps * i : (SizeValue.x + SelectBoxExtraSize.x) / Steps * i);

				AABB.min = ActualPosition + (Vertical ? Vector2((f32)Padding, StepOffset + Padding - SelectBoxExtraSize.y / 2) : 
					Vector2(StepOffset + Padding - SelectBoxExtraSize.x / 2, (f32)Padding)) - SelectBoxExtraSize / 2;
				AABB.max = ActualPosition + (Vertical ? Vector2(SizeValue.x - Padding * 2 + BackgroundTextureRect.Left + BackgroundTextureRect.Right,
					StepOffset + Padding - SelectBoxExtraSize.y / 2 + (f32)MinSize) :
					Vector2(StepOffset + Padding - SelectBoxExtraSize.x / 2 + (f32)MinSize,
					SizeValue.y - Padding * 2 + BackgroundTextureRect.Top + BackgroundTextureRect.Bottom)) + SelectBoxExtraSize / 2;

				if(AABB.IsInside(RendererManager::Instance.Input.MousePosition))
				{
					bool NeedsChange = CurrentStep != i;

					CurrentStep = i;

					if(NeedsChange)
						OnChange(this);

					return;
				};
			};
		};
	};

	void UIScrollbar::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		uint32 Steps = (MaxValue - MinValue) / ValueStep;
		f32 StepOffset = Vertical ? (SizeValue.y + SelectBoxExtraSize.y) / Steps * CurrentStep : (SizeValue.x + SelectBoxExtraSize.x) / Steps * CurrentStep;

		Sprite TheSprite;
		TheSprite.SpriteTexture = BackgroundTexture;
		TheSprite.Options.Position(ActualPosition).NinePatch(true, BackgroundTextureRect).Scale(SizeValue).Color(Vector4(1, 1, 1, GetParentAlpha()));
		TheSprite.Draw(Renderer);

		TheSprite.SpriteTexture = HandleTexture;

		TheSprite.Options.Position(ActualPosition + (Vertical ? Vector2((f32)Padding, StepOffset + Padding) - BackgroundTextureRect.Position() :
			Vector2(StepOffset + Padding, (f32)Padding) - BackgroundTextureRect.Position())).NinePatch(true, HandleTextureRect)
			.Scale(Vertical ? Vector2(SizeValue.x - Padding * 2 + BackgroundTextureRect.Left + BackgroundTextureRect.Right, (f32)MinSize) :
			Vector2((f32)MinSize, SizeValue.y - Padding * 2 + BackgroundTextureRect.Top + BackgroundTextureRect.Bottom));

		TheSprite.Draw(Renderer);

		DrawUIRect(ParentPosition, Renderer);
	};
#endif
};

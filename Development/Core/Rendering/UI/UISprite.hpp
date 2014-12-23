#pragma once
#if USE_GRAPHICS

/*!
*	UI Sprite Element
*/
class UISprite : public UIElement
{
public:
	/*!
	*	The Element's Sprite
	*/
	Sprite TheSprite;

	UISprite(UIManager *Manager);
	virtual const Vector2 &Size() const override;
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
};
#endif

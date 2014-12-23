#pragma once
#if USE_GRAPHICS

namespace UIGroupLayoutMode
{
	enum UIGroupLayoutMode
	{
		None = 0,
		Horizontal = FLAGVALUE(1),
		Vertical = FLAGVALUE(2),
		AdjustWidth = FLAGVALUE(3),
		AdjustHeight = FLAGVALUE(4),
		Center = FLAGVALUE(5),
		VerticalCenter = FLAGVALUE(6),
		AdjustCloser = FLAGVALUE(7)
	};
};

/*!
*	Generic Group Container which does nothing more than perform layout and update/draw children
*/
class UIGroup : public UIElement
{
public:
	uint32 LayoutMode;

	UIGroup(UIManager *Manager);
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
};
#endif

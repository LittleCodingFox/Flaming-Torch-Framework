#pragma once
#if USE_GRAPHICS

/*!
*	Generic Vertical Group Container which does nothing more than perform layout and update/draw children
*/
class UIVerticalGroup : public UIElement
{
public:
	UIVerticalGroup(UIManager *Manager);
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
	virtual void ReportResourceUsage() override {}
};
#endif

#pragma once
#if USE_GRAPHICS

/*!
*	UI Button Element
*/
class UIButton : public UISprite
{
protected:
	DisposablePointer<Texture> Normal, Down, Over;
	std::string Caption;
	bool MouseDown, MouseOver;
	uint32 FontSize;
	Vector4 FontColor;
	Vector2 LabelOffset;

	virtual void PrivOnEvent(uint32 EventType, std::vector<void *> Arguments);
public:

	UIButton(UIManager *Manager);

	virtual void SetSkin(DisposablePointer<GenericConfig> Skin) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
	virtual void SetCaption(const std::string &Caption);
	virtual const std::string &GetCaption() const;
};
#endif

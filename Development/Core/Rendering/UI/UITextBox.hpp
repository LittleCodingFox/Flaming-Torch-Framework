#pragma once
#if USE_GRAPHICS

class UITextBox : public UISprite
{
protected:
	bool MultiLine;
	uint32 FontSize;
	DisposablePointer<Texture> Background, BackgroundMultiline;

	uint32 CursorPosition, Padding;

	virtual void PrivOnEvent(uint32 EventType, std::vector<void *> Arguments);

public:
	std::string Text;

	UITextBox(UIManager *Manager);

	virtual const Vector2 &Size() const override;
	virtual void SetSkin(DisposablePointer<GenericConfig> Skin) override;
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
};
#endif

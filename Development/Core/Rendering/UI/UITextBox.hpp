#pragma once
#if USE_GRAPHICS

class UITextBox : public UISprite
{
protected:
	bool MultiLine;
	uint32 FontSize;
	DisposablePointer<Texture> Background, BackgroundMultiline;

	uint32 CursorPosition, TextOffset, Padding;
	bool IsPasswordValue;

	std::string Text;

	Vector2 LastSizeValue;

	virtual void PrivOnEvent(uint32 EventType, std::vector<void *> Arguments);

public:

	UITextBox(UIManager *Manager);

	virtual const Vector2 &Size() const override;
	virtual void SetSkin(DisposablePointer<GenericConfig> Skin) override;
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;

	/*!
	*	\return whether this textbox is a password
	*/
	bool IsPassword() const
	{
		return IsPasswordValue;
	}

	/*!
	*	Sets whether this textbox is a password
	*	\param Value whether this textbox is a password
	*/
	void SetPassword(bool Value)
	{
		IsPasswordValue = Value;
	}

	/*!
	*	\return the Text of this Text Box
	*/
	const std::string &GetText() const
	{
		return Text;
	}

	/*!
	*	Sets the Text of this Text Box
	*	\param Text the new Text
	*/
	void SetText(const std::string &Text)
	{
		this->Text = Text;
		CursorPosition = TextOffset = 0;
	}
};
#endif

#pragma once
#if USE_GRAPHICS

/*!
*	UI Text Element Alignment
*/
namespace UITextAlignment
{
	enum UITextAlignment
	{
		Left = FLAGVALUE(0), //!<Align Left
		Right = FLAGVALUE(1), //!<Align Right
		Center = FLAGVALUE(2), //!<Align Center
		VCenter = FLAGVALUE(3), //!<Align Vertical Center
		Bottom = FLAGVALUE(4) //!<Align Bottom
	};
}

/*!
*	UI Text Element
*/
class UIText : public UIElement
{
public:
	struct StringInfo
	{
		std::string TheString;
		Rect Size;
	};

private:
	std::vector<StringInfo> Strings;
	std::string TextValue;
public:

	/*!
	*	Text Formatting Params
	*	\note Position is ignored
	*/
	TextParams TextParameters;

	/*!
	*	Text Alignment
	*/
	uint32 TextAlignment;

	UIText(UIManager *Manager);

	/*!
	*	Sets the Text of this UIText
	*	\param String the new Text
	*	\note Required set so we recalculate the strings
	*/
	void SetText(const std::string &String);

	/*!
	*	\return the Text of this UIText
	*/
	const std::string &Text() const;

	/*!
	*	\return the Text's size
	*/
	Vector2 TextSize() const;

	/*!
	*	\return the formatted text strings
	*/
	const std::vector<StringInfo> GetStrings() const
	{
		return Strings;
	}

	virtual void SetSkin(DisposablePointer<GenericConfig> Skin) override;
	virtual void Update(const Vector2 &ParentPosition) override;
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer) override;
};
#endif

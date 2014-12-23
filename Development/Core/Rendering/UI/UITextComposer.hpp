#pragma once

/*!
*	UI Text Composer Element
*	\note Will not autosize
*/
class UITextComposer : public UIElement
{
protected:
	class TextComposerNode
	{
	public:
		std::string Text;
		TextParams Params;
		DisposablePointer<UIText> Instance;
	};

	std::vector<TextComposerNode> Nodes;
	Vector2 LastPosition;
	uint32 LineHeight;
	bool IgnoreHeightBoundsValue;
public:
	UITextComposer(UIManager *Manager);

	/*!
	*	Clears all text in this composer
	*/
	void ClearText();

	/*!
	*	\return the write position for new text
	*/
	const Vector2 &GetWritePosition() const;

	/*!
	*	Sets the write position for new text
	*	\param WritePosition the new write position
	*/
	void SetWritePosition(const Vector2 &WritePosition);

	/*!
	*	Adds a text element to this composer
	*	\param Text the text to use
	*	\param Params the text parameters to use
	*/
	void AddText(const std::string &Text, const TextParams &Params);

	/*!
	*	\return Whether we're ignoring the height bounds and thus can overflow text elements
	*/
	bool IgnoreHeightBounds() const;

	/*!
	*	Enable/Disable height bound ignore so we can overflow text elements if necessary
	*	\param Value whether to ignore height bounds
	*/
	void SetIgnoreHeightBounds(bool Value);

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

#pragma once

#define DEFAULT_GUI_TEXTURE_FILTERING TextureFiltering::Linear

class UIManager;
class UILayout;
class RendererManager;
class RendererManager::Renderer;

class UIPanel;

/*!
*	UI Animation Interface
*/
class FLAMING_API UIAnimation
{
public:
	uint64 StartTime;
	uint64 Duration;
	UIPanel *Target;

	UIAnimation(UIPanel *_Target) : Target(_Target), StartTime(0), Duration(0) {};
	virtual ~UIAnimation() {};

	/*!
	*	Update Function
	*	Do your changes to the element here
	*	\return whether we're done with this animation
	*/
	virtual bool Update() { return true; };

	//Called when we want to finish the animation right now
	virtual void Finish() {};
};

/*!
*	UI Panel class
*/
class FLAMING_API UIPanel 
{
	friend class UIManager;
	friend class UIScrollableFrame;
	friend class UILayout;
protected:
	bool VisibleValue, EnabledValue, MouseInputValue, KeyboardInputValue;
	Vector2 PositionValue, SizeValue, TranslationValue, SelectBoxExtraSize, OffsetValue;
	f32 AlphaValue;
	UILayout *Layout;
	UIManager *Manager;
	bool RespondsToTooltipsValue, TooltipFixed;
	sf::String TooltipValue;
	SuperSmartPointer<UIPanel> ParentValue, TooltipElement;
	//!<Children of this UI Panel
	std::vector<UIPanel *> Children;
	uint64 ClickTimer;
	//!<Whether we are in the middle of a click
	bool ClickPressed;
	//!<Whether we are blocking Input
	bool BlockingInput;
	StringID ID;
	SuperSmartPointer<GenericConfig> Skin;
	bool DraggingValue, IsDraggableValue, IsDroppableValue;
	Vector2 TooltipPosition;

	std::list<SuperSmartPointer<UIAnimation> > AnimationQueue;

	luabind::object OnMouseJustPressedFunction, OnMousePressedFunction, OnMouseReleasedFunction,
		OnKeyJustPressedFunction, OnKeyPressedFunction, OnKeyReleasedFunction, OnMouseMovedFunction,
		OnCharacterEnteredFunction, OnGainFocusFunction, OnLoseFocusFunction, OnUpdateFunction, OnDrawFunction,
		OnMouseEnteredFunction, OnMouseOverFunction, OnMouseLeftFunction, OnClickFunction, OnDragBeginFunction, OnDragEndFunction,
		OnDraggingFunction, OnDropFunction, PositionFunction, SizeFunction, OnStartFunction;

	/*!
	*	Called by the UI Manager when the UI Skin is changed
	*/
	virtual void OnSkinChange() {};

	void OnMouseJustPressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnMousePressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnKeyJustPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o);
	void OnKeyPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o);
	void OnKeyReleasedScript(UIPanel *Self, const InputCenter::KeyInfo &o);
	void OnMouseMovedScript(UIPanel *Self);
	void OnCharacterEnteredScript(UIPanel *Self);
	void OnGainFocusScript(UIPanel *Self);
	void OnLoseFocusScript(UIPanel *Self);
	void OnMouseEnteredScript(UIPanel *Self);
	void OnMouseOverScript(UIPanel *Self);
	void OnMouseLeftScript(UIPanel *Self);
	void OnClickScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnDragBeginScript(UIPanel *Self);
	void OnDragEndScript(UIPanel *Self);
	void OnDraggingScript(UIPanel *Self);
	void OnDropScript(UIPanel *Self, UIPanel *Target);
	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseMovePriv();
	void OnKeyPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o);
	void OnCharacterEnteredPriv();
	void OnLoseFocusPriv();
	void OnGainFocusPriv();
	void OnMouseJustPressedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o);

	/*!
	*	\note Should be called whenever the UI object is created
	*/
	void OnConstructed();
public:
	std::string Name, LayoutName, NativeType;

	Signal1<UIPanel *> OnLoseFocus, OnGainFocus, OnCharacterEntered, OnMouseMove, OnMouseEntered, OnMouseOver, OnMouseLeft;
	Signal2<UIPanel *, const InputCenter::MouseButtonInfo &> OnMouseJustPressed, OnMousePressed, OnMouseReleased, OnClick;
	Signal2<UIPanel *, const InputCenter::KeyInfo &> OnKeyJustPressed, OnKeyPressed, OnKeyReleased;
	Signal1<UIPanel *> OnDragBegin, OnDragging, OnDragEnd;
	//[Self, Target]
	Signal2<UIPanel *, UIPanel *> OnDrop;

	UIPanel(const std::string &NativeTypeName, UIManager *_Manager);

	virtual ~UIPanel();

	/*!
	*	Updates this element based on the parent position
	*	\param ParentPosition the parent's position
	*	\note Should iterate on all children
	*/
	virtual void Update(const Vector2 &ParentPosition);

	/*!
	*	Draws this element based on the parent position
	*	\param ParentPosition the parent's position
	*	\param Renderer the Renderer to draw to
	*	\note Should iterate on all children
	*/
	virtual void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	Draws the UI rect for this element
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element
	*/
	void DrawUIRect(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	\return Whether this element respondes to Tooltips
	*/
	bool RespondsToTooltips() { return RespondsToTooltipsValue && (TooltipValue.getSize() || TooltipElement.Get()); };

	/*!
	*	Clears the current animations from this panel
	*	\return this panel
	*/
	UIPanel *ClearAnimations();
	
	/*!
	*	Fade In Animation
	*/
	UIPanel *FadeIn(uint64 Duration);
	
	/*!
	*	Fade Out Animation
	*/
	UIPanel *FadeOut(uint64 Duration);

	/*!
	*	Sets whether this element has tooltips
	*	\param Value whether it does respond to tooltips
	*/
	void SetRespondsToTooltips(bool Value)
	{
		RespondsToTooltipsValue = Value;
	};

	/*!
	*	\return Whether this element's tooltips are fixed (don't move with the mouse)
	*/
	bool TooltipsFixed()
	{
		return TooltipFixed;
	};

	/*!
	*	Sets whether this element's tooltips are fixed (don't move with the mouse)
	*	\param Value whether the tooltips are fixed
	*/
	void SetTooltipsFixed(bool Value)
	{
		TooltipFixed = Value;
	};

	/*!
	*	\return this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	const Vector2 &TooltipsPosition()
	{
		return TooltipPosition;
	};

	/*!
	*	\return this element's offset
	*/
	const Vector2 &GetOffset()
	{
		return OffsetValue;
	};

	/*!
	*	Sets this element's position offset
	*	\param Offset the offset to set
	*/
	void SetOffset(const Vector2 &Offset)
	{
		OffsetValue = Offset;
	};

	/*!
	*	Sets this element's tooltips position
	*	\param Position this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	void SetTooltipsPosition(const Vector2 &Position)
	{
		TooltipPosition = Position;
	};

	/*!
	*	Perform auto calculation of sizes here
	*/
	virtual void PerformLayout()
	{
		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->PerformLayout();
		};
	};

	/*!
	*	\return the Tooltip Text to display
	*/
	const sf::String &GetTooltipText()
	{
		return TooltipValue;
	};

	/*!
	*	\return the Tooltip Element to display
	*/
	SuperSmartPointer<UIPanel> GetTooltipElement()
	{
		return TooltipElement;
	};

	/*
	*	\return whether this panel is blocking input
	*/
	bool IsBlockingInput()
	{
		return BlockingInput;
	};

	/*
	*	Sets whether this panel is blocking input
	*	\param Value whether to block input
	*/
	void SetBlockingInput(bool Value)
	{
		BlockingInput = Value;
	};

	/*!
	*	Sets the tooltip text for this element
	*	\param Text the tooltip text
	*/
	void SetTooltipText(const sf::String &Text)
	{
		TooltipValue = Text;
	};

	/*!
	*	Sets the tooltip element for this element
	*	\param Element the tooltip element
	*/
	void SetTooltipElement(SuperSmartPointer<UIPanel> Element)
	{
		TooltipElement = Element;
	};

	/*!
	*	Sets whether we can drag this element to other elements
	*	\param Value whether we can drag this element
	*/
	void SetDraggable(bool Value)
	{
		IsDraggableValue = Value;
	};

	/*!
	*	\return whether we can drag this element
	*/
	bool IsDraggable()
	{
		return IsDraggableValue;
	};

	/*!
	*	Sets whether we can drop this element on other elements
	*	\param Value whether we can drop this element
	*/
	void SetDroppable(bool Value)
	{
		IsDroppableValue = Value;
	};

	/*!
	*	\return whether we can drop this element
	*/
	bool IsDroppable()
	{
		return IsDroppableValue;
	};

	/*!
	*	\return the UIManager associated to this UI Element
	*	\sa UIManager
	*/
	UIManager *GetManager()
	{
		return Manager;
	};

	/*!
	*	\return the ID of this UI Element
	*/
	StringID GetID()
	{
		return ID;
	};

	/*!
	*	Calculates the position of this element and all its parents to return the overall (absolute) position
	*	\return the calculated absolute position of this UI Element
	*/
	Vector2 GetParentPosition()
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position;
		UIPanel *p = ParentValue;

		while(p)
		{
			Position += p->GetPosition() + p->GetTranslation() + p->GetOffset();

			p = p->GetParent();
		};

		return Position;
	};

	/*!
	*	Set this Element's Visiblity
	*	\param value whether the element should be visible
	*/
	void SetVisible(bool value)
	{
		VisibleValue = value;
	};

	/*!
	*	\return whether this element is visible
	*/
	bool IsVisible() const
	{
		return VisibleValue;
	};

	/*!
	*	Sets this element's Enabled status
	*	\param value whether this element is enabled
	*/
	void SetEnabled(bool value)
	{
		EnabledValue = value;
	};

	/*!
	*	\return whether this element is enabled
	*/
	bool IsEnabled() const
	{
		return EnabledValue;
	};

	/*!
	*	Sets whether this element receives Mouse Input
	*	\param value whether this element receives Mouse Input
	*/
	void SetMouseInputEnabled(bool value)
	{
		MouseInputValue = value;
	};

	/*!
	*	\return whether this element receives Mouse Input
	*/
	bool IsMouseInputEnabled() const
	{
		return MouseInputValue;
	};

	/*!
	*	Sets whether this element receives Keyboard Input
	*	\param value whether this element receives Keyboard Input
	*/
	void SetKeyboardInputEnabled(bool value)
	{
		KeyboardInputValue = value;
	};

	/*!
	*	\return whether this element receivews Keyboard Input
	*/
	bool IsKeyboardInputEnabled() const
	{
		return KeyboardInputValue;
	};

	/*!
	*	Adds a child to this element
	*	\param Child the Child to add
	*/
	void AddChild(UIPanel *Child);

	/*!
	*	Removes a children from this element
	*	\param Child the Child to remove
	*/
	void RemoveChild(UIPanel *Child)
	{
		for(std::vector<UIPanel *>::iterator it = Children.begin(); it != Children.end(); it++)
		{
			if(*it == Child)
			{
				Child->ParentValue = SuperSmartPointer<UIPanel>();
				Children.erase(it);

				return;
			};
		};
	};

	/*!
	*	\return the amount of children in this element
	*/
	uint32 GetChildrenCount() const
	{
		return Children.size();
	};

	/*!
	*	Gets a children at a specific index
	*	\param Index the index of the children
	*	\return the children or NULL
	*/
	UIPanel *GetChild(uint32 Index)
	{
		return Children.size() > Index ? Children[Index] : NULL;
	};

	/*!
	*	\return the Translation of this element
	*	\note Used by elements such as Scrollbars to indicate how much translation there is
	*/
	const Vector2 &GetTranslation() const
	{
		return TranslationValue;
	};

	/*!
	*	\return the Parent of this Element (or NULL)
	*/
	UIPanel *GetParent() const
	{
		return (UIPanel *)ParentValue.Get();
	};

	/*!
	*	\return the Position of this Element
	*/
	const Vector2 &GetPosition() const
	{
		return PositionValue;
	};

	/*!
	*	Sets the Position of this element
	*	\param Position the new position for this element
	*/
	void SetPosition(const Vector2 &Position)
	{
		PositionValue = Position;
	};

	/*!
	*	\return the Size of this Element
	*/
	const Vector2 &GetSize() const
	{
		return SizeValue;
	};

	/*!
	*	Sets the Size of this element
	*	\param Size the new size for this element
	*/
	void SetSize(const Vector2 &Size)
	{
		SizeValue = Size;

		if(SizeValue.x < 0)
			SizeValue.x = 0;

		if(SizeValue.y < 0)
			SizeValue.y = 0;
	};

	/*!
	*	\return the Alpha Transparency of this Element
	*/
	f32 GetAlpha() const
	{
		return AlphaValue;
	};


	/*!
	*	\return the Alpha Transparency of this element and all its parents
	*/
	f32 GetParentAlpha();

	/*!
	*	Sets the Alpha Transparency of this Element
	*	\param Alpha the new alpha for this element
	*/
	void SetAlpha(f32 Alpha)
	{
		AlphaValue = Alpha;
	};

	/*!
	*	\return the Size of all children in this element
	*/
	Vector2 GetChildrenSize()
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->GetPosition() + Children[i]->GetOffset() + Children[i]->GetSize() + Children[i]->GetExtraSize() / 2;

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		};

		return Out;
	};

	/*!
	*	Clears all children (and destroys them)
	*/
	void Clear();

	/*!
	*	Sets the Skin of this Element
	*	\param Skin the new skin to apply
	*/
	void SetSkin(SuperSmartPointer<GenericConfig> Skin)
	{
		this->Skin = Skin;
		OnSkinChange();
	};

	/*!
	*	Gets the layout this Panel belongs to
	*	\return the Layout associated with this Panel, or NULL
	*/
	UILayout *GetLayout()
	{
		return Layout;
	};

	/*!
	*	Gets the extra size for the element
	*	Extra size is based on the Nine-Patch corner sizes
	*/
	const Vector2 &GetExtraSize()
	{
		return SelectBoxExtraSize;
	};

	/*!
	*	Focuses this element
	*/
	void Focus();
};

namespace UIGroupLayoutMode
{
	enum
	{
		None = 0,
		Horizontal = FLAGVALUE(1),
		Vertical = FLAGVALUE(2),
		AdjustWidth = FLAGVALUE(3),
		AdjustHeight = FLAGVALUE(4),
		Center = FLAGVALUE(5),
		VerticalCenter = FLAGVALUE(6),
		AdjustCloser = FLAGVALUE(7),
		InvertDirection = FLAGVALUE(8),
		InvertX = FLAGVALUE(9),
		InvertY = FLAGVALUE(10)
	};
};

/*!
*	Generic Group Container which does nothing more than perform layout and update/draw children
*/
class FLAMING_API UIGroup : public UIPanel
{
public:
	uint32 LayoutMode;

	UIGroup(UIManager *Manager) : UIPanel("UIGroup", Manager), LayoutMode(UIGroupLayoutMode::None)
	{
		OnConstructed();
	};

	void PerformLayout();

	void Update(const Vector2 &ParentPosition)
	{
		UIPanel::Update(ParentPosition);

		PerformLayout();

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ParentPosition + PositionValue + OffsetValue);
		};
	};

	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->Size().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->Size().y))
			return;

		UIPanel::Draw(ParentPosition, Renderer);

		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->Draw(ActualPosition, Renderer);
		};

		DrawUIRect(ParentPosition, Renderer);
	};
};

/*!
*	UI Frame Element
*/
class FLAMING_API UIFrame : public UIPanel
{
protected:
	SuperSmartPointer<Texture> BackgroundTexture;
	Rect TextureRect;

	void OnSkinChange();
public:
	UIFrame(UIManager *Manager) : UIPanel("UIFrame", Manager)
	{
		OnConstructed();
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Button Element
*	\note Size can be 0, since it'll resize itself on draw
*/
class FLAMING_API UIButton : public UIPanel
{
protected:
	SuperSmartPointer<Texture> NormalTexture, FocusedTexture;
	Rect TextureRect;
	Vector2 LabelOffset;

	void OnSkinChange();
public:
	/*!
	*	The Button's Text
	*/
	sf::String Caption;

	TextParams TextParameters;

	UIButton(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Text Box Element
*	\note Size can be 0 on Y, will autoresize that way
*/
class FLAMING_API UITextBox : public UIPanel
{
protected:
	SuperSmartPointer<Texture> BackgroundTexture;
	Rect TextureRect;
	sf::String Text;
	uint32 CursorPosition, TextOffset, Padding;
	std::string FontName;
	bool IsPasswordValue;

	Vector2 LastSizeValue;

	void OnMouseJustPressedTextBox(UIPanel *This, const InputCenter::MouseButtonInfo &o);
	void OnKeyJustPressedTextBox(UIPanel *This, const InputCenter::KeyInfo &o);
	void OnCharacterEnteredTextBox(UIPanel *This);
	void OnSkinChange();
public:
	TextParams TextParameters;

	UITextBox(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	\return whether this textbox is a password
	*/
	bool IsPassword()
	{
		return IsPasswordValue;
	};

	/*!
	*	Sets whether this textbox is a password
	*	\param Value whether this textbox is a password
	*/
	void SetPassword(bool Value)
	{
		IsPasswordValue = Value;
	};

	/*!
	*	\return the Text of this Text Box
	*/
	const sf::String &GetText() const
	{
		return Text;
	};

	/*!
	*	Sets the Text of this Text Box
	*	\param Text the new Text
	*/
	void SetText(const sf::String &Text)
	{
		this->Text = Text;
		CursorPosition = TextOffset = 0;
	};
};

#define SCROLLBAR_DRAGGABLE_SIZE 15

class UIScrollbar;

/*!
*	UI Scrollable Frame Element
*	\note Will not Autoresize
*	\note Includes SCROLLBAR_DRAGGABLE_SIZE Pixels on the Size's Width should scrolling be required
*/
class FLAMING_API UIScrollableFrame : public UIPanel
{
	friend class UIList;
protected:
	SuperSmartPointer<UIScrollbar> VerticalScroll, HorizontalScroll;

	void OnSkinChange();
	void MakeScrolls();
public:
	UIScrollableFrame(UIManager *Manager) : UIPanel("UIScrollableFrame", Manager)
	{
		OnConstructed();
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI List Element
*	\note Will AutoSize outwards vertically
*	\note Must have a ScrollableFrame parent
*/
class FLAMING_API UIList : public UIPanel
{
private:
	SuperSmartPointer<Texture> SelectorBackgroundTexture;
	void OnSkinChange();
	void OnItemClickCheck(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
public:
	/*!
	*	All items contained by this List
	*/
	std::vector<std::string> Items;

	TextParams TextParameters;

	/*!
	*	(UIList Self, uint32 ItemIndex)
	*/
	Signal2<UIList *, uint32> OnItemMouseOver, OnItemClick;

	UIList(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Check Box Element
*/
class FLAMING_API UICheckBox : public UIPanel
{
private:
	SuperSmartPointer<Texture> CheckTexture, UnCheckTexture;
	bool Checked;
	Vector2 LabelOffset;

	void CheckMouseClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnSkinChange();
public:
	/*!
	*	The Checkbox's Caption Text
	*/
	sf::String Caption;

	TextParams TextParameters;

	/*!
	*	Events for when the Checkbox is Checked or Unchecked
	*/
	Signal1<UICheckBox *> OnChecked, OnUnChecked;

	UICheckBox(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	\return Whether the Checkbox has been checked
	*/
	bool IsChecked()
	{
		return Checked;
	};

	/*!
	*	Set whether the Checkbox is checked
	*	\param Value whether the Checkbox is checked
	*/
	void SetChecked(bool Value)
	{
		Checked = Value;

		if(Checked)
		{
			OnChecked(this);
		}
		else
		{
			OnUnChecked(this);
		};
	};
};

/*!
*	UI Menu Element
*/
class FLAMING_API UIMenu : public UIPanel
{
	friend class UIManager;
private:
	SuperSmartPointer<Texture> BackgroundTexture, SelectorBackgroundTexture;
	Vector2 Padding, TextOffset;
	uint32 SelectorPadding, ItemHeight;
	Rect TextureRect;

public:
	/*!
	*	UI Menu Item
	*/
	class Item
	{
	public:
		/*!
		*	The Item's Caption
		*/
		sf::String Caption;
		/*!
		*	The Item's User Data
		*/
		void *UserData;
		/*!
		*	The Item's Index
		*/
		uint32 Index;
	};

private:
	std::vector<Item> Items;

	void OnSkinChange();
	void OnItemClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void CloseSelf(UIPanel *Self);

	UIMenu(UIManager *Manager);
public:
	TextParams TextParameters;

	/*!
	*	Adds an Item
	*	\param Caption the Item's Caption
	*	\param UserData the Item's User Data (Optional)
	*/
	void AddItem(const sf::String &Caption, void *UserData = NULL)
	{
		Item Out;
		Out.Caption = Caption;
		Out.UserData = UserData;
		Out.Index = Items.size();

		Items.push_back(Out);
	};

	/*!
	*	On Item Selected Event
	*/
	Signal1<Item *> OnItemSelected;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Dropdown Element
*/
class FLAMING_API UIDropdown : public UIPanel
{
private:
	SuperSmartPointer<Texture> BackgroundTexture, DropdownTexture;
	Vector2 TextOffset, DropdownOffset;
	f32 DropdownHeight;
	Rect TextureRect;

	void OnSkinChange();
	void OnItemClickCheck(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void SetSelectedItem(UIMenu::Item *Item);
public:
	/*!
	*	Selected Index for this Dropdown
	*/
	int32 SelectedIndex;

	/*!
	*	All items contained by this Dropdown
	*/
	std::vector<std::string> Items;

	TextParams TextParameters;

	/*!
	*	(UIDropdown Self)
	*/
	Signal1<UIDropdown *> OnItemClick;

	UIDropdown(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Menu Bar Element
*/
class FLAMING_API UIMenuBar : public UIPanel
{
	friend class UIManager;
public:
	/*!
	*	UI Menu Bar Item
	*/
	class Item
	{
	public:
		/*!
		*	The Item's Caption Text
		*/
		sf::String Caption;
		/*!
		*	The Item's User Data
		*/
		void *UserData;

		/*!
		*	The Item's Sub Items
		*/
		std::vector<sf::String> SubItems;
	};
private:
	std::vector<Item> Items;
	SuperSmartPointer<Texture> SelectorBackgroundTexture;
	Vector2 Padding, TextOffset;
	uint32 SelectorPadding;
	Rect TextureRect;

	void OnSkinChange();
	void OnItemClick(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnItemSelected(UIMenu::Item *Item);

	TextParams TextParameters;

	UIMenuBar(UIManager *Manager);
public:
	/*!
	*	On Menu Item Selected Event
	*/
	Signal1<const sf::String &> OnMenuItemSelected;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	Adds an Item to the item list
	*	\param TheItem the Item to add
	*/
	void AddItem(const Item &TheItem)
	{
		Items.push_back(TheItem);
	};
};

/*!
*	UI Sprite Element
*/
class FLAMING_API UISprite : public UIPanel
{
public:
	/*!
	*	The Element's Sprite
	*/
	Sprite TheSprite;

	UISprite(UIManager *Manager) : UIPanel("UISprite", Manager)
	{
		OnConstructed();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Scroll Bar Element
*/
class FLAMING_API UIScrollbar : public UIPanel
{
private:
	Rect BackgroundTextureRect, HandleTextureRect;
	uint32 Padding;
	uint32 MinSize;
	SuperSmartPointer<Texture> BackgroundTexture, HandleTexture;
	bool Vertical;

	void OnSkinChange();
public:
	/*!
	*	Minimum Value
	*/
	uint32 MinValue;
	/*!
	*	Maximum Value
	*/
	uint32 MaxValue;
	/*!
	*	Current Step
	*/
	uint32 CurrentStep;
	/*!
	*	How much to increase per step
	*/
	uint32 ValueStep;

	UIScrollbar(UIManager *Manager, bool IsVertical) : UIPanel("UIScrollbar", Manager), MinValue(0), MaxValue(100),
		CurrentStep(0), ValueStep(1), Vertical(IsVertical)
	{
		OnConstructed();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	\return the current Value
	*/
	uint32 Value()
	{
		if(CurrentStep > MaxValue / ValueStep)
			CurrentStep = MaxValue / ValueStep;

		return MinValue + CurrentStep * ValueStep;
	};

	/*!
	*	On Change Event
	*/
	Signal1<UIPanel *> OnChange;
};

/*!
*	UI Tooltip Element
*/
class FLAMING_API UITooltip : public UIPanel
{
	friend class UIManager;
private:
	//Source Panel that triggered this Tooltip
	SuperSmartPointer<UIPanel> Source;

	UITooltip(UIManager *Manager);
public:
	/*!
	*	Override Text
	*	\note Should have any content to override the Tooltip text
	*/
	sf::String OverrideText;

	TextParams TextParameters;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Text Element Alignment
*/
namespace UITextAlignment
{
	enum
	{
		Left = FLAGVALUE(0), //!<Align Left
		Right = FLAGVALUE(1), //!<Align Right
		Center = FLAGVALUE(2), //!<Align Center
		VCenter = FLAGVALUE(3) //!<Align Vertical Center
	};
};

/*!
*	UI Text Element
*	\note Will not autosize
*/
class FLAMING_API UIText : public UIPanel
{
	std::vector<sf::String> Strings;
	sf::String Text;

protected:
	void OnSkinChange();
public:
	/*!
	*	Text Formatting Params
	*	\note Only colors and border are used, Position is ignored
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
	*	\param AutoExpandHeight whether the Height of this Element should be expanded for the text
	*	\note Required set so we recalculate the strings
	*/
	void SetText(const sf::String &String, bool AutoExpandHeight = false);
	/*!
	*	\return the Text of this UIText
	*/
	const sf::String &GetText();
	/*!
	*	\return the Text's size
	*/
	Vector2 GetTextSize();

	/*!
	*	\return the formatted text strings
	*/
	const std::vector<sf::String> GetStrings()
	{
		return Strings;
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Text Composer Element
*	\note Will not autosize
*/
class FLAMING_API UITextComposer : public UIPanel
{
protected:
	class TextComposerNode
	{
	public:
		sf::String Text;
		TextParams Params;
		SuperSmartPointer<UIText> Instance;
	};

	std::vector<TextComposerNode> Nodes;
	Vector2 LastPosition;
	uint32 LineHeight;
	bool IgnoreHeightBoundsValue;
public:
	UITextComposer(UIManager *Manager) : UIPanel("UITextComposer", Manager), LineHeight(0), IgnoreHeightBoundsValue(false)
	{
		OnConstructed();
	};

	/*!
	*	Clears all text in this composer
	*/
	void ClearText();

	/*!
	*	\return the write position for new text
	*/
	const Vector2 &GetWritePosition();

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
	void AddText(const sf::String &Text, const TextParams &Params);

	/*!
	*	\return Whether we're ignoring the height bounds and thus can overflow text elements
	*/
	bool IgnoreHeightBounds();

	/*!
	*	Enable/Disable height bound ignore so we can overflow text elements if necessary
	*	\param Value whether to ignore height bounds
	*/
	void SetIgnoreHeightBounds(bool Value);

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

class FLAMING_API UISplitPanel : public UIPanel
{
public:
	SuperSmartPointer<UIPanel> Left, Right;
	f32 Percentage;
	bool Horizontal, GrabbingSplitter;
	uint32 SplitSize;

	UISplitPanel(UIManager *Owner);
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
};

/*!
*	UI Layout
*	Contains Elements and can be loaded from a scheme file
*/
class FLAMING_API UILayout 
{
friend class UIManager;
	UIManager *Owner;
public:
	typedef std::map<StringID, SuperSmartPointer<UIPanel> > ElementMap;
	ElementMap Elements;

	std::string Name;

	Json::Value ContainedObjects;

	UILayout() {};
	~UILayout();

	SuperSmartPointer<UILayout> Clone(SuperSmartPointer<UIPanel> Parent, const std::string &ParentElementName);

	SuperSmartPointer<UIPanel> FindPanelById(StringID ID);
	SuperSmartPointer<UIPanel> FindPanelByName(const std::string &Name);
};

/*!
*	UI Manager
*	Handles all input and processing and rendering of UI Elements
*/
class FLAMING_API UIManager : public BaseScriptableInstance
{
	friend class UIPanel;
	friend class UIMenu;
	friend class UILayout;
	friend class UIInputProcessor;
private:

	class ElementInfo
	{
	public:
		SuperSmartPointer<UIPanel> Panel;
		uint32 DrawOrder;

		~ElementInfo()
		{
			Panel.Dispose();
		};
	};

	uint32 DrawOrderCounter;

	typedef std::map<StringID, SuperSmartPointer<ElementInfo> > ElementMap;
	ElementMap Elements;

	SuperSmartPointer<UIPanel> FocusedElementValue;
	SuperSmartPointer<UITooltip> Tooltip;
	SuperSmartPointer<GenericConfig> Skin;
	SuperSmartPointer<sf::Font> DefaultFont;
	SuperSmartPointer<UIMenu> CurrentMenu;
	SuperSmartPointer<UIMenuBar> CurrentMenuBar;

	Vector4 DefaultFontColor, DefaultSecondaryFontColor;
	uint32 DefaultFontSize;

	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseMovePriv();
	void OnKeyPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o);
	void OnCharacterEnteredPriv();

	void RegisterInput();
	void UnRegisterInput();

	void RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement);

	void RemoveMenuFuture(MemoryStream &Stream);

	RendererManager::Renderer *Owner;

	UIPanel *MouseOverElement;

	bool DrawUIRects;

	bool DrawOrderCacheDirty;
	std::vector<ElementInfo *> DrawOrderCache;

	void CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName);
public:
	typedef std::map<StringID, SuperSmartPointer<UILayout> > LayoutMap;
	LayoutMap Layouts;

	UIManager(RendererManager::Renderer *TheOwner);
	~UIManager() { UnRegisterInput(); Clear(); ScriptInstance.Dispose(); };

	/*!
	*	\return the Renderer that owns this UI Manager
	*/
	RendererManager::Renderer *GetOwner()
	{
		return Owner;
	};

	/*!
	*	Updates all visible Elements
	*/
	void Update();

	/*!
	*	Draws all visible Elements
	*	\param Renderer the Renderer to draw to
	*/
	void Draw(RendererManager::Renderer *Renderer);

	/*!
	*	Adds an UI element to this UI Manager
	*	\param ID the UI Element's ID
	*	\param Element the UI Element to add
	*	\note You must be careful not to have multiple Elements with the same ID
	*	\return whether the element was added (fails if there was something wrong like a name conflict)
	*/
	bool AddElement(StringID ID, SuperSmartPointer<UIPanel> Element);

	/*!
	*	Removes an Element
	*	\param ID the UI Element's ID
	*/
	void RemoveElement(StringID ID);

	/*!
	*	Loads one or more Layouts from a Stream
	*	Format is JSON:
	*	[
	*		"GUILayoutSample" : [
	*			"Window": {
	*				"Control": "Frame",
	*				"Xpos": "2%", //May be absolute pixels too
	*				"Ypos": "2%",
	*				"Wide": "96%",
	*				"Tall": "96%",
	*	
	*				"Children": [
	*					"OK": {
	*						"Control": "Button",
	*						"Xpos": "center",
	*						"Ypos": "center",
	*						"Caption": "Test"
	*					}
	*				]
	*			}
	*		]
	*	]
	*
	*	\param In the stream
	*	\param Parent the parent element if there is one
	*/
	bool LoadLayouts(Stream *In, SuperSmartPointer<UIPanel> Parent = SuperSmartPointer<UIPanel>());

	/*!
	*	Gets a Layout from an ID
	*	\param LayoutID the Layout ID as a StringID
	*	\return the layout, or empty
	*/
	SuperSmartPointer<UILayout> GetLayout(StringID LayoutID);

	/*!
	*	Adds a layout with a specific ID to the layout list
	*	\param LayoutID the layout ID as a StringID
	*	\param Layout the Layout to add
	*/
	void AddLayout(StringID LayoutID, SuperSmartPointer<UILayout> Layout);

	/*!
	*	Clears all layouts we have loaded
	*	Does not remove regular elements!
	*/
	void ClearLayouts();

	/*!
	*	Fetches an Element by ID
	*	\param ID the Element's ID
	*	\return an Element or Empty
	*/
	SuperSmartPointer<UIPanel> GetElement(StringID ID);

	/*!
	*	\return the currently Focused Element
	*/
	SuperSmartPointer<UIPanel> GetFocusedElement();

	/*!
	*	\return the element that has the mouse moving over
	*/
	SuperSmartPointer<UIPanel> GetMouseOverElement();

	/*!
	*	\return the UI Tooltip
	*/
	UITooltip &GetTooltip();

	/*!
	*	\return the Default Font Color
	*/
	const Vector4 &GetDefaultFontColor()
	{
		return DefaultFontColor;
	};

	/*!
	*	\return the Default Font Color
	*/
	const Vector4 &GetDefaultSecondaryFontColor()
	{
		return DefaultSecondaryFontColor;
	};

	/*!
	*	\return the Default Font Size
	*/
	uint32 GetDefaultFontSize()
	{
		return DefaultFontSize;
	};

	/*!
	*	\return the Default Font
	*/
	SuperSmartPointer<sf::Font> GetDefaultFont()
	{
		return DefaultFont;
	};

	/*!
	*	Creates an UI Menu at a certain position
	*	\param Position the UI Menu's Position
	*	\return the UI Menu that was created
	*/
	UIMenu *CreateMenu(const Vector2 &Position);

	/*!
	*	\return the current UI Menu
	*/
	UIMenu *GetCurrentMenu()
	{
		return CurrentMenu.Get();
	};

	/*!
	*	\return the UI Menu Bar that was created
	*/
	UIMenuBar *CreateMenuBar();

	/*!
	*	\return the Current UI Menu Bar
	*/
	UIMenuBar *GetCurrentMenuBar()
	{
		return CurrentMenuBar.Get();
	};

	/*!
	*	Removes all elements
	*/
	void Clear();

	/*!
	*	Sets the UI Manager's UI Skin
	*	\param Skin the new Skin
	*/
	void SetSkin(SuperSmartPointer<GenericConfig> Skin);

	/*!
	*	\return the UI Skin we are currently using
	*/
	SuperSmartPointer<GenericConfig> GetSkin()
	{
		return Skin;
	};

	/*!
	*	Removes Focus from the focused UI Element
	*/
	void ClearFocus();
};

#pragma once
#if USE_GRAPHICS

class UIManager;
class UILayout;

#define UIELEMENT_CLICK_INTERVAL 1000
#define UIELEMENT_DEFAULT_FONT_SIZE 14

/*!
*	UI Element
*/
class UIElement
{
	friend class UIManager;
	friend class UILayout;
protected:
	bool VisibleValue, EnabledValue, RespondsToTooltipsValue, IsInputBlockerValue, DrawsInputBlockerBackgroundValue, TooltipsAreFixedValue, ClickPressed, JoystickButtonPressed, PropagatesEventsValue;
	uint32 EnabledInputsValue;
	Vector2 PositionValue, SizeValue, OffsetValue, TooltipPositionValue;
	UILayout *LayoutValue;
	UIManager *ManagerValue;
	StringID IDValue;
	std::string NameValue, LayoutNameValue, NativeTypeValue;
	DisposablePointer<UIElement> ParentValue, TooltipElementValue;
	std::vector<DisposablePointer<UIElement> > ChildrenValue;
	uint64 ClickTimer, JoystickButtonTimer;

	void OnConstructed();

	void OnEventPrivate(UIElement *Self, uint32 EventType, std::vector<void *> Arguments);

	typedef std::map<uint32, bool> EventErrorCacheMap;
	EventErrorCacheMap EventErrorCache;

	DisposablePointer<GenericConfig> SkinValue;

	UIElement(const UIElement &);
	UIElement &operator=(const UIElement &);
public:
	LuaEventGroup EventScriptHandlers[UIEventType::Count];

	LuaGlobalsTracker GlobalsTracker;

	struct LuaProperty
	{
		luabind::object Get, Set, Default;
	};

	typedef std::map<std::string, LuaProperty> PropertyMap;

	/*!
	*	A map of custom UI Element Properties
	*/
	PropertyMap Properties;

	/*!
	*	Lua-specific properties
	*/
	luabind::object PropertyValues;

	SimpleDelegate::SimpleDelegate<UIElement *, uint32, std::vector<void *> > OnEvent;

	UIElement(const std::string &NativeTypeName, UIManager *_Manager);

	virtual ~UIElement();

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
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	Draws the UI rect for this element
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element
	*/
	virtual void DrawUIRect(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	Draws the UI Focus Zone for this element
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element and before drawing the UI Rect
	*/
	virtual void DrawUIFocusZone(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	Sets whether this element has tooltips
	*	\param Value whether it does respond to tooltips
	*/
	virtual void SetRespondsToTooltips(bool Value);

	/*!
	*	\return Whether this element respondes to Tooltips
	*/
	virtual bool RespondsToTooltips() const;

	/*!
	*	Sets this element's UI Skin
	*	\param Skin the new Skin
	*/
	virtual void SetSkin(DisposablePointer<GenericConfig> Skin);

	/*!
	*	Sets whether this Element, as an Input Blocker, draws a background over the screen
	*	\param Value whether to draw the background
	*/
	virtual void SetInputBlockerBackground(bool Value);

	/*!
	*	\return whether this Element draws its Input Blocker Background if it becomes an Input Blocker
	*/
	virtual bool InputBlockerBackground() const;

	/*!
	*	Sets whether this element's tooltips are fixed (don't move with the mouse)
	*	\param Value whether the tooltips are fixed
	*/
	virtual void SetTooltipsFixed(bool Value);

	/*!
	*	\return Whether this element's tooltips are fixed (don't move with the mouse)
	*/
	virtual bool TooltipsFixed() const;

	/*!
	*	Sets this element's tooltips position
	*	\param Position this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	virtual void SetTooltipsPosition(const Vector2 &Position);

	/*!
	*	\return this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	virtual const Vector2 &TooltipsPosition() const;

	/*!
	*	Sets whether this element propagates events
	*	\param Value whether input events are propagated
	*/
	virtual void SetPropagatesEvents(bool Value);

	/*!
	*	\return whether this element propagates input events
	*/
	virtual bool PropagatesEvents() const;

	/*!
	*	Sets this element's position offset
	*	\param Offset the offset to set
	*/
	virtual void SetOffset(const Vector2 &Offset);

	/*!
	*	\return this element's offset
	*/
	virtual const Vector2 &Offset() const;

	/*!
	*	Sets the tooltip element for this element
	*	\param Element the tooltip element
	*/
	virtual void SetTooltipElement(DisposablePointer<UIElement> Element);

	/*!
	*	\return the Tooltip Element to display
	*/
	virtual DisposablePointer<UIElement> TooltipElement() const;

	/*
	*	Sets whether this Element is blocking input
	*	\param Value whether to block input
	*/
	virtual void SetBlockingInput(bool Value);

	/*
	*	\return whether this Element is blocking input
	*/
	virtual bool BlockingInput() const;

	/*!
	*	\return the UIManager associated to this UI Element
	*	\sa UIManager
	*/
	UIManager *Manager() const;

	/*!
	*	\return the ID of this UI Element
	*/
	StringID ID() const;

	/*!
	*	\return the name of this UI Element
	*/
	const std::string &Name() const;

	/*!
	*	\return the Native Type of this UI Element
	*/
	const std::string &NativeType() const;

	/*!
	*	Calculates the position of this element's parents to return the overall (absolute) position
	*	\return the calculated absolute position of this UI Element's parents
	*/
	virtual Vector2 ParentPosition() const;

	/*!
	*	Set this Element's Visiblity
	*	\param value whether the element should be visible
	*/
	virtual void SetVisible(bool value);

	/*!
	*	\return whether this element is visible
	*/
	virtual bool Visible() const;

	/*!
	*	Sets this element's Enabled status
	*	\param value whether this element is enabled
	*/
	virtual void SetEnabled(bool value);

	/*!
	*	\return whether this element is enabled
	*/
	virtual bool Enabled() const;

	/*!
	*	Sets this element's enabled input types
	*	\param value one or more of UIInputType::*
	*/
	virtual void SetEnabledInputTypes(uint32 value);

	/*!
	*	\return this element's enabled input types (one or more of UIInputType::*)
	*/
	virtual uint32 EnabledInputTypes() const;

	/*!
	*	Adds a child to this element
	*	\param Child the Child to add
	*/
	virtual void AddChild(DisposablePointer<UIElement> Child);

	/*!
	*	Removes a child from this element
	*	\param Child the Child to remove
	*/
	virtual void RemoveChild(DisposablePointer<UIElement> Child);

	/*!
	*	\return the amount of children in this element
	*/
	virtual uint32 ChildrenCount() const;

	/*!
	*	Gets a children at a specific index
	*	\param Index the index of the children
	*	\return the children or NULL
	*/
	virtual DisposablePointer<UIElement> Child(uint32 Index) const;

	/*!
	*	\return the Parent of this Element
	*/
	virtual DisposablePointer<UIElement> Parent() const;

	/*!
	*	\return the Position of this Element
	*/
	virtual const Vector2 &Position() const;

	/*!
	*	Sets the Position of this element
	*	\param Position the new position for this element
	*/
	virtual void SetPosition(const Vector2 &Position);

	/*!
	*	\return the Size of this Element
	*/
	virtual const Vector2 &Size() const;

	/*!
	*	Sets the Size of this element
	*	\param Size the new size for this element
	*/
	virtual void SetSize(const Vector2 &Size);

	/*!
	*	\return the Size of all children in this element
	*/
	virtual Vector2 ChildrenSize() const;

	/*!
	*	Clears all children (and destroys them)
	*/
	virtual void Clear();

	/*!
	*	Gets the layout this Element belongs to
	*	\return the Layout associated with this Element, or NULL
	*/
	UILayout *Layout() const;

	/*!
	*	Focuses this element
	*/
	virtual void Focus();
};
#endif

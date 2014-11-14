#pragma once

#define DEFAULT_GUI_TEXTURE_FILTERING TextureFiltering::Linear

#define RUN_GUI_SCRIPT_EVENTS(Variable, Parameters)\
	for(std::list<luabind::object>::iterator mit = Variable.Members.begin(); mit != Variable.Members.end(); mit++)\
	{\
		if(*mit)\
		{\
			try\
			{\
				(*mit) Parameters;\
			}\
			catch(std::exception &e)\
			{\
				Log::Instance.LogInfo(TAG, "Scripting Exception on UI Element '%s': %s", GetStringIDString(ID()).c_str(), e.what());\
			};\
		};\
	};

#define RUN_GUI_SCRIPT_EVENTS2(Variable, Parameters, IDVar)\
	for(std::list<luabind::object>::iterator mit = Variable.Members.begin(); mit != Variable.Members.end(); mit++)\
	{\
		if(*mit)\
		{\
			try\
			{\
				(*mit) Parameters;\
			}\
			catch(std::exception &e)\
			{\
				Log::Instance.LogInfo(TAG, "Scripting Exception on UI Element '%s': %s", GetStringIDString(IDVar).c_str(), e.what());\
			};\
		};\
	};

#if DEBUGGING_UI_INPUT
#	define REPORT_UIPANEL_INPUT_EVENT() Log::Instance.LogInfo("UIInputDebug", "@%s for Panel '%s'!", __FUNCTION__, Self->Name.c_str());
#	define REPORT_UIPANEL_INPUT_EVENTPRIV() Log::Instance.LogInfo("UIInputDebug", "@%s for Panel '%s'!", __FUNCTION__, Name.c_str());
#else
#	define REPORT_UIPANEL_INPUT_EVENT()
#	define REPORT_UIPANEL_INPUT_EVENTPRIV()
#endif

class UIManager;
class UILayout;

class UIPanel;

/*!
*	UI Animation Interface
*/
class UIAnimation
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
class UIPanel 
{
	friend class UIManager;
	friend class UIScrollableFrame;
	friend class UILayout;
protected:
	bool VisibleValue, EnabledValue, MouseInputValue, KeyboardInputValue, JoystickInputValue, TouchInputValue;
	Vector2 PositionValue, SizeValue, TranslationValue, SelectBoxExtraSize, OffsetValue;
	f32 AlphaValue, RotationValue, ExtraSizeScaleValue;
	UILayout *LayoutValue;
	UIManager *ManagerValue;
	bool RespondsToTooltipsValue, TooltipFixedValue;
	std::string TooltipValue;
	SuperSmartPointer<UIPanel> ParentValue, TooltipElementValue, ContentPanelValue;
	//!<Children of this UI Panel
	std::vector<UIPanel *> Children;
	uint64 ClickTimer;
	//!<Whether we are in the middle of a click
	bool ClickPressed;
	//!<Whether we are blocking Input
	bool BlockingInputValue, InputBlockerBackgroundValue;
	StringID IDValue;
	SuperSmartPointer<GenericConfig> Skin;
	bool DraggingValue, IsDraggableValue, IsDroppableValue;
	Vector2 TooltipPosition;

	std::list<SuperSmartPointer<UIAnimation> > AnimationQueue;

	/*!
	*	Called by the UI Manager when the UI Skin is changed
	*/
	virtual void OnSkinChange() {};

	std::stringstream PropertySetFunctionCode;

	LuaGlobalsTracker GlobalsTracker;

	void OnMouseJustPressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnMousePressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o);
	void OnTouchDownScript(UIPanel *Self, const InputCenter::TouchInfo &o);
	void OnTouchUpScript(UIPanel *Self, const InputCenter::TouchInfo &o);
	void OnTouchDragScript(UIPanel *Self, const InputCenter::TouchInfo &o);
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
	void OnJoystickButtonPressedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonJustPressedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonReleasedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o);
	void OnJoystickAxisMovedScript(UIPanel *Self, const InputCenter::JoystickAxisInfo &o);
	void OnJoystickConnectedScript(UIPanel *Self, uint32 JoystickIndex);
	void OnJoystickDisconnectedScript(UIPanel *Self, uint32 JoystickIndex);
	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseMovePriv();
	void OnTouchDownPriv(const InputCenter::TouchInfo &o);
	void OnTouchUpPriv(const InputCenter::TouchInfo &o);
	void OnTouchDragPriv(const InputCenter::TouchInfo &o);
	void OnKeyPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o);
	void OnJoystickButtonPressedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonJustPressedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonReleasedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickAxisMovedPriv(const InputCenter::JoystickAxisInfo &o);
	void OnJoystickConnectedPriv(uint32 JoystickIndex);
	void OnJoystickDisconnectedPriv(uint32 JoystickIndex);
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
	LuaEventGroup OnMouseJustPressedFunction, OnMousePressedFunction, OnMouseReleasedFunction,
		OnKeyJustPressedFunction, OnKeyPressedFunction, OnKeyReleasedFunction, OnMouseMovedFunction,
		OnCharacterEnteredFunction, OnGainFocusFunction, OnLoseFocusFunction, OnUpdateFunction, OnDrawFunction,
		OnMouseEnteredFunction, OnMouseOverFunction, OnMouseLeftFunction, OnClickFunction, OnDragBeginFunction, OnDragEndFunction,
		OnDraggingFunction, OnDropFunction, OnJoystickConnectedFunction, OnJoystickDisconnectedFunction, OnJoystickAxisMovedFunction,
		OnJoystickButtonJustPressedFunction, OnJoystickButtonPressedFunction, OnJoystickButtonReleasedFunction, PositionFunction,
		OnTouchDownFunction, OnTouchUpFunction, OnTouchDragFunction,
		SizeFunction, OnStartFunction, PropertiesStartupDefaultFunction, PropertiesStartupSetFunction;

	std::string Name, LayoutName, NativeType;

	SimpleDelegate::SimpleDelegate<UIPanel *> OnLoseFocus, OnGainFocus, OnCharacterEntered, OnMouseMove, OnMouseEntered, OnMouseOver, OnMouseLeft;
	SimpleDelegate::SimpleDelegate<UIPanel *, const InputCenter::MouseButtonInfo &> OnMouseJustPressed, OnMousePressed, OnMouseReleased, OnClick;
	SimpleDelegate::SimpleDelegate<UIPanel *, const InputCenter::KeyInfo &> OnKeyJustPressed, OnKeyPressed, OnKeyReleased;
	SimpleDelegate::SimpleDelegate<UIPanel *, const InputCenter::TouchInfo &> OnTouchDown, OnTouchUp, OnTouchDrag;
	SimpleDelegate::SimpleDelegate<UIPanel *> OnDragBegin, OnDragging, OnDragEnd;
	//[Self, Target]
	SimpleDelegate::SimpleDelegate<UIPanel *, UIPanel *> OnDrop;

	SimpleDelegate::SimpleDelegate<UIPanel *, uint32> OnJoystickConnected, OnJoystickDisconnected;
	SimpleDelegate::SimpleDelegate<UIPanel *, const InputCenter::JoystickButtonInfo &> OnJoystickButtonJustPressed, OnJoystickButtonPressed,
		OnJoystickButtonReleased;
	SimpleDelegate::SimpleDelegate<UIPanel *, const InputCenter::JoystickAxisInfo &> OnJoystickAxisMoved;

	struct PropertyInfo
	{
		luabind::object GetFunction, SetFunction, DefaultFunction;
	};

	typedef std::map<std::string, PropertyInfo> PropertyMap;

	/*!
	*	A map of custom Panel Properties
	*/
	PropertyMap Properties;

	luabind::object PropertyValues;

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
	virtual void Draw(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	Perform auto calculation of sizes here
	*/
	virtual void PerformLayout();

	/*!
	*	Draws the UI rect for this element
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element
	*/
	void DrawUIRect(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	Draws the UI Focus Zone for this element
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element and before drawing the UI Rect
	*/
	void DrawUIFocusZone(const Vector2 &ParentPosition, Renderer *Renderer);

	/*!
	*	\return Whether this element respondes to Tooltips
	*/
	bool RespondsToTooltips() const;

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
	*	Sets the Content Panel for this Panel
	*	\param Panel the Content Panel of this Panel
	*/
	void SetContentPanel(SuperSmartPointer<UIPanel> Panel);

	/*!
	*	\return the Content Panel for this Panel
	*/
	SuperSmartPointer<UIPanel> ContentPanel() const;

	/*!
	*	Sets whether this Panel, as an Input Blocker, draws a background over the screen
	*	\param Value whether to draw the background
	*/
	void SetInputBlockerBackground(bool Value);

	/*!
	*	\return whether this Panel draws its Input Blocker Background if it becomes an Input Blocker
	*/
	bool InputBlockerBackground() const;

	/*!
	*	Adjust the size and position of this panel and its parents if the size has changed somehow, like on ExtraSize changes
	*	\param PanelToStopAt the panel to stop resizing/positioning. Will be ignored.
	*/
	void AdjustSizeAndPosition(UIPanel *PanelToStopAt);

	/*!
	*	\return the extra size scale
	*/
	f32 ExtraSizeScale() const;

	/*!
	*	Sets the Extra Size Scale
	*	\param Scale the scale of the extra size
	*/
	void SetExtraSizeScale(f32 Scale);

	/*!
	*	Sets whether this element has tooltips
	*	\param Value whether it does respond to tooltips
	*/
	void SetRespondsToTooltips(bool Value);

	/*!
	*	Gets the composed size (size + scaled extrasize)
	*/
	Vector2 ComposedSize() const;

	/*!
	*	\return Whether this element's tooltips are fixed (don't move with the mouse)
	*/
	bool TooltipsFixed() const;

	/*!
	*	Sets this element's rotation
	*	\param Value this element's rotation in Radians
	*/
	void SetRotation(f32 Value);

	/*!
	*	Gets this element's rotation in Radians
	*/
	f32 Rotation() const;

	/*!
	*	Gets the rotation of this element combined with its parents
	*/
	f32 ParentRotation() const;

	/*!
	*	Sets whether this element's tooltips are fixed (don't move with the mouse)
	*	\param Value whether the tooltips are fixed
	*/
	void SetTooltipsFixed(bool Value);

	/*!
	*	\return this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	const Vector2 &TooltipsPosition() const;

	/*!
	*	\return this element's offset
	*/
	const Vector2 &Offset() const;

	/*!
	*	Sets this element's position offset
	*	\param Offset the offset to set
	*/
	void SetOffset(const Vector2 &Offset);

	/*!
	*	Sets this element's tooltips position
	*	\param Position this element's tooltips position (relative to mouse position if not Fixed, relative to Element if Fixed)
	*/
	void SetTooltipsPosition(const Vector2 &Position);

	/*!
	*	\return the Tooltip Text to display
	*/
	const std::string &TooltipText() const;

	/*!
	*	\return the Tooltip Element to display
	*/
	SuperSmartPointer<UIPanel> TooltipElement() const;

	/*
	*	\return whether this panel is blocking input
	*/
	bool BlockingInput() const;

	/*
	*	Sets whether this panel is blocking input
	*	\param Value whether to block input
	*/
	void SetBlockingInput(bool Value);

	/*!
	*	Sets the tooltip text for this element
	*	\param Text the tooltip text
	*/
	void SetTooltipText(const std::string &Text);

	/*!
	*	Sets the tooltip element for this element
	*	\param Element the tooltip element
	*/
	void SetTooltipElement(SuperSmartPointer<UIPanel> Element);

	/*!
	*	Sets whether we can drag this element to other elements
	*	\param Value whether we can drag this element
	*/
	void SetDraggable(bool Value);

	/*!
	*	\return whether we can drag this element
	*/
	bool Draggable() const;

	/*!
	*	Sets whether we can drop this element on other elements
	*	\param Value whether we can drop this element
	*/
	void SetDroppable(bool Value);

	/*!
	*	\return whether we can drop this element
	*/
	bool Droppable() const;

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
	*	Calculates the position of this element's parents to return the overall (absolute) position
	*	\return the calculated absolute position of this UI Element's parents
	*/
	Vector2 ParentPosition() const;

	/*!
	*	Set this Element's Visiblity
	*	\param value whether the element should be visible
	*/
	void SetVisible(bool value);

	/*!
	*	\return whether this element is visible
	*/
	bool Visible() const;

	/*!
	*	Sets this element's Enabled status
	*	\param value whether this element is enabled
	*/
	void SetEnabled(bool value);

	/*!
	*	\return whether this element is enabled
	*/
	bool Enabled() const;

	/*!
	*	Sets whether this element receives Mouse Input
	*	\param value whether this element receives Mouse Input
	*/
	void SetMouseInputEnabled(bool value);

	/*!
	*	\return whether this element receives Mouse Input
	*/
	bool MouseInputEnabled() const;

	/*!
	*	Sets whether this element receives Touch Input
	*	\param value whether this element receives Touch Input
	*/
	void SetTouchInputEnabled(bool value);

	/*!
	*	\return whether this element receives Touch Input
	*/
	bool TouchInputEnabled() const;

	/*!
	*	Sets whether this element receives Keyboard Input
	*	\param value whether this element receives Keyboard Input
	*/
	void SetKeyboardInputEnabled(bool value);

	/*!
	*	\return whether this element receives Keyboard Input
	*/
	bool KeyboardInputEnabled() const;

	/*!
	*	Sets whether this element receives Joystick Input
	*	\param value whether this element receives Joystick Input
	*/
	void SetJoystickInputEnabled(bool value);

	/*!
	*	\return whether this element receives Joystick Input
	*/
	bool JoystickInputEnabled() const;

	/*!
	*	Adds a child to this element
	*	\param Child the Child to add
	*/
	void AddChild(UIPanel *Child);

	/*!
	*	Removes a children from this element
	*	\param Child the Child to remove
	*/
	void RemoveChild(UIPanel *Child);

	/*!
	*	\return the amount of children in this element
	*/
	uint32 ChildrenCount() const;

	/*!
	*	Gets a children at a specific index
	*	\param Index the index of the children
	*	\return the children or NULL
	*/
	UIPanel *Child(uint32 Index) const;

	/*!
	*	\return the Translation of this element
	*	\note Used by elements such as Scrollbars to indicate how much translation there is
	*/
	const Vector2 &Translation() const;

	/*!
	*	\return the Parent of this Element (or NULL)
	*/
	UIPanel *Parent() const;

	/*!
	*	\return the Position of this Element
	*/
	const Vector2 &Position() const;

	/*!
	*	Sets the Position of this element
	*	\param Position the new position for this element
	*/
	void SetPosition(const Vector2 &Position);

	/*!
	*	\return the Size of this Element
	*/
	const Vector2 &Size() const;

	/*!
	*	Sets the Size of this element
	*	\param Size the new size for this element
	*/
	void SetSize(const Vector2 &Size);

	/*!
	*	\return the Alpha Transparency of this Element
	*/
	f32 Alpha() const;

	/*!
	*	\return the Alpha Transparency of this element and all its parents
	*/
	f32 ParentAlpha() const;

	/*!
	*	Sets the Alpha Transparency of this Element
	*	\param Alpha the new alpha for this element
	*/
	void SetAlpha(f32 Alpha);

	/*!
	*	\return the Size of all children in this element
	*/
	Vector2 ChildrenSize() const;

	/*!
	*	\return the Scaled Extra Size of all children in this element
	*/
	Vector2 ChildrenExtraSize() const;

	/*!
	*	Clears all children (and destroys them)
	*/
	void Clear();

	/*!
	*	Sets the Skin of this Element
	*	\param Skin the new skin to apply
	*/
	void SetSkin(SuperSmartPointer<GenericConfig> Skin);

	/*!
	*	Gets the layout this Panel belongs to
	*	\return the Layout associated with this Panel, or NULL
	*/
	UILayout *Layout() const;

	/*!
	*	Gets the extra size for the element
	*	Extra size is based on the Nine-Patch corner sizes
	*/
	const Vector2 &ExtraSize() const;

	/*!
	*	Gets the Scaled Extra Size
	*/
	Vector2 ScaledExtraSize() const;

	/*!
	*	Focuses this element
	*/
	void Focus();
};

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
		AdjustCloser = FLAGVALUE(7),
		InvertDirection = FLAGVALUE(8),
		InvertX = FLAGVALUE(9),
		InvertY = FLAGVALUE(10)
	};
};

/*!
*	Generic Group Container which does nothing more than perform layout and update/draw children
*/
class UIGroup : public UIPanel
{
public:
	uint32 LayoutMode;

	UIGroup(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

/*!
*	UI Sprite Element
*/
class UISprite : public UIPanel
{
public:
	/*!
	*	The Element's Sprite
	*/
	Sprite TheSprite;

	UISprite(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

/*!
*	UI Tooltip Element
*/
class UITooltip : public UIPanel
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
	std::string OverrideText;

	TextParams TextParameters;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

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
};

/*!
*	UI Text Element
*	\note Will not autosize
*/
class UIText : public UIPanel
{
protected:
	void OnSkinChange();
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
	*	\note Only colors and border are used, Position is ignored
	*/
	TextParams TextParameters;

	/*!
	*	Text Alignment
	*/
	uint32 TextAlignment;

	/*!
	*	Whether to expand height by default
	*/
	bool ExpandHeight;

	UIText(UIManager *Manager);

	/*!
	*	Sets the Text of this UIText
	*	\param String the new Text
	*	\param AutoExpandHeight whether the Height of this Element should be expanded for the text
	*	\note Required set so we recalculate the strings
	*/
	void SetText(const std::string &String, bool AutoExpandHeight = false);

	/*!
	*	\return the Text of this UIText
	*/
	const std::string &Text();

	/*!
	*	\return the Text's size
	*/
	Vector2 TextSize();

	/*!
	*	\return the formatted text strings
	*/
	const std::vector<StringInfo> GetStrings()
	{
		return Strings;
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

/*!
*	UI Text Composer Element
*	\note Will not autosize
*/
class UITextComposer : public UIPanel
{
protected:
	class TextComposerNode
	{
	public:
		std::string Text;
		TextParams Params;
		SuperSmartPointer<UIText> Instance;
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
	void AddText(const std::string &Text, const TextParams &Params);

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
	void Draw(const Vector2 &ParentPosition, Renderer *Renderer);
};

/*!
*	UI Layout
*	Contains Elements and can be loaded from a scheme file
*/
class UILayout 
{
friend class UIManager;

	UIManager *Owner;

	Json::Value ContainedObjects;

	void FinalizeProperties(UIPanel *ParentElement);
	void FinalizeAllProperties(UIPanel *ParentElement);
public:
	typedef std::map<StringID, SuperSmartPointer<UIPanel> > ElementMap;
	ElementMap Elements;

	/*!
	*	Layout Name
	*/
	std::string Name;

	/*!
	*	Parent Element of this Layout, if any
	*/
	SuperSmartPointer<UIPanel> Parent;

	UILayout() : Owner(NULL) {};
	~UILayout();

	/*!
	*	Performs Startup Events of these elements
	*	\param ParentElement Empty if we're at the root, otherwise current element being processed
	*/
	void PerformStartupEvents(UIPanel *ParentElement);

	/*!
	*	Displays Parentless Elements
	*/
	void DisplayParentlessElements();

	/*!
	*	Creates a copy of this Layout
	*	\param Parent the Parent Element upon which to put this Layout
	*	\param ParentElementName the name of the Parent Element we use as a prefix
	*	\param PerformStartupEvents whether we should perform startup events now
	*	\param DisplayParentlessElements whether we should turn Elements with no Parents visible
	*/
	SuperSmartPointer<UILayout> Clone(SuperSmartPointer<UIPanel> Parent, const std::string &ParentElementName,
		bool DisplayParentlessElements = false, bool PerformStartupEvents = false);

	/*!
	*	Finds an element by ID
	*	\param ID the element ID
	*/
	SuperSmartPointer<UIPanel> FindPanelById(StringID ID);

	/*!
	*	Finds an element by Name
	*	\param Name the element name
	*/
	SuperSmartPointer<UIPanel> FindPanelByName(const std::string &Name);
};

/*!
*	UI Manager
*	Handles all input and processing and rendering of UI Elements
*/
class UIManager : public BaseScriptableInstance
{
	friend class UIPanel;
	friend class UIText;
	friend class UIGroup;
	friend class UISprite;
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

#if FLPLATFORM_DEBUG
	typedef std::map<std::string, StringID> ElementIDMap;
	ElementIDMap ElementIDs;
#endif

	SuperSmartPointer<UIPanel> FocusedElementValue;
	SuperSmartPointer<UITooltip> Tooltip;
	SuperSmartPointer<GenericConfig> Skin;
	FontHandle DefaultFont;

	Vector4 DefaultFontColor, DefaultSecondaryFontColor;
	uint32 DefaultFontSize;

	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o);
	void OnTouchDownPriv(const InputCenter::TouchInfo &o);
	void OnTouchUpPriv(const InputCenter::TouchInfo &o);
	void OnTouchDragPriv(const InputCenter::TouchInfo &o);
	void OnMouseMovePriv();
	void OnKeyPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o);
	void OnCharacterEnteredPriv();
	void OnJoystickButtonPressedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonJustPressedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickButtonReleasedPriv(const InputCenter::JoystickButtonInfo &o);
	void OnJoystickAxisMovedPriv(const InputCenter::JoystickAxisInfo &o);
	void OnJoystickConnectedPriv(uint8 Index);
	void OnJoystickDisconnectedPriv(uint8 Index);

	void RegisterInput();
	void UnRegisterInput();

	void RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement, bool Mouse, const Vector2 &TargetPosition);

	Renderer *Owner;

	UIPanel *MouseOverElement;

	bool DrawUIRects, DrawUIFocusZones;

	bool DrawOrderCacheDirty;
	std::vector<ElementInfo *> DrawOrderCache;

	void CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, const Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName, UIPanel *ParentToStopAt);

	void ProcessTextProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessTextJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessSpriteProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessSpriteJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessCustomProperty(UIPanel *Panel, const std::string &Property, const Json::Value &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessCustomPropertyJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);

	UIManager();
	UIManager(const UIManager &);
public:
	typedef std::map<StringID, SuperSmartPointer<UILayout> > LayoutMap;
	LayoutMap Layouts, DefaultLayouts;

	UIManager(Renderer *TheOwner);
	~UIManager();

	/*!
	*	\return the Renderer that owns this UI Manager
	*/
	Renderer *GetOwner();

	/*!
	*	Updates all visible Elements
	*/
	void Update();

	/*!
	*	Draws all visible Elements
	*	\param Renderer the Renderer to draw to
	*/
	void Draw(Renderer *Renderer);

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
	*	\param DefaultLayouts whether these layouts are Default Layouts
	*/
	bool LoadLayouts(Stream *In, SuperSmartPointer<UIPanel> Parent = SuperSmartPointer<UIPanel>(), bool DefaultLayouts = false);

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
	const Vector4 &GetDefaultFontColor();

	/*!
	*	\return the Default Font Color
	*/
	const Vector4 &GetDefaultSecondaryFontColor();

	/*!
	*	\return the Default Font Size
	*/
	uint32 GetDefaultFontSize();

	/*!
	*	\return the Default Font
	*/
	FontHandle GetDefaultFont();

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
	SuperSmartPointer<GenericConfig> GetSkin();

	/*!
	*	\return the current Input Blocker, if any
	*/
	SuperSmartPointer<UIPanel> GetInputBlocker();

	/*!
	*	Removes Focus from the focused UI Element
	*/
	void ClearFocus();
};

#pragma once

#define DEFAULT_GUI_TEXTURE_FILTERING TextureFiltering::Linear

#define RUN_GUI_SCRIPT_EVENTS(Variable, Parameters)\
	for(std::list<luabind::object>::iterator mit = Variable.Members.begin(); mit != Variable.Members.end(); mit++)\
	{\
		if(*mit)\
		{\
			try\
			{\
				(*mit) ## Parameters;\
			}\
			catch(std::exception &e)\
			{\
				Log::Instance.LogInfo(TAG, "Scripting Exception on UI Element '%s': %s", GetStringIDString(ID).c_str(), e.what());\
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
				(*mit) ## Parameters;\
			}\
			catch(std::exception &e)\
			{\
				Log::Instance.LogInfo(TAG, "Scripting Exception on UI Element '%s': %s", GetStringIDString(IDVar).c_str(), e.what());\
			};\
		};\
	};

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
	f32 AlphaValue, RotationValue, ExtraSizeScaleValue;
	UILayout *Layout;
	UIManager *Manager;
	bool RespondsToTooltipsValue, TooltipFixed;
	sf::String TooltipValue;
	SuperSmartPointer<UIPanel> ParentValue, TooltipElement, ContentPanelValue;
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
	LuaEventGroup OnMouseJustPressedFunction, OnMousePressedFunction, OnMouseReleasedFunction,
		OnKeyJustPressedFunction, OnKeyPressedFunction, OnKeyReleasedFunction, OnMouseMovedFunction,
		OnCharacterEnteredFunction, OnGainFocusFunction, OnLoseFocusFunction, OnUpdateFunction, OnDrawFunction,
		OnMouseEnteredFunction, OnMouseOverFunction, OnMouseLeftFunction, OnClickFunction, OnDragBeginFunction, OnDragEndFunction,
		OnDraggingFunction, OnDropFunction, PositionFunction, SizeFunction, OnStartFunction;

	luabind::object Properties;
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
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element
	*/
	void DrawUIRect(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	Draws the UI Focus Zone for this element
	*	\param ParentPosition the parent's position
	*	\param Renderer the renderer to render to
	*	\note should be called by all derived classes on Draw() after drawing the element and before drawing the UI Rect
	*/
	void DrawUIFocusZone(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);

	/*!
	*	\return Whether this element respondes to Tooltips
	*/
	bool RespondsToTooltips() const { return RespondsToTooltipsValue && (TooltipValue.getSize() || TooltipElement.Get()); };

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
	void SetContentPanel(SuperSmartPointer<UIPanel> Panel)
	{
		ContentPanelValue = Panel;
	};

	/*!
	*	\return the Content Panel for this Panel
	*/
	SuperSmartPointer<UIPanel> ContentPanel() const
	{
		return ContentPanelValue;
	};

	/*!
	*	Adjust the size and position of this panel and its parents if the size has changed somehow, like on ExtraSize changes
	*	\param PanelToStopAt the panel to stop resizing/positioning. Will be ignored.
	*/
	void AdjustSizeAndPosition(UIPanel *PanelToStopAt);

	/*!
	*	\return the extra size scale
	*/
	f32 ExtraSizeScale() const
	{
		return ExtraSizeScaleValue;
	};

	/*!
	*	Sets the Extra Size Scale
	*	\param Scale the scale of the extra size
	*/
	void SetExtraSizeScale(f32 Scale)
	{
		ExtraSizeScaleValue = Scale;
	};

	/*!
	*	Sets whether this element has tooltips
	*	\param Value whether it does respond to tooltips
	*/
	void SetRespondsToTooltips(bool Value)
	{
		RespondsToTooltipsValue = Value;
	};

	/*!
	*	Gets the composed size (size + scaled extrasize)
	*/
	Vector2 GetComposedSize() const
	{
		return GetSize() + GetScaledExtraSize();
	};

	/*!
	*	\return Whether this element's tooltips are fixed (don't move with the mouse)
	*/
	bool TooltipsFixed() const
	{
		return TooltipFixed;
	};

	/*!
	*	Sets this element's rotation
	*	\param Value this element's rotation in Radians
	*/
	void SetRotation(f32 Value)
	{
		RotationValue = Value;
	};

	/*!
	*	Gets this element's rotation in Radians
	*/
	f32 Rotation() const
	{
		return RotationValue;
	};

	/*!
	*	Gets the rotation of this element combined with its parents
	*/
	f32 GetParentRotation() const
	{
		f32 Rotation = RotationValue;

		UIPanel *Parent = const_cast<UIPanel *>(ParentValue.Get());

		while(Parent != NULL)
		{
			Rotation += Parent->RotationValue;

			Parent = Parent->GetParent();
		};

		return Rotation;
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
	const Vector2 &TooltipsPosition() const
	{
		return TooltipPosition;
	};

	/*!
	*	\return this element's offset
	*/
	const Vector2 &GetOffset() const
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
	const sf::String &GetTooltipText() const
	{
		return TooltipValue;
	};

	/*!
	*	\return the Tooltip Element to display
	*/
	SuperSmartPointer<UIPanel> GetTooltipElement() const
	{
		return TooltipElement;
	};

	/*
	*	\return whether this panel is blocking input
	*/
	bool IsBlockingInput() const
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
	bool IsDraggable() const
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
	bool IsDroppable() const
	{
		return IsDroppableValue;
	};

	/*!
	*	\return the UIManager associated to this UI Element
	*	\sa UIManager
	*/
	UIManager *GetManager() const
	{
		return Manager;
	};

	/*!
	*	\return the ID of this UI Element
	*/
	StringID GetID() const
	{
		return ID;
	};

	/*!
	*	Calculates the position of this element's parents to return the overall (absolute) position
	*	\return the calculated absolute position of this UI Element's parents
	*/
	Vector2 GetParentPosition() const
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position = PositionValue + OffsetValue + TranslationValue - GetScaledExtraSize() / 2;
		UIPanel *p = const_cast<UIPanel *>(ParentValue.Get());

		while(p)
		{
			Position += p->GetPosition() + p->GetTranslation() + p->GetOffset() - p->GetScaledExtraSize() / 2;

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
	UIPanel *GetChild(uint32 Index) const
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
	f32 GetParentAlpha() const;

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
	Vector2 GetChildrenSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->GetPosition() + Children[i]->GetOffset() + Children[i]->GetComposedSize();

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		};

		return Out;
	};

	/*!
	*	\return the Scaled Extra Size of all children in this element
	*/
	Vector2 GetChildrenExtraSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->GetScaledExtraSize();

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
	UILayout *GetLayout() const
	{
		return Layout;
	};

	/*!
	*	Gets the extra size for the element
	*	Extra size is based on the Nine-Patch corner sizes
	*/
	const Vector2 &GetExtraSize() const
	{
		return SelectBoxExtraSize;
	};

	/*!
	*	Gets the Scaled Extra Size
	*/
	Vector2 GetScaledExtraSize() const
	{
		return SelectBoxExtraSize * ExtraSizeScaleValue;
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
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer);
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
protected:
	void OnSkinChange();
public:
	struct StringInfo
	{
		sf::String TheString;
		Rect Size;
	};

private:
	std::vector<StringInfo> Strings;
	sf::String Text;
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
	const std::vector<StringInfo> GetStrings()
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

/*!
*	UI Layout
*	Contains Elements and can be loaded from a scheme file
*/
class FLAMING_API UILayout 
{
friend class UIManager;
	UIManager *Owner;

	Json::Value ContainedObjects;
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

	UILayout() {};
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
class FLAMING_API UIManager : public BaseScriptableInstance
{
	friend class UIPanel;
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

	RendererManager::Renderer *Owner;

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
public:
	typedef std::map<StringID, SuperSmartPointer<UILayout> > LayoutMap;
	LayoutMap Layouts, DefaultLayouts;

	UIManager(RendererManager::Renderer *TheOwner);
	~UIManager() { UnRegisterInput(); Clear(); Tooltip.Dispose(); ScriptInstance.Dispose(); };

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
	*	\param DefaultLayout whether these layouts are Default Layouts
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

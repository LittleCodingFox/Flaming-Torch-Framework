/*!
*	UI Manager
*	Handles all input and processing and rendering of UI Elements
*/
class UIManager : public BaseScriptableInstance
{
	friend class UIElement;
	friend class UIText;
	friend class UIGroup;
	friend class UISprite;
	friend class UILayout;
	friend class UIInputProcessor;
private:
	class ElementInfo
	{
	public:
		DisposablePointer<UIElement> Element;
		uint32 DrawOrder;

		~ElementInfo()
		{
			Element.Dispose();
		};
	};

	uint32 DrawOrderCounter;

	typedef std::map<StringID, DisposablePointer<ElementInfo> > ElementMap;
	ElementMap Elements;

#if FLPLATFORM_DEBUG
	typedef std::map<std::string, StringID> ElementIDMap;
	ElementIDMap ElementIDs;
#endif

	DisposablePointer<UIElement> FocusedElementValue;

	void RegisterInput();
	void UnRegisterInput();

	void RecursiveFindFocusedElement(const Vector2 &ParentPosition, DisposablePointer<UIElement> p, DisposablePointer<UIElement> &FoundElement, bool Mouse, const Vector2 &TargetPosition);

	Renderer *Owner;

	DisposablePointer<UIElement> MouseOverElement;

	bool DrawUIRects, DrawUIFocusZones;

	bool DrawOrderCacheDirty;
	std::vector<DisposablePointer<ElementInfo> > DrawOrderCache;

	void CopyElementsToLayout(DisposablePointer<UILayout> TheLayout, const Json::Value &Elements, UIElement *Parent, const std::string &ParentElementName, UIElement *ParentToStopAt);
	void UpdateFocusedElement(const Vector2 &FocusPoint, uint32 InputType);

	void ProcessTextProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessTextJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessSpriteProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessSpriteJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);

	UIManager();
	UIManager(const UIManager &);
public:
	typedef std::map<StringID, DisposablePointer<UILayout> > LayoutMap;
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
	bool AddElement(StringID ID, DisposablePointer<UIElement> Element);

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
	bool LoadLayouts(Stream *In, DisposablePointer<UIElement> Parent = DisposablePointer<UIElement>(), bool DefaultLayouts = false);

	/*!
	*	Gets a Layout from an ID
	*	\param LayoutID the Layout ID as a StringID
	*	\return the layout, or empty
	*/
	DisposablePointer<UILayout> GetLayout(StringID LayoutID);

	/*!
	*	Adds a layout with a specific ID to the layout list
	*	\param LayoutID the layout ID as a StringID
	*	\param Layout the Layout to add
	*/
	void AddLayout(StringID LayoutID, DisposablePointer<UILayout> Layout);

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
	DisposablePointer<UIElement> GetElement(StringID ID);

	/*!
	*	\return the currently Focused Element
	*/
	DisposablePointer<UIElement> GetFocusedElement();

	/*!
	*	\return the element that has the mouse moving over
	*/
	DisposablePointer<UIElement> GetMouseOverElement();

	/*!
	*	Removes all elements
	*/
	void Clear();

	/*!
	*	\return the current Input Blocker, if any
	*/
	DisposablePointer<UIElement> GetInputBlocker();

	/*!
	*	Removes Focus from the focused UI Element
	*/
	void ClearFocus();
};

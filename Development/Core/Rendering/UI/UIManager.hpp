#pragma once
#if USE_GRAPHICS

class TextureGroup;

/*!
*	UI Manager
*	Handles all input and processing and rendering of UI Elements
*/
class UIManager : public BaseScriptableInstance
{
	friend class UIElement;
	friend class UIText;
	friend class UIGroup;
	friend class UIVerticalGroup;
	friend class UIHorizontalGroup;
	friend class UISprite;
	friend class UILayout;
	friend class UIInputProcessor;
	friend class RendererManager;
private:
	class ElementInfo
	{
	public:
		DisposablePointer<UIElement> Element;
		uint32 DrawOrder;

		~ElementInfo()
		{
			Element.Dispose();
		}
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
	void ProcessButtonProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessButtonJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName);
	void ProcessGroupJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName);

	StringID MakeTextResourceString(uint32 Character, const TextParams &Parameters);
	StringID MakeTextureResourceString(const Path &FileName);

	UIManager();
	UIManager(const UIManager &);

	struct TextResourceInfo
	{
		uint32 Character;
		TextParams TextParameters;
		Glyph Info;

		uint32 References;

		DisposablePointer<Texture> SourceTexture, InstanceTexture;

		TextResourceInfo() : References(0), Character(0) {}
	};

	struct TextureResourceInfo
	{
		Path FileName;

		DisposablePointer<Texture> SourceTexture, InstanceTexture;

		uint32 References;

		TextureResourceInfo() : References(0) {}
	};

	typedef std::map<StringID, TextResourceInfo> TextResourceMap;

	TextResourceMap TextResources;

	typedef std::map<StringID, TextureResourceInfo> TextureResourceMap;

	TextureResourceMap TextureResources;

	DisposablePointer<TextureGroup> ResourcesGroup;

	DisposablePointer<GenericConfig> SkinValue;

	void ClearUnusedResources();
public:
	typedef std::map<StringID, DisposablePointer<UILayout> > LayoutMap;
	LayoutMap Layouts, DefaultLayouts;
	Vector4 DefaultTextColor, DefaultTextSecondaryColor;
	uint32 DefaultTextFontSize;
	DisposablePointer<Font> DefaultFont;

	UIManager(Renderer *TheOwner);
	~UIManager();

	/*!
	*	Used internally by UIElements to report that we're using a texture
	*/
	void ReportTextureUse(DisposablePointer<Texture> TheTexture);

	/*!
	*	\return the active UI texture resources
	*/
	inline uint32 ActiveTextureResources()
	{
		return TextureResources.size();
	}

	/*!
	*	\return the active UI texture resources
	*/
	inline uint32 ActiveTextResources()
	{
		return TextResources.size();
	}

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
	void Draw();

	/*!
	*	\param Text the text string to draw
	*	\param Params the text parameters
	*/
	void DrawText(const std::string &Text, const TextParams &Params);

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
	*	Instances a layout by name
	*	\param Name the layout's name
	*	\param Parent the layout's parent
	*/
	bool InstanceLayout(const std::string &Name, DisposablePointer<UIElement> Parent = DisposablePointer<UIElement>());

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
	*	\param FileName the filename of the texture
	*	\return the loaded texture
	*/
	DisposablePointer<Texture> GetUITexture(const Path &FileName);

	/*!
	*	\param Color the Color for the texture
	*	\return the loaded texture
	*/
	DisposablePointer<Texture> GetUITexture(const Vector4 &Color);

	/*!
	*	\param Text the text string to load
	*	\param Parameters the text parameters for the string
	*/
	void GetUIText(const std::string &Text, const TextParams &Parameters);

	/*!
	*	Removes Focus from the focused UI Element
	*/
	void ClearFocus();

	/*!
	*	Sets the UI's Skin
	*	\param Skin the new Skin
	*/
	void SetSkin(DisposablePointer<GenericConfig> Skin);

	/*!
	*	Gets the UI's Skin
	*/
	DisposablePointer<GenericConfig> GetSkin() const;
};
#endif

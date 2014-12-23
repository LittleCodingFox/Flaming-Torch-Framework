#pragma once

/*!
*	UI Layout
*	Contains Elements and can be loaded from a scheme file
*/
class UILayout
{
	friend class UIManager;
private:

	UIManager *Owner;

	Json::Value ContainedObjects;
public:
	typedef std::map<StringID, DisposablePointer<UIElement> > ElementMap;
	ElementMap Elements;

#if FLPLATFORM_DEBUG
	typedef std::map<std::string, DisposablePointer<UIElement> > NamedElementMap;
	NamedElementMap NamedElements;
#endif

	/*!
	*	Layout Name
	*/
	std::string Name;

	/*!
	*	Parent Element of this Layout, if any
	*/
	DisposablePointer<UIElement> Parent;

	UILayout() : Owner(NULL) {};
	~UILayout();

	/*!
	*	Performs Startup Events of these elements
	*	\param ParentElement Empty if we're at the root, otherwise current element being processed
	*/
	void PerformStartupEvents(UIElement *ParentElement);

	/*!
	*	Sets the visibility of Parentless Elements
	*	\param Visible whether parentless elements should be visible
	*/
	void SetVisibleParentlessElements(bool Visible);

	/*!
	*	Creates a copy of this Layout
	*	\param Parent the Parent Element upon which to put this Layout
	*	\param ParentElementName the name of the Parent Element we use as a prefix
	*	\param PerformStartupEvents whether we should perform startup events now
	*	\param VisibleParentlessElements whether elements with no parents should be visible
	*/
	DisposablePointer<UILayout> Clone(DisposablePointer<UIElement> Parent, const std::string &ParentElementName, bool PerformStartupEvents, bool VisibleParentlessElements);

	/*!
	*	Finds an element by ID
	*	\param ID the element ID
	*/
	DisposablePointer<UIElement> FindElementById(StringID ID);

	/*!
	*	Finds an element by Name
	*	\param Name the element name
	*/
	DisposablePointer<UIElement> FindElementByName(const std::string &Name);
};

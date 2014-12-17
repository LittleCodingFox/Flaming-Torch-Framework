#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	UILayout::~UILayout()
	{
		while(Elements.begin() != Elements.end())
		{
			if(Elements.begin()->second.Get())
			{
				Owner->RemoveElement(Elements.begin()->first);

				Elements.begin()->second.Dispose();
			};

			Elements.erase(Elements.begin());
		};
	};

	DisposablePointer<UIElement> UILayout::FindElementById(StringID ID)
	{
		ElementMap::iterator it = Elements.find(ID);

		if(it == Elements.end())
			return DisposablePointer<UIElement>();

		return it->second;
	};

	DisposablePointer<UIElement> UILayout::FindElementByName(const std::string &Name)
	{
		ElementMap::iterator it = Elements.find(MakeStringID(Name));

		if(it == Elements.end())
			return DisposablePointer<UIElement>();

		return it->second;
	};

	DisposablePointer<UILayout> UILayout::Clone(DisposablePointer<UIElement> Parent, const std::string &ParentElementName, bool PerformStartupEvents, bool VisibleParentlessElements)
	{
		DisposablePointer<UILayout> Out(new UILayout());
		Out->Name = Name;
		Out->ContainedObjects = ContainedObjects;
		Out->Owner = Owner;
		Out->Parent = Parent;

		Owner->CopyElementsToLayout(Out, ContainedObjects, Parent, ParentElementName + "." +
			Out->Name, Parent);

		Out->SetVisibleParentlessElements(VisibleParentlessElements);

		if(PerformStartupEvents)
		{
			Out->PerformStartupEvents(NULL);
		};

		return Out;
	};

	void UILayout::SetVisibleParentlessElements(bool Visible)
	{
		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second.Get() != NULL && it->second->Parent() == Parent.Get())
			{
				it->second->SetVisible(Visible);
			};
		};
	};

	void UILayout::PerformStartupEvents(UIElement *ParentElement)
	{
		if(ParentElement)
		{
			for(uint32 i = 0; i < ParentElement->ChildrenCount(); i++)
			{
				PerformStartupEvents(ParentElement->Child(i));
			};

			ParentElement->OnEvent(UIEventType::Start, {});
		}
		else
		{
			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second.Get() == NULL)
					continue;

				PerformStartupEvents(it->second.Get());
			};
		};
	};
#endif
};

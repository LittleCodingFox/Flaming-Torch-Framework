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

	SuperSmartPointer<UIPanel> UILayout::FindPanelById(StringID ID)
	{
		ElementMap::iterator it = Elements.find(ID);

		if(it == Elements.end())
			return SuperSmartPointer<UIPanel>();

		return it->second;
	};

	SuperSmartPointer<UIPanel> UILayout::FindPanelByName(const std::string &Name)
	{
		ElementMap::iterator it = Elements.find(MakeStringID(Name));

		if(it == Elements.end())
			return SuperSmartPointer<UIPanel>();

		return it->second;
	};

	SuperSmartPointer<UILayout> UILayout::Clone(SuperSmartPointer<UIPanel> Parent, const std::string &ParentElementName)
	{
		SuperSmartPointer<UILayout> Out(new UILayout());
		Out->Name = Name;
		Out->ContainedObjects = ContainedObjects;
		Out->Owner = Owner;

		Owner->CopyElementsToLayout(Out, ContainedObjects, Parent, ParentElementName + "." +
			Out->Name);

		for(ElementMap::iterator it = Out->Elements.begin(); it != Out->Elements.end(); it++)
		{
			if(it->second->GetParent() == Parent.Get())
			{
				it->second->SetVisible(true);
			};

			RUN_GUI_SCRIPT_EVENTS2(it->second->OnStartFunction, (it->second.Get()), it->second->ID)
		};

		return Out;
	};
#endif
};

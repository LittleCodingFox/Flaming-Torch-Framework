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

	SuperSmartPointer<UILayout> UILayout::Clone(SuperSmartPointer<UIPanel> Parent, const std::string &ParentElementName, bool ShouldDisplayParentlessElements, bool DoStartupEvents)
	{
		SuperSmartPointer<UILayout> Out(new UILayout());
		Out->Name = Name;
		Out->ContainedObjects = ContainedObjects;
		Out->Owner = Owner;
		Out->Parent = Parent;

		Owner->CopyElementsToLayout(Out, ContainedObjects, Parent, ParentElementName + "." +
			Out->Name, Parent);

		if(ShouldDisplayParentlessElements)
		{
			Out->DisplayParentlessElements();
		};

		if(DoStartupEvents)
			Out->PerformStartupEvents(NULL);

		return Out;
	};

	void UILayout::DisplayParentlessElements()
	{
		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Parent() == Parent.Get())
			{
				it->second->SetVisible(true);
				Parent->SetRotation(it->second->Rotation());
			};
		};
	};

	void UILayout::PerformStartupEvents(UIPanel *ParentElement)
	{
		if(ParentElement)
		{
			for(uint32 i = 0; i < ParentElement->ChildrenCount(); i++)
			{
				PerformStartupEvents(ParentElement->Child(i));
			};

			//Need to init properties here and finish up the property set function code since we might need to add further
			//property sets before we are finished initing

			ParentElement->PropertySetFunctionCode << "\nend\n";

			luaL_dostring(ParentElement->Manager()->ScriptInstance->State, ParentElement->PropertySetFunctionCode.str().c_str());

			ParentElement->PropertiesStartupSetFunction.Add(luabind::globals(ParentElement->Manager()->ScriptInstance->State)
				["PropertyStartupSet_" + StringUtils::PointerString(ParentElement)]);

			RUN_GUI_SCRIPT_EVENTS2(ParentElement->PropertiesStartupDefaultFunction, (ParentElement), ParentElement->ID());
			RUN_GUI_SCRIPT_EVENTS2(ParentElement->PropertiesStartupSetFunction, (ParentElement), ParentElement->ID());
			RUN_GUI_SCRIPT_EVENTS2(ParentElement->OnStartFunction, (ParentElement), ParentElement->ID());
		}
		else
		{
			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				PerformStartupEvents(it->second.Get());
			};
		};
	};
#endif
};

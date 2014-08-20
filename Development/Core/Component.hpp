#pragma once

class Entity;

/*!
*	Entity Component
*	Adds behaviour to your entities
*/
class FLAMING_API Component 
{
public:
	std::string Name;
	
	StringID ID;
	
	StringID ParentID;

	luabind::object Properties;

	luabind::object OnStartFunction;
	luabind::object OnUpdateFunction;

	Entity *Owner;

	Component() : ID(0), ParentID(0), Owner(NULL) {};
	~Component()
	{
		Properties = luabind::object();
		OnStartFunction = luabind::object();
		OnUpdateFunction = luabind::object();
	};

	/*!
	*	Updates this component
	*/
	void Update();
};

#pragma once

class Component;

/*!
*	Game Entity class
*/
class Entity
{
public:
	/*!
	*	Entity Properties
	*/
	luabind::object Properties;

	/*!
	*	Entity Name
	*/
	std::string Name;

	/*!
	*	Entity ID
	*/
	StringID ID;

	Entity() : ID(0) {};
	~Entity();

	/*!
	*	Destroy this Entity
	*/
	void Destroy();

	/*!
	*	Make a copy of this Entity
	*/
	SuperSmartPointer<Entity> Clone() const;

	/*!
	*	Check whether this entity has a certain component
	*	\param ComponentID the name of the Component
	*/
	bool HasComponent(const std::string &ComponentID) const;

	/*!
	*	Check whether this entity has a certain component
	*	\param PropertyName the name of the Property
	*/
	bool HasProperty(const std::string &PropertyName) const;

	/*!
	*	Gets a component from this Entity
	*	\param ComponentID the name of the Component
	*/
	Component *GetComponent(const std::string &ComponentID);

	/*!
	*	Adds a component to this Entity
	*	\param ComponentID the name of the Component
	*/
	bool AddComponent(const std::string &ComponentID);
};

class World
{
public:
	typedef std::map<StringID, SuperSmartPointer<Entity> > EntityMap;

	//ComponentID -> Component
	typedef std::map<StringID, SuperSmartPointer<Component> > ComponentMap;

	//Entity -> ComponentMap
	typedef std::map<StringID, ComponentMap> EntityComponentMap;

	//ComponentID -> EntityIDList
	typedef std::map<StringID, std::list<StringID> > ComponentEntityMap;

	/*!
	*	List of Entities
	*/
	EntityMap Entities;

	/*!
	*	List of Components of each Entity
	*/
	EntityComponentMap Components;

	/*!
	*	Cache of Components to Entities to allow for quick search of Entities that have a component you're looking for
	*/
	ComponentEntityMap ComponentEntityCache;

	/*!
	*	Component copies
	*/
	ComponentMap ComponentArchetypes;

	static StringID MakeEntityID(Entity *Target);
	static World Instance;

	/*!
	*	Gets a list of Entities with a specific component
	*	\param Component the component name to search for
	*	\return the list of Entities
	*/
	std::vector<Entity *> EntitiesWithComponent(const std::string &Component);

	/*!
	*	Loads a component from a file
	*	\param FileName the file to load
	*/
	bool LoadComponent(const std::string &FileName);

	/*!
	*	Creates an Entity
	*	\param Name the Entity's name
	*/
	Entity *NewEntity(const std::string &Name);

	/*!
	*	Creates an Entity from a script file
	*	\param Name the Entity's Name
	*	\param ScriptFileName the Entity's Script File Name
	*/
	Entity *NewEntity(const std::string &Name, const std::string &ScriptFileName);

	/*!
	*	Iterates through entities with one or more components and calls a function on them
	*	\note Lua-only
	*	\param ComponentNames is the list of components as a lua table
	*	\param Function is the lua function to call
	*/
	void IterateEntitiesWithComponents(luabind::object ComponentNames, luabind::object Function);

	/*!
	*	Destroys all entities and components
	*/
	void Clear();

	/*!
	*	Updates all entities and components
	*/
	void Update();
};

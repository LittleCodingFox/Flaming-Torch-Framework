#pragma once
#define LUASCRIPTTAG "LuaScript"

template <typename T>
T* get_pointer(SuperSmartPointer<T> const& p)
{
	return (T *)p.Get();
};

/*!
*	Trues to cast a luabind object into a native type safely without throwing an exception
*	\param In the object to test
*/
template<typename type>
type ProtectedLuaCast(luabind::object In)
{
	try
	{
		return luabind::object_cast<type>(In);
	}
	catch(std::exception &)
	{
	};

	return type();
};

/*!
*	Script
*/
class LuaScript
{
	friend class LuaScriptManager;

	LuaScript();
public:
	lua_State *State;

	~LuaScript();

	/*!
	*	Runs a string of code
	*	\param Code the code to run
	*	\return the error code (0 == success)
	*/
	int32 DoString(const std::string &Code);
};

/*!
*	Lua Globals Tracker
*	Tracks any Globals you want to clean up later on
*/
class LuaGlobalsTracker
{
private:
	SuperSmartPointer<LuaScript> ScriptInstance;
public:
	std::vector<std::string> Names;

	LuaGlobalsTracker(SuperSmartPointer<LuaScript> Script);

	/*!
	*	Adds a Name to the Names list
	*/
	void Add(const std::string &Name);

	/*!
	*	Sets all globals to nil and clears the Names array
	*/
	void Clear();
};

/*!
*	Base Scriptable Instance class
*	Same as Type, except adds a Script Instance
*/
class BaseScriptableInstance 
{
public:
	SuperSmartPointer<LuaScript> ScriptInstance;
};

class LuaLib
{
public:
	virtual ~LuaLib() {};

	virtual bool Register(lua_State *State) = 0;
};

/*!
*	Registers the entire framework into Lua
*/
class FrameworkLib : public LuaLib
{
public:
	bool Register(lua_State *State);

	static FrameworkLib Instance;
};

/*!
	Script Manager
*/
class LuaScriptManager : public SubSystem
{
	friend class LuaScript;
	std::list<SuperSmartPointer<LuaScript> > InstancedScripts;
public:
	SuperSmartPointer<FileStream> ErrorStream;

	static LuaScriptManager Instance;

	LuaScriptManager() : SubSystem(LUASCRIPTMANAGER_PRIORITY) {};

	/*!
		Creates a script
		\param Code the Script's code
		\param Libs is the LuaLibs to use (FrameworkLib is not used by default)
		\param LibCount is how many libs we are using
		\sa FrameworkLib
	*/
	SuperSmartPointer<LuaScript> CreateScript(const std::string &Code, LuaLib **Libs, uint32 LibCount);

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);

	void LogError(const std::string &Message);

	/*!
		Performs the Main Loop of the application from a script
		\param LoopScript the Script used for the main loop
		\return 0 if we succeeded, non-0 on failure
	*/
	int32 PerformMainLoop(SuperSmartPointer<LuaScript> LoopScript);
};

/*!
*	Groups Lua Functions together
*/
class LuaEventGroup
{
public:
	std::list<luabind::object> Members;

	/*!
	*	Adds a member to the group
	*	\param Member the member to add
	*/
	void Add(luabind::object Member);

	/*!
	*	Removes a member to the group
	*	\param Member the member to remove
	*/
	void Remove(luabind::object Member);
};

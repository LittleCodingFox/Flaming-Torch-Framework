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
class FLAMING_API LuaScript
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
*	Base Scriptable Instance class
*	Same as Type, except adds a Script Instance
*/
class FLAMING_API BaseScriptableInstance 
{
public:
	SuperSmartPointer<LuaScript> ScriptInstance;
};

class FLAMING_API LuaLib
{
public:
	virtual ~LuaLib() {};

	virtual bool Register(lua_State *State) = 0;
};

/*!
*	Registers the entire framework into Lua
*/
class FLAMING_API FrameworkLib : public LuaLib
{
public:
	bool Register(lua_State *State);

	static FrameworkLib Instance;
};

/*!
	Script Manager
*/
class FLAMING_API LuaScriptManager : public SubSystem
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

#pragma once
#define LUASCRIPTTAG "LuaScript"

template <typename T>
T* get_pointer(DisposablePointer<T> const& p)
{
	return (T *)p.Get();
}

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
	}

	return type();
}

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
	
	template<typename type>
	inline luabind::object VectorToLua(const std::vector<type> &In)
	{
		luabind::object Out = luabind::newtable(State);

		uint32 Counter = 0;

		for(typename std::vector<type>::const_iterator it = In.begin(); it != In.end(); it++)
		{
			Out[++Counter] = *it;
		}

		return Out;
	}

	template<typename type>
	inline std::vector<type> VectorFromLua(luabind::object In)
	{
		std::vector<type> Out;

		for (luabind::iterator it(In), end; it != end; ++it)
		{
			Out.push_back(ProtectedLuaCast<type>(*it));
		}

		return Out;
	}

	template<typename key, typename value>
	inline luabind::object MapToLua(const std::map<key, value> &In)
	{
		luabind::object Out = luabind::newtable(State);

		uint32 Counter = 0;

		for (typename std::map<key, value>::const_iterator it = In.begin(); it != In.end(); it++)
		{
			Out[it->first] = it->second;
		}

		return Out;
	}

	template<typename key, typename value>
	inline std::map<key, value> MapFromLua(luabind::object In)
	{
		std::map<key, value> Out;

		for (luabind::iterator it(In), end; it != end; ++it)
		{
			Out[ProtectedLuaCast<key>(it.key())] = ProtectedLuaCast<value>(*it);
		}

		return Out;
	}
};

/*!
*	Lua Globals Tracker
*	Tracks any Globals you want to clean up later on
*/
class LuaGlobalsTracker
{
private:
	DisposablePointer<LuaScript> ScriptInstance;
public:
	std::vector<std::string> Names;

	LuaGlobalsTracker(DisposablePointer<LuaScript> Script);

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
	DisposablePointer<LuaScript> ScriptInstance;
};

class LuaLib
{
public:
	virtual ~LuaLib() {}

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
*	Script Manager
*/
class LuaScriptManager : public SubSystem
{
	friend class LuaScript;
	std::list<DisposablePointer<LuaScript> > InstancedScripts;
public:
	DisposablePointer<FileStream> ErrorStream;

	LuaScriptManager() : SubSystem(LUASCRIPTMANAGER_PRIORITY) {}

	/*!
	*	Creates a script
	*	\param Code the Script's code
	*	\param Libs is the LuaLibs to use (FrameworkLib is not used by default)
	*	\param LibCount is how many libs we are using
	*	\sa FrameworkLib
	*/
	DisposablePointer<LuaScript> CreateScript(const std::string &Code, LuaLib **Libs, uint32 LibCount);

	/*!
	*	Loads a stream and runs whatever code is inside it, returning the result
	*	Behaves similar to loading packages in lua
	*/
	static luabind::object DoStream(Stream *In, lua_State *State);

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);

	void LogError(const std::string &Message);
};

extern LuaScriptManager g_LuaScript;

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

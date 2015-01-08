#include "FlamingCore.hpp"
#include <time.h>
namespace FlamingTorch
{
#	define TAG "LuaScriptManager"
#	define TAG2 "LuaScript"

	void LuaErrorFunction(lua_State *State)
	{
		lua_Debug DebugInfo;
		int32 Depth = 0, LineIndex = 1;
		std::string Error = lua_tostring(State, -1);
		lua_pop(State, 1);

		static std::stringstream str;

		str.str("");
		str << "Error: " << Error << "\n";

		while (lua_getstack(State, Depth, &DebugInfo))
		{
			int status = lua_getinfo(State, "Sln", &DebugInfo);

			if (status <= 0)
				break;

			if (DebugInfo.currentline < 0)
			{
				Depth++;

				continue;
			}

			static std::vector<std::string> Lines;
			Lines = StringUtils::Split(StringUtils::Replace(DebugInfo.source, "\r", ""), '\n');

			if ((int32)Lines.size() <= DebugInfo.currentline)
			{
				Depth++;

				continue;
			}

			str << "#" << LineIndex << "  " << DebugInfo.short_src << ":" << DebugInfo.currentline << ": " << StringUtils::Trim(Lines[DebugInfo.currentline - 1]);

			if (DebugInfo.name != NULL)
			{
				str << "(" << DebugInfo.namewhat << " " << DebugInfo.name << ")";
			}

			str << "\n";

			LineIndex++;
			Depth++;
		}

		if (LuaScriptManager::Instance.ErrorStream)
			LuaScriptManager::Instance.ErrorStream->Write2<char>(str.str().c_str(), str.str().length());

		Log::Instance.LogErr(TAG, str.str().c_str());
	}

	void LuaEventGroup::Add(luabind::object Member)
	{
		if(!Member)
		{
			Log::Instance.LogWarn(TAG, "Unable to add a Member to a Lua Event Group: Invalid Member");

			return;
		}

		for(std::list<luabind::object>::iterator it = Members.begin(); it != Members.end(); it++)
		{
			if(*it == Member)
				return;
		}

		Members.push_back(Member);
	}

	void LuaEventGroup::Remove(luabind::object Member)
	{
		for(std::list<luabind::object>::iterator it = Members.begin(); it != Members.end(); it++)
		{
			if(*it == Member)
			{
				Members.erase(it);

				return;
			}
		}
	}

	LuaGlobalsTracker::LuaGlobalsTracker(DisposablePointer<LuaScript> Script) : ScriptInstance(Script) {}

	void LuaGlobalsTracker::Add(const std::string &Name)
	{
		for(uint32 i = 0; i < Names.size(); i++)
		{
			if(Names[i] == Name)
				return;
		}

		Names.push_back(Name);
	}

	void LuaGlobalsTracker::Clear()
	{
		if(!ScriptInstance.Get())
			return;

		while(Names.size())
		{
			//Best to use dostring to be sure the globals are set to nil
			luaL_dostring(ScriptInstance->State, (*Names.begin() + " = nil").c_str());

			Names.erase(Names.begin());
		}
	}

	int LuabindPCall(lua_State* State)
	{
		LuaErrorFunction(State);

		return 1;
	}

	bool LuaLoadPackage(const std::string &Name)
	{
		DisposablePointer<FileStream> Stream(new FileStream());

		if(!Stream->Open(Name, StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG2, "Unable to add package '%s': Failed to open for reading", Name.c_str());

			return false;
		}

		if(!PackageFileSystemManager::Instance.AddPackage(MakeStringID(Name), Stream))
		{
			Log::Instance.LogErr(TAG2, "Unable to add package '%s': Failed to deserialize", Name.c_str());

			return false;
		}

		Log::Instance.LogInfo(TAG2, "Loaded package '%s'", Name.c_str());

		return true;
	}

	void RegisterBase(lua_State *State)
	{
		lua_pushcfunction(State, luaopen_base);
		lua_call(State, 0, 0);

		lua_pushcfunction(State, luaopen_math);
		lua_call(State, 0, 0);

		lua_pushcfunction(State, luaopen_string);
		lua_call(State, 0, 0);

		lua_pushcfunction(State, luaopen_table);
		lua_call(State, 0, 0);

		luabind::module(State) [
			luabind::class_<BaseScriptableInstance, DisposablePointer<BaseScriptableInstance> >("BaseScriptableInstance")
		];
	}

	LuaScriptManager LuaScriptManager::Instance;

	LuaScript::LuaScript()
	{
		State = luaL_newstate();
		luaL_openlibs(State);
		luabind::open(State);
	}

	LuaScript::~LuaScript()
	{
		lua_close(State);
	}

	int32 LuaScript::DoString(const std::string &Code)
	{
		int32 Error = luaL_dostring(State, Code.c_str());

		if(Error != 0)
			LuaErrorFunction(State);

		return Error;
	}

	DisposablePointer<LuaScript> LuaScriptManager::CreateScript(const std::string &Code, LuaLib **Libs, uint32 LibCount)
	{
		FLASSERT(WasStarted, "Lua ScriptManager not yet started!");

		if(!WasStarted)
			return DisposablePointer<LuaScript>();

		int32 error = 0;

		DisposablePointer<LuaScript> ScriptInstance(new LuaScript());

		RegisterBase(ScriptInstance->State);

		for(uint32 i = 0; i < LibCount; i++)
		{
			if(!Libs[i]->Register(ScriptInstance->State))
			{
				ScriptInstance.Dispose();

				return DisposablePointer<LuaScript>();
			}
		}

		luabind::set_pcall_callback(LuabindPCall);

		//lua_atpanic(ScriptInstance->State, LuabindPCall);

		error = ScriptInstance->DoString(Code.c_str());

		if(error != 0)
			return DisposablePointer<LuaScript>();

		InstancedScripts.push_back(ScriptInstance);

		return ScriptInstance;
	}

	void LuaScriptManager::LogError(const std::string &Message)
	{
		if(!WasStarted)
			return;

		std::string FinalMessage = Message + "\n";

		if(ErrorStream.Get())
			ErrorStream->Write2<char>(FinalMessage.c_str(), FinalMessage.length());

		Log::Instance.LogErr(TAG, FinalMessage.c_str());
	}

	void LuaScriptManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Lua ScriptManager Subsystem starting..");

		ErrorStream.Reset(new FileStream());

		std::stringstream str;
		time_t rawtime;
		time(&rawtime);

		char Buffer[128];

		strftime(Buffer, 128, "%d_%m_%Y", localtime(&rawtime));

		str << FileSystemUtils::PreferredStorageDirectory() << "/ScriptErrors_" << Buffer << ".txt";

		if(!ErrorStream->Open(str.str(), StreamFlags::Write | StreamFlags::Text))
		{
			ErrorStream.Dispose();

			Log::Instance.LogErr(TAG, "Lua ScriptManager running without ScriptErrors logging!");
		}

		luabind::set_pcall_callback(LuabindPCall);
	}

	void LuaScriptManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Lua ScriptManager Subsystem terminating..");

		while(InstancedScripts.begin() != InstancedScripts.end())
		{
			if(InstancedScripts.begin()->Get())
				InstancedScripts.begin()->Dispose();

			InstancedScripts.erase(InstancedScripts.begin());
		}

		ErrorStream.Dispose();

		SubSystem::Shutdown(Priority);
	}

	void LuaScriptManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();
	}

	int32 LuaScriptManager::PerformMainLoop(DisposablePointer<LuaScript> LoopScript)
	{
		FLASSERT(LoopScript.Get(), "Invalid Loop Script!");

		luabind::object ScriptInit = luabind::globals(LoopScript->State)["ScriptInit"];
		luabind::object ScriptLoop = luabind::globals(LoopScript->State)["ScriptLoop"];
		luabind::object ScriptShutdown = luabind::globals(LoopScript->State)["ScriptShutdown"];

		if(!ScriptInit || !ScriptLoop || !ScriptShutdown)
		{
			Log::Instance.LogErr(TAG, "Missing script init, loop, or shutdown functions!");
			DeInitSubsystems();

			return 1;
		}

		if(!ProtectedLuaCast<bool>(ScriptInit()))
		{
			Log::Instance.LogErr(TAG, "Failed to init main loop from a script: ScriptInit returned an error");
			DeInitSubsystems();

			return 1;
		}

		for(;;)
		{
			UpdateSubsystems();

			if(!ProtectedLuaCast<bool>(ScriptLoop()))
				break;
		}

		ScriptShutdown();

		Log::Instance.LogInfo(TAG, "End of Main Loop");

		DeInitSubsystems();

		return 0;
	}
}

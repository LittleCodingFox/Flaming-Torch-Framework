#pragma once

namespace ConsoleVariableType
{
	enum
	{
		UInt, //!<Unsigned integer
		Int, //!<Integer
		Float, //!<Float
		String //!<String
	};
};

struct ConsoleVariable
{
	std::string Name;
	/*!
	*	Should be one of ConsoleVariableType::*
	*/
	uint8 Type;
	uint32 UintValue;
	int32 IntValue;
	f32 FloatValue;
	std::string StringValue;

	ConsoleVariable() : Type(ConsoleVariableType::String) {};
};

class ConsoleCommand
{
public:
	std::string Name;

	Signal1<const std::vector<std::string> &> Method;

	virtual ~ConsoleCommand() {};
};

class ScriptedConsoleCommand : public ConsoleCommand
{
public:
	luabind::object ScriptedMethod;

	void ScriptedMethodProxy(const std::vector<std::string> &Parameters)
	{
		if(!ScriptedMethod)
			return;

		try
		{
			luabind::object ActualParameters = luabind::newtable(GameInterface::Instance.AsDerived<ScriptedGameInterface>()->ScriptInstance->State);

			for(uint32 i = 0; i < Parameters.size(); i++)
			{
				ActualParameters[i + 1] = Parameters[i];
			};

			ScriptedMethod(ActualParameters);
		}
		catch(std::exception &e)
		{
			LuaScriptManager::Instance.LogError("Console Command [" + Name + "]: Scripting Error: " + e.what());
		};
	};

	ScriptedConsoleCommand()
	{
		Method.Connect(this, &ScriptedConsoleCommand::ScriptedMethodProxy);
	};
};

/*!
*	Console Subsystem
*	Used to store game vars and commands
*/
class Console : public SubSystem
{
public:
	static Console Instance;

private:
	std::vector<ConsoleVariable> ConsoleVariables;
	std::vector<SuperSmartPointer<ConsoleCommand> > ConsoleCommands;

	void HelpCommand(const std::vector<std::string> &Parameters);
	void VersionCommand(const std::vector<std::string> &Parameters);

#if USE_GRAPHICS
	void BindCommand(const std::vector<std::string> &Parameters);
	void ValidKeyCommand(const std::vector<std::string> &Parameters);
	void ScreenshotCommand(const std::vector<std::string> &Parameters);
#endif

	void StartUp(uint32 Priority);
	void Update(uint32 Priority);
	void Shutdown(uint32 Priority);
public:
	std::vector<std::string> ConsoleLog;

	Console() : SubSystem(CONSOLE_PRIORITY) {};

	/*!
	*	Registers a console variable
	*	\param Variable the variable to register
	*/
	void RegisterVariable(const ConsoleVariable &Variable);

	/*!
	*	Registers a console command
	*	\param Command the command to register
	*/
	void RegisterCommand(SuperSmartPointer<ConsoleCommand> Command);

	/*!
	*	Gets a console variable by name
	*	\param Name the name of the variable
	*	\return the variable as a pointer, or NULL
	*/
	ConsoleVariable *GetVariable(const std::string &Name);

	/*!
	*	Adds a message to the console log
	*	\param Message the message to add
	*/
	void LogConsole(const std::string &Message);

	/*!
	*	Runs a command on the game console
	*	\param Command the command to run
	*/
	void RunConsoleCommand(const std::string &Command);
};

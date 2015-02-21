#pragma once

namespace ConsoleVariableType
{
	enum ConsoleVariableType
	{
		UInt, //!<Unsigned integer
		Int, //!<Integer
		Float, //!<Float
		String //!<String
	};
}

struct ConsoleVariable
{
	std::string Name;
	/*!
	*	Should be one of ConsoleVariableType::*
	*/
	uint8 Type;
	uint32 UIntValue;
	int32 IntValue;
	f32 FloatValue;
	std::string StringValue;

	ConsoleVariable() : Type(ConsoleVariableType::String), UIntValue(0), IntValue(0), FloatValue(0) {}
	ConsoleVariable(const std::string _Name, uint32 Value) : Name(_Name), UIntValue(Value), Type(ConsoleVariableType::UInt), IntValue(0), FloatValue(0) {}
	ConsoleVariable(const std::string _Name, int32 Value) : Name(_Name), IntValue(Value), Type(ConsoleVariableType::Int), UIntValue(0), FloatValue(0) {}
	ConsoleVariable(const std::string _Name, f32 Value) : Name(_Name), FloatValue(Value), Type(ConsoleVariableType::Float), UIntValue(0), IntValue(0) {}
	ConsoleVariable(const std::string _Name, const std::string Value) : Name(_Name), StringValue(Value), Type(ConsoleVariableType::String), UIntValue(0), IntValue(0), FloatValue(0) {}
};

class ConsoleCommand
{
public:
	std::string Name;

	SimpleDelegate::SimpleDelegate<const std::vector<std::string> &> Method;

	virtual ~ConsoleCommand() {}
};

/*!
*	Console Subsystem
*	Used to store game vars and commands
*/
class Console : public SubSystem
{
private:
	std::vector<ConsoleVariable> ConsoleVariables;
	std::vector<DisposablePointer<ConsoleCommand> > ConsoleCommands;

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
	std::vector<std::string> ConsoleLog, CommandLog;

	Console() : SubSystem(CONSOLE_PRIORITY) {}

	/*!
	*	Registers a console variable
	*	\param Variable the variable to register
	*/
	void RegisterVariable(const ConsoleVariable &Variable);

	/*!
	*	Registers a console command
	*	\param Command the command to register
	*/
	void RegisterCommand(DisposablePointer<ConsoleCommand> Command);

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

extern Console g_Console;

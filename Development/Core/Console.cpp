#include "FlamingCore.hpp"
#define TAG "Console"
namespace FlamingTorch
{
	Console Console::Instance;

	void Console::HelpCommand(const std::vector<std::string> &Parameters)
	{
		LogConsole("Available Commands:");

		for(uint32 i = 0; i < ConsoleCommands.size(); i++)
		{
			LogConsole("  " + ConsoleCommands[i]->Name);
		};
	};

	void Console::VersionCommand(const std::vector<std::string> &Parameters)
	{
		LogConsole(std::string("> ") + (GameInterface::Instance ? GameInterface::Instance->GameName() : "(No Game)") +
			" using core version " + CoreUtils::MakeVersionString(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR));
	};

#if USE_GRAPHICS
	void Console::ValidKeyCommand(const std::vector<std::string> &Parameters)
	{
		std::stringstream str;
		
		bool First = true;

		uint32 Counter = 0;

		for(InputCenter::InfoNameMap::iterator it = InputCenter::KeyInfo::Names.begin(); it != InputCenter::KeyInfo::Names.end(); it++, Counter++)
		{
			if(!First)
			{
				str << ", ";
			};

			First = false;
			str << it->second;

			if(Counter >= 100)
			{
				Counter -= 100;
				str << "\n";
			};
		};

		LogConsole("> Valid Keys: " + str.str());
	};

	void Console::BindCommand(const std::vector<std::string> &Parameters)
	{
		if(Parameters.size() == 1)
		{
			std::string Action = StringUtils::ToUpperCase(Parameters[0]);

			const InputCenter::Action *const TheAction = RendererManager::Instance.Input.GetAction(MakeStringID(Action));

			if(TheAction)
			{
				switch(TheAction->Type)
				{
				case InputActionType::Keyboard:
					LogConsole("> " + Action + " - KEY" + TheAction->AsString());

					break;

				case InputActionType::JoystickAxis:
					LogConsole("> " + Action + " - JOY" + StringUtils::MakeIntString(TheAction->Index) + TheAction->AsString());

					break;

				case InputActionType::JoystickButton:
					LogConsole("> " + Action + " - JOY" + StringUtils::MakeIntString(TheAction->Index) + TheAction->AsString());

					break;

				case InputActionType::MouseButton:
					LogConsole("> " + Action + " - MOUSE" + TheAction->AsString());

					break;

				case InputActionType::MouseScroll:
					LogConsole("> " + Action + " - MOUSESCROLL" + (TheAction->PositiveValues ? "UP" : "DOWN"));

					break;

				default:
					LogConsole("> Invalid Action detected - " + Action);

					break;
				};
			}
			else
			{
				LogConsole("> UNBOUND");
			};

			return;
		};

		if(Parameters.size() != 2)
		{
			LogConsole("Should have 2 parameters (have " + StringUtils::MakeIntString((uint32)Parameters.size()) + ")");

			return;
		};

		std::string Action = StringUtils::ToUpperCase(Parameters[0]), Target = StringUtils::ToUpperCase(Parameters[1]);

		InputCenter::Action TheAction;
		TheAction.Name = Action;

		if(Target.find("JOY") == 0 && Target.find("AXIS") == 4)
		{
			TheAction.Type = InputActionType::JoystickAxis;

			uint32 JoyIndex = 0;

			if(1 != sscanf(Target.c_str(), "JOY%u", &JoyIndex))
			{
				LogConsole("> Invalid key identifier (should be JOY<joyindex>AXIS<axis><+/->, e.g. JOY0AXISX-)");

				return;
			};

			if(Target[Target.length() - 1] == '+')
			{
				TheAction.PositiveValues = true;
			}
			else if(Target[Target.length() - 1] == '-')
			{
				TheAction.PositiveValues = false;
			}
			else
			{
				LogConsole("> Invalid key identifier (Missing + or - at end; Example: JOYAXISX+)");

				return;
			};

			int32 Index = Target.find("AXIS") + strlen("AXIS");
			Target = Target.substr(Index, Target.length() - Index - 1);

			bool Found = false;
			uint32 Counter = 0;

			for(InputCenter::InfoNameMap::iterator it = InputCenter::JoystickAxisInfo::Names.begin(); it != InputCenter::JoystickAxisInfo::Names.end();
				it++, Counter++)
			{
				if(it->second == Target)
				{
					Found = true;

					TheAction.Index = JoyIndex;
					TheAction.SecondaryIndex = Counter;

					break;
				};
			};

			if(!Found)
			{
				LogConsole("> Invalid key identifier (Incorrect axis '" + Target + "')");

				return;
			};
		}
		else if(Target.find("JOYBUTTON") == 0)
		{
			TheAction.Type = InputActionType::JoystickButton;

			uint32 Index = 0;

			if(1 != sscanf(Target.c_str(), "JOYBUTTON%u", &Index))
			{
				LogConsole("> Invalid key identifier");

				return;
			};

			TheAction.Index = 0;
			TheAction.SecondaryIndex = Index;
		}
		else if(Target.find("MOUSE") == 0)
		{
			TheAction.Type = InputActionType::MouseButton;

			Target = Target.substr(Target.find("MOUSE") + strlen("MOUSE"));

			if(Target.find("SCROLL") == 0)
			{
				Target = Target.substr(Target.find("SCROLL") + strlen("SCROLL"));

				TheAction.Type = InputActionType::MouseScroll;

				if(Target == "UP")
				{
					TheAction.PositiveValues = true;
				}
				else if(Target == "DOWN")
				{
					TheAction.PositiveValues = false;
				}
				else
				{
					LogConsole("> Invalid key identifier (Incorrect mouse scroll value '" + Target + "')");

					return;
				};
			}
			else
			{
				bool Found = false;
				uint32 Counter = 0;

				for(InputCenter::InfoNameMap::iterator it = InputCenter::MouseButtonInfo::Names.begin(); it != InputCenter::MouseButtonInfo::Names.end();
					it++, Counter++)
				{
					if(it->second == Target)
					{
						Found = true;

						TheAction.Index = Counter;

						break;
					};
				};

				if(!Found)
				{
					LogConsole("> Invalid key identifier (Incorrect button '" + Target + "')");

					return;
				};
			};
		}
		else if(Target.find("KEY") == 0)
		{
			TheAction.Type = InputActionType::Keyboard;

			Target = Target.substr(Target.find("KEY") + strlen("KEY"));

			bool Found = false;
			uint32 Counter = 0;

			for(InputCenter::InfoNameMap::iterator it = InputCenter::KeyInfo::Names.begin(); it != InputCenter::KeyInfo::Names.end();
				it++, Counter++)
			{
				if(it->second == Target)
				{
					Found = true;

					TheAction.Index = Counter;

					break;
				};
			};

			if(!Found)
			{
				LogConsole("> Invalid key identifier (Incorrect key '" + Target + "')");

				return;
			};
		}
		else
		{
			LogConsole("> Invalid key identifier");
		};

		RendererManager::Instance.Input.RegisterAction(TheAction);

		LogConsole("[" + Action + "] => [" + Target + "]");
	};

	void Console::ScreenshotCommand(const std::vector<std::string> &Parameters)
	{
		bool WasShowing = RendererManager::Instance.ShowConsole;
		//Hide console
		RendererManager::Instance.ShowConsole = false;

		//Get a full frame twice due to double buffering
		RendererManager::Instance.RequestFrame();
		RendererManager::Instance.RequestFrame();

		Renderer *TargetRenderer = RendererManager::Instance.ActiveRenderer();

		TextureBuffer t;

		t.CreateEmpty((uint32)TargetRenderer->Size().x, (uint32)TargetRenderer->Size().y);

		if(!TargetRenderer->CaptureScreen(&t.Data[0], t.Data.size()))
		{
			LogConsole("Failed to take screenshot: Unable to capture screen");

			return;
		};

		t.FlipY();

		//Restore console
		RendererManager::Instance.ShowConsole = WasShowing;

		FileStream Out;

		std::string ScreenFileName = StringUtils::Replace(GameInterface::Instance->GameName(),
			" ", "_") + "_" + StringUtils::Replace(StringUtils::Replace(StringUtils::Replace(GameClock::Instance.CurrentDateTimeAsString(), ":", "_"), "/", "_"), " ", "_") + ".png";

		if(!Out.Open(FileSystemUtils::PreferredStorageDirectory() + "/" + ScreenFileName, StreamFlags::Write))
		{
			LogConsole("Failed to take screenshot: Unable to open '" + ScreenFileName + "' for writing");

			return;
		};

		TextureEncoderInfo TEInfo;
		TEInfo.Encoder = TextureEncoderType::PNG;

		if(!t.Save(&Out, TEInfo))
		{
			LogConsole("Failed to take screenshot: Unable to save '" + ScreenFileName + "'");

			return;
		};

		LogConsole("Successfully took screenshot '" + ScreenFileName + "'");
	};

#endif

	void Console::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK();

		SuperSmartPointer<ConsoleCommand> VersionCommandVar(new ConsoleCommand());
		VersionCommandVar->Name = "version";
		VersionCommandVar->Method.Connect(this, &Console::VersionCommand);

		RegisterCommand(VersionCommandVar);

		SuperSmartPointer<ConsoleCommand> HelpCommand(new ConsoleCommand());
		HelpCommand->Name = "help";
		HelpCommand->Method.Connect(this, &Console::HelpCommand);

		RegisterCommand(HelpCommand);

#if USE_GRAPHICS
		SuperSmartPointer<ConsoleCommand> BindCommand(new ConsoleCommand());
		BindCommand->Name = "bind";
		BindCommand->Method.Connect(this, &Console::BindCommand);

		RegisterCommand(BindCommand);

		SuperSmartPointer<ConsoleCommand> KeyCommand(new ConsoleCommand());
		KeyCommand->Name = "keys";
		KeyCommand->Method.Connect(this, &Console::ValidKeyCommand);

		RegisterCommand(KeyCommand);

		SuperSmartPointer<ConsoleCommand> ScreenshotCommand(new ConsoleCommand());
		ScreenshotCommand->Name = "screenshot";
		ScreenshotCommand->Method.Connect(this, &Console::ScreenshotCommand);

		RegisterCommand(ScreenshotCommand);

		RegisterVariable(ConsoleVariable("r_drawuirects", (uint32)0));
		RegisterVariable(ConsoleVariable("r_drawuifocuszones", (uint32)0));
		RegisterVariable(ConsoleVariable("r_drawui", (uint32)1));
		RegisterVariable(ConsoleVariable("r_drawrenderstats", (uint32)0));
#endif
	};

	void Console::Update(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();
	};

	void Console::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		ConsoleCommands.clear();
		ConsoleVariables.clear();
	};

	void Console::RegisterVariable(const ConsoleVariable &Variable)
	{
		for(uint32 i = 0; i < ConsoleVariables.size(); i++)
		{
			if(ConsoleVariables[i].Name == Variable.Name)
			{
				Log::Instance.LogWarn(TAG, "Unable to register ConVar '%s': Already exists", Variable.Name.c_str());

				return;
			};
		};

		ConsoleVariables.push_back(Variable);
	};

	ConsoleVariable *Console::GetVariable(const std::string &Name)
	{
		for(uint32 i = 0; i < ConsoleVariables.size(); i++)
		{
			if(ConsoleVariables[i].Name == Name)
				return &ConsoleVariables[i];
		};

		return NULL;
	};

	void Console::RegisterCommand(SuperSmartPointer<ConsoleCommand> Command)
	{
		for(uint32 i = 0; i < ConsoleCommands.size(); i++)
		{
			if(ConsoleCommands[i]->Name == Command->Name)
			{
				Log::Instance.LogWarn(TAG, "Unable to register ConCmd '%s': Already exists", Command->Name.c_str());

				return;
			};
		};

		ConsoleCommands.push_back(Command);
	};

	void Console::LogConsole(const std::string &Message)
	{
		Log::Instance.LogInfo(TAG, "%s", Message.c_str());

		if(Message.find('\n') != std::string::npos)
		{
			std::vector<std::string> Lines = StringUtils::Split(Message, '\n');

			for(uint32 i = 0; i < Lines.size(); i++)
			{
				ConsoleLog.push_back(Lines[i]);
			};
		}
		else
		{
			ConsoleLog.push_back(Message);
		};
	};

	void Console::RunConsoleCommand(const std::string &ConsoleText)
	{
		LogConsole(ConsoleText);

		std::string ActualConsoleText = ConsoleText;
		std::string Command = ActualConsoleText;

		int32 Index = ActualConsoleText.find(' ');

		std::vector<std::string> ParameterStrings;

		if(Index != std::wstring::npos)
		{
			Command = ActualConsoleText.substr(0, Index);

			std::string Parameters = ActualConsoleText.substr(Index + 1);

			bool InsideString = false;
			bool Escaping = false;
			std::string CurrentParameter;

			for(uint32 i = 0; i < Parameters.size(); i++)
			{
				if(Escaping)
				{
					Escaping = false;
					CurrentParameter.push_back(Parameters[i]);

					continue;
				};

				if(Parameters[i] == '\\')
				{
					Escaping = true;

					continue;
				};

				if(Parameters[i] == '\"')
				{
					if(InsideString)
					{
						ParameterStrings.push_back(CurrentParameter);

						InsideString = false;

						CurrentParameter.clear();
					}
					else
					{
						InsideString = true;

						if(CurrentParameter.length())
						{
							ParameterStrings.push_back(CurrentParameter);

							CurrentParameter.clear();
						};
					};

					continue;
				};

				if(Parameters[i] == ' ' && !InsideString)
				{
					if(CurrentParameter.length())
					{
						ParameterStrings.push_back(CurrentParameter);

						CurrentParameter.clear();
					};

					continue;
				};

				CurrentParameter.push_back(Parameters[i]);
			};

			if(CurrentParameter.length())
				ParameterStrings.push_back(CurrentParameter);
		};

		bool Found = false;

		for(uint32 i = 0; i < ConsoleCommands.size(); i++)
		{
			if(ConsoleCommands[i]->Name == Command)
			{
				Found = true;

				ConsoleCommands[i]->Method(ParameterStrings);

				break;
			};
		};

		if(!Found)
		{
			for(uint32 i = 0; i < ConsoleVariables.size(); i++)
			{
				if(ConsoleVariables[i].Name == Command)
				{
					Found = true;

					switch(ConsoleVariables[i].Type)
					{
					case ConsoleVariableType::UInt:
						if(ParameterStrings.size())
						{
							uint32 Value = 0;

							if(1 != sscanf(ParameterStrings[0].c_str(), "%u", &Value))
							{
								ConsoleVariables[i].UintValue = 0;
							}
							else
							{
								ConsoleVariables[i].UintValue = Value;
							};
						};

						LogConsole(std::string("> ") + StringUtils::MakeIntString(ConsoleVariables[i].UintValue));

						break;

					case ConsoleVariableType::Int:
						if(ParameterStrings.size())
						{
							int32 Value = 0;

							if(1 != sscanf(ParameterStrings[0].c_str(), "%d", &Value))
							{
								ConsoleVariables[i].IntValue = 0;
							}
							else
							{
								ConsoleVariables[i].IntValue = Value;
							};
						};

						LogConsole(std::string("> ") + StringUtils::MakeIntString(ConsoleVariables[i].IntValue));

						break;

					case ConsoleVariableType::Float:
						if(ParameterStrings.size())
						{
							f32 Value = 0;

							if(1 != sscanf(ParameterStrings[0].c_str(), "%f", &Value))
							{
								ConsoleVariables[i].FloatValue = 0;
							}
							else
							{
								ConsoleVariables[i].FloatValue = Value;
							};
						};

						LogConsole(std::string("> ") + StringUtils::MakeFloatString(ConsoleVariables[i].FloatValue));

						break;

					case ConsoleVariableType::String:
						if(ParameterStrings.size())
							ConsoleVariables[i].StringValue = ParameterStrings[0];

						LogConsole(std::string("> ") + ConsoleVariables[i].StringValue);

						break;
					};

					break;
				};
			};
		};

		if(!Found)
		{
			LogConsole("Invalid Command or Variable");
		};
	};
};

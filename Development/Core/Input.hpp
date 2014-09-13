#pragma once
namespace InputActionType
{
	enum
	{
		Keyboard,
		MouseButton,
		MouseScroll,
		JoystickButton,
		JoystickAxis,
		Sequence
	};
};

#define JOYSTICKDEADZONE 30

/*!
*	Input Center
*/
class InputCenter
{
	friend class RendererManager;
public:
	typedef std::map<uint32, std::string> InfoNameMap;

	/*!
	*	Keyboard Key Information
	*/
	class KeyInfo
	{
    public:
		uint16 Name;
		bool Alt, Control, Shift;
		bool Pressed, JustPressed, JustReleased, FirstPress;

		static InfoNameMap Names;

		KeyInfo() : Name(0), Alt(false), Control(false), Shift(false),
			Pressed(false), JustPressed(false), JustReleased(false), FirstPress(false) {};

		std::string NameAsString() const;
	};

	/*!
	*	Mouse Button Information
	*/
	class MouseButtonInfo
	{
    public:
		uint8 Name;
		bool Pressed, JustPressed, JustReleased, FirstPress;

		static InfoNameMap Names;

		MouseButtonInfo() : Pressed(false), JustPressed(false), JustReleased(false), FirstPress(false) {};

		std::string NameAsString() const;
	};

	/*!
	*	Joystick Button Information
	*/
	class JoystickButtonInfo
	{
    public:
		uint8 Name, JoystickIndex;
		bool Pressed, JustPressed, JustReleased, FirstPress;

		JoystickButtonInfo() : Pressed(false), JustPressed(false), JustReleased(false), FirstPress(false) {};

		std::string NameAsString() const;
	};

	/*!
	*	Joystick Axis Information
	*/
	class JoystickAxisInfo
	{
    public:
		uint8 Name, JoystickIndex;
		f32 Position; //[-1, 1]

		static InfoNameMap Names;

		JoystickAxisInfo() {};

		std::string NameAsString() const;
	};

	KeyInfo Keys[sf::Keyboard::KeyCount];
	MouseButtonInfo MouseButtons[sf::Mouse::ButtonCount];
	JoystickButtonInfo JoystickButtons[sf::Joystick::Count][sf::Joystick::ButtonCount];
	JoystickAxisInfo JoystickAxis[sf::Joystick::Count][sf::Joystick::AxisCount];

	/*!
	*	Input Action class
	*/
	class Action
	{
	public:
		std::string Name;
		/*!
		*	One of InputActionType::*
		*/
		uint32 Type;
		uint32 Index, SecondaryIndex;
		//Used for JoyAxis and MouseScroll
		bool PositiveValues;

		//Fill in Index/SecondaryIndex/Type/PositiveValues for these actions only
		std::vector<Action> Sequence;
		uint64 MaxTimeBetweenSequenceKeyPresses, LastSequenceTime;
		uint8 CurrentSequenceIndex;

		Action() : Type(InputActionType::Keyboard), Index(0), SecondaryIndex(0), MaxTimeBetweenSequenceKeyPresses(0),
			LastSequenceTime(0), CurrentSequenceIndex(0) {};
		KeyInfo *Key() const;
		MouseButtonInfo *MouseButton() const;
		JoystickButtonInfo *JoystickButton() const;
		JoystickAxisInfo *JoystickAxis() const;
		std::string AsString() const;
	};

	/*!
	*	Input Context
	*/
	class Context
	{
	public:
		std::string Name;

		virtual ~Context() {};
		//Return true to stop propagating
		virtual bool OnKey(const KeyInfo &Key) = 0;
		//Return true to stop propagating
		virtual bool OnMouseButton(const MouseButtonInfo &Button) = 0;
		//Return true to stop propagating
		virtual bool OnJoystickButton(const JoystickButtonInfo &Button) = 0;
		//Return true to stop propagating
		virtual bool OnJoystickAxis(const JoystickAxisInfo &Axis) = 0;
		virtual void OnJoystickConnected(uint8 Index) = 0;
		virtual void OnJoystickDisconnected(uint8 Index) = 0;
		virtual void OnMouseMove(const Vector3 &Position) = 0;
		virtual void OnCharacterEntered(wchar_t Character) = 0;
		virtual void OnAction(const Action &TheAction) = 0;
		virtual void OnGainFocus() = 0;
		virtual void OnLoseFocus() = 0;
	};

	class ScriptedContext : public Context
	{
	public:
		luabind::object OnKeyFunction, OnMouseButtonFunction, OnJoystickButtonFunction, OnJoystickAxisFunction,
			OnJoystickConnectedFunction, OnJoystickDisconnectedFunction, OnMouseMoveFunction, OnCharacterEnteredFunction,
			OnActionFunction, OnGainFocusFunction, OnLoseFocusFunction;

		ScriptedContext() {};

		ScriptedContext(const std::string &_Name)
		{
			Name = _Name;
		};

		bool OnKey(const KeyInfo &Key)
		{
			if(OnKeyFunction)
			{
				try
				{
					return ProtectedLuaCast<bool>(OnKeyFunction(Key));
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnKey: Scripting Error: " + e.what());
				};
			};

			return false;
		};

		bool OnMouseButton(const MouseButtonInfo &Button)
		{
			if(OnMouseButtonFunction)
			{
				try
				{
					return ProtectedLuaCast<bool>(OnMouseButtonFunction(Button));
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnMouseButton: Scripting Error: " + e.what());
				};
			};

			return false;
		};

		bool OnJoystickButton(const JoystickButtonInfo &Button)
		{
			if(OnJoystickButtonFunction)
			{
				try
				{
					return ProtectedLuaCast<bool>(OnJoystickButtonFunction(Button));
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnJoystickButton: Scripting Error: " + e.what());
				};
			};

			return false;
		};

		bool OnJoystickAxis(const JoystickAxisInfo &Axis)
		{
			if(OnJoystickAxisFunction)
			{
				try
				{
					return ProtectedLuaCast<bool>(OnJoystickAxisFunction(Axis));
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnJoystickAxis: Scripting Error: " + e.what());
				};
			};

			return false;
		};

		void OnJoystickConnected(uint8 Index)
		{
			if(OnJoystickConnectedFunction)
			{
				try
				{
					OnJoystickConnectedFunction(Index);
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnJoystickConnected: Scripting Error: " + e.what());
				};
			};
		};

		void OnJoystickDisconnected(uint8 Index)
		{
			if(OnJoystickDisconnectedFunction)
			{
				try
				{
					OnJoystickDisconnectedFunction(Index);
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnJoystickDisconnected: Scripting Error: " + e.what());
				};
			};
		};

		void OnMouseMove(const Vector3 &Position)
		{
			if(OnMouseMoveFunction)
			{
				try
				{
					OnMouseMoveFunction(Position);
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnMouseMove: Scripting Error: " + e.what());
				};
			};
		};

		void OnCharacterEntered(wchar_t Character)
		{
			if(OnCharacterEnteredFunction)
			{
				try
				{
					OnCharacterEnteredFunction((uint32)Character);
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnCharacterEntered: Scripting Error: " + e.what());
				};
			};
		};

		void OnAction(const Action &TheAction)
		{
			if(OnActionFunction)
			{
				try
				{
					OnActionFunction(TheAction);
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnAction: Scripting Error: " + e.what());
				};
			};
		};

		void OnGainFocus()
		{
			if(OnGainFocusFunction)
			{
				try
				{
					OnGainFocusFunction();
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnGainFocus: Scripting Error: " + e.what());
				};
			};
		};

		void OnLoseFocus()
		{
			if(OnLoseFocusFunction)
			{
				try
				{
					OnLoseFocusFunction();
				}
				catch(std::exception &e)
				{
					LuaScriptManager::Instance.LogError("InputContext[" + Name + "] OnLoseFocus: Scripting Error: " + e.what());
				};
			};
		};
	};

	/*!
	*	Whether we have focus
	*/
	bool HasFocus;
	/*!
	*	Current Mouse Position
	*/
	Vector2 MousePosition;
	/*!
	*	Last Typed character
	*/
	wchar_t Character;
	/*!
	*	Current Mouse Wheel Value
	*/
	long MouseWheel;
	/*!
	*	Per-frame Mouse Movement (X,Y = Position, Z = Wheel)
	*/
	Vector3 MouseMovement;

	/*!
	*	Center the mouse cursor
	*	\param Renderer the Renderer that should be centered
	*/
	void CenterMouse(void *Renderer);

	/*!
	*	\return Whether the current input event was consumed by something else
	*/
	bool InputConsumed();

	/*!
	*	Consumes input for this event
	*/
	void ConsumeInput();

	/*!
	*	Registers an action
	*	\param TheAction the action to register (name must be uppercase)
	*/
	void RegisterAction(const Action &TheAction);

	/*!
	*	Unregisters an action
	*	\param Name the action name (Uppercase)
	*/
	void UnregisterAction(const std::string &Name);

	/*!
	*	Gets an action by ID
	*	\param Name the action name as a string ID
	*	\return the action, or NULL
	*/
	const Action *const GetAction(StringID Name);

	/*!
	*	Gets an Input Context
	*	\param Name the input context as a string ID
	*	\return the context, or NULL
	*/
	const Context *const GetContext(StringID Name);

	/*!
	*	Adds an Input Context
	*	\param TheContext the input context to add
	*/
	void AddContext(SuperSmartPointer<Context> TheContext);

	/*!
	*	Enables an Input Context
	*	\param Name the input context's name as a string ID
	*	\param ToFront whether this context will need to handle everything before other contexts
	*/
	void EnableContext(StringID Name, bool ToFront = false);

	/*!
	*	Disables an Input Context
	*	\param Name the input context's name as a string ID
	*/
	void DisableContext(StringID Name);

	/*!
	*	Clears all registered contexts
	*/
	void ClearContexts()
	{
		Contexts.clear();
	};

private:
	typedef std::map<StringID, Action> ActionMap;
	ActionMap Actions;

	typedef std::map<StringID, SuperSmartPointer<Context> > ContextMap;
	ContextMap Contexts;

	std::vector<StringID> EnabledContexts;

	bool InputConsumedValue;

	InputCenter();
	bool Update(void *Renderer);
	void FireAction(const JoystickAxisInfo &Axis);
	void FireAction(const JoystickButtonInfo &Button);
	void FireAction(const MouseButtonInfo &Button);
	void FireAction(const KeyInfo &Key);
	void FireAction(f32 MouseScrollDelta);
};

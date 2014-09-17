#pragma once

/*!
*	Input Action Type Enumeration
*/
namespace InputActionType
{
	enum
	{
		Keyboard, //!<Keyboard Action
		MouseButton, //!<Mouse Button Action
		MouseScroll, //!<Mouse Scroll Action
		JoystickButton, //!<Joystick Button Action
		JoystickAxis, //!<Joystick Axis Action
		Sequence //!<Sequence Action
	};
};

/*!
*	Input Keys Enumeration
*	\note Taken from SFML
*/
namespace InputKey
{
	enum
	{
        A = 0, //!< The A key
        B, //!< The B key
        C, //!< The C key
        D, //!< The D key
        E, //!< The E key
        F, //!< The F key
        G, //!< The G key
        H, //!< The H key
        I, //!< The I key
        J, //!< The J key
        K, //!< The K key
        L, //!< The L key
        M, //!< The M key
        N, //!< The N key
        O, //!< The O key
        P, //!< The P key
        Q, //!< The Q key
        R, //!< The R key
        S, //!< The S key
        T, //!< The T key
        U, //!< The U key
        V, //!< The V key
        W, //!< The W key
        X, //!< The X key
        Y, //!< The Y key
        Z, //!< The Z key
        Num0, //!< The 0 key
        Num1, //!< The 1 key
        Num2, //!< The 2 key
        Num3, //!< The 3 key
        Num4, //!< The 4 key
        Num5, //!< The 5 key
        Num6, //!< The 6 key
        Num7, //!< The 7 key
        Num8, //!< The 8 key
        Num9, //!< The 9 key
        Escape, //!< The Escape key
        LControl, //!< The left Control key
        LShift, //!< The left Shift key
        LAlt, //!< The left Alt key
        LSystem, //!< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
        RControl, //!< The right Control key
        RShift, //!< The right Shift key
        RAlt, //!< The right Alt key
        RSystem, //!< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
        Menu, //!< The Menu key
        LBracket, //!< The [ key
        RBracket, //!< The ] key
        SemiColon, //!< The ; key
        Comma, //!< The , key
        Period, //!< The . key
        Quote, //!< The ' key
        Slash, //!< The / key
        BackSlash, //!< The \ key
        Tilde, //!< The ~ key
        Equal, //!< The = key
        Dash, //!< The - key
        Space, //!< The Space key
        Return, //!< The Return key
        BackSpace, //!< The Backspace key
        Tab, //!< The Tabulation key
        PageUp, //!< The Page up key
        PageDown, //!< The Page down key
        End, //!< The End key
        Home, //!< The Home key
        Insert, //!< The Insert key
        Delete, //!< The Delete key
        Add, //!< The + key
        Subtract, //!< The - key
        Multiply, //!< The * key
        Divide, //!< The / key
        Left, //!< Left arrow
        Right, //!< Right arrow
        Up, //!< Up arrow
        Down, //!< Down arrow
        Numpad0, //!< The numpad 0 key
        Numpad1, //!< The numpad 1 key
        Numpad2, //!< The numpad 2 key
        Numpad3, //!< The numpad 3 key
        Numpad4, //!< The numpad 4 key
        Numpad5, //!< The numpad 5 key
        Numpad6, //!< The numpad 6 key
        Numpad7, //!< The numpad 7 key
        Numpad8, //!< The numpad 8 key
        Numpad9, //!< The numpad 9 key
        F1, //!< The F1 key
        F2, //!< The F2 key
        F3, //!< The F3 key
        F4, //!< The F4 key
        F5, //!< The F5 key
        F6, //!< The F6 key
        F7, //!< The F7 key
        F8, //!< The F8 key
        F9, //!< The F9 key
        F10, //!< The F10 key
        F11, //!< The F11 key
        F12, //!< The F12 key
        F13, //!< The F13 key
        F14, //!< The F14 key
        F15, //!< The F15 key
        Pause, //!< The Pause key
		Count
	};
};

/*!
*	Input Mouse Buttons Enumeration
*	\note Taken from SFML
*/
namespace InputMouseButton
{
	enum
	{
        Left, //!< The left mouse button
        Right, //!< The right mouse button
        Middle, //!< The middle (wheel) mouse button
        XButton1, //!< The first extra mouse button
        XButton2, //!< The second extra mouse button
		Count
	};
};

/*!
*	Input Joystick Axis Enumeration
*	\note Taken from SFML
*/
namespace InputJoystickAxis
{
	enum
	{
        X, //!< The X axis
        Y, //!< The Y axis
        Z, //!< The Z axis
        R, //!< The R axis
        U, //!< The U axis
        V, //!< The V axis
        PovX, //!< The X axis of the point-of-view hat
        PovY, //!< The Y axis of the point-of-view hat
		Count
	};
};

class Renderer;

/*!
*	Input Center
*/
class InputCenter
{
	friend class RendererManager;
public:
	static const uint32 JoystickCount = 8;
	static const uint32 JoystickButtonCount = 32;

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

	KeyInfo Keys[InputKey::Count];
	MouseButtonInfo MouseButtons[InputMouseButton::Count];
	JoystickButtonInfo JoystickButtons[JoystickCount][JoystickButtonCount];
	JoystickAxisInfo JoystickAxis[JoystickCount][InputJoystickAxis::Count];

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
	f32 MouseWheel;
	/*!
	*	Per-frame Mouse Movement (X,Y = Position, Z = Wheel)
	*/
	Vector3 MouseMovement;

	/*!
	*	Center the mouse cursor
	*	\param Renderer the Renderer that should be centered
	*/
	void CenterMouse(Renderer *TheRenderer);

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
	bool Update(Renderer *TheRenderer);
	void FireAction(const JoystickAxisInfo &Axis);
	void FireAction(const JoystickButtonInfo &Button);
	void FireAction(const MouseButtonInfo &Button);
	void FireAction(const KeyInfo &Key);
	void FireAction(f32 MouseScrollDelta);
};

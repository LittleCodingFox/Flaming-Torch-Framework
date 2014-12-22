/*!
*	UI Event Types
*/
namespace UIEventType
{
	enum UIEventType
	{
		MouseJustPressed = 0,
		MousePressed,
		MouseReleased,
		KeyJustPressed,
		KeyPressed,
		KeyReleased,
		TouchJustPressed,
		TouchPressed,
		TouchReleased,
		TouchDragged,
		Click,
		Tap,
		JoystickTap,
		JoystickButtonJustPressed,
		JoystickButtonPressed,
		JoystickButtonReleased,
		JoystickAxisMoved,
		JoystickConnected,
		JoystickDisconnected,
		GainedFocus,
		LostFocus,
		MouseMoved,
		CharacterEntered,
		MouseEntered,
		MouseLeft,
		MouseOver,
		Start,
		Update,
		Draw,
		Count
	};
};

/*!
*	UI Input Types
*/
namespace UIInputType
{
	enum UIInputType
	{
		Mouse = FLAGVALUE(0),
		Keyboard = FLAGVALUE(1),
		Touch = FLAGVALUE(2),
		Joystick = FLAGVALUE(3),
		All = Mouse | Keyboard | Touch | Joystick
	};
};

/*!
*	Runs UI Script Events on a Lua Event Group
*	\param Group the Event Group
*	\param ID the ID of the Element
*	\param ErroredBefore whether we had a previous error. Used for easier coding.
*	\return Whether an error occurred to prevent log spamming
*/
template<typename... Args>
bool RunUIScriptEvents(LuaEventGroup &Group, const std::string &ID, bool ErroredBefore, Args... Parameters)
{
	if(ErroredBefore)
		return false;

	for (std::list<luabind::object>::iterator it = Group.Members.begin(); it != Group.Members.end(); it++)
	{
		if (*it)
		{
			try
			{
				(*it)(Parameters...);
			}
			catch (std::exception &e)
			{
				Log::Instance.LogInfo("UIManager", "Scripting Exception on UI Element '%s'", ID.c_str()); 

				return false;
			};
		};
	};

	return true;
};

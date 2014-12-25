#if USE_GRAPHICS
/*!
*	Renderer Event Type
*/
namespace RendererEventType
{
	enum RendererEventType
	{
		JoystickConnected, //!<Joystick Connected
		JoystickDisconnected, //!Joystick Disconnected
		JoystickButtonPressed, //!<Joystick Button Pressed
		JoystickButtonReleased, //!<Joystick Button Released
		JoystickAxisMoved, //!<Joystick Axis Moved
		MouseMoved, //!<Mouse Moved
		MouseButtonPressed, //!<Mouse Button Pressed
		MouseButtonReleased, //!<Mouse Button Released
		MouseDeltaMoved, //!<Mouse Delta (Wheel) Moved
		TouchDown, //!<Touched Down (Pressed finger on screen)
		TouchUp, //!<Touched Up (Released finger from screen)
		TouchDrag, //!<Dragged Touch (Moved finger on screen)
		KeyPressed, //!<Key Pressed
		KeyReleased, //!<Key Released
		CharacterEntered, //!<Character Entered
		WindowGotFocus, //!<Window Gained Focus 
		WindowLostFocus, //!<Window Lost Focus
		WindowResized, //!<Window was Resized
		WindowClosed, //!<Window was Closed
		MouseLeft, //!<Mouse has left
		MouseEntered //!<Mouse has entered
	};
};

/*!
*	Renderer Event class
*/
class RendererEvent
{
public:
	/*!
	*	Event Type (one of RendererEventType::*)
	*/
	uint8 Type;

	/*!
	*	Mouse Position
	*/
	Vector2 MousePosition;

	/*!
	*	Window Size
	*/
	Vector2 WindowSize;

	/*!
	*	Mouse Wheel Delta
	*/
	f32 MouseDelta;

	/*!
	*	Joystick Axis Position [-1, 1]
	*/
	f32 JoystickAxisPosition;

	/*!
	*	Last Typed Character
	*/
	wchar_t TypedCharacter;

	/*!
	*	Current Keypress Keycode
	*/
	uint8 KeyCode;

	/*!
	*	Current Mouse Button Index
	*/
	uint8 MouseButtonIndex;

	/*!
	*	Joystick Axis Index
	*/
	uint8 JoystickAxisIndex;

	/*!
	*	Joystick Button Index
	*/
	uint8 JoystickButtonIndex;

	/*!
	*	Joystick Index
	*/
	uint8 JoystickIndex;

	/*!
	*	Touch Index
	*/
	uint8 TouchIndex;

	/*!
	*	Touch Position
	*/
	Vector2 TouchPosition;

	RendererEvent() : Type(0), MouseDelta(0), JoystickAxisPosition(0), TypedCharacter(0), KeyCode(0), MouseButtonIndex(0), JoystickAxisIndex(0), JoystickButtonIndex(0), JoystickIndex(0), TouchIndex(0) {};
};
#endif

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "InputCenter"

	InputCenter::InfoNameMap InputCenter::KeyInfo::Names;
	InputCenter::InfoNameMap InputCenter::JoystickAxisInfo::Names;
	InputCenter::InfoNameMap InputCenter::MouseButtonInfo::Names;
	InputCenter::InfoNameMap InputCenter::TouchInfo::Names;

	std::string InputCenter::KeyInfo::NameAsString() const
	{
		return Names.find(Name) != Names.end() ? Names[Name] : "UNKNOWN";
	};

	std::string InputCenter::TouchInfo::NameAsString() const
	{
		return Names.find(Index) != Names.end() ? Names[Index] : "UNKNOWN";
	};

	std::string InputCenter::MouseButtonInfo::NameAsString() const
	{
		return Names.find(Name) != Names.end() ? Names[Name] : "UNKNOWN";
	};

	std::string InputCenter::JoystickButtonInfo::NameAsString() const
	{
		return "BUTTON" + StringUtils::MakeIntString((uint32)Name);
	};

	std::string InputCenter::JoystickAxisInfo::NameAsString() const
	{
		return Names.find(Name) != Names.end() ? "AXIS" + Names[Name] : "UNKNOWN";
	};

	InputCenter::InputCenter() : HasFocus(true), InputConsumedValue(false), Character(false), MouseWheel(false)
	{
	};

	void InputCenter::Initialize()
	{
		for(wchar_t i = 0; i < InputKey::Count; i++)
		{
			Keys[i].Name = i;
		};

		for(uint8 i = 0; i < InputMouseButton::Count; i++)
		{
			MouseButtons[i].Name = i;
		};

		for(uint32 i = 0; i < JoystickCount; i++)
		{
			for(uint8 j = 0; j < JoystickButtonCount; j++)
			{
				JoystickButtons[i][j].Name = j;
				JoystickButtons[i][j].JoystickIndex = (uint8)i;
			};

			for(uint8 j = 0; j < InputJoystickAxis::Count; j++)
			{
				JoystickAxis[i][j].Name = j;
				JoystickAxis[i][j].JoystickIndex = (uint8)i;
			};
		};

		Touches[0].Index = 0;
		Touches[1].Index = 1;
		Touches[2].Index = 2;
		Touches[3].Index = 3;
		Touches[4].Index = 4;
		Touches[5].Index = 5;
		Touches[6].Index = 6;
		Touches[7].Index = 7;
		Touches[8].Index = 8;
		Touches[9].Index = 9;

		MouseWheel = 0;

#define REGISTER_KEYBOARD_NAME(name)\
	KeyInfo::Names[InputKey::name] = StringUtils::ToUpperCase(#name);

#define REGISTER_MOUSEBUTTON_NAME(name)\
	MouseButtonInfo::Names[InputMouseButton::name] = StringUtils::ToUpperCase(#name);

#define REGISTER_JOYAXIS_NAME(name)\
	JoystickAxisInfo::Names[InputJoystickAxis::name] = StringUtils::ToUpperCase(#name);

#define REGISTER_TOUCH_NAME(name)\
	TouchInfo::Names[name] = StringUtils::ToUpperCase(#name);

		REGISTER_KEYBOARD_NAME(A);
		REGISTER_KEYBOARD_NAME(B);
        REGISTER_KEYBOARD_NAME(C);
        REGISTER_KEYBOARD_NAME(D);
        REGISTER_KEYBOARD_NAME(E);
        REGISTER_KEYBOARD_NAME(F);
        REGISTER_KEYBOARD_NAME(G);
        REGISTER_KEYBOARD_NAME(H);
        REGISTER_KEYBOARD_NAME(I);
        REGISTER_KEYBOARD_NAME(J);
        REGISTER_KEYBOARD_NAME(K);
        REGISTER_KEYBOARD_NAME(L);
        REGISTER_KEYBOARD_NAME(M);
        REGISTER_KEYBOARD_NAME(N);
        REGISTER_KEYBOARD_NAME(O);
        REGISTER_KEYBOARD_NAME(P);
        REGISTER_KEYBOARD_NAME(Q);
        REGISTER_KEYBOARD_NAME(R);
        REGISTER_KEYBOARD_NAME(S);
        REGISTER_KEYBOARD_NAME(T);
        REGISTER_KEYBOARD_NAME(U);
        REGISTER_KEYBOARD_NAME(V);
        REGISTER_KEYBOARD_NAME(W);
        REGISTER_KEYBOARD_NAME(X);
        REGISTER_KEYBOARD_NAME(Y);
        REGISTER_KEYBOARD_NAME(Z);
        REGISTER_KEYBOARD_NAME(Num0);
        REGISTER_KEYBOARD_NAME(Num1);
        REGISTER_KEYBOARD_NAME(Num2);
        REGISTER_KEYBOARD_NAME(Num3);
        REGISTER_KEYBOARD_NAME(Num4);
        REGISTER_KEYBOARD_NAME(Num5);
        REGISTER_KEYBOARD_NAME(Num6);
        REGISTER_KEYBOARD_NAME(Num7);
        REGISTER_KEYBOARD_NAME(Num8);
        REGISTER_KEYBOARD_NAME(Num9);
        REGISTER_KEYBOARD_NAME(Escape);
        REGISTER_KEYBOARD_NAME(LControl);
        REGISTER_KEYBOARD_NAME(LShift);
        REGISTER_KEYBOARD_NAME(LAlt);
        REGISTER_KEYBOARD_NAME(LSystem);
        REGISTER_KEYBOARD_NAME(RControl);
        REGISTER_KEYBOARD_NAME(RShift);
        REGISTER_KEYBOARD_NAME(RAlt);
        REGISTER_KEYBOARD_NAME(RSystem);
        REGISTER_KEYBOARD_NAME(Menu);
        REGISTER_KEYBOARD_NAME(LBracket);
        REGISTER_KEYBOARD_NAME(RBracket);
        REGISTER_KEYBOARD_NAME(SemiColon);
        REGISTER_KEYBOARD_NAME(Comma);
        REGISTER_KEYBOARD_NAME(Period);
        REGISTER_KEYBOARD_NAME(Quote);
        REGISTER_KEYBOARD_NAME(Slash);
        REGISTER_KEYBOARD_NAME(BackSlash);
        REGISTER_KEYBOARD_NAME(Tilde);
        REGISTER_KEYBOARD_NAME(Equal);
        REGISTER_KEYBOARD_NAME(Dash);
        REGISTER_KEYBOARD_NAME(Space);
        REGISTER_KEYBOARD_NAME(Return);
        REGISTER_KEYBOARD_NAME(BackSpace);
        REGISTER_KEYBOARD_NAME(Tab);
        REGISTER_KEYBOARD_NAME(PageUp);
        REGISTER_KEYBOARD_NAME(PageDown);
        REGISTER_KEYBOARD_NAME(End);
        REGISTER_KEYBOARD_NAME(Home);
        REGISTER_KEYBOARD_NAME(Insert);
        REGISTER_KEYBOARD_NAME(Delete);
        REGISTER_KEYBOARD_NAME(Add);
        REGISTER_KEYBOARD_NAME(Subtract);
        REGISTER_KEYBOARD_NAME(Multiply);
        REGISTER_KEYBOARD_NAME(Divide);
        REGISTER_KEYBOARD_NAME(Left);
        REGISTER_KEYBOARD_NAME(Right);
        REGISTER_KEYBOARD_NAME(Up);
        REGISTER_KEYBOARD_NAME(Down);
        REGISTER_KEYBOARD_NAME(Numpad0);
        REGISTER_KEYBOARD_NAME(Numpad1);
        REGISTER_KEYBOARD_NAME(Numpad2);
        REGISTER_KEYBOARD_NAME(Numpad3);
        REGISTER_KEYBOARD_NAME(Numpad4);
        REGISTER_KEYBOARD_NAME(Numpad5);
        REGISTER_KEYBOARD_NAME(Numpad6);
        REGISTER_KEYBOARD_NAME(Numpad7);
        REGISTER_KEYBOARD_NAME(Numpad8);
        REGISTER_KEYBOARD_NAME(Numpad9);
        REGISTER_KEYBOARD_NAME(F1);
        REGISTER_KEYBOARD_NAME(F2);
        REGISTER_KEYBOARD_NAME(F3);
        REGISTER_KEYBOARD_NAME(F4);
        REGISTER_KEYBOARD_NAME(F5);
        REGISTER_KEYBOARD_NAME(F6);
        REGISTER_KEYBOARD_NAME(F7);
        REGISTER_KEYBOARD_NAME(F8);
        REGISTER_KEYBOARD_NAME(F9);
        REGISTER_KEYBOARD_NAME(F10);
        REGISTER_KEYBOARD_NAME(F11);
        REGISTER_KEYBOARD_NAME(F12);
        REGISTER_KEYBOARD_NAME(F13);
        REGISTER_KEYBOARD_NAME(F14);
        REGISTER_KEYBOARD_NAME(F15);
        REGISTER_KEYBOARD_NAME(Pause);

		REGISTER_MOUSEBUTTON_NAME(Left);
		REGISTER_MOUSEBUTTON_NAME(Middle);
		REGISTER_MOUSEBUTTON_NAME(Right);
		REGISTER_MOUSEBUTTON_NAME(XButton1);
		REGISTER_MOUSEBUTTON_NAME(XButton2);

		REGISTER_JOYAXIS_NAME(X);
		REGISTER_JOYAXIS_NAME(Y);
		REGISTER_JOYAXIS_NAME(Z);
		REGISTER_JOYAXIS_NAME(R);
		REGISTER_JOYAXIS_NAME(U);
		REGISTER_JOYAXIS_NAME(V);
		REGISTER_JOYAXIS_NAME(PovX);
		REGISTER_JOYAXIS_NAME(PovY);

		REGISTER_TOUCH_NAME(0);
		REGISTER_TOUCH_NAME(1);
		REGISTER_TOUCH_NAME(2);
		REGISTER_TOUCH_NAME(3);
		REGISTER_TOUCH_NAME(4);
		REGISTER_TOUCH_NAME(5);
		REGISTER_TOUCH_NAME(6);
		REGISTER_TOUCH_NAME(7);
		REGISTER_TOUCH_NAME(8);
		REGISTER_TOUCH_NAME(9);
	};

	bool InputCenter::Update(Renderer *TheRenderer)
	{
		FLASSERT(TheRenderer != NULL, "Invalid renderer passed to an InputCenter!");

		if(TheRenderer == NULL)
			return false;

		for(uint32 i = 0; i < InputKey::Count; i++)
		{
			Keys[i].JustPressed = Keys[i].JustReleased = Keys[i].FirstPress = false;
		};

		for(uint32 i = 0; i < InputMouseButton::Count; i++)
		{
			MouseButtons[i].JustPressed = MouseButtons[i].JustReleased = MouseButtons[i].FirstPress = false;
		};

		for(uint32 i = 0; i < JoystickCount; i++)
		{
			for(uint8 j = 0; j < JoystickButtonCount; j++)
			{
				JoystickButtons[i][j].JustPressed = JoystickButtons[i][j].JustReleased = JoystickButtons[i][j].FirstPress = false;
			};
		};

		for(TouchMap::iterator it = Touches.begin(); it != Touches.end(); it++)
		{
			it->second.Dragged = it->second.JustPressed = it->second.JustReleased = false;
		};

		Character = L'\0';

		MouseMovement = Vector3(MousePosition.x, MousePosition.y, (float)MouseWheel);

		MouseWheel = 0;

		static RendererEvent Event;

		bool HasPendingResize = false, HadFocus = HasFocus;
		Vector2 PendingResizeSize;

		while(TheRenderer->PollEvent(Event))
		{
			InputConsumedValue = false;

			switch(Event.Type)
			{
			case RendererEventType::JoystickButtonPressed:
				{
					JoystickButtonInfo &Button = JoystickButtons[Event.JoystickIndex][Event.JoystickButtonIndex];

					if(Button.Pressed == false)
						Button.FirstPress = true;

					Button.Pressed = Button.JustPressed = true;
				};

				break;

			case RendererEventType::JoystickButtonReleased:
				{
					JoystickButtonInfo &Button = JoystickButtons[Event.JoystickIndex][Event.JoystickButtonIndex];

					Button.Pressed = false;
					Button.JustReleased = true;
				};

				break;

			case RendererEventType::JoystickConnected:
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnJoystickConnected((uint8)Event.JoystickIndex);
				};

				break;

			case RendererEventType::JoystickDisconnected:
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnJoystickDisconnected((uint8)Event.JoystickIndex);
				};

				break;

			case RendererEventType::JoystickAxisMoved:
				{
					JoystickAxisInfo &Axis = JoystickAxis[Event.JoystickIndex][Event.JoystickAxisIndex];
					Axis.Position = Event.JoystickAxisPosition;
				};

				break;

			case RendererEventType::MouseLeft:
				for(uint32 i = 0; i < InputMouseButton::Count; i++)
				{
					MouseButtons[i].Pressed = MouseButtons[i].JustPressed = false;
					MouseButtons[i].JustReleased = true;
				};

#if FLPLATFORM_MOBILE
				HasFocus = false;
#endif

				break;

			case RendererEventType::MouseEntered:
				for(uint32 i = 0; i < InputMouseButton::Count; i++)
				{
					//TODO: Pool for input somehow
					MouseButtons[i].Pressed = MouseButtons[i].JustPressed = false;
				};

#if FLPLATFORM_MOBILE
				HasFocus = true;
#endif

				break;

			case RendererEventType::TouchDown:
				Touches[Event.TouchIndex].JustPressed = Touches[Event.TouchIndex].Pressed = true;
				Touches[Event.TouchIndex].Position = Event.TouchPosition;

				break;

			case RendererEventType::TouchUp:
				Touches[Event.TouchIndex].JustPressed = Touches[Event.TouchIndex].Pressed = false;
				Touches[Event.TouchIndex].JustReleased = true;
				Touches[Event.TouchIndex].Position = Event.TouchPosition;

				break;

			case RendererEventType::TouchDrag:
				Touches[Event.TouchIndex].Pressed = Touches[Event.TouchIndex].Dragged = true;
				Touches[Event.TouchIndex].DragDifference = Event.TouchPosition - Touches[Event.TouchIndex].Position;
				Touches[Event.TouchIndex].Position = Event.TouchPosition;

				break;

			case RendererEventType::KeyPressed:
				{
					KeyInfo &Key = Keys[Event.KeyCode];

					if(Key.Pressed == false)
						Key.FirstPress = true;

					Key.Pressed = Key.JustPressed = true;
				};

				break;

			case RendererEventType::KeyReleased:
				{
					KeyInfo &Key = Keys[Event.KeyCode];

					Key.Pressed = Key.JustPressed = false;
					Key.JustReleased = true;
				};

				break;

			case RendererEventType::MouseMoved:
				MousePosition = Event.MousePosition;

				break;

			case RendererEventType::MouseDeltaMoved:
				MouseWheel = Event.MouseDelta;

				break;

			case RendererEventType::MouseButtonPressed:
				{
					MouseButtonInfo &Button = MouseButtons[Event.MouseButtonIndex];

					if(Button.Pressed == false)
						Button.FirstPress = true;

					Button.Pressed = Button.JustPressed = true;
				};

				break;

			case RendererEventType::MouseButtonReleased:
				{
					MouseButtonInfo &Button = MouseButtons[Event.MouseButtonIndex];
					Button.Pressed = Button.JustPressed = false;
					Button.JustReleased = true;
				};

				break;

			case RendererEventType::WindowClosed:
				return false;

				break;

			case RendererEventType::CharacterEntered:
				Character = Event.TypedCharacter;

				break;

			case RendererEventType::WindowResized:
				HasPendingResize = true;
				PendingResizeSize = Event.WindowSize;

				break;

			case RendererEventType::WindowGotFocus:
				HasFocus = true;

				break;

			case RendererEventType::WindowLostFocus:
				HasFocus = false;

				break;
			};
		};

		if(HasPendingResize)
		{
			TheRenderer->OnResized(TheRenderer, (uint32)PendingResizeSize.x, (uint32)PendingResizeSize.y);
		};

		if(HadFocus != HasFocus)
		{
			for(uint32 i = 0; i < EnabledContexts.size(); i++)
			{
				HasFocus ? Contexts[EnabledContexts[i]]->OnGainFocus() : Contexts[EnabledContexts[i]]->OnLoseFocus();
			};
		};

		if(Character != L'\0')
		{
			for(uint32 i = 0; i < EnabledContexts.size(); i++)
			{
				Contexts[EnabledContexts[i]]->OnCharacterEntered(Character);
			};
		};

		InputConsumedValue = false;

		Vector3 NewFrameMouseMovement(MousePosition.x, MousePosition.y, (f32)MouseWheel);
		MouseMovement = NewFrameMouseMovement - MouseMovement;

		if(MouseMovement != Vector3(0, 0, 0))
		{
			for(uint32 i = 0; i < EnabledContexts.size(); i++)
			{
				Contexts[EnabledContexts[i]]->OnMouseMove(MouseMovement);

				if(MouseMovement.z != 0 && !InputConsumed())
				{
					FireAction(MouseMovement.z);
				};
			};
		};

		InputConsumedValue = false;

		for(uint32 i = 0; i < InputKey::Count; i++)
		{
			if(Keys[i].JustPressed || Keys[i].Pressed || Keys[i].JustReleased)
			{
				bool IgnoreAction = false;

				for(uint32 j = 0; j < EnabledContexts.size(); j++)
				{
					if(Contexts[EnabledContexts[j]]->OnKey(Keys[i]))
					{
						IgnoreAction = true;

						break;
					};
				};

				if(!IgnoreAction)
					FireAction(Keys[i]);
			};
		};

		InputConsumedValue = false;

		for(TouchMap::iterator it = Touches.begin(); it != Touches.end(); it++)
		{
			if(it->second.JustPressed || it->second.Pressed || it->second.Dragged || it->second.JustReleased)
			{
				bool IgnoreAction = false;

				for(uint32 j = 0; j < EnabledContexts.size(); j++)
				{
					if(Contexts[EnabledContexts[j]]->OnTouch(it->second))
					{
						IgnoreAction = true;

						break;
					};
				};

				if(!IgnoreAction)
					FireAction(it->second);
			};
		};

		InputConsumedValue = false;

		for(uint32 i = 0; i < InputMouseButton::Count; i++)
		{
			if(MouseButtons[i].JustPressed || MouseButtons[i].Pressed || MouseButtons[i].JustReleased)
			{
				bool IgnoreAction = false;

				for(uint32 j = 0; j < EnabledContexts.size(); j++)
				{
					if(Contexts[EnabledContexts[j]]->OnMouseButton(MouseButtons[i]))
					{
						IgnoreAction = true;

						break;
					};
				};

				if(!IgnoreAction)
					FireAction(MouseButtons[i]);
			};
		};

		InputConsumedValue = false;

		for(uint32 i = 0; i < JoystickCount; i++)
		{
			for(uint32 j = 0; j < JoystickButtonCount; j++)
			{
				JoystickButtonInfo &Button = JoystickButtons[i][j];

				if(Button.JustPressed || Button.JustReleased || Button.Pressed)
				{
					bool IgnoreAction = false;

					for(uint32 k = 0; k < EnabledContexts.size(); k++)
					{
						if(Contexts[EnabledContexts[k]]->OnJoystickButton(Button))
						{
							IgnoreAction = true;

							break;
						};
					};

					if(!IgnoreAction)
						FireAction(Button);
				};
			};

			for(uint32 j = 0; j < InputJoystickAxis::Count; j++)
			{
				JoystickAxisInfo &Axis = JoystickAxis[i][j];

				//Maybe this condition is not needed?
				if(Axis.Position != 0)
				{
					bool IgnoreAction = false;

					for(uint32 k = 0; k < EnabledContexts.size(); k++)
					{
						if(Contexts[EnabledContexts[k]]->OnJoystickAxis(Axis))
						{
							IgnoreAction = true;

							break;
						};
					};

					if(!IgnoreAction)
						FireAction(Axis);
				};
			};
		};

		InputConsumedValue = false;

		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::Sequence)
			{
				if(it->second.CurrentSequenceIndex == it->second.Sequence.size() && it->second.CurrentSequenceIndex > 0)
				{
					for(uint32 i = 0; i < EnabledContexts.size(); i++)
					{
						Contexts[EnabledContexts[i]]->OnAction(it->second);
					};

					it->second.CurrentSequenceIndex = 0;
				}
				else if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.CurrentSequenceIndex = 0;
				};
			};
		};

		return true;
	};

	void InputCenter::CenterMouse(Renderer *TheRenderer)
	{
		TheRenderer->SetMousePosition(TheRenderer->Size() / 2);

		RendererManager::Instance.Input.MousePosition = TheRenderer->Size() / 2;
	};

	bool InputCenter::InputConsumed()
	{
		return InputConsumedValue;
	};

	void InputCenter::ConsumeInput()
	{
		InputConsumedValue = true;
	};

	void InputCenter::RegisterAction(const Action &TheAction)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Index == TheAction.Index && it->second.SecondaryIndex == TheAction.SecondaryIndex && it->second.Type == TheAction.Type)
			{
				if((it->second.Type == InputActionType::JoystickAxis || it->second.Type == InputActionType::MouseScroll) &&
					it->second.PositiveValues != TheAction.PositiveValues)
					continue;

				Actions.erase(it);
				it = Actions.begin();

				if(it == Actions.end())
					break;
			};
		};

		Actions[MakeStringID(TheAction.Name)] = TheAction;
	};

	void InputCenter::UnregisterAction(const std::string &Name)
	{
		ActionMap::iterator it = Actions.find(MakeStringID(Name));

		if(it != Actions.end())
		{
			Actions.erase(it);

			Log::Instance.LogDebug(TAG, "Removed action '%s'", Name.c_str());
		};
	};

	void InputCenter::FireAction(const JoystickAxisInfo &Axis)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::JoystickAxis && it->second.SecondaryIndex == Axis.Name)
			{
				if((it->second.PositiveValues && Axis.Position > 0) || (!it->second.PositiveValues && Axis.Position < 0))
				{
					for(uint32 i = 0; i < EnabledContexts.size(); i++)
					{
						Contexts[EnabledContexts[i]]->OnAction(it->second);
					};

					return;
				};
			}
			else if(fabs(Axis.Position) >= 0.8f && it->second.Type == InputActionType::Sequence)
			{
				if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.LastSequenceTime = GameClockTimeNoPause();
					it->second.CurrentSequenceIndex = 0;
				};
				
				if(it->second.CurrentSequenceIndex < it->second.Sequence.size() &&
					it->second.Sequence[it->second.CurrentSequenceIndex].Type == InputActionType::JoystickAxis &&
					((it->second.Sequence[it->second.CurrentSequenceIndex].PositiveValues && Axis.Position > 0) ||
					(!it->second.Sequence[it->second.CurrentSequenceIndex].PositiveValues && Axis.Position < 0)))
				{
					if(it->second.Sequence[it->second.CurrentSequenceIndex].SecondaryIndex == Axis.Name &&
						GameClockDiffNoPause(it->second.LastSequenceTime) < it->second.MaxTimeBetweenSequenceKeyPresses)
					{
						it->second.CurrentSequenceIndex++;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					}
					else
					{
						it->second.CurrentSequenceIndex = 0;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					};

					return;
				};
			};
		};
	};

	void InputCenter::FireAction(const JoystickButtonInfo &Button)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::JoystickButton && it->second.SecondaryIndex == Button.Name)
			{
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					it->second.Index = Button.JoystickIndex;

					Contexts[EnabledContexts[i]]->OnAction(it->second);
				};

				return;
			}
			else if(Button.FirstPress && it->second.Type == InputActionType::Sequence)
			{
				if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.LastSequenceTime = GameClockTimeNoPause();
					it->second.CurrentSequenceIndex = 0;
				};

				if(it->second.CurrentSequenceIndex < it->second.Sequence.size() &&
					it->second.Sequence[it->second.CurrentSequenceIndex].Type == InputActionType::JoystickButton)
				{
					if(it->second.Sequence[it->second.CurrentSequenceIndex].Index == Button.Name &&
						GameClockDiffNoPause(it->second.LastSequenceTime) < it->second.MaxTimeBetweenSequenceKeyPresses)
					{
						it->second.CurrentSequenceIndex++;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					}
					else
					{
						it->second.CurrentSequenceIndex = 0;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					};

					return;
				};
			};
		};
	};

	void InputCenter::FireAction(const MouseButtonInfo &Button)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::MouseButton && it->second.Index == Button.Name)
			{
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnAction(it->second);
				};

				return;
			}
			else if(Button.FirstPress && it->second.Type == InputActionType::Sequence)
			{
				if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.LastSequenceTime = GameClockTimeNoPause();
					it->second.CurrentSequenceIndex = 0;
				};

				if(it->second.CurrentSequenceIndex < it->second.Sequence.size() &&
					it->second.Sequence[it->second.CurrentSequenceIndex].Type == InputActionType::MouseButton)
				{
					if(it->second.Sequence[it->second.CurrentSequenceIndex].Index == Button.Name &&
						GameClockDiffNoPause(it->second.LastSequenceTime) < it->second.MaxTimeBetweenSequenceKeyPresses)
					{
						it->second.CurrentSequenceIndex++;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					}
					else
					{
						it->second.CurrentSequenceIndex = 0;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					};

					return;
				};
			};
		};
	};
	
	void InputCenter::FireAction(const TouchInfo &Touch)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Index != Touch.Index)
				continue;

			if(((it->second.Type == InputActionType::TouchDown || it->second.Type == InputActionType::TouchDrag) && (Touch.JustPressed || Touch.Pressed)) ||
				(it->second.Type == InputActionType::TouchUp && Touch.JustReleased))
			{
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnAction(it->second);
				};

				return;
			};
		};
	};

	void InputCenter::FireAction(f32 MouseScrollDelta)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::MouseScroll && ((it->second.PositiveValues && MouseScrollDelta > 0) ||
				(!it->second.PositiveValues && MouseScrollDelta < 0)))
			{
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnAction(it->second);
				};

				return;
			}
			else if(it->second.Type == InputActionType::Sequence)
			{
				if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.LastSequenceTime = GameClockTimeNoPause();
					it->second.CurrentSequenceIndex = 0;
				};

				if(it->second.CurrentSequenceIndex < it->second.Sequence.size() &&
					it->second.Sequence[it->second.CurrentSequenceIndex].Type == InputActionType::MouseScroll)
				{
					if(((it->second.Sequence[it->second.CurrentSequenceIndex].PositiveValues && MouseScrollDelta > 0) ||
						(!it->second.Sequence[it->second.CurrentSequenceIndex].PositiveValues && MouseScrollDelta < 0)) &&
						GameClockDiffNoPause(it->second.LastSequenceTime) < it->second.MaxTimeBetweenSequenceKeyPresses)
					{
						it->second.CurrentSequenceIndex++;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					}
					else
					{
						it->second.CurrentSequenceIndex = 0;
						it->second.LastSequenceTime = GameClockTimeNoPause();
					};

					return;
				};
			};
		};
	};

	void InputCenter::FireAction(const KeyInfo &Key)
	{
		for(ActionMap::iterator it = Actions.begin(); it != Actions.end(); it++)
		{
			if(it->second.Type == InputActionType::Keyboard && it->second.Index == Key.Name)
			{
				for(uint32 i = 0; i < EnabledContexts.size(); i++)
				{
					Contexts[EnabledContexts[i]]->OnAction(it->second);
				};

				return;
			}
			else if(Key.FirstPress && it->second.Type == InputActionType::Sequence && it->second.CurrentSequenceIndex < it->second.Sequence.size() &&
				it->second.Sequence[it->second.CurrentSequenceIndex].Type == InputActionType::Keyboard)
			{
				if(GameClockDiffNoPause(it->second.LastSequenceTime) >= it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.LastSequenceTime = GameClockTimeNoPause();
					it->second.CurrentSequenceIndex = 0;
				};

				if(it->second.Sequence[it->second.CurrentSequenceIndex].Index == Key.Name &&
					GameClockDiffNoPause(it->second.LastSequenceTime) < it->second.MaxTimeBetweenSequenceKeyPresses)
				{
					it->second.CurrentSequenceIndex++;
					it->second.LastSequenceTime = GameClockTimeNoPause();
				}
				else
				{
					it->second.CurrentSequenceIndex = 0;
					it->second.LastSequenceTime = GameClockTimeNoPause();
				};

				return;
			};
		};
	};

	const InputCenter::Action *const InputCenter::GetAction(StringID Name)
	{
		ActionMap::iterator it = Actions.find(Name);

		return it != Actions.end() ? &it->second : NULL;
	};

	InputCenter::KeyInfo *InputCenter::Action::Key() const
	{
		return Type == InputActionType::Keyboard ? &RendererManager::Instance.Input.Keys[Index] : NULL;
	};

	InputCenter::TouchInfo *InputCenter::Action::Touch() const
	{
		return (Type == InputActionType::TouchDown || Type == InputActionType::TouchUp || Type == InputActionType::TouchDrag) ?
			&RendererManager::Instance.Input.Touches[Index] : NULL;
	};

	InputCenter::MouseButtonInfo *InputCenter::Action::MouseButton() const
	{
		return Type == InputActionType::MouseButton ? &RendererManager::Instance.Input.MouseButtons[Index] : NULL;
	};

	InputCenter::JoystickButtonInfo *InputCenter::Action::JoystickButton() const
	{
		return Type == InputActionType::JoystickButton ? &RendererManager::Instance.Input.JoystickButtons[Index][SecondaryIndex] : NULL;
	};

	InputCenter::JoystickAxisInfo *InputCenter::Action::JoystickAxis() const
	{
		return Type == InputActionType::JoystickAxis ? &RendererManager::Instance.Input.JoystickAxis[Index][SecondaryIndex] : NULL;
	};

	std::string InputCenter::Action::AsString() const
	{
		switch(Type)
		{
		case InputActionType::Keyboard:
			return Key()->NameAsString();

			break;

		case InputActionType::MouseButton:
			return "MOUSE " + MouseButton()->NameAsString();

			break;

		case InputActionType::JoystickButton:
			return JoystickButton()->NameAsString();

			break;

		case InputActionType::JoystickAxis:
			return JoystickAxis()->NameAsString() + (PositiveValues ? "+" : "-");

			break;

		case InputActionType::TouchDown:
		case InputActionType::TouchUp:
		case InputActionType::TouchDrag:
			return Touch()->NameAsString();

			break;

		case InputActionType::Sequence:
			{
				static std::stringstream str;
				str.str("");

				for(uint32 i = 0; i < Sequence.size(); i++)
				{
					str << (i > 0 ? " + " : "") << Sequence[i].AsString();
				};

				return str.str();
			};

			break;
		};

		return "UNKNOWN";
	};

	const InputCenter::Context *const InputCenter::GetContext(StringID Name)
	{
		ContextMap::iterator it = Contexts.find(Name);

		return it != Contexts.end() ? it->second.Get() : NULL;
	};

	void InputCenter::AddContext(DisposablePointer<Context> TheContext)
	{
		FLASSERT(TheContext.Get(), "Invalid Context!");

		Contexts[MakeStringID(TheContext->Name)] = TheContext;
	};

	void InputCenter::EnableContext(StringID Name, bool ToFront)
	{
		if(Contexts.find(Name) == Contexts.end())
			return;

		for(uint32 i = 0; i < EnabledContexts.size(); i++)
		{
			if(EnabledContexts[i] == Name)
				return;
		};

		if(ToFront)
		{
			EnabledContexts.insert(EnabledContexts.begin(), Name);
		}
		else
		{
			EnabledContexts.push_back(Name);
		};
	};

	void InputCenter::DisableContext(StringID Name)
	{
		for(uint32 i = 0; i < EnabledContexts.size(); i++)
		{
			if(EnabledContexts[i] == Name)
			{
				EnabledContexts.erase(EnabledContexts.begin() + i);

				return;
			};
		};
	};
#endif
};

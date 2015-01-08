#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIElement::Focus()
	{
		UIManager *Owner = Manager();

		for(UIManager::ElementMap::iterator it = Owner->Elements.begin(); it != Owner->Elements.end(); it++)
		{
			if(it->second.Get() && it->second->Element.Get() == this)
			{
				if(Owner->GetFocusedElement().Get() != this)
				{
					Owner->ClearFocus();
				}

				Owner->FocusedElementValue = it->second->Element;

				Owner->FocusedElementValue->OnEvent(UIEventType::GainedFocus, { Owner });
			}
		}
	}

	UIElement::UIElement(const std::string &NativeTypeName, UIManager *_Manager) : NativeTypeValue(NativeTypeName),
		ManagerValue(_Manager), VisibleValue(true),
		EnabledValue(true), ClickPressed(false), JoystickButtonPressed(false), IsInputBlockerValue(false), TooltipsAreFixedValue(false),
		DrawsInputBlockerBackgroundValue(true), GlobalsTracker(_Manager->ScriptInstance), RespondsToTooltipsValue(false),
		ClickTimer(0), JoystickButtonTimer(0), IDValue(0), EnabledInputsValue(UIInputType::All), PropagatesEventsValue(true)
	{
		FLASSERT(ManagerValue != NULL, "Invalid UI Manager!");
		FLASSERT(ManagerValue->ScriptInstance, "Invalid UI Manager Script!");

		for(uint32 i = 0; i < UIEventType::Count; i++)
		{
			EventErrorCache[i] = true;
		}

		OnEvent.Connect<UIElement, &UIElement::OnEventPrivate>(this);
	}

	void UIElement::SetSkin(DisposablePointer<GenericConfig> Skin)
	{
		SkinValue = Skin;
	}

	void UIElement::Update(const Vector2 &ParentPosition)
	{
		OnEvent(UIEventType::Update, { const_cast<Vector2 *>(&ParentPosition) });
	}

	void UIElement::OnEventPrivate(uint32 Type, std::vector<void *> Args)
	{
		bool FailedArgCount = false;

		switch (Type)
		{
		case UIEventType::MouseJustPressed:
			if((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			ClickTimer = GameClockTimeNoPause();
			ClickPressed = true;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::MouseButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::MousePressed:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::MouseButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::MouseReleased:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::MouseButtonInfo *>(Args[0]));

			if (ClickPressed && GameClockDiffNoPause(ClickTimer) < UIELEMENT_CLICK_INTERVAL)
			{
				ClickPressed = false;

				OnEvent(UIEventType::Click, Args);
			}

			ClickPressed = false;

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::KeyJustPressed:
			if ((EnabledInputsValue & UIInputType::Keyboard) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::KeyInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::KeyPressed:
			if ((EnabledInputsValue & UIInputType::Keyboard) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::KeyInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::KeyReleased:
			if ((EnabledInputsValue & UIInputType::Keyboard) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::KeyInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::TouchJustPressed:
			if ((EnabledInputsValue & UIInputType::Touch) == 0 || !EnabledValue)
				return;

			ClickTimer = GameClockTimeNoPause();
			ClickPressed = true;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::TouchInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::TouchPressed:
			if ((EnabledInputsValue & UIInputType::Touch) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::TouchInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::TouchReleased:
			if ((EnabledInputsValue & UIInputType::Touch) == 0 || !EnabledValue)
				return;

			if (ClickPressed && GameClockDiffNoPause(ClickTimer) < UIELEMENT_CLICK_INTERVAL)
			{
				ClickPressed = false;

				OnEvent(UIEventType::Click, Args);
			}

			ClickPressed = false;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::TouchInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::TouchDragged:
			if ((EnabledInputsValue & UIInputType::Touch) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::TouchInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::Click:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::MouseButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::Tap:
			if ((EnabledInputsValue & UIInputType::Touch) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::TouchInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickTap:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::JoystickButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickButtonJustPressed:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			JoystickButtonTimer = GameClockTimeNoPause();
			JoystickButtonPressed = true;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::JoystickButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickButtonPressed:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::JoystickButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickButtonReleased:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			if (JoystickButtonPressed && GameClockDiffNoPause(JoystickButtonTimer) < UIELEMENT_CLICK_INTERVAL)
			{
				JoystickButtonPressed = false;

				OnEvent(UIEventType::JoystickTap, Args);
			}

			JoystickButtonPressed = false;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::JoystickButtonInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickAxisMoved:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, static_cast<const InputCenter::JoystickAxisInfo *>(Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickConnected:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, *static_cast<uint8 * > (Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::JoystickDisconnected:
			if ((EnabledInputsValue & UIInputType::Joystick) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, *static_cast<uint8 * > (Args[0]));

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::GainedFocus:
			if (!EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::LostFocus:
			if (!EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::MouseMoved:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			if (PropagatesEvents() && !RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
				ParentValue->OnEvent(Type, Args);

			break;

		case UIEventType::CharacterEntered:
			if ((EnabledInputsValue & UIInputType::Keyboard) == 0 || !EnabledValue)
				return;

			{
				std::string Temp;
				Temp.resize(1);
				Temp[0] = *static_cast<char *>(Args[0]);

				FailedArgCount = Args.size() != 1;
				EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *, std::string>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this, Temp);
			}

			break;

		case UIEventType::MouseEntered:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::MouseLeft:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::MouseOver:
			if ((EnabledInputsValue & UIInputType::Mouse) == 0 || !EnabledValue)
				return;

			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::Start:
			FailedArgCount = Args.size() != 0;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *>(EventScriptHandlers[Type], NameValue, !EventErrorCache[Type], this);

			break;

		case UIEventType::Update:
		case UIEventType::Draw:
			FailedArgCount = Args.size() != 1;
			EventErrorCache[Type] = !FailedArgCount && RunUIScriptEvents<UIElement *, const Vector2 &>(EventScriptHandlers[Type], NameValue, 
				!EventErrorCache[Type], this, *static_cast<const Vector2 *>(Args[0]));

			break;
		}
	}

	void UIElement::OnConstructed()
	{
		EnabledValue = VisibleValue = true;
		ClickPressed = IsInputBlockerValue = RespondsToTooltipsValue = JoystickButtonPressed = false;

		PropertyValues = luabind::newtable(Manager()->ScriptInstance->State);
	}

	void UIElement::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		OnEvent(UIEventType::Draw, { const_cast<Vector2 *>(&ParentPosition) });
	}

	void UIElement::DrawUIFocusZone(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		if(Manager()->DrawUIFocusZones)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = Size();

			Vector2 PanelOffset(3, 3);

			Vector2 ActualPosition = ParentPosition + Position() + Offset() + PanelOffset;

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize - PanelOffset;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize - PanelOffset * 2).Color(Vector4(0, 0, 0.5f, 1));
			
			TheSprite.Draw(Renderer);

			uint32 ParentCounter = 0;

			DisposablePointer<UIElement> ParentPanel = ParentValue;

			f32 IndicatorPosition = 0;

			Vector2 GlobalPosition = this->ParentPosition();

			while(ParentPanel.Get())
			{
				if(ParentPanel->ParentPosition() == GlobalPosition)
					IndicatorPosition += RenderTextUtils::MeasureTextSimple(Renderer, StringUtils::MakeIntString(ParentCounter), TextParams()).Size().x + 5;

				ParentCounter++;

				ParentPanel = ParentPanel->ParentValue;
			}

			RenderTextUtils::RenderText(Renderer, StringUtils::MakeIntString(ParentCounter), TextParams().Position(AABB.min.ToVector2() + Vector2(IndicatorPosition, 0)));
		}
	}

	void UIElement::DrawUIRect(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		if(Manager()->DrawUIRects)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = Size();

			Vector2 ActualPosition = ParentPosition + Position() + Offset();

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize).Color(Vector4(1, 0.75f, 0, 0.75f));
			
			TheSprite.Draw(Renderer);
		}
	}

	void UIElement::Clear()
	{
		while(ChildrenValue.size())
		{
			uint32 OldSize = ChildrenValue.size();

			Manager()->RemoveElement(ChildrenValue[0]->ID());

			uint32 NewSize = ChildrenValue.size();

			if(NewSize == OldSize)
				ChildrenValue.erase(ChildrenValue.begin());
		}
	}

	UIElement::~UIElement()
	{
		if(ParentValue)
			ParentValue->RemoveChild(DisposablePointer<UIElement>::MakeWeak(this));

		Manager()->RemoveElement(IDValue);

		Clear();

		if(TooltipElementValue.Get())
			TooltipElementValue.Dispose();

		Properties.clear();

		GlobalsTracker.Clear();
	}

	void UIElement::SetPropagatesEvents(bool Value)
	{
		PropagatesEventsValue = Value;
	}

	bool UIElement::PropagatesEvents() const
	{
		return PropagatesEventsValue;
	}

	void UIElement::SetInputBlockerBackground(bool Value)
	{
		DrawsInputBlockerBackgroundValue = Value;
	}

	bool UIElement::InputBlockerBackground() const
	{
		return DrawsInputBlockerBackgroundValue;
	}

	void UIElement::SetRespondsToTooltips(bool Value)
	{
		RespondsToTooltipsValue = Value;
	}

	void UIElement::SetTooltipsFixed(bool Value)
	{
		TooltipsAreFixedValue = Value;
	}

	bool UIElement::TooltipsFixed() const
	{
		return TooltipsAreFixedValue;
	}

	void UIElement::SetTooltipsPosition(const Vector2 &Position)
	{
		TooltipPositionValue = Position;
	}

	const Vector2 &UIElement::TooltipsPosition() const
	{
		return TooltipPositionValue;
	}

	void UIElement::SetEnabledInputTypes(uint32 Types)
	{
		EnabledInputsValue = Types;
	}

	uint32 UIElement::EnabledInputTypes() const
	{
		return EnabledInputsValue;
	}

	const std::string &UIElement::Name() const
	{
		return NameValue;
	}

	const std::string &UIElement::NativeType() const
	{
		return NativeTypeValue;
	}

	const Vector2 &UIElement::Offset() const
	{
		return OffsetValue;
	}

	void UIElement::SetOffset(const Vector2 &Offset)
	{
		OffsetValue = Offset;
	}

	DisposablePointer<UIElement> UIElement::TooltipElement() const
	{
		return TooltipElementValue;
	}

	bool UIElement::BlockingInput() const
	{
		return IsInputBlockerValue;
	}

	void UIElement::SetBlockingInput(bool Value)
	{
		IsInputBlockerValue = Value;
	}

	void UIElement::SetTooltipElement(DisposablePointer<UIElement> Element)
	{
		TooltipElementValue = Element;
	}

	UIManager *UIElement::Manager() const
	{
		return ManagerValue;
	}

	StringID UIElement::ID() const
	{
		return IDValue;
	}

	Vector2 UIElement::ParentPosition() const
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position = PositionValue + OffsetValue;
		DisposablePointer<UIElement> Parent = ParentValue;

		while(Parent.Get())
		{
			Position += Parent->Position() + Parent->Offset();

			Parent = Parent->Parent();
		}

		return Position;
	}

	void UIElement::SetVisible(bool value)
	{
		VisibleValue = value;
	}

	bool UIElement::Visible() const
	{
		return VisibleValue;
	}

	void UIElement::SetEnabled(bool value)
	{
		EnabledValue = value;
	}

	bool UIElement::Enabled() const
	{
		return EnabledValue;
	}

	void UIElement::AddChild(DisposablePointer<UIElement> Child)
	{
		FLASSERT(Child.Get(), "Invalid Child!");

		if (Child.Get() == NULL)
			return;

		FLASSERT(Child->ManagerValue == ManagerValue, "Different UI Managers!");

		if (Child->ManagerValue != ManagerValue)
			return;

		if (Child->ParentValue)
		{
			Child->ParentValue->RemoveChild(Child);
		}

		ChildrenValue.push_back(Child);
		Child->ParentValue = Manager()->GetElement(IDValue);
	}

	void UIElement::RemoveChild(DisposablePointer<UIElement> Child)
	{
		for (std::vector<DisposablePointer<UIElement> >::iterator it = ChildrenValue.begin(); it != ChildrenValue.end(); it++)
		{
			if(*it == Child)
			{
				Child->ParentValue = DisposablePointer<UIElement>();
				ChildrenValue.erase(it);

				return;
			}
			else if ((*it).Get() == nullptr)
			{
				ChildrenValue.erase(it);

				it = ChildrenValue.begin();

				if(it == ChildrenValue.end())
					return;
			}
		}
	}

	uint32 UIElement::ChildrenCount() const
	{
		return ChildrenValue.size();
	}

	DisposablePointer<UIElement> UIElement::Child(uint32 Index) const
	{
		return ChildrenValue.size() > Index ? ChildrenValue[Index] : DisposablePointer<UIElement>();
	}

	DisposablePointer<UIElement> UIElement::Parent() const
	{
		return ParentValue;
	}

	const Vector2 &UIElement::Position() const
	{
		return PositionValue;
	}

	void UIElement::SetPosition(const Vector2 &Position)
	{
		PositionValue = Position;
	}

	const Vector2 &UIElement::Size() const
	{
		return SizeValue;
	}

	void UIElement::SetSize(const Vector2 &Size)
	{
		SizeValue = Size;

		if(SizeValue.x < 0)
			SizeValue.x = 0;

		if(SizeValue.y < 0)
			SizeValue.y = 0;
	}

	Vector2 UIElement::ChildrenSize() const
	{
		Vector2 Out, Size;

		for (uint32 i = 0; i < ChildrenValue.size(); i++)
		{
			if (!ChildrenValue[i]->Visible())
				continue;

			Size = ChildrenValue[i]->Position() + ChildrenValue[i]->Offset() + ChildrenValue[i]->Size();

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		}

		return Out;
	}

	UILayout *UIElement::Layout() const
	{
		return LayoutValue;
	}

	bool UIElement::RespondsToTooltips() const
	{
		return RespondsToTooltipsValue && TooltipElementValue.Get();
	}
#endif
}

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	class UIFadeInAnimation : public UIAnimation
	{
	public:
		UIFadeInAnimation(UIPanel *Tar, uint64 _Duration) : UIAnimation(Tar)
		{
			Duration = _Duration;
		};

		bool Update()
		{
			Target->SetAlpha(MathUtils::Clamp(GameClockDiffNoPause(StartTime) / (f32)Duration));

			return Target->Alpha() == 1;
		};

		void Finish()
		{
			Target->SetAlpha(1);
		};
	};

	class UIFadeOutAnimation : public UIAnimation
	{
	public:
		UIFadeOutAnimation(UIPanel *Tar, uint64 _Duration) : UIAnimation(Tar)
		{
			Duration = _Duration;
		};

		bool Update()
		{
			Target->SetAlpha(1 - (MathUtils::Clamp(GameClockDiffNoPause(StartTime) / (f32)Duration)));

			return Target->Alpha() == 0;
		};

		void Finish()
		{
			Target->SetAlpha(0);
		};
	};

	UIPanel *UIPanel::ClearAnimations()
	{
		if(AnimationQueue.size())
		{
			(*AnimationQueue.begin())->Finish();

			AnimationQueue.clear();
		};

		return this;
	};

	
	UIPanel *UIPanel::FadeIn(uint64 Duration)
	{
		AnimationQueue.push_back(SuperSmartPointer<UIAnimation>(new UIFadeInAnimation(this, Duration)));

		return this;
	};

	
	UIPanel *UIPanel::FadeOut(uint64 Duration)
	{
		AnimationQueue.push_back(SuperSmartPointer<UIAnimation>(new UIFadeOutAnimation(this, Duration)));

		return this;
	};

	void UIPanel::Focus()
	{
		UIManager *Owner = Manager();

		for(UIManager::ElementMap::iterator it = Owner->Elements.begin(); it != Owner->Elements.end(); it++)
		{
			if(it->second.Get() && it->second->Panel.Get() == this)
			{
				if(Owner->GetFocusedElement().Get() != this)
				{
					Owner->ClearFocus();
				};

				Owner->FocusedElementValue = it->second->Panel;

				Owner->FocusedElementValue->OnGainFocusPriv();
			};
		};
	};

	UIPanel::UIPanel(const std::string &NativeTypeName, UIManager *_Manager) : NativeType(NativeTypeName),
		ManagerValue(_Manager), VisibleValue(true),
		EnabledValue(true), MouseInputValue(true), KeyboardInputValue(true), AlphaValue(1),
		ClickPressed(false), BlockingInputValue(false), IsDraggableValue(false), IsDroppableValue(false),
		DraggingValue(false), TooltipFixedValue(false), RotationValue(0), ExtraSizeScaleValue(1),
		InputBlockerBackgroundValue(true), GlobalsTracker(_Manager->ScriptInstance)
	{
		FLASSERT(ManagerValue != NULL, "Invalid UI Manager!");
		FLASSERT(ManagerValue->ScriptInstance, "Invalid UI Manager Script!");

		OnMouseJustPressed.Connect(this, &UIPanel::OnMouseJustPressedDraggable);
		OnMouseReleased.Connect(this, &UIPanel::OnMouseReleasedDraggable);

		OnMouseJustPressed.Connect(this, &UIPanel::OnMouseJustPressedScript);
		OnMousePressed.Connect(this, &UIPanel::OnMousePressedScript);
		OnMouseReleased.Connect(this, &UIPanel::OnMouseReleasedScript);
		OnKeyJustPressed.Connect(this, &UIPanel::OnKeyJustPressedScript);
		OnKeyPressed.Connect(this, &UIPanel::OnKeyPressedScript);
		OnKeyReleased.Connect(this, &UIPanel::OnKeyReleasedScript);
		OnMouseMove.Connect(this, &UIPanel::OnMouseMovedScript);
		OnCharacterEntered.Connect(this, &UIPanel::OnCharacterEnteredScript);
		OnGainFocus.Connect(this, &UIPanel::OnGainFocusScript);
		OnLoseFocus.Connect(this, &UIPanel::OnLoseFocusScript);
		OnMouseEntered.Connect(this, &UIPanel::OnMouseEnteredScript);
		OnMouseOver.Connect(this, &UIPanel::OnMouseOverScript);
		OnMouseLeft.Connect(this, &UIPanel::OnMouseLeftScript);
		OnClick.Connect(this, &UIPanel::OnClickScript);
		OnDragBegin.Connect(this, &UIPanel::OnDragBeginScript);
		OnDragEnd.Connect(this, &UIPanel::OnDragEndScript);
		OnDragging.Connect(this, &UIPanel::OnDraggingScript);
		OnDrop.Connect(this, &UIPanel::OnDropScript);
		OnJoystickConnected.Connect(this, &UIPanel::OnJoystickConnectedScript);
		OnJoystickDisconnected.Connect(this, &UIPanel::OnJoystickDisconnectedScript);
		OnJoystickButtonJustPressed.Connect(this, &UIPanel::OnJoystickButtonJustPressedScript);
		OnJoystickButtonPressed.Connect(this, &UIPanel::OnJoystickButtonPressedScript);
		OnJoystickButtonReleased.Connect(this, &UIPanel::OnJoystickButtonReleasedScript);
		OnJoystickAxisMoved.Connect(this, &UIPanel::OnJoystickAxisMovedScript);
	};

	f32 UIPanel::ParentAlpha() const
	{
		if(!ParentValue)
			return AlphaValue;

		f32 Alpha = AlphaValue;
		UIPanel *p = const_cast<UIPanel *>(ParentValue.Get());

		while(p)
		{
			Alpha *= p->AlphaValue;

			p = p->ParentValue;
		};

		return Alpha;
	};
	
	void UIPanel::Update(const Vector2 &ParentPosition)
	{
		if(AnimationQueue.size())
		{
			UIAnimation *Animation = (*AnimationQueue.begin()).Get();

			if(Animation->StartTime == 0)
			{
				Animation->StartTime = GameClockTimeNoPause();
			};
			
			if(Animation->Update())
			{
				AnimationQueue.erase(AnimationQueue.begin());
			};
		};

		RUN_GUI_SCRIPT_EVENTS(OnUpdateFunction, (this, ParentPosition))
	};

	void UIPanel::OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		ClickTimer = GameClockTimeNoPause();
		ClickPressed = true;

		OnMouseJustPressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnMouseJustPressedPriv(o);
		};
	};

	void UIPanel::OnMousePressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnMousePressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnMousePressedPriv(o);
		};
	};

	void UIPanel::OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		if(ClickPressed && GameClockDiffNoPause(ClickTimer) < 500)
		{
			OnClick(this, o);
		};

		ClickPressed = false;

		OnMouseReleased(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnMouseReleasedPriv(o);
		};
	};

	void UIPanel::OnMouseMovePriv()
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		OnMouseMove(this);
	};

	void UIPanel::OnKeyPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnKeyPressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnKeyPressedPriv(o);
		};
	};

	void UIPanel::OnKeyJustPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnKeyJustPressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnKeyJustPressedPriv(o);
		};
	};

	void UIPanel::OnKeyReleasedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnKeyReleased(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnKeyReleasedPriv(o);
		};
	};

	void UIPanel::OnCharacterEnteredPriv()
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnCharacterEntered(this);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnCharacterEnteredPriv();
		};
	};

	void UIPanel::OnJoystickButtonPressedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnJoystickButtonPressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickButtonPressedPriv(o);
		};
	};

	void UIPanel::OnJoystickButtonJustPressedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnJoystickButtonJustPressed(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickButtonJustPressedPriv(o);
		};
	};

	void UIPanel::OnJoystickButtonReleasedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnJoystickButtonReleased(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickButtonReleasedPriv(o);
		};
	};

	void UIPanel::OnJoystickAxisMovedPriv(const InputCenter::JoystickAxisInfo &o)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		OnJoystickAxisMoved(this, o);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickAxisMovedPriv(o);
		};
	};

	void UIPanel::OnJoystickConnectedPriv(uint32 JoystickIndex)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnJoystickConnected(this, JoystickIndex);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickConnectedPriv(JoystickIndex);
		};
	};

	void UIPanel::OnJoystickDisconnectedPriv(uint32 JoystickIndex)
	{
		if(!JoystickInputValue || !EnabledValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnJoystickDisconnected(this, JoystickIndex);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnJoystickDisconnectedPriv(JoystickIndex);
		};
	};

	void UIPanel::OnLoseFocusPriv()
	{
		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnLoseFocus(this);
	};

	void UIPanel::OnGainFocusPriv()
	{
		REPORT_UIPANEL_INPUT_EVENTPRIV();

		OnGainFocus(this);
	};

	void UIPanel::OnConstructed()
	{
		EnabledValue = MouseInputValue = KeyboardInputValue = VisibleValue = true;
		AlphaValue = 1;
		ClickPressed = BlockingInputValue = RespondsToTooltipsValue = false;

		PropertyValues = luabind::newtable(Manager()->ScriptInstance->State);
	};

	void UIPanel::OnMouseJustPressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMouseJustPressedFunction, (Self, o));
	};

	void UIPanel::OnMousePressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMousePressedFunction, (Self, o));
	};

	void UIPanel::OnMouseReleasedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMouseReleasedFunction, (Self, o))
	};

	void UIPanel::OnKeyJustPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnKeyJustPressedFunction, (Self, o))
	};

	void UIPanel::OnKeyPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnKeyPressedFunction, (Self, o))
	};

	void UIPanel::OnKeyReleasedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnKeyReleasedFunction, (Self, o))
	};

	void UIPanel::OnMouseMovedScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMouseMovedFunction, (Self))
	};

	void UIPanel::OnCharacterEnteredScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnCharacterEnteredFunction, (Self, RendererManager::Instance.Input.Character))
	};

	void UIPanel::OnGainFocusScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnGainFocusFunction, (Self))
	};

	void UIPanel::OnLoseFocusScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnLoseFocusFunction, (Self))
	};

	void UIPanel::OnMouseEnteredScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMouseEnteredFunction, (Self))
	};

	void UIPanel::OnMouseOverScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnMouseOverFunction, (Self))
	};

	void UIPanel::OnMouseLeftScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnMouseLeftFunction, (Self))
	};

	void UIPanel::OnClickScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnClickFunction, (Self, o))
	};

	void UIPanel::OnDragBeginScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnDragBeginFunction, (Self))
	};

	void UIPanel::OnDragEndScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnDragEndFunction, (Self))
	};

	void UIPanel::OnDraggingScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnDraggingFunction, (Self))
	};

	void UIPanel::OnDropScript(UIPanel *Self, UIPanel *Tar)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnDropFunction, (Self, Tar))
	};
	
	void UIPanel::OnMouseJustPressedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != InputMouseButton::Left || !IsDraggableValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		DraggingValue = true;

		OnDragBegin(this);
	};

	void UIPanel::OnMouseReleasedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != InputMouseButton::Left || !IsDraggableValue)
			return;

		REPORT_UIPANEL_INPUT_EVENTPRIV();

		if(DraggingValue)
		{
			OnDragEnd(this);

			DraggingValue = false;

			SuperSmartPointer<UIPanel> Drop = RendererManager::Instance.ActiveRenderer()->UI->GetMouseOverElement();

			if(!Drop.Get() || !Drop->Droppable())
				return;

			Drop->OnDrop(Drop, this);
		};
	};

	void UIPanel::OnJoystickButtonPressedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnJoystickButtonPressedFunction, (this, o))
	};

	void UIPanel::OnJoystickButtonJustPressedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnJoystickButtonJustPressedFunction, (Self, o))
	};

	void UIPanel::OnJoystickButtonReleasedScript(UIPanel *Self, const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnJoystickButtonReleasedFunction, (Self, o))
	};

	void UIPanel::OnJoystickAxisMovedScript(UIPanel *Self, const InputCenter::JoystickAxisInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnJoystickAxisMovedFunction, (Self, o))
	};

	void UIPanel::OnJoystickConnectedScript(UIPanel *Self, uint32 JoystickIndex)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnJoystickConnectedFunction, (Self, JoystickIndex))
	};

	void UIPanel::OnJoystickDisconnectedScript(UIPanel *Self, uint32 JoystickIndex)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		REPORT_UIPANEL_INPUT_EVENT();

		RUN_GUI_SCRIPT_EVENTS(OnJoystickDisconnectedFunction, (Self, JoystickIndex))
	};

	void UIPanel::AdjustSizeAndPosition(UIPanel *PanelToStopAt)
	{
		if(this == PanelToStopAt || ParentValue == NULL)
			return;

		if(ParentValue->SizeValue.x < ComposedSize().x)
			ParentValue->SizeValue.x = ComposedSize().x;

		if(ParentValue->SizeValue.y < ComposedSize().y)
			ParentValue->SizeValue.y = ComposedSize().y;

		PositionFunction.Members.front()(this, ParentValue, Manager()->GetOwner()->Size().x, Manager()->GetOwner()->Size().y);

		ParentValue->AdjustSizeAndPosition(PanelToStopAt);
	};

	void UIPanel::Draw(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		RUN_GUI_SCRIPT_EVENTS(OnDrawFunction, (this, ParentPosition))
	};

	void UIPanel::DrawUIFocusZone(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		if(Manager()->DrawUIFocusZones)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = ComposedSize();

			Vector2 PanelOffset(3, 3);

			Vector2 ActualPosition = ParentPosition + Position() + Offset() + PanelOffset;

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize - PanelOffset;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize - PanelOffset * 2).Color(Vector4(0, 0, 0.5f, 1)).Rotation(ParentRotation());
			
			TheSprite.Draw(Renderer);

			uint32 ParentCounter = 0;

			UIPanel *ParentPanel = ParentValue;

			f32 IndicatorPosition = 0;

			Vector2 GlobalPosition = this->ParentPosition();

			while(ParentPanel != NULL)
			{
				if(ParentPanel->ParentPosition() == GlobalPosition)
					IndicatorPosition += RenderTextUtils::MeasureTextSimple(Renderer, StringUtils::MakeIntString(ParentCounter), TextParams()).ToFullSize().x + 5;

				ParentCounter++;

				ParentPanel = ParentPanel->ParentValue;
			};

			RenderTextUtils::RenderText(Renderer, StringUtils::MakeIntString(ParentCounter), TextParams().Position(AABB.min.ToVector2() + Vector2(IndicatorPosition, 0)));
		};
	};

	void UIPanel::DrawUIRect(const Vector2 &ParentPosition, Renderer *Renderer)
	{
		if(Manager()->DrawUIRects)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = ComposedSize();

			Vector2 ActualPosition = ParentPosition + Position() + Offset();

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize).Color(Vector4(1, 0.75f, 0, 0.75f)).Rotation(ParentRotation());
			
			TheSprite.Draw(Renderer);
		};
	};

	void UIPanel::AddChild(UIPanel *Child)
	{
		FLASSERT(Child->ManagerValue == ManagerValue, "Combining GUI Managers is illegal!");

		if(Child->ManagerValue != ManagerValue)
			return;

		if(Child->ParentValue)
		{
			Child->ParentValue->RemoveChild(Child);
		};

		Children.push_back(Child);
		Child->ParentValue = Manager()->GetElement(IDValue);
	};

	void UIPanel::Clear()
	{
		while(Children.size())
		{
			uint32 OldSize = Children.size();

			Manager()->RemoveElement(Children[0]->ID());

			uint32 NewSize = Children.size();

			if(NewSize == OldSize)
				Children.erase(Children.begin());
		};
	};

	UIPanel::~UIPanel()
	{
		if(ParentValue)
			ParentValue->RemoveChild(this);

		Manager()->RemoveElement(IDValue);

		Clear();

		if(TooltipElementValue.Get())
			TooltipElementValue.Dispose();

		Properties.clear();

		GlobalsTracker.Clear();
	};

	void UIPanel::SetContentPanel(SuperSmartPointer<UIPanel> Panel)
	{
		ContentPanelValue = Panel;
	};

	SuperSmartPointer<UIPanel> UIPanel::ContentPanel() const
	{
		return ContentPanelValue;
	};

	void UIPanel::SetInputBlockerBackground(bool Value)
	{
		InputBlockerBackgroundValue = Value;
	};

	bool UIPanel::InputBlockerBackground() const
	{
		return InputBlockerBackgroundValue;
	};

	f32 UIPanel::ExtraSizeScale() const
	{
		return ExtraSizeScaleValue;
	};

	void UIPanel::SetExtraSizeScale(f32 Scale)
	{
		ExtraSizeScaleValue = Scale;
	};

	void UIPanel::SetRespondsToTooltips(bool Value)
	{
		RespondsToTooltipsValue = Value;
	};

	Vector2 UIPanel::ComposedSize() const
	{
		return Size() + ScaledExtraSize();
	};

	bool UIPanel::TooltipsFixed() const
	{
		return TooltipFixedValue;
	};

	void UIPanel::SetRotation(f32 Value)
	{
		RotationValue = Value;
	};

	f32 UIPanel::Rotation() const
	{
		return RotationValue;
	};

	f32 UIPanel::ParentRotation() const
	{
		f32 Rotation = RotationValue;

		UIPanel *Parent = const_cast<UIPanel *>(ParentValue.Get());

		while(Parent != NULL)
		{
			Rotation += Parent->RotationValue;

			Parent = Parent->Parent();
		};

		return Rotation;
	};

	void UIPanel::SetTooltipsFixed(bool Value)
	{
		TooltipFixedValue = Value;
	};

	const Vector2 &UIPanel::TooltipsPosition() const
	{
		return TooltipPosition;
	};

	const Vector2 &UIPanel::Offset() const
	{
		return OffsetValue;
	};

	void UIPanel::SetOffset(const Vector2 &Offset)
	{
		OffsetValue = Offset;
	};

	void UIPanel::SetTooltipsPosition(const Vector2 &Position)
	{
		TooltipPosition = Position;
	};

	void UIPanel::PerformLayout()
	{
		for(uint32 i = 0; i < Children.size(); i++)
		{
			Children[i]->PerformLayout();
		};
	};

	const std::string &UIPanel::TooltipText() const
	{
		return TooltipValue;
	};

	SuperSmartPointer<UIPanel> UIPanel::TooltipElement() const
	{
		return TooltipElementValue;
	};

	bool UIPanel::BlockingInput() const
	{
		return BlockingInputValue;
	};

	void UIPanel::SetBlockingInput(bool Value)
	{
		BlockingInputValue = Value;
	};

	void UIPanel::SetTooltipText(const std::string &Text)
	{
		TooltipValue = Text;
	};

	void UIPanel::SetTooltipElement(SuperSmartPointer<UIPanel> Element)
	{
		TooltipElementValue = Element;
	};

	void UIPanel::SetDraggable(bool Value)
	{
		IsDraggableValue = Value;
	};

	bool UIPanel::Draggable() const
	{
		return IsDraggableValue;
	};

	void UIPanel::SetDroppable(bool Value)
	{
		IsDroppableValue = Value;
	};

	bool UIPanel::Droppable() const
	{
		return IsDroppableValue;
	};

	UIManager *UIPanel::Manager() const
	{
		return ManagerValue;
	};

	StringID UIPanel::ID() const
	{
		return IDValue;
	};

	Vector2 UIPanel::ParentPosition() const
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position = PositionValue + OffsetValue + TranslationValue - ScaledExtraSize() / 2;
		UIPanel *p = const_cast<UIPanel *>(ParentValue.Get());

		while(p)
		{
			Position += p->Position() + p->Translation() + p->Offset() - p->ScaledExtraSize() / 2;

			p = p->Parent();
		};

		return Position;
	};

	void UIPanel::SetVisible(bool value)
	{
		VisibleValue = value;
	};

	bool UIPanel::Visible() const
	{
		return VisibleValue;
	};

	void UIPanel::SetEnabled(bool value)
	{
		EnabledValue = value;
	};

	bool UIPanel::Enabled() const
	{
		return EnabledValue;
	};

	void UIPanel::SetMouseInputEnabled(bool value)
	{
		MouseInputValue = value;
	};

	bool UIPanel::MouseInputEnabled() const
	{
		return MouseInputValue;
	};

	void UIPanel::SetKeyboardInputEnabled(bool value)
	{
		KeyboardInputValue = value;
	};

	bool UIPanel::KeyboardInputEnabled() const
	{
		return KeyboardInputValue;
	};

	void UIPanel::SetJoystickInputEnabled(bool value)
	{
		JoystickInputValue = value;
	};

	bool UIPanel::JoystickInputEnabled() const
	{
		return JoystickInputValue;
	};

	void UIPanel::RemoveChild(UIPanel *Child)
	{
		for(std::vector<UIPanel *>::iterator it = Children.begin(); it != Children.end(); it++)
		{
			if(*it == Child)
			{
				Child->ParentValue = SuperSmartPointer<UIPanel>();
				Children.erase(it);

				return;
			};
		};
	};

	uint32 UIPanel::ChildrenCount() const
	{
		return Children.size();
	};

	UIPanel *UIPanel::Child(uint32 Index) const
	{
		return Children.size() > Index ? Children[Index] : NULL;
	};

	const Vector2 &UIPanel::Translation() const
	{
		return TranslationValue;
	};

	UIPanel *UIPanel::Parent() const
	{
		return (UIPanel *)ParentValue.Get();
	};

	const Vector2 &UIPanel::Position() const
	{
		return PositionValue;
	};

	void UIPanel::SetPosition(const Vector2 &Position)
	{
		PositionValue = Position;
	};

	const Vector2 &UIPanel::Size() const
	{
		return SizeValue;
	};

	void UIPanel::SetSize(const Vector2 &Size)
	{
		SizeValue = Size;

		if(SizeValue.x < 0)
			SizeValue.x = 0;

		if(SizeValue.y < 0)
			SizeValue.y = 0;
	};

	f32 UIPanel::Alpha() const
	{
		return AlphaValue;
	};

	void UIPanel::SetAlpha(f32 Alpha)
	{
		AlphaValue = Alpha;
	};

	Vector2 UIPanel::ChildrenSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->Visible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->Position() + Children[i]->Offset() + Children[i]->ComposedSize();

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		};

		return Out;
	};

	Vector2 UIPanel::ChildrenExtraSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->Visible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->ScaledExtraSize();

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		};

		return Out;
	};

	void UIPanel::SetSkin(SuperSmartPointer<GenericConfig> Skin)
	{
		this->Skin = Skin;
		OnSkinChange();
	};

	UILayout *UIPanel::Layout() const
	{
		return LayoutValue;
	};

	const Vector2 &UIPanel::ExtraSize() const
	{
		return SelectBoxExtraSize;
	};

	bool UIPanel::RespondsToTooltips() const
	{
		return RespondsToTooltipsValue && (TooltipValue.length() || TooltipElementValue.Get());
	};

	Vector2 UIPanel::ScaledExtraSize() const
	{
		return SelectBoxExtraSize * ExtraSizeScaleValue;
	};
#endif
};

#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	class UIFadeInAnimation : public UIAnimation
	{
	public:
		UIFadeInAnimation(UIPanel *Target, uint64 _Duration) : UIAnimation(Target)
		{
			Duration = _Duration;
		};

		bool Update()
		{
			Target->SetAlpha(MathUtils::Clamp(GameClockDiffNoPause(StartTime) / (f32)Duration));

			return Target->GetAlpha() == 1;
		};

		void Finish()
		{
			Target->SetAlpha(1);
		};
	};

	class UIFadeOutAnimation : public UIAnimation
	{
	public:
		UIFadeOutAnimation(UIPanel *Target, uint64 _Duration) : UIAnimation(Target)
		{
			Duration = _Duration;
		};

		bool Update()
		{
			Target->SetAlpha(1 - (MathUtils::Clamp(GameClockDiffNoPause(StartTime) / (f32)Duration)));

			return Target->GetAlpha() == 0;
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
		UIManager *Owner = GetManager();

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

	UIPanel::UIPanel(const std::string &NativeTypeName, UIManager *_Manager) : NativeType(NativeTypeName), Manager(_Manager), VisibleValue(true),
		EnabledValue(true), MouseInputValue(true), KeyboardInputValue(true), AlphaValue(1),
		ClickPressed(false), BlockingInput(false), IsDraggableValue(false), IsDroppableValue(false),
		DraggingValue(false), TooltipFixed(false), RotationValue(0), ExtraSizeScaleValue(1),
		InputBlockerBackgroundValue(true)
	{
		FLASSERT(Manager != NULL, "Invalid UI Manager!");
		FLASSERT(Manager->ScriptInstance, "Invalid UI Manager Script!");

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

		Properties = luabind::newtable(Manager->ScriptInstance->State);
	};

	f32 UIPanel::GetParentAlpha() const
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

		OnCharacterEntered(this);

		if(!RendererManager::Instance.Input.InputConsumed() && ParentValue.Get())
		{
			ParentValue->OnCharacterEnteredPriv();
		};
	};

	void UIPanel::OnLoseFocusPriv()
	{
		OnLoseFocus(this);
	};

	void UIPanel::OnGainFocusPriv()
	{
		OnGainFocus(this);
	};

	void UIPanel::OnConstructed()
	{
		EnabledValue = MouseInputValue = KeyboardInputValue = VisibleValue = true;
		AlphaValue = 1;
		ClickPressed = BlockingInput = RespondsToTooltipsValue = false;
	};

	void UIPanel::OnMouseJustPressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnMouseJustPressedFunction, (Self, o));
	};

	void UIPanel::OnMousePressedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnMousePressedFunction, (Self, o));
	};

	void UIPanel::OnMouseReleasedScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnMouseReleasedFunction, (Self, o))
	};

	void UIPanel::OnKeyJustPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnKeyJustPressedFunction, (Self, o))
	};

	void UIPanel::OnKeyPressedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnKeyPressedFunction, (Self, o))
	};

	void UIPanel::OnKeyReleasedScript(UIPanel *Self, const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnKeyReleasedFunction, (Self, o))
	};

	void UIPanel::OnMouseMovedScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnMouseMovedFunction, (Self))
	};

	void UIPanel::OnCharacterEnteredScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnCharacterEnteredFunction, (Self, RendererManager::Instance.Input.Character))
	};

	void UIPanel::OnGainFocusScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnGainFocusFunction, (Self))
	};

	void UIPanel::OnLoseFocusScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnLoseFocusFunction, (Self))
	};

	void UIPanel::OnMouseEnteredScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

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

		RUN_GUI_SCRIPT_EVENTS(OnMouseLeftFunction, (Self))
	};

	void UIPanel::OnClickScript(UIPanel *Self, const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnClickFunction, (Self, o))
	};

	void UIPanel::OnDragBeginScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnDragBeginFunction, (Self))
	};

	void UIPanel::OnDragEndScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnDragEndFunction, (Self))
	};

	void UIPanel::OnDraggingScript(UIPanel *Self)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnDraggingFunction, (Self))
	};

	void UIPanel::OnDropScript(UIPanel *Self, UIPanel *Target)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		RUN_GUI_SCRIPT_EVENTS(OnDropFunction, (Self, Target))
	};
	
	void UIPanel::OnMouseJustPressedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != sf::Mouse::Left || !IsDraggableValue)
			return;

		DraggingValue = true;

		OnDragBegin(this);
	};

	void UIPanel::OnMouseReleasedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
	{
		if(o.Name != sf::Mouse::Left || !IsDraggableValue)
			return;

		if(DraggingValue)
		{
			OnDragEnd(this);

			DraggingValue = false;

			SuperSmartPointer<UIPanel> Drop = RendererManager::Instance.ActiveRenderer()->UI->GetMouseOverElement();

			if(!Drop.Get() || !Drop->IsDroppable())
				return;

			Drop->OnDrop(Drop, this);
		};
	};

	void UIPanel::AdjustSizeAndPosition(UIPanel *PanelToStopAt)
	{
		if(this == PanelToStopAt || ParentValue == NULL)
			return;

		if(ParentValue->SizeValue.x < GetComposedSize().x)
			ParentValue->SizeValue.x = GetComposedSize().x;

		if(ParentValue->SizeValue.y < GetComposedSize().y)
			ParentValue->SizeValue.y = GetComposedSize().y;

		PositionFunction.Members.front()(this, ParentValue, GetManager()->GetOwner()->Size().x, GetManager()->GetOwner()->Size().y);

		ParentValue->AdjustSizeAndPosition(PanelToStopAt);
	};

	void UIPanel::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		RUN_GUI_SCRIPT_EVENTS(OnDrawFunction, (this, ParentPosition))
	};

	void UIPanel::DrawUIFocusZone(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		if(GetManager()->DrawUIFocusZones)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = GetComposedSize();

			Vector2 PanelOffset(3, 3);

			Vector2 ActualPosition = ParentPosition + GetPosition() + GetOffset() + PanelOffset;

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize - PanelOffset;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize - PanelOffset * 2).Color(Vector4(0, 0, 0.5f, 1)).Rotation(GetParentRotation());
			
			TheSprite.Draw(Renderer);

			uint32 ParentCounter = 0;

			UIPanel *ParentPanel = ParentValue;

			f32 IndicatorPosition = 0;

			Vector2 GlobalPosition = GetParentPosition();

			while(ParentPanel != NULL)
			{
				if(ParentPanel->GetParentPosition() == GlobalPosition)
					IndicatorPosition += RenderTextUtils::MeasureTextSimple(StringUtils::MakeIntString(ParentCounter), TextParams()).ToFullSize().x + 5;

				ParentCounter++;

				ParentPanel = ParentPanel->ParentValue;
			};

			RenderTextUtils::RenderText(Renderer, StringUtils::MakeIntString(ParentCounter), TextParams().Position(AABB.min.ToVector2() + Vector2(IndicatorPosition, 0)));
		};
	};

	void UIPanel::DrawUIRect(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		if(GetManager()->DrawUIRects)
		{
			static AxisAlignedBoundingBox AABB;

			Vector2 PanelSize = GetComposedSize();

			Vector2 ActualPosition = ParentPosition + GetPosition() + GetOffset();

			AABB.min = ActualPosition;
			AABB.max = AABB.min + PanelSize;

			Sprite TheSprite;
			TheSprite.Options.Wireframe(true).WireframePixelSize(1).Position(AABB.min.ToVector2()).Scale(PanelSize).Color(Vector4(1, 0.75f, 0, 0.75f)).Rotation(GetParentRotation());
			
			TheSprite.Draw(Renderer);
		};
	};

	void UIPanel::AddChild(UIPanel *Child)
	{
		FLASSERT(Child->Manager == Manager, "Combining GUI Managers is illegal!");

		if(Child->Manager != Manager)
			return;

		if(Child->ParentValue)
		{
			Child->ParentValue->RemoveChild(Child);
		};

		Children.push_back(Child);
		Child->ParentValue = GetManager()->GetElement(ID);
	};

	void UIPanel::Clear()
	{
		while(Children.size())
		{
			uint32 OldSize = Children.size();

			Manager->RemoveElement(Children[0]->GetID());

			uint32 NewSize = Children.size();

			if(NewSize == OldSize)
				Children.erase(Children.begin());
		};
	};

	UIPanel::~UIPanel()
	{
		if(ParentValue)
			ParentValue->RemoveChild(this);

		GetManager()->RemoveElement(ID);

		Clear();

		if(TooltipElement.Get())
			TooltipElement.Dispose();
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

	Vector2 UIPanel::GetComposedSize() const
	{
		return GetSize() + GetScaledExtraSize();
	};

	bool UIPanel::TooltipsFixed() const
	{
		return TooltipFixed;
	};

	void UIPanel::SetRotation(f32 Value)
	{
		RotationValue = Value;
	};

	f32 UIPanel::Rotation() const
	{
		return RotationValue;
	};

	f32 UIPanel::GetParentRotation() const
	{
		f32 Rotation = RotationValue;

		UIPanel *Parent = const_cast<UIPanel *>(ParentValue.Get());

		while(Parent != NULL)
		{
			Rotation += Parent->RotationValue;

			Parent = Parent->GetParent();
		};

		return Rotation;
	};

	void UIPanel::SetTooltipsFixed(bool Value)
	{
		TooltipFixed = Value;
	};

	const Vector2 &UIPanel::TooltipsPosition() const
	{
		return TooltipPosition;
	};

	const Vector2 &UIPanel::GetOffset() const
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

	const sf::String &UIPanel::GetTooltipText() const
	{
		return TooltipValue;
	};

	SuperSmartPointer<UIPanel> UIPanel::GetTooltipElement() const
	{
		return TooltipElement;
	};

	bool UIPanel::IsBlockingInput() const
	{
		return BlockingInput;
	};

	void UIPanel::SetBlockingInput(bool Value)
	{
		BlockingInput = Value;
	};

	void UIPanel::SetTooltipText(const sf::String &Text)
	{
		TooltipValue = Text;
	};

	void UIPanel::SetTooltipElement(SuperSmartPointer<UIPanel> Element)
	{
		TooltipElement = Element;
	};

	void UIPanel::SetDraggable(bool Value)
	{
		IsDraggableValue = Value;
	};

	bool UIPanel::IsDraggable() const
	{
		return IsDraggableValue;
	};

	void UIPanel::SetDroppable(bool Value)
	{
		IsDroppableValue = Value;
	};

	bool UIPanel::IsDroppable() const
	{
		return IsDroppableValue;
	};

	UIManager *UIPanel::GetManager() const
	{
		return Manager;
	};

	StringID UIPanel::GetID() const
	{
		return ID;
	};

	Vector2 UIPanel::GetParentPosition() const
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position = PositionValue + OffsetValue + TranslationValue - GetScaledExtraSize() / 2;
		UIPanel *p = const_cast<UIPanel *>(ParentValue.Get());

		while(p)
		{
			Position += p->GetPosition() + p->GetTranslation() + p->GetOffset() - p->GetScaledExtraSize() / 2;

			p = p->GetParent();
		};

		return Position;
	};

	void UIPanel::SetVisible(bool value)
	{
		VisibleValue = value;
	};

	bool UIPanel::IsVisible() const
	{
		return VisibleValue;
	};

	void UIPanel::SetEnabled(bool value)
	{
		EnabledValue = value;
	};

	bool UIPanel::IsEnabled() const
	{
		return EnabledValue;
	};

	void UIPanel::SetMouseInputEnabled(bool value)
	{
		MouseInputValue = value;
	};

	bool UIPanel::IsMouseInputEnabled() const
	{
		return MouseInputValue;
	};

	void UIPanel::SetKeyboardInputEnabled(bool value)
	{
		KeyboardInputValue = value;
	};

	bool UIPanel::IsKeyboardInputEnabled() const
	{
		return KeyboardInputValue;
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

	uint32 UIPanel::GetChildrenCount() const
	{
		return Children.size();
	};

	UIPanel *UIPanel::GetChild(uint32 Index) const
	{
		return Children.size() > Index ? Children[Index] : NULL;
	};

	const Vector2 &UIPanel::GetTranslation() const
	{
		return TranslationValue;
	};

	UIPanel *UIPanel::GetParent() const
	{
		return (UIPanel *)ParentValue.Get();
	};

	const Vector2 &UIPanel::GetPosition() const
	{
		return PositionValue;
	};

	void UIPanel::SetPosition(const Vector2 &Position)
	{
		PositionValue = Position;
	};

	const Vector2 &UIPanel::GetSize() const
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

	f32 UIPanel::GetAlpha() const
	{
		return AlphaValue;
	};

	void UIPanel::SetAlpha(f32 Alpha)
	{
		AlphaValue = Alpha;
	};

	Vector2 UIPanel::GetChildrenSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->GetPosition() + Children[i]->GetOffset() + Children[i]->GetComposedSize();

			if(Out.x < Size.x)
				Out.x = Size.x;

			if(Out.y < Size.y)
				Out.y = Size.y;
		};

		return Out;
	};

	Vector2 UIPanel::GetChildrenExtraSize() const
	{
		Vector2 Out, Size;

		for(uint32 i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			Size = Children[i]->GetScaledExtraSize();

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

	UILayout *UIPanel::GetLayout() const
	{
		return Layout;
	};

	const Vector2 &UIPanel::GetExtraSize() const
	{
		return SelectBoxExtraSize;
	};

	bool UIPanel::RespondsToTooltips() const
	{
		return RespondsToTooltipsValue && (TooltipValue.getSize() || TooltipElement.Get());
	};

	Vector2 UIPanel::GetScaledExtraSize() const
	{
		return SelectBoxExtraSize * ExtraSizeScaleValue;
	};
#endif
};

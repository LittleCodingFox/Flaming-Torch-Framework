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
		DraggingValue(false), TooltipFixed(false)
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

	f32 UIPanel::GetParentAlpha()
	{
		if(!ParentValue)
			return AlphaValue;

		f32 Alpha = AlphaValue;
		UIPanel *p = ParentValue;

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
	};

	void UIPanel::OnMousePressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		OnMousePressed(this, o);
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
	};

	void UIPanel::OnKeyJustPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnKeyJustPressed(this, o);
	};

	void UIPanel::OnKeyReleasedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnKeyReleased(this, o);
	};

	void UIPanel::OnCharacterEnteredPriv()
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnCharacterEntered(this);
	};

	void UIPanel::OnLoseFocusPriv()
	{
		OnLoseFocus(this);
	};

	void UIPanel::OnGainFocusPriv()
	{
		OnGainFocus(this);
	};

	/*!
	*	\note Should be called whenever the UI object is created
	*/
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

	void UIPanel::Draw(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		RUN_GUI_SCRIPT_EVENTS(OnDrawFunction, (this, ParentPosition))
	};

	void UIPanel::DrawUIRect(const Vector2 &ParentPosition, RendererManager::Renderer *Renderer)
	{
		if(GetManager()->DrawUIRects)
		{
			SpriteCache::Instance.Flush(Renderer);

			Vector2 ActualPosition = ParentPosition + PositionValue + OffsetValue;

			Renderer->EnableState(GL_VERTEX_ARRAY);
			Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
			Renderer->DisableState(GL_COLOR_ARRAY);
			Renderer->DisableState(GL_NORMAL_ARRAY);
			Renderer->BindTexture(NULL);

			Vector2 Vertices[8] = {
				ActualPosition - SelectBoxExtraSize / 2,
				ActualPosition + Vector2(SizeValue.x + SelectBoxExtraSize.x / 2, -SelectBoxExtraSize.y / 2),
				ActualPosition + Vector2(SizeValue.x + SelectBoxExtraSize.x / 2, -SelectBoxExtraSize.y / 2),
				ActualPosition + SizeValue + SelectBoxExtraSize / 2,
				ActualPosition + SizeValue + SelectBoxExtraSize / 2,
				ActualPosition + Vector2(-SelectBoxExtraSize.x / 2, SizeValue.y + SelectBoxExtraSize.y / 2),
				ActualPosition + Vector2(-SelectBoxExtraSize.x / 2, SizeValue.y + SelectBoxExtraSize.y / 2),
				ActualPosition - SelectBoxExtraSize / 2,
			};

			glColor4f(1, 0.75f, 0, 0.75f);

			glVertexPointer(2, GL_FLOAT, 0, Vertices);

			glDrawArrays(GL_LINES, 0, 8);
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
#endif
};

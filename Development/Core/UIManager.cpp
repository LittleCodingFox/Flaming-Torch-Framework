#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void RemoveElementFuture(MemoryStream &Stream)
	{
		StringID ID = 0;
		UIManager *Manager = NULL;
		uint32 ManagerAddr = 0;

		Stream.Seek(0);
		Stream.Read2<StringID>(&ID);
		Stream.Read2<uint32>(&ManagerAddr);

		Manager = (UIManager *)ManagerAddr;

		Manager->RemoveElement(ID);
	};

	class UIInputProcessor : public InputCenter::Context
	{
		bool OnKey(const InputCenter::KeyInfo &Key)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(Key.JustPressed)
			{
				TheGUIManager.OnKeyJustPressedPriv(Key);
			}
			else if(Key.Pressed)
			{
				TheGUIManager.OnKeyPressedPriv(Key);
			}
			else if(Key.JustReleased)
			{
				TheGUIManager.OnKeyReleasedPriv(Key);
			};

			return TheManager.Input.InputConsumed();
		};

		bool OnMouseButton(const InputCenter::MouseButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(Button.JustPressed)
			{
				TheGUIManager.OnMouseJustPressedPriv(Button);
			}
			else if(Button.Pressed)
			{
				TheGUIManager.OnMousePressedPriv(Button);
			}
			else if(Button.JustReleased)
			{
				TheGUIManager.OnMouseReleasedPriv(Button);
			};

			return TheManager.Input.InputConsumed();
		};

		bool OnJoystickButton(const InputCenter::JoystickButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			//TODO

			return false;
		};
		
		bool OnJoystickAxis(const InputCenter::JoystickAxisInfo &Axis)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			//TODO

			return false;
		};

		void OnJoystickConnected(uint8 Index) {};

		void OnJoystickDisconnected(uint8 Index) {};

		void OnMouseMove(const Vector3 &Position)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			TheGUIManager.OnMouseMovePriv();
		};

		void OnCharacterEntered(wchar_t Character)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			TheGUIManager.OnCharacterEnteredPriv();
		};

		void OnAction(const InputCenter::Action &TheAction)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			//TODO
		};

		void OnGainFocus() {};
		void OnLoseFocus() {};
	};

	SuperSmartPointer<UILayout> UIManager::GetLayout(StringID LayoutID)
	{
		if(Layouts.find(LayoutID) != Layouts.end())
			return Layouts[LayoutID];

		return SuperSmartPointer<UILayout>();
	};

	void UIManager::AddLayout(StringID LayoutID, SuperSmartPointer<UILayout> Layout)
	{
		if(Layouts.find(LayoutID) != Layouts.end())
			Layouts[LayoutID].Dispose();

		Layouts[LayoutID] = Layout;
	};

#define CHECKJSONVALUE(Value, Name, type)\
	if(!Value.isNull())\
	{\
		Log::Instance.LogWarn(TAG, "While parsing a layout: Value '%s' is non-null and not the expected type '%s'", Name, #type);\
	};

	void UIManager::CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName)
	{
		if(Elements.type() != Json::arrayValue)
			return;

		for(uint32 i = 0; i < Elements.size(); i+=2)
		{
			Json::Value Value = Elements[i];

			if(!Value.isString())
			{
				static std::stringstream str;
				str.str("");
				str << TheLayout->Name + "_ELEMENT " << i;

				CHECKJSONVALUE(Value, str.str().c_str(), string);

				continue;
			};

			std::string ElementName = Value.asString();
			std::string ElementIDName = ParentElementName + "." + ElementName;
			StringID ElementID = MakeStringID(ElementIDName);

			Json::Value &Data = Elements[i + 1];

			Value = Data.get("Control", Json::Value());

			if(!Value.isString())
			{
				CHECKJSONVALUE(Value, "Control", string);

				continue;
			};

			std::string Control = Value.asString();

			if(Control.length() == 0)
			{
				Log::Instance.LogWarn(TAG, "Unable to add widget '%s' on layout '%s' due to missing 'Control' property", ElementName.c_str(), TheLayout->Name.c_str());

				continue;
			};

			SuperSmartPointer<UIPanel> Panel;

			Control = StringUtils::ToUpperCase(Control);

			RendererManager::Renderer *Renderer = RendererManager::Instance.ActiveRenderer();

			if(Control == "FRAME")
			{
				Panel.Reset(new UIFrame(Renderer->UI));
			}
			else if(Control == "BUTTON")
			{
				Panel.Reset(new UIButton(Renderer->UI));
			}
			else if(Control == "CHECKBOX")
			{
				Panel.Reset(new UICheckBox(Renderer->UI));
			}
			else if(Control == "SPRITE")
			{
				Panel.Reset(new UISprite(Renderer->UI));
			}
			else if(Control == "TEXT")
			{
				Panel.Reset(new UIText(Renderer->UI));
			}
			else if(Control == "TEXTCOMPOSER")
			{
				Panel.Reset(new UITextComposer(Renderer->UI));
			}
			else if(Control == "GROUP" || Control == "LAYOUT")
			{
				Panel.Reset(new UIGroup(Renderer->UI));
			}
			else if(Control == "TEXTBOX")
			{
				Panel.Reset(new UITextBox(Renderer->UI));
			}
			else if(Control == "LIST")
			{
				Panel.Reset(new UIList(Renderer->UI));
			}
			else if(Control == "SCROLLABLEFRAME")
			{
				Panel.Reset(new UIScrollableFrame(Renderer->UI));
			}
			else if(Control == "HSCROLL")
			{
				Panel.Reset(new UIScrollbar(Renderer->UI, false));
			}
			else if(Control == "VSCROLL")
			{
				Panel.Reset(new UIScrollbar(Renderer->UI, true));
			}
			else if(Control == "DROPDOWN")
			{
				Panel.Reset(new UIDropdown(Renderer->UI));
			}
			else if(Control == "SPLITPANEL")
			{
				Panel.Reset(new UISplitPanel(Renderer->UI));
			};

			if(Panel.Get() == NULL)
			{
				Log::Instance.LogErr(TAG, "Failed to read a widget '%s' from a layout '%s': Invalid control '%s'", ElementName.c_str(),
					TheLayout->Name.c_str(), Control.c_str());

				return;
			};

			Panel->Layout = TheLayout;
			Panel->LayoutName = ElementIDName;

			std::stringstream BaseFunctionName;

			BaseFunctionName << Control << "_" << StringUtils::MakeIntString((uint32)ElementID, true) << "_";

			Json::Value EnabledValue = Data.get("Enabled", Json::Value(true)),
				KeyboardInputEnabledValue = Data.get("KeyboardInput", Json::Value(true)),
				MouseInputEnabledValue = Data.get("MouseInput", Json::Value(true)),
				AlphaValue = Data.get("Opacity", Json::Value(1.0)),
				VisibleValue = Data.get("Visible", Json::Value(true)),
				BlockingInputValue = Data.get("BlockingInput", Json::Value(false)),
				DraggableValue = Data.get("Draggable", Json::Value(false)),
				DroppableValue = Data.get("Droppable", Json::Value(false));

			if(EnabledValue.isBool())
			{
				Panel->SetEnabled(EnabledValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(EnabledValue, "Enabled", bool);
			};

			if(KeyboardInputEnabledValue.isBool())
			{
				Panel->SetKeyboardInputEnabled(KeyboardInputEnabledValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(KeyboardInputEnabledValue, "KeyboardInput", bool);
			};

			if(MouseInputEnabledValue.isBool())
			{
				Panel->SetMouseInputEnabled(MouseInputEnabledValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(MouseInputEnabledValue, "MouseInput", bool);
			};

			if(AlphaValue.isDouble())
			{
				Panel->SetAlpha((f32)AlphaValue.asDouble());
			}
			else
			{
				CHECKJSONVALUE(AlphaValue, "Opacity", double);
			};

			if(VisibleValue.isBool())
			{
				Panel->SetVisible(VisibleValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(VisibleValue, "Visible", bool);
			};

			if(BlockingInputValue.isBool())
			{
				Panel->SetBlockingInput(BlockingInputValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(BlockingInputValue, "BlockingInput", bool);
			};

			if(DraggableValue.isBool())
			{
				Panel->SetDraggable(DraggableValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(DraggableValue, "Draggable", bool);
			};

			if(DroppableValue.isBool())
			{
				Panel->SetDroppable(DroppableValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(DroppableValue, "Droppable", bool);
			};

#define REGISTER_LUA_EVENT(name, extraparams)\
			{\
				Value = Data.get(#name, Json::Value(""));\
				\
				if(Value.isString())\
				{\
					if(Value.asString().length() > 0)\
					{\
						std::string Code = Value.asString();\
						std::stringstream ComposedCode, ComposedFunctionName;\
						\
						ComposedFunctionName << BaseFunctionName.str() << #name;\
						\
						ComposedCode << ComposedFunctionName.str() << " = function(Self" << extraparams << ")\n" << Code << "\nend\n";\
						\
						ScriptInstance->DoString(ComposedCode.str().c_str());\
						\
						Panel->name##Function = luabind::globals(ScriptInstance->State)[ComposedFunctionName.str().c_str()];\
					}\
				}\
				else\
				{\
					CHECKJSONVALUE(Value, #name, string);\
				};\
			};

			REGISTER_LUA_EVENT(OnMouseJustPressed, ", Button");
			REGISTER_LUA_EVENT(OnMousePressed, ", Button");
			REGISTER_LUA_EVENT(OnMouseReleased, ", Button");
			REGISTER_LUA_EVENT(OnKeyJustPressed, ", Key");
			REGISTER_LUA_EVENT(OnKeyPressed, ", Key");
			REGISTER_LUA_EVENT(OnKeyReleased, ", Key");
			REGISTER_LUA_EVENT(OnMouseMoved, "");
			REGISTER_LUA_EVENT(OnCharacterEntered, "");
			REGISTER_LUA_EVENT(OnGainFocus, "");
			REGISTER_LUA_EVENT(OnLoseFocus, "");
			REGISTER_LUA_EVENT(OnUpdate, "");
			REGISTER_LUA_EVENT(OnDraw, "");
			REGISTER_LUA_EVENT(OnMouseEntered, "");
			REGISTER_LUA_EVENT(OnMouseOver, "");
			REGISTER_LUA_EVENT(OnMouseLeft, "");
			REGISTER_LUA_EVENT(OnClick, ", Button");
			REGISTER_LUA_EVENT(OnDragBegin, "");
			REGISTER_LUA_EVENT(OnDragEnd, "");
			REGISTER_LUA_EVENT(OnDragging, "");
			REGISTER_LUA_EVENT(OnDrop, ", Target");
			REGISTER_LUA_EVENT(OnStart, "");

			TheLayout->Elements[MakeStringID(ElementIDName)] = Panel;

			if(!Renderer->UI->AddElement(ElementID, Panel))
				return;

			if(Parent)
				Parent->AddChild(Panel);

			std::stringstream PositionScriptX, PositionScriptY, SizeScript;

			Value = Data.get("Wide", Json::Value("0"));
			std::string Temp;

			if(Value.isString())
			{
				Temp = Value.asString();

				bool Invalid = false;

				int32 Offset = 0;

				while(Temp.find('%', Offset) != std::string::npos)
				{
					int32 Index = Temp.find('%', Offset);

					bool Found = false;

					for(int32 j = Index - 1; j >= 0; j--)
					{
						if((Temp[j] < '0' || Temp[j] > '9') && Temp[j] != '.')
						{
							Found = true;

							if(j == Index - 1) //In case of modulus
							{
								Offset = Index + 1;

								break;
							};

							std::string Percentage = Temp.substr(j, Index - Offset);

							f32 Percent = 0;

							if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
							{
								Percent /= 100.f;

								Temp = Temp.substr(0, j) + "parent_wide * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);

								Offset = Index;
								Offset -= Percentage.length();
							}
							else
							{
								Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

								Invalid = true;

								break;
							};
						};
					};

					if(!Found)
					{
						std::string Percentage = Temp.substr(0, Index);

						f32 Percent = 0;

						if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
						{
							Percent /= 100.f;

							Temp = "parent_wide * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);
						}
						else
						{
							Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

							Invalid = true;

							break;
						};
					};

					if(Invalid)
						break;
				};

				if(Invalid)
					continue;

				SizeScript << "local SizeX = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Wide", string);
			};

			Value = Data.get("Tall", Json::Value("0"));

			if(Value.isString())
			{
				Temp = Value.asString();

				bool Invalid = false;

				int32 Offset = 0;

				while(Temp.find('%', Offset) != std::string::npos)
				{
					int32 Index = Temp.find('%', Offset);

					bool Found = false;

					for(int32 j = Index - 1; j >= 0; j--)
					{
						if((Temp[j] < '0' || Temp[j] > '9') && Temp[j] != '.')
						{
							Found = true;

							if(j == Index - 1) //In case of modulus
							{
								Offset = Index + 1;

								break;
							};

							std::string Percentage = Temp.substr(j, Index - Offset);

							f32 Percent = 0;

							if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
							{
								Percent /= 100.f;

								Temp = Temp.substr(0, j) + "parent_tall * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);

								Offset = Index;
								Offset -= Percentage.length();
							}
							else
							{
								Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

								Invalid = true;

								break;
							};
						};
					};

					if(!Found)
					{
						std::string Percentage = Temp.substr(0, Index);

						f32 Percent = 0;

						if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
						{
							Percent /= 100.f;

							Temp = "parent_tall * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);
						}
						else
						{
							Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

							Invalid = true;

							break;
						};
					};

					if(Invalid)
						break;
				};

				if(Invalid)
					continue;

				SizeScript << "local SizeY = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Tall", string);
			};

			Value = Data.get("Xpos", Json::Value("0"));

			if(Value.isString())
			{
				Temp = Value.asString();

				bool Invalid = false;

				int32 Offset = 0;

				while(Temp.find('%', Offset) != std::string::npos)
				{
					int32 Index = Temp.find('%', Offset);

					bool Found = false;

					for(int32 j = Index - 1; j >= 0; j--)
					{
						if((Temp[j] < '0' || Temp[j] > '9') && Temp[j] != '.')
						{
							Found = true;

							if(j == Index - 1) //In case of modulus
							{
								Offset = Index + 1;

								break;
							};

							std::string Percentage = Temp.substr(j, Index - Offset);

							f32 Percent = 0;

							if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
							{
								Percent /= 100.f;

								Temp = Temp.substr(0, j) + "parent_wide * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);

								Offset = Index;
								Offset -= Percentage.length();
							}
							else
							{
								Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

								Invalid = true;

								break;
							};

							break;
						};
					};

					if(!Found)
					{
						std::string Percentage = Temp.substr(0, Index);

						f32 Percent = 0;

						if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
						{
							Percent /= 100.f;

							Temp = "parent_wide * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);
						}
						else
						{
							Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

							Invalid = true;

							break;
						};
					};

					if(Invalid)
						break;
				};

				if(Invalid)
					continue;

				PositionScriptX << "local PositionX = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Xpos", string);
			};

			Value = Data.get("Ypos", Json::Value("0"));

			if(Value.isString())
			{
				Temp = Value.asString();

				bool Invalid = false;

				int32 Offset = 0;

				while(Temp.find('%', Offset) != std::string::npos)
				{
					int32 Index = Temp.find('%', Offset);

					bool Found = false;

					for(int32 j = Index - 1; j >= 0; j--)
					{
						if((Temp[j] < '0' || Temp[j] > '9') && Temp[j] != '.')
						{
							Found = true;

							if(j == Index - 1) //In case of modulus
							{
								Offset = Index + 1;

								break;
							};

							std::string Percentage = Temp.substr(j, Index - Offset);

							f32 Percent = 0;

							if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
							{
								Percent /= 100.f;

								Temp = Temp.substr(0, j) + "parent_tall * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);

								Offset = Index;
								Offset -= Percentage.length();
							}
							else
							{
								Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

								Invalid = true;

								break;
							};
						};
					};

					if(!Found)
					{
						std::string Percentage = Temp.substr(0, Index);

						f32 Percent = 0;

						if(1 == sscanf(Percentage.c_str(), "%f", &Percent))
						{
							Percent /= 100.f;

							Temp = "parent_tall * " + StringUtils::MakeFloatString(Percent) + Temp.substr(Index + 1);
						}
						else
						{
							Log::Instance.LogWarn(TAG, "Unable to position or size element '%s': Invalid Percentage", ElementName.c_str());

							Invalid = true;

							break;
						};
					};

					if(Invalid)
						break;
				};

				if(Invalid)
					continue;

				PositionScriptY << "local PositionY = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Ypos", string);
			};

			std::stringstream ComposedPositionFunction, ComposedSizeFunction;

			ComposedPositionFunction << "function " << BaseFunctionName.str() << "Position(Self, Parent, ScreenWidth, ScreenHeight)\n"
				"local parent_wide = 0\n"
				"local parent_tall = 0\n"
				"local parent_extra_size_x = 0\n"
				"local parent_extra_size_y = 0\n"
				"local left = 0\n"
				"local right = 0\n"
				"local top = 0\n"
				"local bottom = 0\n"
				"local center = 0\n\n"
				"if Parent ~= nil then\n"
				"  parent_wide = Parent.Size.x\n"
				"  parent_tall = Parent.Size.y\n"
				"  parent_extra_size_x = Parent.ExtraSize.x\n"
				"  parent_extra_size_y = Parent.ExtraSize.y\n"
				"else\n"
				"  parent_wide = ScreenWidth\n"
				"  parent_tall = ScreenHeight\n"
				"end\n\n"
				"left = Self.ExtraSize.x / 2\n"
				"right = (parent_wide + parent_extra_size_x / 2) - (Self.Size.x + Self.ExtraSize.x / 2)\n"
				"top = Self.ExtraSize.y / 2\n"
				"bottom = (parent_tall + parent_extra_size_y / 2) - (Self.Size.y + Self.ExtraSize.y / 2)\n"
				"center = ((parent_wide + parent_extra_size_x / 2) - (Self.Size.x + Self.ExtraSize.x / 2)) / 2\n\n" <<
				PositionScriptX.str() <<
				"center = ((parent_tall + parent_extra_size_y / 2) - (Self.Size.y + Self.ExtraSize.y / 2)) / 2\n" <<
				PositionScriptY.str() <<
				"Self.Position = Vector2(PositionX, PositionY)\n"
				"\nend\n";

			ComposedSizeFunction << "function " << BaseFunctionName.str() << "Size(Self, Parent, ScreenWidth, ScreenHeight)\n"
				"local parent_wide = 0\n"
				"local parent_tall = 0\n\n"
				"if Parent ~= nil then\n"
				"	parent_wide = Parent.Size.x\n"
				"	parent_tall = Parent.Size.y\n"
				"else\n"
				"   parent_wide = ScreenWidth\n"
				"   parent_tall = ScreenHeight\n"
				"end\n\n" <<
				SizeScript.str() <<
				"Self.Size = Vector2(SizeX, SizeY)\n"
				"\nend\n";

			luaL_dostring(ScriptInstance->State, ComposedPositionFunction.str().c_str());

			luaL_dostring(ScriptInstance->State, ComposedSizeFunction.str().c_str());

			Panel->PositionFunction = luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Position"];
			Panel->SizeFunction = luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Size"];

			if(!Panel->PositionFunction)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Position", ElementName.c_str());

				continue;
			}
			else if(!Panel->SizeFunction)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Size", ElementName.c_str());

				continue;
			};

			try
			{
				Panel->SizeFunction(Panel, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Size", ElementName.c_str(), e.what());

				continue;
			};

			try
			{
				Panel->PositionFunction(Panel, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Position", ElementName.c_str(), e.what());

				continue;
			};

			if(Control == "BUTTON")
			{
				UIButton *Button = Panel.AsDerived<UIButton>();
				Value = Data.get("Caption", Json::Value());

				if(Value.isString())
				{
					Button->Caption = Value.asString();
				}
				else
				{
					CHECKJSONVALUE(Value, "Caption", string);
				};

				Value = Data.get("FontSize", Json::Value((Json::Value::Int)Button->TextParameters.FontSizeValue));

				if(Value.isInt())
				{
					int32 FontSize = Value.asInt();

					Button->TextParameters.FontSize(FontSize);
				}
				else
				{
					CHECKJSONVALUE(Value, "FontSize", int);
				};
			}
			else if(Control == "CHECKBOX")
			{
				Value = Data.get("Checked", Json::Value(false));

				if(Value.isBool())
				{
					bool Checked = Value.asBool();

					Panel.AsDerived<UICheckBox>()->SetChecked(Checked);
				}
				else
				{
					CHECKJSONVALUE(Value, "Checked", bool);
				};

				Value = Data.get("Caption", Json::Value(""));

				if(Value.isString())
				{
					Panel.AsDerived<UICheckBox>()->Caption = Value.asString();
				}
				else
				{
					CHECKJSONVALUE(Value, "Caption", string);
				};
			}
			else if(Control == "SPRITE")
			{
				UISprite *TheSprite = Panel.AsDerived<UISprite>();
				Value = Data.get("Path", Json::Value(""));

				if(Value.isString())
				{
					std::string FileName = Value.asString();

					if(FileName.length() == 0)
					{
						TheSprite->TheSprite.Options.Scale(TheSprite->GetSize()).Color(Vector4());
					}
					else
					{
						SuperSmartPointer<Texture> SpriteTexture;

						if(FileName.find('/') == 0)
						{
							SpriteTexture = ResourceManager::Instance.GetTextureFromPackage(FileName.substr(0, FileName.rfind('/') + 1),
								FileName.substr(FileName.rfind('/') + 1));
						};

						if(!SpriteTexture.Get())
						{
							SpriteTexture = ResourceManager::Instance.GetTexture(FileName);
						};

						if(!SpriteTexture.Get())
						{
							Log::Instance.LogWarn(TAG, "Unable to load texture '%s' for UI Sprite '%s' on Layout '%s'", FileName.c_str(), ElementName.c_str(),
								TheLayout->Name.c_str());
						}
						else
						{
							TheSprite->TheSprite.SpriteTexture = SpriteTexture;
							TheSprite->TheSprite.Options.Scale(Panel->GetSize() != Vector2() ? Panel->GetSize() / SpriteTexture->Size() : Vector2(1, 1));
							TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Linear);

							if(TheSprite->GetSize() == Vector2())
							{
								TheSprite->SetSize(SpriteTexture->Size());
							};
						};
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "Path", string);
				};

				Value = Data.get("Filtering", Json::Value("LINEAR"));

				if(Value.isString())
				{
					std::string Temp = Value.asString();

					if(TheSprite->TheSprite.SpriteTexture.Get())
					{
						if(StringUtils::ToUpperCase(Temp) == "LINEAR")
						{
							TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Linear);
						}
						else if(StringUtils::ToUpperCase(Temp) == "NEAREST")
						{
							TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Nearest);
						};
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "Filtering", string);
				};

				Value = Data.get("CropTiled", Json::Value(""));

				if(Value.isString())
				{
					std::string CropTiledString = Value.asString();

					if(CropTiledString.length() && TheSprite->TheSprite.SpriteTexture.Get())
					{
						Vector2 FrameSize, FrameID;

						sscanf(CropTiledString.c_str(), "%f, %f, %f, %f", &FrameSize.x, &FrameSize.y, &FrameID.x, &FrameID.y);

						TheSprite->TheSprite.Options.Scale(FrameSize / TheSprite->TheSprite.SpriteTexture->Size()).Crop(CropMode::CropTiled,
							Rect(FrameSize.x, FrameID.x, FrameSize.y, FrameID.y));
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "CropTiled", string);
				};

				Value = Data.get("NinePatch", Json::Value(""));

				if(Value.isString())
				{
					std::string NinePatchString = Value.asString();

					if(NinePatchString.length())
					{
						Rect NinePatchRect;

						sscanf(NinePatchString.c_str(), "%f,%f,%f,%f", &NinePatchRect.Left, &NinePatchRect.Right, &NinePatchRect.Top,
							&NinePatchRect.Bottom);

						TheSprite->TheSprite.Options.NinePatch(true, NinePatchRect).Scale(Panel->GetSize());
						TheSprite->SelectBoxExtraSize = NinePatchRect.ToFullSize();
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "NinePatch", string)
				};

				Value = Data.get("Color", Json::Value(""));

				if(Value.isString())
				{
					std::string ColorString = Value.asString();

					Vector4 Color;

					if(4 == sscanf(ColorString.c_str(), "%f, %f, %f, %f", &Color.x, &Color.y, &Color.z, &Color.w))
					{
						TheSprite->TheSprite.Options.Color(Color);
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "Color", string)
				};

				Value = Data.get("ScaleWide", Json::Value(""));

				if(Value.isDouble())
				{
					TheSprite->TheSprite.Options.Scale(Vector2((f32)Value.asDouble(), TheSprite->TheSprite.Options.ScaleValue.y));
				};

				Value = Data.get("ScaleTall", Json::Value(""));

				if(Value.isDouble())
				{
					TheSprite->TheSprite.Options.Scale(Vector2(TheSprite->TheSprite.Options.ScaleValue.x, (f32)Value.asDouble()));
				};
			}
			else if(Control == "GROUP")
			{
				Value = Data.get("LayoutMode", Json::Value("None"));

				if(Value.isString())
				{
					std::string Mode = StringUtils::ToUpperCase(Value.asString());

					if(Mode == "HORIZONTAL")
					{
						Panel.AsDerived<UIGroup>()->LayoutMode = UIGroupLayoutMode::Horizontal;
					}
					else if(Mode == "VERTICAL")
					{
						Panel.AsDerived<UIGroup>()->LayoutMode = UIGroupLayoutMode::Vertical;
					}
					else if(Mode == "NONE")
					{
						Panel.AsDerived<UIGroup>()->LayoutMode = UIGroupLayoutMode::None;
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "LayoutMode", string)
				};
			}
			else if(Control == "TEXT")
			{
				Value = Data.get("ExpandHeight", Json::Value(false));
				bool AutoExpand = Value.isBool() && Value.asBool();
				Value = Data.get("FontSize", Json::Value((Json::Value::Int)Renderer->UI->GetDefaultFontSize()));
				int32 FontSize = Renderer->UI->GetDefaultFontSize();

				if(Value.isInt())
				{
					FontSize = Value.asInt();
				}
				else
				{
					CHECKJSONVALUE(Value, "FontSize", int);
				};

				std::string Text;
				Value = Data.get("Text", Json::Value(""));

				if(Value.isString())
				{
					Text = Value.asString();
				}
				else
				{
					CHECKJSONVALUE(Value, "Text", string);
				};

				uint32 TextStyle = sf::Text::Regular;

				Value = Data.get("Bold", Json::Value(false));

				if(Value.isBool())
				{
					if(Value.asBool())
						TextStyle |= sf::Text::Bold;
				}
				else
				{
					CHECKJSONVALUE(Value, "Bold", string);
				};

				Value = Data.get("Italic", Json::Value(false));

				if(Value.isBool())
				{
					if(Value.asBool())
						TextStyle |= sf::Text::Italic;
				}
				else
				{
					CHECKJSONVALUE(Value, "Italic", string);
				};

				Value = Data.get("Underline", Json::Value(false));

				if(Value.isBool())
				{
					if(Value.asBool())
						TextStyle |= sf::Text::Underlined;
				}
				else
				{
					CHECKJSONVALUE(Value, "Underline", string);
				};

				uint32 Alignment = 0;

				Value = Data.get("Alignment", Json::Value(""));

				if(Value.isString())
				{
					std::string AlignmentString = StringUtils::ToUpperCase(Value.asString());

					std::vector<std::string> Fragments = StringUtils::Split(AlignmentString, '|');

					for(uint32 j = 0; j < Fragments.size(); j++)
					{
						if(Fragments[j] == "CENTER")
						{
							Alignment |= UITextAlignment::Center;
						}
						else if(Fragments[j] == "LEFT")
						{
							Alignment |= UITextAlignment::Left;
						}
						else if(Fragments[j] == "RIGHT")
						{
							Alignment |= UITextAlignment::Right;
						}
						else if(Fragments[j] == "VCENTER")
						{
							Alignment |= UITextAlignment::VCenter;
						};
					};

					if(Fragments.size() == 0)
						Alignment = UITextAlignment::Left;
				}
				else
				{
					CHECKJSONVALUE(Value, "Alignment", string);
				};

				UIText *TheText = Panel.AsDerived<UIText>();

				TheText->TextParameters.Style(TextStyle);

				static std::stringstream str;
				str.str("");
				str << TheText->TextParameters.TextColorValue.x << "," << TheText->TextParameters.TextColorValue.y << "," << TheText->TextParameters.TextColorValue.z << "," <<
					TheText->TextParameters.TextColorValue.w;

				Value = Data.get("TextColor", Json::Value());

				if(Value.type() == Json::stringValue)
				{
					if(Value.asString().length())
					{
						sscanf(Value.asString().c_str(), "%f,%f,%f,%f", &TheText->TextParameters.TextColorValue.x, &TheText->TextParameters.TextColorValue.y,
							&TheText->TextParameters.TextColorValue.z, &TheText->TextParameters.TextColorValue.w);
						TheText->TextParameters.SecondaryTextColorValue = TheText->TextParameters.TextColorValue;
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "TextColor", string);
				};

				str.str("");
				str << TheText->TextParameters.SecondaryTextColorValue.x << "," << TheText->TextParameters.SecondaryTextColorValue.y << "," <<
					TheText->TextParameters.SecondaryTextColorValue.z << "," << TheText->TextParameters.SecondaryTextColorValue.w;

				Value = Data.get("SecondaryTextColor", Json::Value(str.str()));

				if(Value.type() == Json::stringValue)
				{
					sscanf(Value.asString().c_str(), "%f,%f,%f,%f", &TheText->TextParameters.SecondaryTextColorValue.x, &TheText->TextParameters.SecondaryTextColorValue.y,
						&TheText->TextParameters.SecondaryTextColorValue.z, &TheText->TextParameters.SecondaryTextColorValue.w);
				}
				else
				{
					CHECKJSONVALUE(Value, "SecondaryTextColor", string);
				};

				Value = Data.get("Border", Json::Value("0"));

				if(Value.type() == Json::stringValue)
				{
					sscanf(Value.asString().c_str(), "%f", &TheText->TextParameters.BorderSizeValue);
				}
				else
				{
					CHECKJSONVALUE(Value, "Border", string);
				};

				str.str("");
				str << TheText->TextParameters.BorderColorValue.x << "," << TheText->TextParameters.BorderColorValue.y << "," << TheText->TextParameters.BorderColorValue.z << "," <<
					TheText->TextParameters.BorderColorValue.w;

				Value = Data.get("BorderColor", Json::Value(str.str()));

				if(Value.type() == Json::stringValue)
				{
					sscanf(Value.asString().c_str(), "%f,%f,%f,%f", &TheText->TextParameters.BorderColorValue.x, &TheText->TextParameters.BorderColorValue.y,
						&TheText->TextParameters.BorderColorValue.z, &TheText->TextParameters.BorderColorValue.w);
				}
				else
				{
					CHECKJSONVALUE(Value, "BorderColor", string);
				};

				TheText->TextAlignment = Alignment;
				TheText->TextParameters.FontSize(FontSize);
				TheText->SetText(Text, AutoExpand);
			}
			else if(Control == "TEXTBOX")
			{
				UITextBox *TheTextBox = Panel.AsDerived<UITextBox>();

				Value = Data.get("Text", Json::Value(""));
				std::string Text;

				if(Value.isString())
				{
					Text = Value.asString();
				}
				else
				{
					CHECKJSONVALUE(Value, "Text", string);
				};

				Value = Data.get("Password", Json::Value(false));

				if(Value.isBool())
				{
					TheTextBox->SetPassword(Value.asBool());
				}
				else
				{
					CHECKJSONVALUE(Value, "Password", bool);
				};

				int32 FontSize = TheTextBox->TextParameters.FontSizeValue;

				Value = Data.get("FontSize", Json::Value((Json::Value::Int)FontSize));

				if(Value.isInt())
				{
					FontSize = Value.asInt();;
				}
				else
				{
					CHECKJSONVALUE(Value, "FontSize", int);
				};

				TheTextBox->TextParameters.FontSize(FontSize);
				TheTextBox->SetText(Text);
			}
			else if(Control == "LIST")
			{
				UIList *TheList = Panel.AsDerived<UIList>();

				Value = Data.get("Elements", Json::Value(""));

				if(Value.isString())
				{
					std::string ElementString = Value.asString();

					std::vector<std::string> Items = StringUtils::Split(ElementString, '|');

					TheList->Items = Items;
				}
				else
				{
					CHECKJSONVALUE(Value, "Elements", string);
				};

				int32 FontSize = TheList->GetManager()->GetDefaultFontSize();

				Value = Data.get("FontSize", Json::Value((Json::Value::Int)TheList->GetManager()->GetDefaultFontSize()));

				if(Value.isInt())
				{
					FontSize = Value.asInt();
				}
				else
				{
					CHECKJSONVALUE(Value, "FontSize", int);
				};

				TheList->TextParameters.FontSize(FontSize);
			}
			else if(Control == "LAYOUT")
			{
				Json::Value Value = Data.get("ID", Json::Value());
				std::string LayoutIDName;

				if(Value.isString())
				{
					LayoutIDName = Value.asString();
				}
				else
				{
					CHECKJSONVALUE(Value, "ID", string);
				};

				if(LayoutIDName.length() == 0)
				{
					Log::Instance.LogErr(TAG, "While processing Layout element '%s' of layout '%s': Invalid Layout ID", ElementName.c_str(),
						TheLayout->Name.c_str());

					return;
				};

				SuperSmartPointer<UILayout> TargetLayout;

				for(LayoutMap::iterator it = Layouts.begin(); it != Layouts.end(); it++)
				{
					if(it->second->Name ==  LayoutIDName)
					{
						TargetLayout = it->second;

						break;
					};
				};

				if(TargetLayout.Get() == NULL)
				{
					Log::Instance.LogErr(TAG, "While processing Layout element '%s' of layout '%s': Layout '%s' not found (may not have been created already)",
						ElementName.c_str(), TheLayout->Name.c_str(), LayoutIDName.c_str());

					return;
				};

				Panel->SetSize(Parent ? Parent->GetSize() : Vector2());

				SuperSmartPointer<UILayout> NewLayout = TargetLayout->Clone(Panel, ParentElementName + "." + ElementName);

				Panel->SetSize(Panel->GetChildrenSize());

				StringID LayoutID = MakeStringID(ParentElementName + "." + ElementName + "." + NewLayout->Name);

				LayoutMap::iterator it = Layouts.find(LayoutID);

				//Should be impossible for this to happen, but still...
				if(it != Layouts.end())
				{
					Log::Instance.LogWarn(TAG, "Found duplicate layout '%s', erasing old.", (Panel->GetLayout()->Name + "_" + NewLayout->Name).c_str());

					Layouts.erase(it);
				};

				Layouts[LayoutID] = NewLayout;

				for(uint32 j = 0; j < Data.size(); j++)
				{
					std::string Name = Data.getMemberNames()[j];

					Json::Value Value = Data.get(Name, Json::Value());

					if(Name == "ID")
						continue;

					for(UILayout::ElementMap::iterator it = NewLayout->Elements.begin(); it != NewLayout->Elements.end(); it++)
					{
						std::string ElementName = GetStringIDString(it->first);

						int32 Index = ElementName.find(LayoutIDName);

						ElementName = ElementName.substr(Index + LayoutIDName.size() + 1);

						if(Name.find(ElementName) == 0)
						{
							Name = Name.substr(ElementName.length() + 1);

							UIPanel *TargetElement = it->second;

							std::vector<std::string> Parts = StringUtils::Split(Name, '.');

							if(Parts.size() > 1)
							{
								std::string CurrentElementID = GetStringIDString(it->first);

								for(int32 k = 0; k < Parts.size() - 1; k++)
								{
									CurrentElementID += "." + Parts[k];

									StringID ActualElementID = MakeStringID(CurrentElementID);

									bool Found = false;

									for(uint32 l = 0; l < TargetElement->GetChildrenCount(); l++)
									{
										if(TargetElement->GetChild(l)->GetID() == ActualElementID)
										{
											Found = true;

											TargetElement = TargetElement->GetChild(l);

											break;
										};
									};

									if(!Found)
									{
										TargetElement = NULL;

										break;
									};
								};
							};

							if(TargetElement != NULL)
							{
								std::string ControlName = StringUtils::ToUpperCase(TargetElement->NativeType);
								std::string Property = Parts[Parts.size() - 1];
								std::string CurrentElementID = GetStringIDString(TargetElement->ID);

								if(ControlName == "UIBUTTON")
								{
									UIButton *Button = (UIButton *)TargetElement;

									if(Property == "Caption")
									{
										if(Value.isString())
										{
											Button->Caption = Value.asString();
										}
										else
										{
											CHECKJSONVALUE(Value, "Caption", string);
										};
									};

									if(Property == "FontSize")
									{
										if(Value.isInt())
										{
											int32 FontSize = Value.asInt();

											Button->TextParameters.FontSize(FontSize);
										}
										else
										{
											CHECKJSONVALUE(Value, "FontSize", int);
										};
									};
								}
								else if(ControlName == "UICHECKBOX")
								{
									if(Property == "Checked")
									{
										if(Value.isBool())
										{
											bool Checked = Value.asBool();

											((UICheckBox *)TargetElement)->SetChecked(Checked);
										}
										else
										{
											CHECKJSONVALUE(Value, "Checked", bool);
										};
									};

									if(Property == "Caption")
									{
										if(Value.isString())
										{
											((UICheckBox *)TargetElement)->Caption = Value.asString();
										}
										else
										{
											CHECKJSONVALUE(Value, "Caption", string);
										};
									};
								}
								else if(ControlName == "UISPRITE")
								{
									UISprite *TheSprite = (UISprite *)TargetElement;

									if(Property == "Path")
									{
										if(Value.isString())
										{
											std::string FileName = Value.asString();

											if(FileName.length() == 0)
											{
												TheSprite->TheSprite.Options.Scale(TheSprite->GetSize()).Color(Vector4());
											}
											else
											{
												SuperSmartPointer<Texture> SpriteTexture;

												if(FileName.find('/') == 0)
												{
													SpriteTexture = ResourceManager::Instance.GetTextureFromPackage(FileName.substr(0, FileName.rfind('/') + 1),
														FileName.substr(FileName.rfind('/') + 1));
												};

												if(!SpriteTexture.Get())
												{
													SpriteTexture = ResourceManager::Instance.GetTexture(FileName);
												};

												if(!SpriteTexture.Get())
												{
													Log::Instance.LogWarn(TAG, "Unable to load texture '%s' for UI Sprite '%s' on Layout '%s'", FileName.c_str(), ElementName.c_str(),
														TheLayout->Name.c_str());
												}
												else
												{
													TheSprite->TheSprite.SpriteTexture = SpriteTexture;
													TheSprite->TheSprite.Options.Scale(Panel->GetSize() != Vector2() ? Panel->GetSize() / SpriteTexture->Size() : Vector2(1, 1));
													TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Linear);

													if(TheSprite->GetSize() == Vector2())
													{
														TheSprite->SetSize(SpriteTexture->Size());
													};
												};
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "Path", string);
										};
									};

									if(Property == "Filtering")
									{
										if(Value.isString())
										{
											std::string Temp = Value.asString();

											if(TheSprite->TheSprite.SpriteTexture.Get())
											{
												if(StringUtils::ToUpperCase(Temp) == "LINEAR")
												{
													TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Linear);
												}
												else if(StringUtils::ToUpperCase(Temp) == "NEAREST")
												{
													TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Nearest);
												};
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "Filtering", string);
										};
									};

									if(Property == "CropTiled")
									{
										if(Value.isString())
										{
											std::string CropTiledString = Value.asString();

											if(CropTiledString.length() && TheSprite->TheSprite.SpriteTexture.Get())
											{
												Vector2 FrameSize, FrameID;

												sscanf(CropTiledString.c_str(), "%f, %f, %f, %f", &FrameSize.x, &FrameSize.y, &FrameID.x, &FrameID.y);

												TheSprite->TheSprite.Options.Scale(FrameSize / TheSprite->TheSprite.SpriteTexture->Size()).Crop(CropMode::CropTiled,
													Rect(FrameSize.x, FrameID.x, FrameSize.y, FrameID.y));
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "CropTiled", string);
										};
									};

									if(Property == "NinePatch")
									{
										if(Value.isString())
										{
											std::string NinePatchString = Value.asString();

											if(NinePatchString.length())
											{
												Rect NinePatchRect;

												sscanf(NinePatchString.c_str(), "%f,%f,%f,%f", &NinePatchRect.Left, &NinePatchRect.Right, &NinePatchRect.Top,
													&NinePatchRect.Bottom);

												TheSprite->TheSprite.Options.NinePatch(true, NinePatchRect).Scale(Panel->GetSize());
												TheSprite->SelectBoxExtraSize = NinePatchRect.ToFullSize();
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "NinePatch", string)
										};
									};

									if(Property == "Color")
									{
										if(Value.isString())
										{
											std::string ColorString = Value.asString();

											Vector4 Color;

											if(4 == sscanf(ColorString.c_str(), "%f, %f, %f, %f", &Color.x, &Color.y, &Color.z, &Color.w))
											{
												TheSprite->TheSprite.Options.Color(Color);
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "Color", string)
										};
									};

									if(Property == "ScaleWide")
									{
										if(Value.isDouble())
										{
											TheSprite->TheSprite.Options.Scale(Vector2((f32)Value.asDouble(), TheSprite->TheSprite.Options.ScaleValue.y));
										};
									};

									if(Property == "ScaleTall")
									{
										if(Value.isDouble())
										{
											TheSprite->TheSprite.Options.Scale(Vector2(TheSprite->TheSprite.Options.ScaleValue.x, (f32)Value.asDouble()));
										};
									};
								}
								else if(ControlName == "UIGROUP")
								{
									if(Property == "LayoutMode")
									{
										if(Value.isString())
										{
											std::string Mode = StringUtils::ToUpperCase(Value.asString());

											if(Mode == "HORIZONTAL")
											{
												((UIGroup *)TargetElement)->LayoutMode = UIGroupLayoutMode::Horizontal;
											}
											else if(Mode == "VERTICAL")
											{
												((UIGroup *)TargetElement)->LayoutMode = UIGroupLayoutMode::Vertical;
											}
											else if(Mode == "NONE")
											{
												((UIGroup *)TargetElement)->LayoutMode = UIGroupLayoutMode::None;
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "LayoutMode", string)
										};
									};
								}
								else if(ControlName == "UITEXT")
								{
									UIText *TheText = (UIText *)TargetElement;

									if(Property == "FontSize")
									{
										int32 FontSize = Renderer->UI->GetDefaultFontSize();

										if(Value.isInt())
										{
											FontSize = Value.asInt();
										}
										else
										{
											CHECKJSONVALUE(Value, "FontSize", int);
										};

										TheText->TextParameters.FontSize(FontSize);
									};

									if(Property == "Text")
									{
										if(Value.isString())
										{
											std::string Text = Value.asString();

											TheText->SetText(Text, false);
										}
										else
										{
											CHECKJSONVALUE(Value, "Text", string);
										};
									};

									if(Property == "Alignment")
									{
										uint32 Alignment = 0;

										if(Value.isString())
										{
											std::string AlignmentString = StringUtils::ToUpperCase(Value.asString());

											std::vector<std::string> Fragments = StringUtils::Split(AlignmentString, '|');

											for(uint32 j = 0; j < Fragments.size(); j++)
											{
												if(Fragments[j] == "CENTER")
												{
													Alignment |= UITextAlignment::Center;
												}
												else if(Fragments[j] == "LEFT")
												{
													Alignment |= UITextAlignment::Left;
												}
												else if(Fragments[j] == "RIGHT")
												{
													Alignment |= UITextAlignment::Right;
												}
												else if(Fragments[j] == "VCENTER")
												{
													Alignment |= UITextAlignment::VCenter;
												};
											};

											if(Fragments.size() == 0)
												Alignment = UITextAlignment::Left;

											TheText->TextAlignment = Alignment;
										}
										else
										{
											CHECKJSONVALUE(Value, "Alignment", string);
										};
									};

									if(Property == "Border")
									{
										if(Value.type() == Json::stringValue)
										{
											sscanf(Value.asString().c_str(), "%f", &TheText->TextParameters.BorderSizeValue);
										}
										else
										{
											CHECKJSONVALUE(Value, "Border", string);
										};
									};

									if(Property == "BorderColor")
									{
										static std::stringstream str;

										str.str("");
										str << TheText->TextParameters.BorderColorValue.x << "," << TheText->TextParameters.BorderColorValue.y << "," << TheText->TextParameters.BorderColorValue.z << "," <<
											TheText->TextParameters.BorderColorValue.w;

										if(Value.type() == Json::stringValue)
										{
											sscanf(Value.asString().c_str(), "%f,%f,%f,%f", &TheText->TextParameters.BorderColorValue.x, &TheText->TextParameters.BorderColorValue.y,
												&TheText->TextParameters.BorderColorValue.z, &TheText->TextParameters.BorderColorValue.w);
										}
										else
										{
											CHECKJSONVALUE(Value, "BorderColor", string);
										};
									};
								}
								else if(ControlName == "UITEXTBOX")
								{
									UITextBox *TheTextBox = (UITextBox *)TargetElement;

									if(Property == "Text")
									{
										std::string Text;

										if(Value.isString())
										{
											Text = Value.asString();

											TheTextBox->SetText(Text);
										}
										else
										{
											CHECKJSONVALUE(Value, "Text", string);
										};
									};

									if(Property == "Password")
									{
										if(Value.isBool())
										{
											TheTextBox->SetPassword(Value.asBool());
										}
										else
										{
											CHECKJSONVALUE(Value, "Password", bool);
										};
									};

									if(Property == "FontSize")
									{
										int32 FontSize = TheTextBox->TextParameters.FontSizeValue;

										if(Value.isInt())
										{
											FontSize = Value.asInt();

											TheTextBox->TextParameters.FontSize(FontSize);
										}
										else
										{
											CHECKJSONVALUE(Value, "FontSize", int);
										};
									};
								}
								else if(ControlName == "UILIST")
								{
									UIList *TheList = (UIList *)TargetElement;

									if(Property == "Elements")
									{
										if(Value.isString())
										{
											std::string ElementString = Value.asString();

											std::vector<std::string> Items = StringUtils::Split(ElementString, '|');

											TheList->Items = Items;
										}
										else
										{
											CHECKJSONVALUE(Value, "Elements", string);
										};
									};

									if(Property == "FontSize")
									{
										int32 FontSize = TheList->GetManager()->GetDefaultFontSize();

										if(Value.isInt())
										{
											FontSize = Value.asInt();
										}
										else
										{
											CHECKJSONVALUE(Value, "FontSize", int);
										};

										TheList->TextParameters.FontSize(FontSize);
									};
								};

								if(Property == "TooltipPosition")
								{
									if(Value.asString().length() > 0)
									{
										Vector2 Position;

										if(2 != sscanf(Value.asString().c_str(), "%f,%f", &Position.x, &Position.y))
										{
											Log::Instance.LogWarn(TAG, "While processing Layout element '%s' of layout '%s': Invalid Tooltip Position, expected \"x, y\"",
												ElementName.c_str(), TheLayout->Name.c_str());
										}
										else
										{
											TargetElement->SetTooltipsPosition(Position);
										};
									};
								};

								if(Property == "Tooltip")
								{
									if(Value.isString() && Value.asString().length())
									{
										TargetElement->SetRespondsToTooltips(true);
										TargetElement->SetTooltipText(Value.asString());
									}
									else
									{
										if(Value.isArray())
										{
											SuperSmartPointer<UIPanel> TooltipGroup(new UIGroup(Renderer->UI));
											TooltipGroup->SetVisible(false);
											Renderer->UI->AddElement(MakeStringID(CurrentElementID + "_TOOLTIPELEMENT"), TooltipGroup);
											TheLayout->Elements[TooltipGroup->GetID()] = TooltipGroup;

											CopyElementsToLayout(TheLayout, Value, TooltipGroup, CurrentElementID + "_TOOLTIPELEMENT");

											TooltipGroup->SetSize(TooltipGroup->GetChildrenSize());

											TargetElement->SetRespondsToTooltips(true);
											TargetElement->SetTooltipElement(TooltipGroup);
										};
									};
								};

								if(Property == "TooltipFixed")
								{
									if(Value.isBool())
									{
										TargetElement->SetTooltipsFixed(Value.asBool());
									}
									else
									{
										CHECKJSONVALUE(Value, "TooltipFixed", bool);
									};
								}
								else if(ControlName == "UISPLITPANEL")
								{
									UISplitPanel *SplitPanel = (UISplitPanel *)TargetElement;

									if(Property == "Percentage")
									{
										if(Value.isDouble())
										{
											SplitPanel->Percentage = (f32)Value.asDouble();
										};
									};

									if(Property == "Orientation")
									{
										if(Value.isString())
										{
											std::string Temp = StringUtils::ToUpperCase(Value.asString());

											if(Temp == "HORIZONTAL")
											{
												SplitPanel->Horizontal = true;
											}
											else if(Temp == "VERTICAL")
											{
												SplitPanel->Horizontal = false;
											}
											else
											{
												Log::Instance.LogWarn(TAG, "While parsing a layout: Value '%s' is neither Vertical or Horizontal", "Orientation");\
											};
										}
										else
										{
											CHECKJSONVALUE(Value, "Orientation", int);
										};
									};

									if(Property == "Left")
									{
										if(Value.isArray())
										{
											SuperSmartPointer<UIPanel> Group(new UIGroup(Renderer->UI));
											Renderer->UI->AddElement(MakeStringID(CurrentElementID + "_LEFTGROUP"), Group);
											TheLayout->Elements[Group->GetID()] = Group;

											if(SplitPanel->Horizontal)
											{
												Group->SetSize(SplitPanel->GetSize() * Vector2(SplitPanel->Percentage, 1) - Vector2(SplitPanel->SplitSize / 2.f, 0));
											}
											else
											{
												Group->SetSize(SplitPanel->GetSize() * Vector2(1, SplitPanel->Percentage) - Vector2(0, SplitPanel->SplitSize / 2.f));
											};

											CopyElementsToLayout(TheLayout, Value, Group, CurrentElementID + "_LEFTGROUP");

											SplitPanel->Left = Group;
											SplitPanel->AddChild(Group);
										};
									};

									if(Property == "Right")
									{
										if(Value.isArray())
										{
											SuperSmartPointer<UIPanel> Group(new UIGroup(Renderer->UI));
											Renderer->UI->AddElement(MakeStringID(CurrentElementID + "_RIGHTGROUP"), Group);
											TheLayout->Elements[Group->GetID()] = Group;

											if(SplitPanel->Horizontal)
											{
												Group->SetSize(SplitPanel->GetSize() * Vector2(1 - SplitPanel->Percentage, 1) - Vector2(SplitPanel->SplitSize / 2.f, 0));
											}
											else
											{
												Group->SetSize(SplitPanel->GetSize() * Vector2(1, 1 - SplitPanel->Percentage) - Vector2(0, SplitPanel->SplitSize / 2.f));
											};

											CopyElementsToLayout(TheLayout, Value, Group, CurrentElementID + "_RIGHTGROUP");

											SplitPanel->Right = Group;
											SplitPanel->AddChild(Group);
										};
									};
								}
								else if(ControlName == "UIDROPDOWN")
								{
									UIDropdown *TheDropdown = (UIDropdown *)TargetElement;

									if(Property == "Elements")
									{
										if(Value.isString())
										{
											std::string ElementString = Value.asString();

											std::vector<std::string> Items = StringUtils::Split(ElementString, '|');

											TheDropdown->Items = Items;
										}
										else
										{
											CHECKJSONVALUE(Value, "Elements", string);
										};
									};

									if(Property == "FontSize")
									{
										int32 FontSize = TheDropdown->GetManager()->GetDefaultFontSize();

										if(Value.isInt())
										{
											FontSize = Value.asInt();
										}
										else
										{
											CHECKJSONVALUE(Value, "FontSize", int);
										};

										TheDropdown->TextParameters.FontSize(FontSize);
									};
								};

								break;
							};
						};
					};

					break;
				};
			}
			else if(Control == "DROPDOWN")
			{
				UIDropdown *TheDropdown = Panel.AsDerived<UIDropdown>();

				Value = Data.get("Elements", Json::Value(""));

				if(Value.isString())
				{
					std::string ElementString = Value.asString();

					std::vector<std::string> Items = StringUtils::Split(ElementString, '|');

					TheDropdown->Items = Items;
				}
				else
				{
					CHECKJSONVALUE(Value, "Elements", string);
				};

				int32 FontSize = TheDropdown->GetManager()->GetDefaultFontSize();

				Value = Data.get("FontSize", Json::Value((Json::Value::Int)TheDropdown->GetManager()->GetDefaultFontSize()));

				if(Value.isInt())
				{
					FontSize = Value.asInt();
				}
				else
				{
					CHECKJSONVALUE(Value, "FontSize", int);
				};

				TheDropdown->TextParameters.FontSize(FontSize);
			}
			else if(Control == "SPLITPANEL")
			{
				UISplitPanel *SplitPanel = Panel.AsDerived<UISplitPanel>();

				Value = Data.get("Percentage", Json::Value(""));

				if(Value.isDouble())
				{
					SplitPanel->Percentage = (f32)Value.asDouble();
				};

				Value = Data.get("Orientation", Json::Value("Horizontal"));

				if(Value.isString())
				{
					std::string Temp = StringUtils::ToUpperCase(Value.asString());

					if(Temp == "HORIZONTAL")
					{
						SplitPanel->Horizontal = true;
					}
					else if(Temp == "VERTICAL")
					{
						SplitPanel->Horizontal = false;
					}
					else
					{
						Log::Instance.LogWarn(TAG, "While parsing a layout: Value '%s' is neither Vertical or Horizontal", "Orientation");\
					};
				}
				else
				{
					CHECKJSONVALUE(Value, "Orientation", int);
				};

				Value = Data.get("Left", Json::Value(""));

				if(Value.isArray())
				{
					SuperSmartPointer<UIPanel> Group(new UIGroup(Renderer->UI));
					Renderer->UI->AddElement(MakeStringID(ParentElementName + "." + ElementName + "_LEFTGROUP"), Group);
					TheLayout->Elements[Group->GetID()] = Group;

					if(SplitPanel->Horizontal)
					{
						Group->SetSize(SplitPanel->GetSize() * Vector2(SplitPanel->Percentage, 1) - Vector2(SplitPanel->SplitSize / 2.f, 0));
					}
					else
					{
						Group->SetSize(SplitPanel->GetSize() * Vector2(1, SplitPanel->Percentage) - Vector2(0, SplitPanel->SplitSize / 2.f));
					};

					CopyElementsToLayout(TheLayout, Value, Group, ParentElementName + "." + ElementName + "_LEFTGROUP");

					SplitPanel->Left = Group;
					SplitPanel->AddChild(Group);
				};

				Value = Data.get("Right", Json::Value(""));

				if(Value.isArray())
				{
					SuperSmartPointer<UIPanel> Group(new UIGroup(Renderer->UI));
					Renderer->UI->AddElement(MakeStringID(ParentElementName + "." + ElementName + "_RIGHTGROUP"), Group);
					TheLayout->Elements[Group->GetID()] = Group;

					if(SplitPanel->Horizontal)
					{
						Group->SetSize(SplitPanel->GetSize() * Vector2(1 - SplitPanel->Percentage, 1) - Vector2(SplitPanel->SplitSize / 2.f, 0));
					}
					else
					{
						Group->SetSize(SplitPanel->GetSize() * Vector2(1, 1 - SplitPanel->Percentage) - Vector2(0, SplitPanel->SplitSize / 2.f));
					};

					CopyElementsToLayout(TheLayout, Value, Group, ParentElementName + "." + ElementName + "_RIGHTGROUP");

					SplitPanel->Right = Group;
					SplitPanel->AddChild(Group);
				};

				//Ignore tooltips and children
				continue;
			};

			Panel->PerformLayout();

			Value = Data.get("TooltipText", Json::Value(""));

			if(Value.isString() && Value.asString().length())
			{
				Panel->SetRespondsToTooltips(true);
				Panel->SetTooltipText(Value.asString());
			}
			else
			{
				Value = Data.get("TooltipElement", Json::Value(""));

				if(Value.isArray())
				{
					SuperSmartPointer<UIPanel> TooltipGroup(new UIGroup(Renderer->UI));
					TooltipGroup->SetVisible(false);
					Renderer->UI->AddElement(MakeStringID(ParentElementName + "." + ElementName + "_TOOLTIPELEMENT"), TooltipGroup);
					TheLayout->Elements[TooltipGroup->GetID()] = TooltipGroup;

					CopyElementsToLayout(TheLayout, Value, TooltipGroup, ParentElementName + "." + ElementName + "_TOOLTIPELEMENT");

					TooltipGroup->SetSize(TooltipGroup->GetChildrenSize());

					Panel->SetRespondsToTooltips(true);
					Panel->SetTooltipElement(TooltipGroup);
				};
			};

			Value = Data.get("TooltipFixed", Json::Value(false));

			if(Value.isBool())
			{
				Panel->SetTooltipsFixed(Value.asBool());
			}
			else
			{
				CHECKJSONVALUE(Value, "TooltipFixed", bool);
			};

			Value = Data.get("TooltipPosition", Json::Value(""));

			if(Value.isString()) 
			{
				if(Value.asString().length() > 0)
				{
					Vector2 Position;

					if(2 != sscanf(Value.asString().c_str(), "%f,%f", &Position.x, &Position.y))
					{
						Log::Instance.LogWarn(TAG, "While processing Layout element '%s' of layout '%s': Invalid Tooltip Position, expected \"x, y\"",
							ElementName.c_str(), TheLayout->Name.c_str());
					}
					else
					{
						Panel->SetTooltipsPosition(Position);
					};
				};
			}
			else
			{
				CHECKJSONVALUE(Value, "TooltipPosition", string);
			};

			Json::Value Children = Data.get("Children", Json::Value());

			if(!Children.isArray())
				continue;

			CopyElementsToLayout(TheLayout, Children, Panel, ParentElementName + "." + ElementName);
		};
	};

	bool UIManager::LoadLayouts(Stream *In, SuperSmartPointer<UIPanel> Parent)
	{
		Json::Value Root;
		Json::Reader Reader;

		if(!Reader.parse(In->AsString(), Root))
		{
			Log::Instance.LogErr(TAG, "Failed to parse a GUI layout resource: %s", Reader.getFormatedErrorMessages().c_str());

			return false;
		};

		for(uint32 i = 0; i < Root.size(); i+=2)
		{
			std::string LayoutName = Root[i].asString();
			Json::Value Elements = Root[i + 1];
			SuperSmartPointer<UILayout> Layout(new UILayout());
			Layout->Name = LayoutName;
			Layout->Owner = this;
			Layout->ContainedObjects = Elements;

			CopyElementsToLayout(Layout, Elements, Parent, Layout->Name);

			for(UILayout::ElementMap::iterator it = Layout->Elements.begin(); it != Layout->Elements.end(); it++)
			{
				if(it->second->OnStartFunction)
				{
					try
					{
						it->second->OnStartFunction(it->second.Get());
					}
					catch(std::exception &e)
					{
						Log::Instance.LogDebug(TAG, "Scripting Exception: %s", e.what());
					};
				};
			};

			StringID LayoutID = MakeStringID((Parent.Get() ? Parent->GetLayout()->Name + "_" : "") + LayoutName);

			LayoutMap::iterator it = Layouts.find(LayoutID);

			if(it != Layouts.end())
			{
				Log::Instance.LogWarn(TAG, "Found duplicate layout '%s', erasing old.", LayoutName.c_str());

				Layouts.erase(it);
			};

			Layouts[LayoutID] = Layout;
		};

		return true;
	};

	void UIManager::ClearLayouts()
	{
		while(Layouts.begin() != Layouts.end())
		{
			Layouts.begin()->second.Dispose();
			Layouts.erase(Layouts.begin());
		};

		MouseOverElement = NULL;
	};

	SuperSmartPointer<UIPanel> UIManager::GetMouseOverElement()
	{
		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->GetParent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		UIPanel *FoundElement = NULL;

		SuperSmartPointer<UIPanel> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			while(it != Elements.end() && it->second.Get() == NULL)
			{
				Elements.erase(it);
				it = Elements.begin();
			};

			if(it == Elements.end())
				break;

			if(it->second->Panel->BlockingInput && it->second->Panel->IsVisible())
			{
				InputBlocker = it->second->Panel;

				break;
			};
		};

		if(InputBlocker.Get())
		{
			UIPanel *p = InputBlocker;
			RecursiveFindFocusedElement(p->GetParentPosition(), p, FoundElement);
		}
		else
		{
			for(int32 i = DrawOrderCounter; i >= 0; i--)
			{
				for(uint32 j = 0; j < DrawOrderCache.size(); j++)
				{
					if(!DrawOrderCache[j]->Panel->MouseInputValue || DrawOrderCache[j]->DrawOrder != i)
						continue;

					UIPanel *p = DrawOrderCache[j]->Panel;

					RecursiveFindFocusedElement(Vector2(), p, FoundElement);

					if(FoundElement)
						break;
				};

				if(FoundElement)
					break;
			};
		};

		if(MouseOverElement != FoundElement)
		{
			if(MouseOverElement != NULL)
				MouseOverElement->OnMouseLeft(MouseOverElement);
		}
		else if(MouseOverElement)
		{
			MouseOverElement->OnMouseOver(MouseOverElement);
		};

		if(FoundElement && Elements[FoundElement->ID].Get())
		{
			if(MouseOverElement != FoundElement)
				FoundElement->OnMouseEntered(FoundElement);

			MouseOverElement = FoundElement;

			return Elements[FoundElement->ID]->Panel;
		};

		MouseOverElement = FoundElement;

		return SuperSmartPointer<UIPanel>();
	};

	void UIManager::Update()
	{
		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->GetParent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		for(uint32 i = 0; i < DrawOrderCache.size(); i++)
		{
			if(DrawOrderCache[i]->Panel.Get() == NULL)
				continue;

			if(DrawOrderCache[i]->Panel->IsVisible())
			{
				DrawOrderCache[i]->Panel->Update(Vector2());
			};
		};
	};

	void UIManager::Draw(RendererManager::Renderer *Renderer)
	{
		DrawUIRects = !!Console::Instance.GetVariable("r_drawuirects")->UintValue;

		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->GetParent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		SuperSmartPointer<UIPanel> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Panel->BlockingInput && it->second->Panel->IsVisible())
			{
				InputBlocker = it->second->Panel;

				break;
			};
		};

		for(uint32 i = 0; i <= DrawOrderCounter; i++)
		{
			for(uint32 j = 0; j < DrawOrderCache.size(); j++)
			{
				if(DrawOrderCache[j]->Panel.Get() == NULL || DrawOrderCache[j]->DrawOrder != i || !DrawOrderCache[j]->Panel->IsVisible())
					continue;

				if(DrawOrderCache[j]->Panel == InputBlocker)
				{
					Renderer->BindTexture(NULL);
					Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
					Renderer->DisableState(GL_NORMAL_ARRAY);
					Renderer->DisableState(GL_COLOR_ARRAY);

					Vector2 Vertices[6] = {
						Vector2(),
						Vector2(0, Renderer->Size().y),
						Renderer->Size(),
						Renderer->Size(),
						Vector2(Renderer->Size().x, 0),
						Vector2(),
					};

					glVertexPointer(2, GL_FLOAT, 0, Vertices);

					glColor4f(0, 0, 0, 0.3f);

					glDrawArrays(GL_TRIANGLES, 0, 6);

					glColor4f(1, 1, 1, 1);
				};

				DrawOrderCache[j]->Panel->Draw(Vector2(), Renderer);
			};
		};

		if(FocusedElementValue.Get() && FocusedElementValue->DraggingValue)
		{
			FocusedElementValue->Draw(RendererManager::Instance.Input.MousePosition, Renderer);
		};

		Tooltip->Update(Vector2());
		Tooltip->Draw(RendererManager::Instance.Input.MousePosition, Renderer);
	};

	void UIManager::RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement)
	{
		if(!p->IsVisible() || !p->IsEnabled() || !p->IsMouseInputEnabled())
			return;

		static AxisAlignedBoundingBox AABB;

		AABB.min = ParentPosition + p->GetPosition() - p->SelectBoxExtraSize / 2;
		AABB.max = AABB.min + p->GetSize() + p->SelectBoxExtraSize;

		if(AABB.IsInside(RendererManager::Instance.Input.MousePosition) &&
			p->MouseInputValue)
		{
			FoundElement = p;

			for(uint32 i = 0; i < p->Children.size(); i++)
			{
				RecursiveFindFocusedElement(ParentPosition + p->GetPosition() - p->GetTranslation(),
					p->Children[i], FoundElement);
			};
		};
	};

	void UIManager::OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->GetParent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		SuperSmartPointer<UIPanel> PreviouslyFocusedElement = FocusedElementValue;
		FocusedElementValue = SuperSmartPointer<UIPanel>();
		UIPanel *FoundElement = NULL;

		SuperSmartPointer<UIPanel> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Panel->BlockingInput && it->second->Panel->IsVisible())
			{
				InputBlocker = it->second->Panel;

				break;
			};
		};

		if(InputBlocker.Get())
		{
			UIPanel *p = InputBlocker;
			RecursiveFindFocusedElement(p->GetParentPosition(), p, FoundElement);
		}
		else
		{
			for(int32 i = DrawOrderCounter; i >= 0; i--)
			{
				for(uint32 j = 0; j < DrawOrderCache.size(); j++)
				{
					if(!DrawOrderCache[j]->Panel->MouseInputValue || DrawOrderCache[j]->DrawOrder != i)
						continue;

					UIPanel *p = DrawOrderCache[j]->Panel;

					RecursiveFindFocusedElement(Vector2(), p, FoundElement);

					if(FoundElement)
						break;
				};

				if(FoundElement)
					break;
			};
		};

		if(FoundElement)
		{
			FocusedElementValue = Elements[FoundElement->ID]->Panel;
		};

		if(PreviouslyFocusedElement && PreviouslyFocusedElement.Get() != FocusedElementValue.Get())
		{
			PreviouslyFocusedElement->OnLoseFocusPriv();
		};

		if(FoundElement)
		{
			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel.Get() == FoundElement)
				{
					FocusedElementValue = it->second->Panel;
				};
			};
		};

		if(FocusedElementValue)
		{
			FocusedElementValue->OnGainFocusPriv();
		};

		if(CurrentMenu.Get() && FocusedElementValue.Get() != CurrentMenu.Get())
		{
			MemoryStream Temp;
			RemoveMenuFuture(Temp);
		};

		if(FocusedElementValue)
		{
			FocusedElementValue->OnMouseJustPressedPriv(o);
			RendererManager::Instance.Input.ConsumeInput();
		};
	};

	void UIManager::OnMousePressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnMousePressedPriv(o);
			RendererManager::Instance.Input.ConsumeInput();
		};
	};

	void UIManager::OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnMouseReleasedPriv(o);
			RendererManager::Instance.Input.ConsumeInput();
		};
	};

	void UIManager::OnMouseMovePriv()
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnMouseMovePriv();
		};

		SuperSmartPointer<UIPanel> SelectedElement = GetMouseOverElement();

		if(SelectedElement.Get() == NULL)
		{
			if(Tooltip->Source)
			{
				Tooltip->Source = SuperSmartPointer<UIPanel>();
			}
		}
		else if(SelectedElement->RespondsToTooltips())
		{
			Tooltip->Source = SelectedElement;
		};
	};

	void UIManager::OnKeyJustPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnKeyJustPressedPriv(o);
		};
	};

	void UIManager::OnKeyPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnKeyPressedPriv(o);
		};
	};

	void UIManager::OnKeyReleasedPriv(const InputCenter::KeyInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnKeyReleasedPriv(o);
		};
	};

	void UIManager::OnCharacterEnteredPriv()
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnCharacterEnteredPriv();
		};
	};

	void UIManager::RegisterInput()
	{
		SuperSmartPointer<UIInputProcessor> Out(new UIInputProcessor());
		Out->Name = "GUIPROCESSOR_" + StringUtils::MakeIntString((int32)this, true);

		RendererManager::Instance.Input.AddContext(Out);
		RendererManager::Instance.Input.EnableContext(MakeStringID(Out->Name));
	};

	void UIManager::UnRegisterInput()
	{
		RendererManager::Instance.Input.DisableContext(MakeStringID("GUIPROCESSOR_" + StringUtils::MakeIntString((int32)this, true)));
	};

	bool UIManager::AddElement(StringID ID, SuperSmartPointer<UIPanel> Element)
	{
		DrawOrderCacheDirty = true;

		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			if(!it->second.Get() || !it->second->Panel.Get())
			{
				Elements.erase(it);
			}
			else
			{
				Log::Instance.LogWarn(TAG, "Failed to add Element 0x%08x: Duplicate ID 0x%08x", Element.Get(), ID);

				return false;
			};
		};

		FLASSERT(Element->Manager == this, "Found Element from another UI Manager!");

		if(Element->Manager != this)
			return false;

		Elements[ID].Reset(new ElementInfo());
		Elements[ID]->Panel = Element;
		Elements[ID]->DrawOrder = ++DrawOrderCounter;
		Element->ID = ID;
		Element->Name = GetStringIDString(ID);
		Element->SetSkin(Skin);

		return true;
	};

	void UIManager::RemoveElement(StringID ID)
	{
		DrawOrderCacheDirty = true;

		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			SuperSmartPointer<ElementInfo> Element = it->second;

			if(Element.Get() && MouseOverElement == Element->Panel.Get())
				MouseOverElement = NULL;

			Elements.erase(it);

			Element.Dispose();
		};
	};

	void UIManager::Clear()
	{
		DrawOrderCacheDirty = true;

		while(Elements.begin() != Elements.end())
		{
			Elements.begin()->second.Dispose();
		};

		FocusedElementValue.Dispose();
		MouseOverElement = NULL;
	};

	void UIManager::ClearFocus()
	{
		if(FocusedElementValue.Get())
			FocusedElementValue->OnLoseFocusPriv();

		if(CurrentMenu.Get())
		{
			MemoryStream Temp;
			RemoveMenuFuture(Temp);
		};

		FocusedElementValue = SuperSmartPointer<UIPanel>();
	};

	SuperSmartPointer<UIPanel> UIManager::GetElement(StringID ID)
	{
		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			return it->second->Panel;
		};

		return SuperSmartPointer<UIPanel>();
	};

	SuperSmartPointer<UIPanel> UIManager::GetFocusedElement()
	{
		return FocusedElementValue;
	};

	UITooltip &UIManager::GetTooltip()
	{
		//Will always be non-null
		return *Tooltip.Get();
	};

	void UIManager::SetSkin(SuperSmartPointer<GenericConfig> Skin)
	{
		this->Skin = Skin;

		std::string DefaultFontColorValue = Skin->GetString("General", "DefaultFontColor");

		sscanf(DefaultFontColorValue.c_str(), "%f,%f,%f,%f", &DefaultFontColor.x, &DefaultFontColor.y,
			&DefaultFontColor.z, &DefaultFontColor.w);

		std::string DefaultSecondaryFontColorValue = Skin->GetString("General", "DefaultSecondaryFontColor");

		if(DefaultSecondaryFontColorValue.length())
		{
			sscanf(DefaultSecondaryFontColorValue.c_str(), "%f,%f,%f,%f", &DefaultSecondaryFontColor.x, &DefaultSecondaryFontColor.y,
				&DefaultSecondaryFontColor.z, &DefaultSecondaryFontColor.w);
		}
		else
		{
			DefaultSecondaryFontColor = DefaultFontColor;
		};

		std::string DefaultFontSizeValue = Skin->GetString("General", "DefaultFontSize");

		sscanf(DefaultFontSizeValue.c_str(), "%u", &DefaultFontSize);

		std::string DefaultFontValue = Skin->GetString("General", "DefaultFont");
		std::string FontDirectory = DefaultFontValue.substr(0, DefaultFontValue.rfind('/') + 1);
		std::string FontName = DefaultFontValue.substr(DefaultFontValue.rfind('/') + 1);

		if(FontDirectory.length() == 0)
			FontDirectory = "/";

		DefaultFont = ResourceManager::Instance.GetFontFromPackage(FontDirectory, FontName);

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second.Get() == NULL)
				continue;

			it->second->Panel->SetSkin(Skin);
		};
	};

	UIMenu *UIManager::CreateMenu(const Vector2 &Position)
	{
		if(CurrentMenu.Get())
			RemoveElement(CurrentMenu->ID);

		CurrentMenu.Reset(new UIMenu(this));
		CurrentMenu->SetPosition(Position);

		AddElement(MakeStringID("__UIMANAGER_CURRENT_MENU__"), CurrentMenu);

		return CurrentMenu;
	};

	UIMenuBar *UIManager::CreateMenuBar()
	{
		if(CurrentMenuBar.Get())
			RemoveElement(CurrentMenuBar->ID);

		CurrentMenuBar.Dispose();
		CurrentMenuBar.Reset(new UIMenuBar(this));

		AddElement(MakeStringID("__UIMANAGER_CURRENT_MENU_BAR__"), CurrentMenuBar);

		return CurrentMenuBar;
	};

	void UIManager::RemoveMenuFuture(MemoryStream &Stream)
	{
		RemoveElement(MakeStringID("__UIMANAGER_CURRENT_MENU__"));
	};

#endif
};

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

			if (!TheGUIManager.GetFocusedElement().Get())
				return !!TheGUIManager.GetInputBlocker().Get();

			if(Key.JustPressed)
			{
				if(TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::KeyJustPressed, { const_cast<InputCenter::KeyInfo *>(&Key) });
				};
			}
			else if(Key.Pressed)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::KeyPressed, { const_cast<InputCenter::KeyInfo *>(&Key) });
				};
			}
			else if(Key.JustReleased)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::KeyReleased, { const_cast<InputCenter::KeyInfo *>(&Key) });
				};
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};

			return TheManager.Input.InputConsumed();
		};

		bool OnTouch(const InputCenter::TouchInfo &Touch)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if (Touch.JustPressed)
			{
				TheGUIManager.UpdateFocusedElement(Touch.Position, UIInputType::Touch);

				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::TouchJustPressed, { const_cast<InputCenter::TouchInfo *>(&Touch) });
				};
			}
			else if (Touch.Pressed)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::TouchPressed, { const_cast<InputCenter::TouchInfo *>(&Touch) });
				};
			};

			if(Touch.Dragged)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::TouchDragged, { const_cast<InputCenter::TouchInfo *>(&Touch) });
				};
			};
			
			if(Touch.JustReleased)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::TouchReleased, { const_cast<InputCenter::TouchInfo *>(&Touch) });
				};
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};

			return TheManager.Input.InputConsumed();
		};

		bool OnMouseButton(const InputCenter::MouseButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(Button.JustPressed)
			{
				TheGUIManager.UpdateFocusedElement(TheManager.Input.MousePosition, UIInputType::Mouse);

				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::MouseJustPressed, { const_cast<InputCenter::MouseButtonInfo *>(&Button) });
				};
			}
			else if(Button.Pressed)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::MousePressed, { const_cast<InputCenter::MouseButtonInfo *>(&Button) });
				};
			}
			else if(Button.JustReleased)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::MouseReleased, { const_cast<InputCenter::MouseButtonInfo *>(&Button) });
				};
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};

			return TheManager.Input.InputConsumed();
		};

		bool OnJoystickButton(const InputCenter::JoystickButtonInfo &Button)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(Button.JustPressed)
			{
				if(TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickButtonJustPressed, { const_cast<InputCenter::JoystickButtonInfo *>(&Button) });
				};
			}
			else if(Button.Pressed)
			{
				if (TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickButtonPressed, { const_cast<InputCenter::JoystickButtonInfo *>(&Button) });
				};
			}
			else if(Button.JustReleased)
			{
				if(TheGUIManager.GetFocusedElement().Get())
				{
					TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickButtonReleased, { const_cast<InputCenter::JoystickButtonInfo *>(&Button) });
				};
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};

			return TheManager.Input.InputConsumed();
		};
		
		bool OnJoystickAxis(const InputCenter::JoystickAxisInfo &Axis)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(TheGUIManager.GetFocusedElement().Get())
			{
				TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickAxisMoved, { const_cast<InputCenter::JoystickAxisInfo *>(&Axis) });
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};

			return TheManager.Input.InputConsumed();
		};

		void OnJoystickConnected(uint8 Index)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if(TheGUIManager.GetFocusedElement().Get())
			{
				TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickConnected, { &Index });
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnJoystickDisconnected(uint8 Index)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if (TheGUIManager.GetFocusedElement().Get())
			{
				TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::JoystickDisconnected, { &Index });
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnMouseMove(const Vector3 &Position)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if (TheGUIManager.GetFocusedElement().Get())
			{
				TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::MouseMoved, { });
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnCharacterEntered(wchar_t Character)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			if (TheGUIManager.GetFocusedElement().Get())
			{
				TheGUIManager.GetFocusedElement()->OnEvent(UIEventType::CharacterEntered, { &Character });
			};

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnAction(const InputCenter::Action &TheAction)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			//TODO?

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnGainFocus() {};
		void OnLoseFocus() {};
	};

	bool TextParamsAreSimilar(const TextParams &A, const TextParams &B)
	{
		return A.BorderColorValue == B.BorderColorValue && A.BorderSizeValue == B.BorderSizeValue && A.FontSizeValue == B.FontSizeValue &&
			A.FontValue == B.FontValue && A.SecondaryTextColorValue == B.SecondaryTextColorValue && A.StyleValue == B.StyleValue &&
			A.TextColorValue == B.TextColorValue;
	};

#define COPYOFFSET(var, type)\
	memcpy(&Buffer[Offset], &var, sizeof(type));\
	\
	Offset += sizeof(type);

	StringID UIManager::MakeTextResourceString(uint32 Character, const TextParams &Parameters)
	{
		const uint32 Size = sizeof(uint32) + sizeof(TextParams) - sizeof(f32) - sizeof(Vector2);
		static uint8 Buffer[Size];

		uint32 Offset = 0;

		COPYOFFSET(Character, uint32);
		COPYOFFSET(Parameters.BorderColorValue, Vector4);
		COPYOFFSET(Parameters.BorderSizeValue, f32);
		COPYOFFSET(Parameters.FontSizeValue, uint32);
		COPYOFFSET(Parameters.FontValue, FontHandle);
		COPYOFFSET(Parameters.SecondaryTextColorValue, Vector4);
		COPYOFFSET(Parameters.TextColorValue, Vector4);
		COPYOFFSET(Parameters.StyleValue, uint32);

		return CRC32::Instance.CRC(Buffer, Size);
	};

	StringID UIManager::MakeTextureResourceString(const Path &FileName)
	{
		return MakeStringID(FileName.FullPath());
	};

	DisposablePointer<Texture> UIManager::GetUITexture(const Path &FileName)
	{
		StringID ID = MakeTextureResourceString(FileName);

		TextureResourceMap::iterator it = TextureResources.find(ID);

		if (it != TextureResources.end())
		{
			return it->second.InstanceTexture;
		};

		DisposablePointer<Texture> SourceTexture = ResourceManager::Instance.GetTextureFromPackage(FileName);

		if (!SourceTexture.Get())
			SourceTexture = ResourceManager::Instance.GetTexture(FileName);

		if (!SourceTexture.Get())
			return SourceTexture;

		TextureResourceInfo TheResource;
		TheResource.FileName = FileName;
		TheResource.References = 1;

		TheResource.SourceTexture = SourceTexture;
		TheResource.InstanceTexture = ResourcesGroup->Get(ResourcesGroup->Add(SourceTexture));

		return TheResource.InstanceTexture;
	};

	void UIManager::GetUIText(const std::string &Text, const TextParams &Parameters)
	{
		for (uint32 i = 0; i < Text.size(); i++)
		{
			if (Text[i] == ' ' || Text[i] == '\n' || Text[i] == '\r')
				continue;

			StringID ID = MakeTextResourceString(Text[i], Parameters);
			TextResourceMap::iterator it = TextResources.find(ID);

			if (it != TextResources.end())
			{
				it->second.References++;
			}
			else
			{
				TextResourceInfo TheResource;

				TheResource.Character = Text[i];
				TheResource.TextParameters = Parameters;
				TheResource.References = 1;

				TheResource.Info = Owner->GetTextGlyph(Text[i], Parameters);
				TheResource.InstanceTexture = ResourcesGroup->Get(ResourcesGroup->Add(Texture::CreateFromBuffer(TheResource.Info.Pixels)));

				TextResources[ID] = TheResource;
			};
		};
	};

	void UIManager::DrawText(const std::string &Text, const TextParams &Params)
	{
		static Sprite TheSprite;

		TextParams ActualParams = Params.FontValue ? Params : TextParams(Params).Font(RenderTextUtils::DefaultFont);
		Vector2 Position = Params.PositionValue, InitialPosition = Position;

		uint32 SpaceSize = Owner->GetTextGlyph(' ', ActualParams).Advance;

		GetUIText(Text, ActualParams);

		for (uint32 i = 0; i < Text.length(); i++)
		{
			switch (Text[i])
			{
			case ' ':
				Position.x = Position.x + SpaceSize;

				break;

			case '\n':
				Position.x = InitialPosition.x;
				Position.y += Params.FontSizeValue;

				break;

			case '\r':

				break;

			default:
				{
					StringID ID = MakeTextResourceString(Text[i], ActualParams);
					TextResourceMap::iterator it = TextResources.find(ID);

					if (i > 0)
						Position.x += Owner->GetTextKerning(Text[i - 1], Text[i], ActualParams);

					if (it != TextResources.end())
					{
						TheSprite.Options.Position(Position + it->second.Info.Offset);
						TheSprite.SpriteTexture = it->second.InstanceTexture;

						TheSprite.Draw(Owner);

						Position.x = Position.x + it->second.Info.Advance;
					}
					else
					{
						Position.x = Position.x + SpaceSize;
					};
				};

				break;
			};
		};
	};

	UIManager::UIManager(Renderer *TheOwner) : Owner(TheOwner), DrawOrderCounter(0), DrawOrderCacheDirty(false), DrawUIRects(0), DrawUIFocusZones(0)
	{
		ResourcesGroup.Reset(new TextureGroup(4096, 4096));

		std::vector<LuaLib *> Libs;
		Libs.push_back(&FrameworkLib::Instance);

		if(GameInterface::Instance)
		{
			Libs.push_back(GameInterface::Instance);

			ScriptInstance = GameInterface::Instance->GetScriptInstance();
		};

		if(ScriptInstance.Get() == NULL)
			ScriptInstance = LuaScriptManager::Instance.CreateScript("", &Libs[0], Libs.size());

		RegisterInput();
	};

	DisposablePointer<UILayout> UIManager::GetLayout(StringID LayoutID)
	{
		if(Layouts.find(LayoutID) != Layouts.end())
			return Layouts[LayoutID];

		return DisposablePointer<UILayout>();
	};

	void UIManager::AddLayout(StringID LayoutID, DisposablePointer<UILayout> Layout)
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

	void UIManager::ProcessTextProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UIText *TheText = (UIText *)Element;

		if(Property == "Font")
		{
			FontHandle Handle = ResourceManager::Instance.GetFontFromPackage(Owner, Path(Value));

			if(Handle == 0)
			{
				Handle = ResourceManager::Instance.GetFont(Owner, Path(Value));

				if(Handle == 0)
				{
					Log::Instance.LogWarn(TAG, "Unable to load font '%s'", Value.c_str());

					return;
				};
			};

			TheText->TextParameters.Font(Handle);
		}
		else if(Property == "FontSize")
		{
			uint32 FontSize = 0;

			if(1 == sscanf(Value.c_str(), "%u", &FontSize))
			{
				TheText->TextParameters.FontSize(FontSize);
			};
		}
		else if(Property == "Text")
		{
			TheText->SetText(Value);
		}
		else if(Property == "Bold")
		{
			uint32 IntValue = 0;

			if(1 == sscanf(Value.c_str(), "%u", &IntValue))
			{
				if(!!IntValue)
				{
					TheText->TextParameters.StyleValue |= TextStyle::Bold;
				}
				else
				{
					TheText->TextParameters.StyleValue = TheText->TextParameters.StyleValue & ~TextStyle::Bold;
				};
			};
		}
		else if(Property == "Italic")
		{
			uint32 IntValue = 0;

			if(1 == sscanf(Value.c_str(), "%u", &IntValue))
			{
				if(!!IntValue)
				{
					TheText->TextParameters.StyleValue |= TextStyle::Italic;
				}
				else
				{
					TheText->TextParameters.StyleValue = TheText->TextParameters.StyleValue & ~TextStyle::Italic;
				};
			};
		}
		else if(Property == "Underline")
		{
			uint32 IntValue = 0;

			if(1 == sscanf(Value.c_str(), "%u", &IntValue))
			{
				if(!!IntValue)
				{
					TheText->TextParameters.StyleValue |= TextStyle::Underline;
				}
				else
				{
					TheText->TextParameters.StyleValue = TheText->TextParameters.StyleValue & ~TextStyle::Underline;
				};
			};
		}
		else if(Property == "Alignment")
		{
			uint32 Alignment = 0;

			std::string AlignmentString = StringUtils::ToUpperCase(Value);

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
				}
				else if(Fragments[j] == "BOTTOM")
				{
					Alignment |= UITextAlignment::Bottom;
				};
			};

			if(Fragments.size() == 0)
				Alignment = UITextAlignment::Left;

			TheText->TextAlignment = Alignment;
		}
		else if(Property == "TextColor")
		{
			Vector4 Color;

			if(4 != sscanf(Value.c_str(), "%f,%f,%f,%f", &Color.x, &Color.y, &Color.z, &Color.w))
				return;

			TheText->TextParameters.TextColorValue = Color;
			TheText->TextParameters.SecondaryTextColorValue = TheText->TextParameters.TextColorValue;
		}
		else if(Property == "SecondaryTextColor")
		{
			Vector4 Color;

			if(4 != sscanf(Value.c_str(), "%f,%f,%f,%f", &Color.x, &Color.y, &Color.z, &Color.w))
				return;

			TheText->TextParameters.SecondaryTextColorValue = Color;
		}
		else if(Property == "Border")
		{
			f32 Size = 0;

			if(1 != sscanf(Value.c_str(), "%f", &Size))
				return;

			TheText->TextParameters.BorderSizeValue = Size;
		}
		else if(Property == "BorderColor")
		{
			Vector4 Color;

			if(4 != sscanf(Value.c_str(), "%f,%f,%f,%f", &Color.x, &Color.y, &Color.z, &Color.w))
				return;

			TheText->TextParameters.BorderColorValue = Color;
		};
	};

	void UIManager::ProcessTextJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		UIText *TheText = (UIText *)Element;
		Json::Value Value = Data.get("ExpandHeight", Json::Value(false));

		Value = Data.get("FontSize", Json::Value((Json::Value::Int)UIELEMENT_DEFAULT_FONT_SIZE));

		if(Value.isInt())
		{
			ProcessTextProperty(Element, "FontSize", StringUtils::MakeIntString((uint32)Value.asInt()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "FontSize", int);
		};

		Value = Data.get("Font", Json::Value(""));

		if(Value.isString())
		{
			if(Value.asString().length() > 0)
				ProcessTextProperty(Element, "Font", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Font", string);
		};

		Value = Data.get("Text", Json::Value(""));

		if(Value.isString())
		{
			ProcessTextProperty(Element, "Text", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Text", string);
		};

		Value = Data.get("Bold", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Element, "Bold", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Bold", string);
		};

		Value = Data.get("Italic", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Element, "Italic", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Italic", string);
		};

		Value = Data.get("Underline", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Element, "Underline", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Underline", string);
		};

		Value = Data.get("Alignment", Json::Value(""));

		if(Value.isString())
		{
			ProcessTextProperty(Element, "Alignment", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Alignment", string);
		};

		static std::stringstream str;
		str.str("");
		str << TheText->TextParameters.TextColorValue.x << "," << TheText->TextParameters.TextColorValue.y << "," << TheText->TextParameters.TextColorValue.z << "," <<
			TheText->TextParameters.TextColorValue.w;

		Value = Data.get("TextColor", Json::Value());

		if(Value.type() == Json::stringValue)
		{
			if(Value.asString().length())
			{
				ProcessTextProperty(Element, "TextColor", Value.asString(), ElementName, LayoutName);
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
			if(Value.asString().length())
			{
				ProcessTextProperty(Element, "SecondaryTextColor", Value.asString(), ElementName, LayoutName);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, "SecondaryTextColor", string);
		};

		Value = Data.get("Border", Json::Value("0"));

		if(Value.type() == Json::stringValue)
		{
			ProcessTextProperty(Element, "Border", Value.asString(), ElementName, LayoutName);
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
			ProcessTextProperty(Element, "BorderColor", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "BorderColor", string);
		};
	};

	void UIManager::ProcessSpriteProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UISprite *TheSprite = (UISprite *)Element;

		if(Property == "Path")
		{
			std::string FileName = Value;

			if(FileName.length() == 0)
			{
				TheSprite->TheSprite.Options.Scale(TheSprite->Size()).Color(Vector4());
			}
			else
			{
				DisposablePointer<Texture> SpriteTexture = GetUITexture(Path(FileName));

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
						LayoutName.c_str());
				}
				else
				{
					TheSprite->TheSprite.SpriteTexture = SpriteTexture;
					TheSprite->TheSprite.Options.Scale(Element->Size() != Vector2() ? Element->Size() / SpriteTexture->Size() : Vector2(1, 1));
					TheSprite->TheSprite.SpriteTexture->SetTextureFiltering(TextureFiltering::Linear);

					if(TheSprite->Size() == Vector2())
					{
						TheSprite->SetSize(SpriteTexture->Size());
					};
				};
			};
		}
		else if(Property == "Filtering")
		{
			if(TheSprite->TheSprite.SpriteTexture.Get() == NULL)
				return;

			std::string Temp = Value;

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
		else if(Property == "NinePatch")
		{
			if(TheSprite->TheSprite.SpriteTexture.Get() == NULL)
				return;

			std::string NinePatchString = Value;

			if(NinePatchString.length())
			{
				Rect NinePatchRect;

				if(4 != sscanf(NinePatchString.c_str(), "%f,%f,%f,%f", &NinePatchRect.Left, &NinePatchRect.Right, &NinePatchRect.Top,
					&NinePatchRect.Bottom))
					return;

				TheSprite->TheSprite.Options.NinePatch(true, NinePatchRect).Scale(Element->Size());
			};
		}
		else if(Property == "Color")
		{
			std::string ColorString = Value;

			Vector4 Color;

			if(4 == sscanf(ColorString.c_str(), "%f, %f, %f, %f", &Color.x, &Color.y, &Color.z, &Color.w))
			{
				TheSprite->TheSprite.Options.Color(Color);
			};
		}
		else if(Property == "ScaleWide")
		{
			f32 FloatValue = 0;

			if(1 == sscanf(Value.c_str(), "%f", &FloatValue))
				TheSprite->TheSprite.Options.Scale(Vector2(FloatValue, TheSprite->TheSprite.Options.ScaleValue.y));
		}
		else if(Property == "ScaleTall")
		{
			f32 FloatValue = 0;

			if(1 == sscanf(Value.c_str(), "%f", &FloatValue))
				TheSprite->TheSprite.Options.Scale(Vector2(TheSprite->TheSprite.Options.ScaleValue.x, FloatValue));
		}
		else if(Property == "Colors")
		{
			std::vector<std::string> Elements = StringUtils::Split(Value, '|');

			if(Elements.size() != 4)
				return;

			Vector4 Colors[4];

			for(uint32 i = 0; i < 4; i++)
			{
				if(4 != sscanf(Elements[i].c_str(), "%f,%f,%f,%f", &Colors[i].x, &Colors[i].y, &Colors[i].z, &Colors[i].w))
					return;
			};

			TheSprite->TheSprite.Options.Colors(Colors[0], Colors[1], Colors[2], Colors[3]);
		}
		else if(Property == "Wireframe")
		{
			TheSprite->TheSprite.Options.Wireframe(Value == "true");
		};
	};

	void UIManager::ProcessSpriteJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		Json::Value Value = Data.get("Path", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Element, "Path", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Path", string);
		};

		Value = Data.get("Filtering", Json::Value("LINEAR"));

		if(Value.isString())
		{
			ProcessSpriteProperty(Element, "Filtering", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Filtering", string);
		};

		Value = Data.get("NinePatch", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Element, "NinePatch", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "NinePatch", string)
		};

		Value = Data.get("NinePatchScale", Json::Value(1.0));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Element, "NinePatchScale", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "NinePatchScale", double);
		};

		Value = Data.get("Colors", Json::Value("1,1,1,1"));

		if(Value.isString())
		{
			ProcessSpriteProperty(Element, "Colors", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Colors", string)
		};

		Value = Data.get("Color", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Element, "Color", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Color", string)
		};

		Value = Data.get("Wireframe", Json::Value(false));

		if(Value.isBool())
		{
			ProcessSpriteProperty(Element, "Wireframe", Value.asBool() ? "true" : "false", ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Wireframe", bool)
		};

		Value = Data.get("ScaleWide", Json::Value(""));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Element, "ScaleWide", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		};

		Value = Data.get("ScaleTall", Json::Value(""));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Element, "ScaleTall", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		};
	};

	void UIManager::ProcessGroupProperty(UIElement *Element, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UIGroup *Group = (UIGroup *)Element;

		if(Property == "LayoutMode")
		{
			std::vector<std::string> Modes = StringUtils::Split(StringUtils::ToUpperCase(Value), '|');

			for(uint32 j = 0; j < Modes.size(); j++)
			{
				if(Modes[j] == "HORIZONTAL")
				{
					Group->LayoutMode |= UIGroupLayoutMode::Horizontal;
				}
				else if(Modes[j] == "VERTICAL")
				{
					Group->LayoutMode |= UIGroupLayoutMode::Vertical;
				}
				else if(Modes[j] == "ADJUSTHEIGHT")
				{
					Group->LayoutMode |= UIGroupLayoutMode::AdjustHeight;
				}
				else if(Modes[j] == "ADJUSTWIDTH")
				{
					Group->LayoutMode |= UIGroupLayoutMode::AdjustWidth;
				}
				else if(Modes[j] == "NONE")
				{
					Group->LayoutMode = UIGroupLayoutMode::None;
				}
				else if(Modes[j] == "CENTER")
				{
					Group->LayoutMode |= UIGroupLayoutMode::Center;
				}
				else if(Modes[j] == "VCENTER")
				{
					Group->LayoutMode |= UIGroupLayoutMode::VerticalCenter;
				}
				else if(Modes[j] == "ADJUSTCLOSER")
				{
					Group->LayoutMode |= UIGroupLayoutMode::AdjustCloser;
				};
			};
		};
	};

	void UIManager::ProcessGroupJSON(UIElement *Element, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		Json::Value Value = Data.get("LayoutMode", Json::Value("None"));

		if(Value.isString())
		{
			ProcessGroupProperty(Element, "LayoutMode", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "LayoutMode", string)
		};
	};

	std::string JsonToLuaString(const Json::Value &Value)
	{
		static std::stringstream Out;
		Out.str("");

		switch(Value.type())
		{
		case Json::arrayValue:
			Out << "{ ";

			for(uint32 i = 0; i < Value.size(); i++)
			{
				if(i > 0)
					Out << ", ";

				switch(Value[i].type())
				{
				case Json::arrayValue:
				case Json::objectValue:
				case Json::nullValue:
					Out << JsonToLuaString(Value[i]);

					break;

				case Json::booleanValue:
					Out << Value[i].asBool();

					break;

				case Json::intValue:
					Out << Value[i].asInt();

					break;

				case Json::realValue:
					Out << Value[i].asDouble();

					break;

				case Json::stringValue:
					Out << "\"" << Value[i].asString() << "\"";

					break;

				case Json::uintValue:
					Out << Value[i].asUInt();

					break;
				};
			};

			Out << " }";

			break;

		case Json::nullValue:
			return "nil";

			break;

		case Json::objectValue:
			Out << "{ ";

			for(uint32 i = 0; i < Value.size(); i++)
			{
				if(i > 0)
					Out << ", ";

				switch(Value[Value.getMemberNames()[i]].type())
				{
				case Json::arrayValue:
				case Json::objectValue:
				case Json::nullValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = " << JsonToLuaString(Value[Value.getMemberNames()[i]]);

					break;

				case Json::booleanValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = " << Value[Value.getMemberNames()[i]].asBool();

					break;

				case Json::intValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = " << Value[Value.getMemberNames()[i]].asInt();

					break;

				case Json::realValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = " << Value[Value.getMemberNames()[i]].asDouble();

					break;

				case Json::stringValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = \"" << Value[Value.getMemberNames()[i]].asString() << "\"";

					break;

				case Json::uintValue:
					Out << "\"" << Value.getMemberNames()[i] << "\" = " << Value[Value.getMemberNames()[i]].asUInt();

					break;
				};
			};

			Out << "}";

			break;
		};

		return Out.str();
	};

	void UIManager::CopyElementsToLayout(DisposablePointer<UILayout> TheLayout, const Json::Value &Elements, UIElement *Parent, const std::string &ParentElementName, UIElement *ParentLimit)
	{
		if(Elements.type() != Json::arrayValue)
			return;

		Renderer *Renderer = Owner;

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

			const Json::Value &Data = Elements[i + 1];

			if(!Data.isObject())
			{
				static std::stringstream str;
				str.str("");
				str << ElementIDName;

				CHECKJSONVALUE(Value, str.str().c_str(), object);

				continue;
			};

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

			DisposablePointer<UIElement> Element;

			Control = StringUtils::ToUpperCase(Control);

			if(Control == "SPRITE")
			{
				Element.Reset(new UISprite(Renderer->UI));
			}
			else if(Control == "TEXT")
			{
				Element.Reset(new UIText(Renderer->UI));
			}
			else if(Control == "TEXTCOMPOSER")
			{
				Element.Reset(new UITextComposer(Renderer->UI));
			}
			else
			{
				Element.Reset(new UIGroup(Renderer->UI));
			};

			Element->LayoutValue = TheLayout;
			Element->LayoutNameValue = ElementIDName;

			std::stringstream BaseFunctionName;

			BaseFunctionName << StringUtils::Replace(ElementIDName, ".", "_") << "_";

			Json::Value EnabledValue = Data.get("Enabled", Json::Value(true)),
				KeyboardInputEnabledValue = Data.get("KeyboardInput", Json::Value(true)),
				JoystickInputEnabledValue = Data.get("JoystickInput", Json::Value(true)),
				MouseInputEnabledValue = Data.get("MouseInput", Json::Value(true)),
				TouchInputEnabledValue = Data.get("TouchInput", Json::Value(true)),
				VisibleValue = Data.get("Visible", Json::Value(true)),
				BlockingInputValue = Data.get("BlockingInput", Json::Value(false)),
				InputBlockBackgroundValue = Data.get("InputBlockBackground", Json::Value(true));

			uint32 EnabledInputs = UIInputType::All;

			if(EnabledValue.isBool())
			{
				Element->SetEnabled(EnabledValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(EnabledValue, "Enabled", bool);
			};

			if(KeyboardInputEnabledValue.isBool())
			{
				if (!KeyboardInputEnabledValue.asBool())
				{
					EnabledInputs = EnabledInputs | ~UIInputType::Keyboard;
				};
			}
			else
			{
				CHECKJSONVALUE(KeyboardInputEnabledValue, "KeyboardInput", bool);
			};

			if(TouchInputEnabledValue.isBool())
			{
				if(!TouchInputEnabledValue.asBool())
				{
					EnabledInputs = EnabledInputs | ~UIInputType::Touch;
				};
			}
			else
			{
				CHECKJSONVALUE(TouchInputEnabledValue, "TouchInput", bool);
			};

			if(JoystickInputEnabledValue.isBool())
			{
				if (!JoystickInputEnabledValue.asBool())
				{
					EnabledInputs = EnabledInputs | ~UIInputType::Joystick;
				};
			}
			else
			{
				CHECKJSONVALUE(JoystickInputEnabledValue, "JoystickInput", bool);
			};

			if(MouseInputEnabledValue.isBool())
			{
				if(!MouseInputEnabledValue.asBool())
				{
					EnabledInputs = EnabledInputs | ~UIInputType::Mouse;
				};
			}
			else
			{
				CHECKJSONVALUE(MouseInputEnabledValue, "MouseInput", bool);
			};

			Element->SetEnabledInputTypes(EnabledInputs);

			if(VisibleValue.isBool())
			{
				Element->SetVisible(VisibleValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(VisibleValue, "Visible", bool);
			};

			if(BlockingInputValue.isBool())
			{
				Element->SetBlockingInput(BlockingInputValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(BlockingInputValue, "BlockingInput", bool);
			};

			if(InputBlockBackgroundValue.isBool())
			{
				Element->SetInputBlockerBackground(InputBlockBackgroundValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(InputBlockBackgroundValue, "InputBlockBackground", bool);
			};

#define REGISTER_LUA_EVENT(name, type, extraparams)\
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
						Element->EventScriptHandlers[type].Add(luabind::globals(ScriptInstance->State)[ComposedFunctionName.str().c_str()]);\
						Element->GlobalsTracker.Add(ComposedFunctionName.str());\
					}\
				}\
				else\
				{\
					CHECKJSONVALUE(Value, #name, string);\
				};\
			};

			REGISTER_LUA_EVENT(OnMouseJustPressed, UIEventType::MouseJustPressed, ", Button");
			REGISTER_LUA_EVENT(OnMousePressed, UIEventType::MousePressed, ", Button");
			REGISTER_LUA_EVENT(OnMouseReleased, UIEventType::MouseReleased, ", Button");
			REGISTER_LUA_EVENT(OnTouchJustPressed, UIEventType::TouchJustPressed, ", Touch");
			REGISTER_LUA_EVENT(OnTouchPressed, UIEventType::TouchPressed, ", Touch");
			REGISTER_LUA_EVENT(OnTouchReleased, UIEventType::TouchReleased, ", Touch");
			REGISTER_LUA_EVENT(OnTouchDragged, UIEventType::TouchDragged, ", Touch");
			REGISTER_LUA_EVENT(OnJoystickButtonJustPressed, UIEventType::JoystickButtonJustPressed, ", Button");
			REGISTER_LUA_EVENT(OnJoystickButtonPressed, UIEventType::JoystickButtonPressed, ", Button");
			REGISTER_LUA_EVENT(OnJoystickButtonReleased, UIEventType::JoystickButtonReleased, ", Button");
			REGISTER_LUA_EVENT(OnJoystickAxisMoved, UIEventType::JoystickAxisMoved, ", Axis");
			REGISTER_LUA_EVENT(OnJoystickConnected, UIEventType::JoystickConnected, ", Index");
			REGISTER_LUA_EVENT(OnJoystickDisconnected, UIEventType::JoystickDisconnected, ", Index");
			REGISTER_LUA_EVENT(OnKeyJustPressed, UIEventType::KeyJustPressed, ", Key");
			REGISTER_LUA_EVENT(OnKeyPressed, UIEventType::KeyPressed, ", Key");
			REGISTER_LUA_EVENT(OnKeyReleased, UIEventType::KeyReleased, ", Key");
			REGISTER_LUA_EVENT(OnMouseMoved, UIEventType::MouseMoved, "");
			REGISTER_LUA_EVENT(OnCharacterEntered, UIEventType::CharacterEntered, "");
			REGISTER_LUA_EVENT(OnGainFocus, UIEventType::GainedFocus, "");
			REGISTER_LUA_EVENT(OnLoseFocus, UIEventType::LostFocus, "");
			REGISTER_LUA_EVENT(OnUpdate, UIEventType::Update, "");
			REGISTER_LUA_EVENT(OnDraw, UIEventType::Draw, "");
			REGISTER_LUA_EVENT(OnMouseEntered, UIEventType::MouseEntered, "");
			REGISTER_LUA_EVENT(OnMouseOver, UIEventType::MouseOver, "");
			REGISTER_LUA_EVENT(OnMouseLeft, UIEventType::MouseLeft, "");
			REGISTER_LUA_EVENT(OnClick, UIEventType::Click, ", Button");
			REGISTER_LUA_EVENT(OnTap, UIEventType::Tap, ", Touch");
			REGISTER_LUA_EVENT(OnJoystickTap, UIEventType::JoystickTap, ", Button");
			REGISTER_LUA_EVENT(OnStart, UIEventType::Start, "");

			TheLayout->Elements[MakeStringID(ElementIDName)] = Element;

			if (!Renderer->UI->AddElement(ElementID, Element))
				return;

			if(Parent)
				Parent->AddChild(Element);

			std::stringstream PositionScriptX, PositionScriptY, OffsetScriptX, OffsetScriptY, SizeScript;

			Value = Data.get("Wide", Json::Value("0"));

			if(Value.isString())
			{
				SizeScript << "local SizeX = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Wide", string);
			};

			Value = Data.get("Tall", Json::Value("0"));

			if(Value.isString())
			{
				SizeScript << "local SizeY = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Tall", string);
			};

			Value = Data.get("Xpos", Json::Value("0"));

			if(Value.isString())
			{
				PositionScriptX << "local PositionX = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Xpos", string);
			};

			Value = Data.get("Ypos", Json::Value("0"));

			if(Value.isString())
			{
				PositionScriptY << "local PositionY = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "Ypos", string);
			};

			Value = Data.get("XOffset", Json::Value("0"));

			if(Value.isString())
			{
				OffsetScriptX << "local XOffset = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "XOffset", string);
			};

			Value = Data.get("YOffset", Json::Value("0"));

			if(Value.isString())
			{
				OffsetScriptY << "local YOffset = " << Value.asString() << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "YOffset", string);
			};

			std::stringstream ComposedPositionFunction, ComposedSizeFunction;

			ComposedPositionFunction << "function " << BaseFunctionName.str() << "Position(Self, Parent, ScreenWidth, ScreenHeight)\n"
				"local parent_wide = 0\n"
				"local parent_tall = 0\n"
				"local left = 0\n"
				"local right = 0\n"
				"local top = 0\n"
				"local bottom = 0\n"
				"local center = 0\n\n"
				"if Parent ~= nil then\n"
				"	parent_wide = Parent.Size.x\n"
				"	parent_tall = Parent.Size.y\n"
				"else\n"
				"	parent_wide = ScreenWidth\n"
				"	parent_tall = ScreenHeight\n"
				"end\n\n"
				"left = 0\n"
				"right = parent_wide - Self.Size.x\n"
				"top = 0\n"
				"bottom = parent_tall - Self.Size.y\n"
				"center = (parent_wide - Self.Size.x) / 2\n\n" <<
				PositionScriptX.str() << 
				OffsetScriptX.str() << 
				"center = (parent_tall - Self.Size.y) / 2\n" <<
				PositionScriptY.str() << 
				OffsetScriptY.str() << 
				"Self.Position = Vector2(PositionX, PositionY)\n"
				"Self.Offset = Vector2(XOffset, YOffset)\n"
				"\nend\n";

			ComposedSizeFunction << "function " << BaseFunctionName.str() << "Size(Self, Parent, ScreenWidth, ScreenHeight)\n"
				"local parent_wide = 0\n"
				"local parent_tall = 0\n\n"
				"if Parent ~= nil then\n"
				"	parent_wide = Parent.Size.x\n"
				"	parent_tall = Parent.Size.y\n"
				"else\n"
				"	parent_wide = ScreenWidth\n"
				"	parent_tall = ScreenHeight\n"
				"end\n\n" <<
				SizeScript.str() <<
				"Self.Size = Vector2(SizeX, SizeY)\n"
				"\nend\n";

			ScriptInstance->DoString(ComposedPositionFunction.str().c_str());

			ScriptInstance->DoString(ComposedSizeFunction.str().c_str());

			Element->GlobalsTracker.Add(BaseFunctionName.str() + "Position");
			Element->GlobalsTracker.Add(BaseFunctionName.str() + "Size");

			LuaEventGroup PositionFunction, SizeFunction;
			PositionFunction.Add(luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Position"]);
			SizeFunction.Add(luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Size"]);

			if (!PositionFunction.Members.size() || !PositionFunction.Members.front())
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Position", ElementName.c_str());

				Element.Dispose();

				continue;
			}
			else if (!SizeFunction.Members.size() || !SizeFunction.Members.front())
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Size", ElementName.c_str());

				Element.Dispose();

				continue;
			};

			try
			{
				SizeFunction.Members.front()(Element, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Size", ElementName.c_str(), e.what());

				Element.Dispose();

				continue;
			};

			try
			{
				PositionFunction.Members.front()(Element, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Position", ElementName.c_str(), e.what());

				Element.Dispose();

				continue;
			};

			if(Control == "SPRITE")
			{
				ProcessSpriteJSON(Element, Data, ElementName, TheLayout->Name);
			}
			else if(Control == "GROUP")
			{
				ProcessGroupJSON(Element, Data, ElementName, TheLayout->Name);
			}
			else if(Control == "TEXT")
			{
				ProcessTextJSON(Element, Data, ElementName, TheLayout->Name);
			}
			else if (Control == "TEXTCOMPOSER")
			{
				//Do nothing for now
			}
			else
			{
				DisposablePointer<UILayout> TargetLayout;

				for(LayoutMap::iterator it = Layouts.begin(); it != Layouts.end(); it++)
				{
					if(StringUtils::ToUpperCase(it->second->Name) == Control)
					{
						TargetLayout = it->second;

						break;
					};
				};

				if(TargetLayout.Get() == NULL)
				{
					for(LayoutMap::iterator it = DefaultLayouts.begin(); it != DefaultLayouts.end(); it++)
					{
						if(StringUtils::ToUpperCase(it->second->Name) == Control)
						{
							TargetLayout = it->second;

							break;
						};
					};
				};

				if(TargetLayout.Get() == NULL)
				{
					Log::Instance.LogErr(TAG, "While processing Layout element '%s' of layout '%s': Layout '%s' not found (may not have been created already)",
						ElementName.c_str(), TheLayout->Name.c_str(), Control.c_str());

					TheLayout->Elements.erase(Element->ID());
					RemoveElement(Element->ID());

					return;
				};

				//We shouldn't override previous set sizes...
				bool ResetElementSize = Element->Size() == Vector2();

				if(ResetElementSize)
				{
					Element->SetSize(Parent ? Parent->Size() : Element->Size());
				};

				DisposablePointer<UILayout> NewLayout = TargetLayout->Clone(Element, ParentElementName + "." + ElementName, true, true);

				StringID LayoutID = MakeStringID(ParentElementName + "." + ElementName + "." + NewLayout->Name);

				LayoutMap::iterator it = Layouts.find(LayoutID);

				//Should be impossible for this to happen, but still...
				if(it != Layouts.end())
				{
					Log::Instance.LogWarn(TAG, "Found duplicate layout '%s', erasing old.", (Element->Layout()->Name + "_" + NewLayout->Name).c_str());

					Layouts.erase(it);
				};

				Layouts[LayoutID] = NewLayout;

				for(uint32 j = 0; j < Data.size(); j++)
				{
					std::string Name = Data.getMemberNames()[j];

					Json::Value Value = Data.get(Name, Json::Value());

					for(UILayout::ElementMap::iterator it = NewLayout->Elements.begin(); it != NewLayout->Elements.end(); it++)
					{
						std::string ElementName = StringUtils::ToUpperCase(GetStringIDString(it->first));

						ElementName = ElementName.substr(ParentElementName.length() + 1);

						int32 Index = ElementName.find(Control);

						ElementName = ElementName.substr(Index + Control.size() + 1);

						if(StringUtils::ToUpperCase(Name).find(ElementName) == 0)
						{
							Name = Name.substr(ElementName.length() + 1);

							UIElement *TargetElement = it->second;

							std::vector<std::string> Parts = StringUtils::Split(Name, '.');

							if(Parts.size() > 1)
							{
								std::string CurrentElementID = GetStringIDString(it->first);

								for(uint32 k = 0; k < Parts.size() - 1; k++)
								{
									CurrentElementID += "." + Parts[k];

									StringID ActualElementID = MakeStringID(CurrentElementID);

									bool Found = false;

									for(uint32 l = 0; l < TargetElement->ChildrenCount(); l++)
									{
										if(TargetElement->Child(l)->ID() == ActualElementID)
										{
											Found = true;

											TargetElement = TargetElement->Child(l);

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
								std::string ControlName = StringUtils::ToUpperCase(TargetElement->NativeTypeValue);
								std::string Property = Parts[Parts.size() - 1];
								std::string CurrentElementID = GetStringIDString(TargetElement->ID());

								std::string FinalValue;

								if(Value.isBool())
								{
									FinalValue = StringUtils::MakeIntString((uint32)Value.asBool());
								}
								else if(Value.isInt())
								{
									FinalValue = StringUtils::MakeIntString((int32)Value.asInt());
								}
								else if(Value.isUInt())
								{
									FinalValue = StringUtils::MakeIntString((uint32)Value.asUInt());
								}
								else if(Value.isDouble())
								{
									FinalValue = StringUtils::MakeFloatString((f32)Value.asDouble());
								}
								else if(Value.isString())
								{
									FinalValue = Value.asString();
								}
								else
								{
									//ProcessCustomProperty(TargetElement, Property, Value, ElementName, NewLayout->Name);

									continue;
								};

								if(ControlName == "UISPRITE")
								{
									ProcessSpriteProperty(TargetElement, Property, FinalValue, ElementName, NewLayout->Name);
								}
								else if(ControlName == "UIGROUP")
								{
									ProcessGroupProperty(TargetElement, Property, FinalValue, ElementName, NewLayout->Name);
								}
								else if(ControlName == "UITEXT")
								{
									ProcessTextProperty(TargetElement, Property, FinalValue, ElementName, NewLayout->Name);
								};

								//ProcessCustomProperty(TargetElement, Property, Value, ElementName, NewLayout->Name);

								break;
							};
						};
					};
				};

				if(ResetElementSize)
				{
					Element->SetSize(Element->ChildrenSize());
				};
			};

			Value = Data.get("TooltipElement", Json::Value(""));

			if (Value.isArray())
			{
				DisposablePointer<UIElement> TooltipGroup(new UIGroup(Renderer->UI));
				TooltipGroup->SetVisible(false);
				Renderer->UI->AddElement(MakeStringID(ParentElementName + "." + ElementName + "_TOOLTIPELEMENT"), TooltipGroup);
				TheLayout->Elements[TooltipGroup->ID()] = TooltipGroup;

				CopyElementsToLayout(TheLayout, Value, TooltipGroup, ParentElementName + "." + ElementName + "_TOOLTIPELEMENT", Element);

				TooltipGroup->SetSize(TooltipGroup->ChildrenSize());

				Element->SetRespondsToTooltips(true);
				Element->SetTooltipElement(TooltipGroup);
			};

			Value = Data.get("TooltipFixed", Json::Value(false));

			if(Value.isBool())
			{
				Element->SetTooltipsFixed(Value.asBool());
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
					//TODO: Turn into a Script function
					Vector2 Position;

					if(2 != sscanf(Value.asString().c_str(), "%f,%f", &Position.x, &Position.y))
					{
						Log::Instance.LogWarn(TAG, "While processing Layout element '%s' of layout '%s': Invalid Tooltip Position, expected \"x, y\"",
							ElementName.c_str(), TheLayout->Name.c_str());
					}
					else
					{
						Element->SetTooltipsPosition(Position);
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

			CopyElementsToLayout(TheLayout, Children, Element, ParentElementName + "." + ElementName, ParentLimit);
		};
	};

	bool UIManager::LoadLayouts(Stream *In, DisposablePointer<UIElement> Parent, bool DefaultLayout)
	{
		Json::Value Root;
		Json::Reader Reader;
		std::string Content = In->AsString();

		if(!Reader.parse(Content, Root))
		{
			Log::Instance.LogErr(TAG, "Failed to parse a GUI layout resource: %s", Reader.getFormatedErrorMessages().c_str());

			return false;
		};

		for(uint32 i = 0; i < Root.size(); i+=2)
		{
			std::string LayoutName = Root[i].asString();
			Json::Value Elements = Root[i + 1];
			DisposablePointer<UILayout> Layout(new UILayout());
			Layout->Name = LayoutName;
			Layout->Owner = this;
			Layout->ContainedObjects = Elements;
			Layout->Parent = Parent;

			if (!DefaultLayout)
				CopyElementsToLayout(Layout, Elements, Parent, Layout->Name, Parent);

			StringID LayoutID = MakeStringID((Parent.Get() ? Parent->Layout()->Name + "_" : "") + LayoutName);

			LayoutMap &TargetLayoutMap = DefaultLayout ? DefaultLayouts : Layouts;

			LayoutMap::iterator it = TargetLayoutMap.find(LayoutID);

			if(it != TargetLayoutMap.end())
			{
				Log::Instance.LogWarn(TAG, "Found duplicate layout '%s' in %s, erasing old.", LayoutName.c_str(), DefaultLayout ? "Default Layouts" : "Layouts");

				TargetLayoutMap.erase(it);
			};

			TargetLayoutMap[LayoutID] = Layout;

			if(!DefaultLayout)
			{
				Layout->PerformStartupEvents(NULL);
			};
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

		while(DefaultLayouts.begin() != DefaultLayouts.end())
		{
			DefaultLayouts.begin()->second.Dispose();
			DefaultLayouts.erase(DefaultLayouts.begin());
		};

		MouseOverElement = DisposablePointer<UIElement>();
		FocusedElementValue = DisposablePointer<UIElement>();
	};

	DisposablePointer<UIElement> UIManager::GetInputBlocker()
	{
		DisposablePointer<UIElement> Out;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			while(it != Elements.end() && it->second.Get() == NULL)
			{
				Elements.erase(it);
				it = Elements.begin();
			};

			if(it == Elements.end())
				break;

			if(it->second->Element->BlockingInput() && it->second->Element->Visible())
			{
				Out = it->second->Element;

				break;
			};
		};

		return Out;
	};

	DisposablePointer<UIElement> UIManager::GetMouseOverElement()
	{
#if !FLPLATFORM_MOBILE //Mobile has no mouse over events
		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Element->Parent().Get() == nullptr)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		DisposablePointer<UIElement> FoundElement;

		DisposablePointer<UIElement> InputBlocker = GetInputBlocker();

		if(InputBlocker.Get())
		{
			DisposablePointer<UIElement> p = InputBlocker;
			RecursiveFindFocusedElement(p->ParentPosition(), p, FoundElement, true, RendererManager::Instance.Input.MousePosition);
		}
		else
		{
			for(int32 i = DrawOrderCounter; i >= 0; i--)
			{
				for(uint32 j = 0; j < DrawOrderCache.size(); j++)
				{
					if((DrawOrderCache[j]->Element->EnabledInputTypes() & UIInputType::Mouse) == 0 || DrawOrderCache[j]->DrawOrder != i)
						continue;

					DisposablePointer<UIElement> p = DrawOrderCache[j]->Element;

					RecursiveFindFocusedElement(Vector2(), p, FoundElement, true, RendererManager::Instance.Input.MousePosition);

					if(FoundElement)
						break;
				};

				if(FoundElement)
					break;
			};
		};

		if(MouseOverElement != FoundElement)
		{
			if (MouseOverElement.Get() != nullptr)
				MouseOverElement->OnEvent(UIEventType::MouseLeft, {});
		}
		else if(MouseOverElement)
		{
			MouseOverElement->OnEvent(UIEventType::MouseOver, {});
		};

		if(FoundElement && Elements[FoundElement->ID()].Get())
		{
			if(MouseOverElement != FoundElement && MouseOverElement.Get())
				MouseOverElement->OnEvent(UIEventType::MouseEntered, {});

			MouseOverElement = FoundElement;

			return Elements[FoundElement->ID()]->Element;
		};

		MouseOverElement = FoundElement;
#endif

		return DisposablePointer<UIElement>();
	};

	void UIManager::Update()
	{
		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Element->Parent().Get() == nullptr)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		for(uint32 i = 0; i < DrawOrderCache.size(); i++)
		{
			if(DrawOrderCache[i]->Element.Get() == nullptr)
				continue;

			if(DrawOrderCache[i]->Element->Visible())
			{
				DrawOrderCache[i]->Element->Update(Vector2());
			};
		};

		//TEMP: Need to fix mouseover not being calculated unless we put this here
		GetMouseOverElement();
	};

	void UIManager::Draw()
	{
		DrawUIRects = !!(Console::Instance.GetVariable("r_drawuirects") ? Console::Instance.GetVariable("r_drawuirects")->UIntValue : 0);
		DrawUIFocusZones = !!(Console::Instance.GetVariable("r_drawuifocuszones") ? Console::Instance.GetVariable("r_drawuifocuszones")->UIntValue : 0);

		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Element->Parent().Get() == nullptr)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		DisposablePointer<UIElement> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Element->BlockingInput() && it->second->Element->Visible())
			{
				InputBlocker = it->second->Element;

				break;
			};
		};

		for(uint32 i = 0; i <= DrawOrderCounter; i++)
		{
			for(uint32 j = 0; j < DrawOrderCache.size(); j++)
			{
				if(DrawOrderCache[j]->Element.Get() == NULL || DrawOrderCache[j]->DrawOrder != i || !DrawOrderCache[j]->Element->Visible())
					continue;

				if(DrawOrderCache[j]->Element == InputBlocker && DrawOrderCache[j]->Element->InputBlockerBackground())
				{
					Sprite BackgroundSprite;
					BackgroundSprite.Options.Scale(Owner->Size()).Color(Vector4(0, 0, 0, 0.3f));
					BackgroundSprite.Draw(Owner);
				};

				DrawOrderCache[j]->Element->Draw(Vector2(), Owner);
			};
		};
	};

	void UIManager::UpdateFocusedElement(const Vector2 &FocusPoint, uint32 InputType)
	{
		if (DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for (ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if (it->second->Element->Parent().Get() == nullptr)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		DisposablePointer<UIElement> PreviouslyFocusedElement = FocusedElementValue;
		FocusedElementValue = DisposablePointer<UIElement>();

		DisposablePointer<UIElement> FoundElement;

		DisposablePointer<UIElement> InputBlocker = GetInputBlocker();

		if (InputBlocker.Get())
		{
			DisposablePointer<UIElement> p = InputBlocker;
			RecursiveFindFocusedElement(p->ParentPosition(), p, FoundElement, true, RendererManager::Instance.Input.MousePosition);
		}
		else
		{
			for (int32 i = DrawOrderCounter; i >= 0; i--)
			{
				for (uint32 j = 0; j < DrawOrderCache.size(); j++)
				{
					if ((DrawOrderCache[j]->Element->EnabledInputTypes() & InputType) == 0 || DrawOrderCache[j]->DrawOrder != i)
						continue;

					DisposablePointer<UIElement> p = DrawOrderCache[j]->Element;

					RecursiveFindFocusedElement(Vector2(), p, FoundElement, true, RendererManager::Instance.Input.MousePosition);

					if (FoundElement)
						break;
				};

				if (FoundElement)
					break;
			};
		};

		if (FoundElement)
		{
			FocusedElementValue = Elements[FoundElement->ID()]->Element;
		};

		if (PreviouslyFocusedElement && PreviouslyFocusedElement.Get() != FocusedElementValue.Get())
		{
			PreviouslyFocusedElement->OnEvent(UIEventType::LostFocus, {});
		};

		if (FocusedElementValue)
		{
			FocusedElementValue->OnEvent(UIEventType::GainedFocus, {});
		};
	};

	void UIManager::RecursiveFindFocusedElement(const Vector2 &ParentPosition, DisposablePointer<UIElement> p, DisposablePointer<UIElement> &FoundElement, bool Mouse, const Vector2 &TargetPosition)
	{
		if (!p->Visible() || !p->Enabled() || (Mouse && (p->EnabledInputTypes() & UIInputType::Mouse) == 0) || (!Mouse && (p->EnabledInputTypes() & UIInputType::Touch) == 0))
			return;

		static AxisAlignedBoundingBox AABB;

		Vector2 PanelSize = p->Size();

		Vector2 ActualPosition = ParentPosition + p->Position() + p->Offset();

		AABB.min = ActualPosition;
		AABB.max = AABB.min + PanelSize;

		if(AABB.IsInside(TargetPosition))
		{
			FoundElement = p;

			for(uint32 i = 0; i < p->ChildrenValue.size(); i++)
			{
				RecursiveFindFocusedElement(ActualPosition, p->ChildrenValue[i], FoundElement, Mouse, TargetPosition);
			};
		}
		else
		{
			//Log::Instance.LogDebug(TAG, "Failed Focus Test on '%s'", p->Name.c_str());
		};
	};

	void UIManager::RegisterInput()
	{
		DisposablePointer<UIInputProcessor> Out(new UIInputProcessor());
		Out->Name = "GUIPROCESSOR_" + StringUtils::PointerString(this);

		RendererManager::Instance.Input.AddContext(Out);
		RendererManager::Instance.Input.EnableContext(MakeStringID(Out->Name));
	};

	void UIManager::UnRegisterInput()
	{
		RendererManager::Instance.Input.DisableContext(MakeStringID("GUIPROCESSOR_" + StringUtils::PointerString(this)));
	};

	bool UIManager::AddElement(StringID ID, DisposablePointer<UIElement> Element)
	{
		DrawOrderCacheDirty = true;

		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			if(!it->second.Get() || !it->second->Element.Get())
			{
				Elements.erase(it);
			}
			else
			{
				Log::Instance.LogWarn(TAG, "Failed to add Element 0x%08x: Duplicate ID 0x%08x", Element.Get(), ID);

				return false;
			};
		};

		FLASSERT(Element->ManagerValue == this, "Found Element from another UI Manager!");

		if(Element->ManagerValue != this)
			return false;

		Elements[ID].Reset(new ElementInfo());
		Elements[ID]->Element = Element;
		Elements[ID]->DrawOrder = ++DrawOrderCounter;
		Element->IDValue = ID;
		Element->NameValue = GetStringIDString(ID);

#if FLPLATFORM_DEBUG
		std::string ElementID = GetStringIDString(ID);

		if(ElementID.length())
		{
			ElementIDs[ElementID] = ID;
		};
#endif

		return true;
	};

	void UIManager::RemoveElement(StringID ID)
	{
		DrawOrderCacheDirty = true;

		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			DisposablePointer<ElementInfo> Element = it->second;

			if(Element.Get() && MouseOverElement == Element->Element.Get())
				MouseOverElement = DisposablePointer<UIElement>();

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
		MouseOverElement = DisposablePointer<UIElement>();
	};

	void UIManager::ClearFocus()
	{
		if(FocusedElementValue.Get())
			FocusedElementValue->OnEvent(UIEventType::LostFocus, {});

		FocusedElementValue = DisposablePointer<UIElement>();
	};

	DisposablePointer<UIElement> UIManager::GetElement(StringID ID)
	{
		ElementMap::iterator it = Elements.find(ID);

		if(it != Elements.end())
		{
			return it->second->Element;
		};

		return DisposablePointer<UIElement>();
	};

	DisposablePointer<UIElement> UIManager::GetFocusedElement()
	{
		return FocusedElementValue;
	};

	UIManager::~UIManager()
	{
		UnRegisterInput();
		Clear();

		ScriptInstance.Dispose();
	};

	Renderer *UIManager::GetOwner()
	{
		return Owner;
	};
#endif
};

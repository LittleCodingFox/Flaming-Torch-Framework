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
				TheGUIManager.OnJoystickButtonJustPressedPriv(Button);
			}
			else if(Button.Pressed)
			{
				TheGUIManager.OnJoystickButtonPressedPriv(Button);
			}
			else if(Button.JustReleased)
			{
				TheGUIManager.OnJoystickButtonReleasedPriv(Button);
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

			TheGUIManager.OnJoystickAxisMovedPriv(Axis);

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

			TheGUIManager.OnJoystickConnectedPriv(Index);

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnJoystickDisconnected(uint8 Index)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			TheGUIManager.OnJoystickDisconnectedPriv(Index);

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnMouseMove(const Vector3 &Position)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			TheGUIManager.OnMouseMovePriv();

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnCharacterEntered(wchar_t Character)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			TheGUIManager.OnCharacterEnteredPriv();

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnAction(const InputCenter::Action &TheAction)
		{
			RendererManager &TheManager = RendererManager::Instance;
			UIManager &TheGUIManager = *TheManager.ActiveRenderer()->UI.Get();

			//TODO

			if(TheGUIManager.GetInputBlocker().Get())
			{
				TheManager.Input.ConsumeInput();
			};
		};

		void OnGainFocus() {};
		void OnLoseFocus() {};
	};

	UIManager::UIManager(Renderer *TheOwner) : Owner(TheOwner), DrawOrderCounter(0), DrawOrderCacheDirty(false), DefaultFontColor(1, 1, 1, 1),
		DefaultSecondaryFontColor(1, 1, 1, 1), DefaultFontSize(16), MouseOverElement(NULL)
	{
		std::vector<LuaLib *> Libs;
		Libs.push_back(&FrameworkLib::Instance);

		if(GameInterface::Instance != NULL)
		{
			Libs.push_back(GameInterface::Instance);

			ScriptInstance = GameInterface::Instance->GetScriptInstance();
		};

		if(ScriptInstance.Get() == NULL)
			ScriptInstance = LuaScriptManager::Instance.CreateScript("", &Libs[0], Libs.size());

		Tooltip.Reset(new UITooltip(this));
		RegisterInput();
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

	void UIManager::ProcessTextProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UIText *TheText = (UIText *)Panel;

		if(Property == "ExpandHeight")
		{
			uint32 IntValue = 0;

			if(1 == sscanf(Value.c_str(), "%u", &IntValue))
			{
				TheText->ExpandHeight = !!IntValue;
			};
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
			TheText->SetText(Value, TheText->ExpandHeight);
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

	void UIManager::ProcessTextJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		UIText *TheText = (UIText *)Panel;
		Json::Value Value = Data.get("ExpandHeight", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Panel, "ExpandHeight", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		};

		Value = Data.get("FontSize", Json::Value((Json::Value::Int)GetDefaultFontSize()));

		if(Value.isInt())
		{
			ProcessTextProperty(Panel, "FontSize", StringUtils::MakeIntString((uint32)Value.asInt()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "FontSize", int);
		};

		Value = Data.get("Text", Json::Value(""));

		if(Value.isString())
		{
			ProcessTextProperty(Panel, "Text", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Text", string);
		};

		Value = Data.get("Bold", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Panel, "Bold", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Bold", string);
		};

		Value = Data.get("Italic", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Panel, "Italic", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Italic", string);
		};

		Value = Data.get("Underline", Json::Value(false));

		if(Value.isBool())
		{
			ProcessTextProperty(Panel, "Underline", StringUtils::MakeIntString((uint32)Value.asBool()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Underline", string);
		};

		Value = Data.get("Alignment", Json::Value(""));

		if(Value.isString())
		{
			ProcessTextProperty(Panel, "Alignment", Value.asString(), ElementName, LayoutName);
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
				ProcessTextProperty(Panel, "TextColor", Value.asString(), ElementName, LayoutName);
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
				ProcessTextProperty(Panel, "SecondaryTextColor", Value.asString(), ElementName, LayoutName);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, "SecondaryTextColor", string);
		};

		Value = Data.get("Border", Json::Value("0"));

		if(Value.type() == Json::stringValue)
		{
			ProcessTextProperty(Panel, "Border", Value.asString(), ElementName, LayoutName);
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
			ProcessTextProperty(Panel, "BorderColor", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "BorderColor", string);
		};
	};

	void UIManager::ProcessSpriteProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UISprite *TheSprite = (UISprite *)Panel;

		if(Property == "Path")
		{
			std::string FileName = Value;

			if(FileName.length() == 0)
			{
				TheSprite->TheSprite.Options.Scale(TheSprite->Size()).Color(Vector4());
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
						LayoutName.c_str());
				}
				else
				{
					TheSprite->TheSprite.SpriteTexture = SpriteTexture;
					TheSprite->TheSprite.Options.Scale(Panel->Size() != Vector2() ? Panel->Size() / SpriteTexture->Size() : Vector2(1, 1));
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
		else if(Property == "CropTiled")
		{
			if(TheSprite->TheSprite.SpriteTexture.Get() == NULL)
				return;

			std::string CropTiledString = Value;

			if(CropTiledString.length() && TheSprite->TheSprite.SpriteTexture.Get())
			{
				Vector2 FrameSize, FrameID;

				sscanf(CropTiledString.c_str(), "%f, %f, %f, %f", &FrameSize.x, &FrameSize.y, &FrameID.x, &FrameID.y);

				TheSprite->TheSprite.Options.Scale(FrameSize / TheSprite->TheSprite.SpriteTexture->Size()).Crop(CropMode::CropTiled,
					Rect(FrameSize.x, FrameID.x, FrameSize.y, FrameID.y));
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

				sscanf(NinePatchString.c_str(), "%f,%f,%f,%f", &NinePatchRect.Left, &NinePatchRect.Right, &NinePatchRect.Top,
					&NinePatchRect.Bottom);

				TheSprite->TheSprite.Options.NinePatch(true, NinePatchRect).Scale(Panel->Size());
				TheSprite->SelectBoxExtraSize = NinePatchRect.ToFullSize();
			};
		}
		else if(Property == "NinePatchScale")
		{
			if(TheSprite->TheSprite.SpriteTexture.Get() == NULL)
				return;

			f32 Scale = 0;

			if(1 == sscanf(Value.c_str(), "%f", &Scale))
			{
				TheSprite->SetExtraSizeScale(Scale);
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
		};
	};

	void UIManager::ProcessSpriteJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		Json::Value Value = Data.get("Path", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "Path", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Path", string);
		};

		Value = Data.get("Filtering", Json::Value("LINEAR"));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "Filtering", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Filtering", string);
		};

		Value = Data.get("CropTiled", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "CropTiled", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "CropTiled", string);
		};

		Value = Data.get("NinePatch", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "NinePatch", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "NinePatch", string)
		};

		Value = Data.get("NinePatchScale", Json::Value(1.0));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Panel, "NinePatchScale", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "NinePatchScale", double);
		};

		Value = Data.get("Colors", Json::Value("1,1,1,1"));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "Colors", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Colors", string)
		};

		Value = Data.get("Color", Json::Value(""));

		if(Value.isString())
		{
			ProcessSpriteProperty(Panel, "Color", Value.asString(), ElementName, LayoutName);
		}
		else
		{
			CHECKJSONVALUE(Value, "Color", string)
		};

		Value = Data.get("ScaleWide", Json::Value(""));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Panel, "ScaleWide", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		};

		Value = Data.get("ScaleTall", Json::Value(""));

		if(Value.isDouble())
		{
			ProcessSpriteProperty(Panel, "ScaleTall", StringUtils::MakeFloatString((f32)Value.asDouble()), ElementName, LayoutName);
		};
	};

	void UIManager::ProcessGroupProperty(UIPanel *Panel, const std::string &Property, const std::string &Value, const std::string &ElementName, const std::string &LayoutName)
	{
		UIGroup *Group = (UIGroup *)Panel;

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
					Group->LayoutMode = UIGroupLayoutMode::Center;
				}
				else if(Modes[j] == "VCENTER")
				{
					Group->LayoutMode = UIGroupLayoutMode::VerticalCenter;
				}
				else if(Modes[j] == "ADJUSTCLOSER")
				{
					Group->LayoutMode |= UIGroupLayoutMode::AdjustCloser;
				}
				else if(Modes[j] == "INVERTDIRECTION")
				{
					Group->LayoutMode |= UIGroupLayoutMode::InvertDirection;
				}
				else if(Modes[j] == "INVERTX")
				{
					Group->LayoutMode |= UIGroupLayoutMode::InvertX;
				}
				else if(Modes[j] == "INVERTY")
				{
					Group->LayoutMode |= UIGroupLayoutMode::InvertY;
				};
			};
		};
	};

	void UIManager::ProcessGroupJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		Json::Value Value = Data.get("LayoutMode", Json::Value("None"));

		if(Value.isString())
		{
			ProcessGroupProperty(Panel, "LayoutMode", Value.asString(), ElementName, LayoutName);
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

	void UIManager::ProcessCustomProperty(UIPanel *Panel, const std::string &Property, const Json::Value &Value, const std::string &ElementName,
		const std::string &LayoutName)
	{
		UIPanel::PropertyMap::iterator it = Panel->Properties.find(Property);

		if(it == Panel->Properties.end())
			return;

		if(it->second.SetFunction)
		{
			Panel->PropertySetFunctionCode << "Self.Properties[\"" << Property << "\"].Set(Self, \"" << Property << "\", ";

			try
			{
				switch(Value.type())
				{
				case Json::arrayValue:
				case Json::objectValue:
				case Json::nullValue:
					Panel->PropertySetFunctionCode << JsonToLuaString(Value);

					break;

				case Json::booleanValue:
					Panel->PropertySetFunctionCode << Value.asBool();

					break;

				case Json::intValue:
					Panel->PropertySetFunctionCode << Value.asInt();

					break;

				case Json::realValue:
					Panel->PropertySetFunctionCode << Value.asDouble();

					break;

				case Json::stringValue:
					Panel->PropertySetFunctionCode << "\"" << Value.asString() << "\"";

					break;

				case Json::uintValue:
					Panel->PropertySetFunctionCode << Value.asUInt();

					break;
				};
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Error while setting property '%s' of Panel '%s': '%s'", Property.c_str(), Panel->Name.c_str(),
					ElementName.c_str(), e.what());

				Panel->PropertySetFunctionCode << "nil";
			};

			Panel->PropertySetFunctionCode << ")\n";
		};
	};

	void UIManager::ProcessCustomPropertyJSON(UIPanel *Panel, const Json::Value &Data, const std::string &ElementName, const std::string &LayoutName)
	{
		for(uint32 i = 0; i < Data.size(); i++)
		{
			ProcessCustomProperty(Panel, Data.getMemberNames()[i], Data.type() == Json::objectValue ? Data[Data.getMemberNames()[i]] : Data[i],
				ElementName, LayoutName);
		};
	};

	void UIManager::CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, const Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName, UIPanel *ParentLimit)
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

			SuperSmartPointer<UIPanel> Panel;

			Control = StringUtils::ToUpperCase(Control);

			if(Control == "SPRITE")
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
			else
			{
				Panel.Reset(new UIGroup(Renderer->UI));
			};

			Panel->LayoutValue = TheLayout;
			Panel->LayoutName = ElementIDName;

			std::stringstream BaseFunctionName;

			BaseFunctionName << Control << "_" << StringUtils::MakeIntString((uint32)ElementID, true) << "_";

			Json::Value EnabledValue = Data.get("Enabled", Json::Value(true)),
				KeyboardInputEnabledValue = Data.get("KeyboardInput", Json::Value(true)),
				JoystickInputEnabledValue = Data.get("JoystickInput", Json::Value(true)),
				MouseInputEnabledValue = Data.get("MouseInput", Json::Value(true)),
				AlphaValue = Data.get("Opacity", Json::Value(1.0)),
				VisibleValue = Data.get("Visible", Json::Value(true)),
				BlockingInputValue = Data.get("BlockingInput", Json::Value(false)),
				InputBlockBackgroundValue = Data.get("InputBlockBackground", Json::Value(true)),
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

			if(JoystickInputEnabledValue.isBool())
			{
				Panel->SetJoystickInputEnabled(JoystickInputEnabledValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(JoystickInputEnabledValue, "JoystickInput", bool);
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

			if(InputBlockBackgroundValue.isBool())
			{
				Panel->SetInputBlockerBackground(InputBlockBackgroundValue.asBool());
			}
			else
			{
				CHECKJSONVALUE(InputBlockBackgroundValue, "InputBlockBackground", bool);
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
						Panel->name##Function.Add(luabind::globals(ScriptInstance->State)[ComposedFunctionName.str().c_str()]);\
						Panel->GlobalsTracker.Add(ComposedFunctionName.str());\
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
			REGISTER_LUA_EVENT(OnJoystickButtonJustPressed, ", Button");
			REGISTER_LUA_EVENT(OnJoystickButtonPressed, ", Button");
			REGISTER_LUA_EVENT(OnJoystickButtonReleased, ", Button");
			REGISTER_LUA_EVENT(OnJoystickAxisMoved, ", Axis");
			REGISTER_LUA_EVENT(OnJoystickConnected, ", Index");
			REGISTER_LUA_EVENT(OnJoystickDisconnected, ", Index");
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

			std::stringstream PositionScriptX, PositionScriptY, OffsetScriptX, OffsetScriptY, SizeScript;

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

			Value = Data.get("XOffset", Json::Value("0"));

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

				OffsetScriptX << "local XOffset = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "XOffset", string);
			};

			Value = Data.get("YOffset", Json::Value("0"));

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

				OffsetScriptY << "local YOffset = " << Temp << "\n";
			}
			else
			{
				CHECKJSONVALUE(Value, "YOffset", string);
			};

			Value = Data.get("ExtraSizeScale", Json::Value(1.0));

			if(Value.isDouble() || Value.isInt() || Value.isUInt())
			{
				f32 Scale = 0;

				if(Value.isDouble())
				{
					Scale = (f32)Value.asDouble();
				}
				else if(Value.isInt())
				{
					Scale = (f32)Value.asInt();
				}
				else if(Value.isUInt())
				{
					Scale = (f32)Value.asUInt();
				};

				Panel->SetExtraSizeScale(Scale);
			}
			else
			{
				CHECKJSONVALUE(Value, "ExtraSizeScale", number);
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
				"	parent_wide = Parent.Size.x\n"
				"	parent_tall = Parent.Size.y\n"
				"	parent_extra_size_x = Parent.ScaledExtraSize.x\n"
				"	parent_extra_size_y = Parent.ScaledExtraSize.y\n"
				"else\n"
				"	parent_wide = ScreenWidth\n"
				"	parent_tall = ScreenHeight\n"
				"	parent_extra_size_x = 0\n"
				"	parent_extra_size_y = 0\n"
				"end\n\n"
				"left = Self.ExtraSize.x / 2\n"
				"right = (parent_wide + parent_extra_size_x) - (Self.Size.x + Self.ScaledExtraSize.x)\n"
				"top = Self.ExtraSize.y / 2\n"
				"bottom = (parent_tall + parent_extra_size_y) - (Self.Size.y + Self.ScaledExtraSize.y)\n"
				"center = ((parent_wide + parent_extra_size_x) - (Self.Size.x + Self.ScaledExtraSize.x)) / 2\n\n" <<
				PositionScriptX.str() << 
				OffsetScriptX.str() << 
				"center = ((parent_tall + parent_extra_size_y) - (Self.Size.y + Self.ScaledExtraSize.y)) / 2\n" <<
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

			luaL_dostring(ScriptInstance->State, ComposedPositionFunction.str().c_str());

			luaL_dostring(ScriptInstance->State, ComposedSizeFunction.str().c_str());

			Panel->GlobalsTracker.Add(BaseFunctionName.str() + "Position");
			Panel->GlobalsTracker.Add(BaseFunctionName.str() + "Size");

			Panel->PositionFunction.Add(luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Position"]);
			Panel->SizeFunction.Add(luabind::globals(ScriptInstance->State)[BaseFunctionName.str() + "Size"]);

			if(!Panel->PositionFunction.Members.front())
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Position", ElementName.c_str());

				continue;
			}
			else if(!Panel->SizeFunction.Members.front())
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s' due to invalid Size", ElementName.c_str());

				continue;
			};

			try
			{
				Panel->SizeFunction.Members.front()(Panel, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Size", ElementName.c_str(), e.what());

				continue;
			};

			try
			{
				Panel->PositionFunction.Members.front()(Panel, Parent, GetOwner()->Size().x, GetOwner()->Size().y);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogWarn(TAG, "Unable to create an element '%s': Error '%s' on Position", ElementName.c_str(), e.what());

				continue;
			};

			Value = Data.get("Properties", Json::Value());

			if(Value.type() == Json::objectValue)
			{
				static std::stringstream PropertiesStream;
				PropertiesStream.str("");

				std::string PropertiesStreamFunctionName = "PropertyStartupDefault_" + StringUtils::PointerString(Panel.Get());

				Panel->GlobalsTracker.Add(PropertiesStreamFunctionName);

				PropertiesStream << "function " << PropertiesStreamFunctionName << "(Self)\n";

				for(uint32 j = 0; j < Value.size(); j++)
				{
					std::string PropertyName = Value.getMemberNames()[j];

					if(Panel->Properties.find(PropertyName) != Panel->Properties.end())
					{
						Log::Instance.LogWarn(TAG, "Unable to add function '%s' to Panel '%s': Function already exists!", PropertyName.c_str(), ElementName.c_str());

						continue;
					};

					if(Value[PropertyName].type() != Json::objectValue)
					{
						CHECKJSONVALUE(Value[j], ("Property '" + PropertyName + "'").c_str(), object);

						continue;
					};

					UIPanel::PropertyInfo PInfo;

					luabind::object LuaPropertyInfo = luabind::newtable(ScriptInstance->State);

					if(Value[PropertyName]["Get"].type() == Json::stringValue)
					{
						static std::stringstream str;
						str.str("");

						std::string FunctionName = "PropertyGet_" + StringUtils::PointerString(Panel.Get()) + "_" +
							StringUtils::MakeIntString(MakeStringID(PropertyName), true);

						Panel->GlobalsTracker.Add(FunctionName);

						str << "function " << FunctionName << "(Self, PropertyName)\n"
							"	local Value = Self.Properties[PropertyName].Value\n	";

						str << Value[PropertyName]["Get"].asString();

						str << "\nend\n";

						luaL_dostring(ScriptInstance->State, str.str().c_str());

						PInfo.GetFunction = luabind::globals(ScriptInstance->State)[FunctionName];

						LuaPropertyInfo["Get"] = PInfo.GetFunction;
					};

					if(Value[PropertyName]["Set"].type() == Json::stringValue)
					{
						static std::stringstream str;
						str.str("");

						std::string FunctionName = "PropertySet_" + StringUtils::PointerString(Panel.Get()) + "_" +
							StringUtils::MakeIntString(MakeStringID(PropertyName), true);

						Panel->GlobalsTracker.Add(FunctionName);

						str << "function " << FunctionName << "(Self, PropertyName, Value)\n";

						str << Value[PropertyName]["Set"].asString();

						str << "\nend\n";

						luaL_dostring(ScriptInstance->State, str.str().c_str());

						PInfo.SetFunction = luabind::globals(ScriptInstance->State)[FunctionName];

						LuaPropertyInfo["Set"] = PInfo.SetFunction;
					};

					//We need to account for the JSON Data Types... Strings like "false" become bool, so we must convert to lua strings anyway.
					std::string DefaultString;

					switch(Value[PropertyName]["Default"].type())
					{
					case Json::stringValue:
						DefaultString = Value[PropertyName]["Default"].asString();

						break;

					case Json::nullValue:

						break;

					default:
						DefaultString = JsonToLuaString(Value[PropertyName]["Default"]);

						break;
					};

					if(DefaultString.length())
					{
						static std::stringstream str;
						str.str("");

						std::string FunctionName = "PropertyDefault_" + StringUtils::PointerString(Panel.Get()) + "_" +
							StringUtils::MakeIntString(MakeStringID(PropertyName), true);

						Panel->GlobalsTracker.Add(FunctionName);

						str << "function " << FunctionName << "(Self, PropertyName)\n"
							"	local Value = " << Value[PropertyName]["Default"].asString() << "\n"
							"	if type(Value) == type(nil) then\n"
							"		g_Log:Warn(\"While setting a Panel '\" .. Self.Name .. \"'s Value for Property '\" .. PropertyName .. \"': Value is nil!\")\n"
							"	end\n"
							"\n"
							"	if Self.Properties[PropertyName].Set ~= nil then\n"
							"		Self.Properties[PropertyName].Set(Self, PropertyName, Value)\n"
							"	else\n"
							"		Self.Properties[PropertyName].Value = Value\n"
							"	end\n"
							"end\n";

						luaL_dostring(ScriptInstance->State, str.str().c_str());

						PInfo.DefaultFunction = luabind::globals(ScriptInstance->State)[FunctionName];

						if(PInfo.DefaultFunction)
							PropertiesStream << FunctionName << "(Self, \"" << PropertyName << "\")\n";

						LuaPropertyInfo["Default"] = PInfo.DefaultFunction;
					};

					if(!PInfo.GetFunction && !PInfo.SetFunction)
						continue;

					Panel->Properties[PropertyName] = PInfo;
					Panel->PropertyValues[PropertyName] = LuaPropertyInfo;
				};
				
				PropertiesStream << "\nend\n";

				luaL_dostring(ScriptInstance->State, PropertiesStream.str().c_str());

				Panel->PropertiesStartupDefaultFunction.Add(luabind::globals(ScriptInstance->State)[PropertiesStreamFunctionName]);
			}
			else if(Value.type() != Json::nullValue)
			{
				CHECKJSONVALUE(Value, "Properties", object);
			};

			std::string PropertyStartupSetFunctionName = "PropertyStartupSet_" + StringUtils::PointerString(Panel.Get());

			Panel->GlobalsTracker.Add(PropertyStartupSetFunctionName);

			Panel->PropertySetFunctionCode.str("");
			Panel->PropertySetFunctionCode << "function " << PropertyStartupSetFunctionName << "(Self)\n";

			ProcessCustomPropertyJSON(Panel, Data, ElementName, TheLayout->Name);

			if(Control == "SPRITE")
			{
				ProcessSpriteJSON(Panel, Data, ElementName, TheLayout->Name);
			}
			else if(Control == "GROUP")
			{
				ProcessGroupJSON(Panel, Data, ElementName, TheLayout->Name);
			}
			else if(Control == "TEXT")
			{
				ProcessTextJSON(Panel, Data, ElementName, TheLayout->Name);
			}
			else
			{
				SuperSmartPointer<UILayout> TargetLayout;

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

					return;
				};

				//We shouldn't override previous set sizes...
				bool ResetPanelSize = Panel->Size() == Vector2();

				if(ResetPanelSize)
				{
					Panel->SetSize(Parent ? Parent->ComposedSize() : Panel->ComposedSize());
				};

				SuperSmartPointer<UILayout> NewLayout = TargetLayout->Clone(Panel, ParentElementName + "." + ElementName, true);

				//Probably not needed anymore?
				if(!ResetPanelSize)
				{
					//Panel->SetSize(Panel->GetChildrenSize());
				};

				StringID LayoutID = MakeStringID(ParentElementName + "." + ElementName + "." + NewLayout->Name);

				LayoutMap::iterator it = Layouts.find(LayoutID);

				//Should be impossible for this to happen, but still...
				if(it != Layouts.end())
				{
					Log::Instance.LogWarn(TAG, "Found duplicate layout '%s', erasing old.", (Panel->Layout()->Name + "_" + NewLayout->Name).c_str());

					Layouts.erase(it);
				};

				Layouts[LayoutID] = NewLayout;

				for(uint32 j = 0; j < Panel->ChildrenCount(); j++)
				{
					ProcessCustomPropertyJSON(Panel->Child(j), Data, Panel->Child(j)->Name, NewLayout->Name);
				};

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

							UIPanel *TargetElement = it->second;

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
								std::string ControlName = StringUtils::ToUpperCase(TargetElement->NativeType);
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
									ProcessCustomProperty(TargetElement, Property, Value, ElementName, NewLayout->Name);

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

								ProcessCustomProperty(TargetElement, Property, Value, ElementName, NewLayout->Name);

								break;
							};
						};
					};
				};
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
					TheLayout->Elements[TooltipGroup->ID()] = TooltipGroup;

					CopyElementsToLayout(TheLayout, Value, TooltipGroup, ParentElementName + "." + ElementName + "_TOOLTIPELEMENT", Panel);

					TooltipGroup->SetSize(TooltipGroup->ChildrenSize());

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

			Value = Data.get("Rotation", Json::Value(0.0));

			if(Value.isDouble() || Value.isInt() || Value.isUInt())
			{
				f32 Angle = 0;

				if(Value.isDouble())
				{
					Angle = (f32)Value.asDouble();
				}
				else if(Value.isInt())
				{
					Angle = (f32)Value.asInt();
				}
				else if(Value.isUInt())
				{
					Angle = (f32)Value.asUInt();
				};

				for(uint32 j = 0; j < Panel->Children.size(); j++)
				{
					Panel->Children[j]->SetRotation(Panel->Children[j]->Rotation() - Panel->Rotation());
				};

				Panel->SetRotation(Panel->Rotation() + MathUtils::DegToRad(Angle));
			}
			else
			{
				CHECKJSONVALUE(Value, "Rotation", number);
			};

			Value = Data.get("ContentPanel", Json::Value(false));

			if(Value.isBool())
			{
				if(Value.asBool() && Panel->Parent() != NULL)
				{
					Panel->Parent()->SetContentPanel(Panel);
				};
			}
			else
			{
				CHECKJSONVALUE(Value, "ContentPanel", bool);
			};

			Panel->AdjustSizeAndPosition(ParentLimit);

			Json::Value Children = Data.get("Children", Json::Value());

			if(!Children.isArray())
				continue;

			CopyElementsToLayout(TheLayout, Children, Panel->ContentPanel() ? Panel->ContentPanel() : Panel, ParentElementName + "." + ElementName, ParentLimit);
		};
	};

	bool UIManager::LoadLayouts(Stream *In, SuperSmartPointer<UIPanel> Parent, bool DefaultLayout)
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
			SuperSmartPointer<UILayout> Layout(new UILayout());
			Layout->Name = LayoutName;
			Layout->Owner = this;
			Layout->ContainedObjects = Elements;
			Layout->Parent = Parent;

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

		MouseOverElement = NULL;
	};

	SuperSmartPointer<UIPanel> UIManager::GetInputBlocker()
	{
		SuperSmartPointer<UIPanel> Out;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			while(it != Elements.end() && it->second.Get() == NULL)
			{
				Elements.erase(it);
				it = Elements.begin();
			};

			if(it == Elements.end())
				break;

			if(it->second->Panel->BlockingInput() && it->second->Panel->Visible())
			{
				Out = it->second->Panel;

				break;
			};
		};

		return Out;
	};

	SuperSmartPointer<UIPanel> UIManager::GetMouseOverElement()
	{
		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->Parent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		UIPanel *FoundElement = NULL;

		SuperSmartPointer<UIPanel> InputBlocker = GetInputBlocker();

		if(InputBlocker.Get())
		{
			UIPanel *p = InputBlocker;
			RecursiveFindFocusedElement(p->ParentPosition(), p, FoundElement);
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

		if(FoundElement && Elements[FoundElement->ID()].Get())
		{
			if(MouseOverElement != FoundElement)
				FoundElement->OnMouseEntered(FoundElement);

			MouseOverElement = FoundElement;

			return Elements[FoundElement->ID()]->Panel;
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
				if(it->second->Panel->Parent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		for(uint32 i = 0; i < DrawOrderCache.size(); i++)
		{
			if(DrawOrderCache[i]->Panel.Get() == NULL)
				continue;

			if(DrawOrderCache[i]->Panel->Visible())
			{
				DrawOrderCache[i]->Panel->Update(Vector2());
			};
		};
	};

	void UIManager::Draw(Renderer *Renderer)
	{
		DrawUIRects = !!(Console::Instance.GetVariable("r_drawuirects") ? Console::Instance.GetVariable("r_drawuirects")->UintValue : 0);
		DrawUIFocusZones = !!(Console::Instance.GetVariable("r_drawuifocuszones") ? Console::Instance.GetVariable("r_drawuifocuszones")->UintValue : 0);

		if(DrawOrderCacheDirty)
		{
			DrawOrderCacheDirty = false;
			DrawOrderCache.clear();

			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel->Parent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		SuperSmartPointer<UIPanel> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Panel->BlockingInput() && it->second->Panel->Visible())
			{
				InputBlocker = it->second->Panel;

				break;
			};
		};

		for(uint32 i = 0; i <= DrawOrderCounter; i++)
		{
			for(uint32 j = 0; j < DrawOrderCache.size(); j++)
			{
				if(DrawOrderCache[j]->Panel.Get() == NULL || DrawOrderCache[j]->DrawOrder != i || !DrawOrderCache[j]->Panel->Visible())
					continue;

				if(DrawOrderCache[j]->Panel == InputBlocker && DrawOrderCache[j]->Panel->InputBlockerBackground())
				{
					Sprite BackgroundSprite;
					BackgroundSprite.Options.Scale(Renderer->Size()).Color(Vector4(0, 0, 0, 0.3f));
					BackgroundSprite.Draw(Renderer);
				};

				DrawOrderCache[j]->Panel->Draw(Vector2(), Renderer);
			};
		};

		if(FocusedElementValue.Get() && FocusedElementValue->DraggingValue)
		{
			FocusedElementValue->Draw(RendererManager::Instance.Input.MousePosition, Renderer);
		};

		Tooltip->Update(RendererManager::Instance.Input.MousePosition);
		Tooltip->Draw(RendererManager::Instance.Input.MousePosition, Renderer);
	};

	void UIManager::RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement)
	{
		if(!p->Visible() || !p->Enabled() || !p->MouseInputEnabled())
			return;

		static AxisAlignedBoundingBox AABB;

		static RotateableRect Rectangle;

		Vector2 PanelSize = p->ComposedSize();

		Vector2 ActualPosition = ParentPosition + p->Position() + p->Offset();

		AABB.min = ActualPosition;
		AABB.max = AABB.min + PanelSize;

		Sprite TheSprite;
		TheSprite.Options.Position(AABB.min.ToVector2()).Scale(PanelSize);
		//TheSprite.Draw(Owner);

		Rectangle.Left = AABB.min.x;
		Rectangle.Right = AABB.max.x;
		Rectangle.Top = AABB.min.y;
		Rectangle.Bottom = AABB.max.y;
		Rectangle.Rotation = p->ParentRotation();

		if(Rectangle.Rotation != 0)
		{
			PanelSize /= 2;

			Rectangle.Left += PanelSize.x;
			Rectangle.Right += PanelSize.x;
			Rectangle.Top += PanelSize.y;
			Rectangle.Bottom += PanelSize.y;

			PanelSize *= 2;
		};

		if(Rectangle.IsInside(RendererManager::Instance.Input.MousePosition))
		{
			FoundElement = p;

			Vector2 ParentSizeHalf = PanelSize / 2;

			for(uint32 i = 0; i < p->Children.size(); i++)
			{
				Vector2 ChildrenSizeHalf = (p->Children[i]->ComposedSize()) / 2;
				Vector2 ChildrenPosition = p->Children[i]->Position() - p->Children[i]->Translation() + p->Children[i]->Offset();

				RecursiveFindFocusedElement(ActualPosition + Vector2::Rotate(ChildrenPosition - ParentSizeHalf + ChildrenSizeHalf, p->ParentRotation()) + ParentSizeHalf -
					ChildrenSizeHalf - ChildrenPosition, p->Children[i], FoundElement);
			};
		}
		else
		{
			//Log::Instance.LogDebug(TAG, "Failed Focus Test on '%s'", p->Name.c_str());
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
				if(it->second->Panel->Parent() == NULL)
				{
					DrawOrderCache.push_back(it->second);
				};
			};
		};

		SuperSmartPointer<UIPanel> PreviouslyFocusedElement = FocusedElementValue;
		FocusedElementValue = SuperSmartPointer<UIPanel>();
		UIPanel *FoundElement = NULL;

		SuperSmartPointer<UIPanel> InputBlocker = GetInputBlocker();

		if(InputBlocker.Get())
		{
			UIPanel *p = InputBlocker;
			RecursiveFindFocusedElement(p->ParentPosition(), p, FoundElement);
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
			FocusedElementValue = Elements[FoundElement->ID()]->Panel;
		};

		if(PreviouslyFocusedElement && PreviouslyFocusedElement.Get() != FocusedElementValue.Get())
		{
			PreviouslyFocusedElement->OnLoseFocusPriv();
		};

		if(FocusedElementValue)
		{
			FocusedElementValue->OnGainFocusPriv();
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

	void UIManager::OnJoystickButtonPressedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickButtonPressedPriv(o);
		};
	};

	void UIManager::OnJoystickButtonJustPressedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickButtonJustPressedPriv(o);
		};
	};

	void UIManager::OnJoystickButtonReleasedPriv(const InputCenter::JoystickButtonInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickButtonReleasedPriv(o);
		};
	};

	void UIManager::OnJoystickAxisMovedPriv(const InputCenter::JoystickAxisInfo &o)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickAxisMovedPriv(o);
		};
	};

	void UIManager::OnJoystickConnectedPriv(uint8 Index)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickConnectedPriv(Index);
		};
	};

	void UIManager::OnJoystickDisconnectedPriv(uint8 Index)
	{
		if(RendererManager::Instance.Input.InputConsumed())
			return;

		if(FocusedElementValue)
		{
			FocusedElementValue->OnJoystickDisconnectedPriv(Index);
		};
	};

	void UIManager::RegisterInput()
	{
		SuperSmartPointer<UIInputProcessor> Out(new UIInputProcessor());
		Out->Name = "GUIPROCESSOR_" + StringUtils::PointerString(this);

		RendererManager::Instance.Input.AddContext(Out);
		RendererManager::Instance.Input.EnableContext(MakeStringID(Out->Name));
	};

	void UIManager::UnRegisterInput()
	{
		RendererManager::Instance.Input.DisableContext(MakeStringID("GUIPROCESSOR_" + StringUtils::PointerString(this)));
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

		FLASSERT(Element->ManagerValue == this, "Found Element from another UI Manager!");

		if(Element->ManagerValue != this)
			return false;

		Elements[ID].Reset(new ElementInfo());
		Elements[ID]->Panel = Element;
		Elements[ID]->DrawOrder = ++DrawOrderCounter;
		Element->IDValue = ID;
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

		DefaultFont = ResourceManager::Instance.GetFontFromPackage(GetOwner(), FontDirectory, FontName);

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second.Get() == NULL)
				continue;

			it->second->Panel->SetSkin(Skin);
		};
	};

	UIManager::~UIManager()
	{
		UnRegisterInput();
		Clear();

		Tooltip.Dispose();
		ScriptInstance.Dispose();
	};

	Renderer *UIManager::GetOwner()
	{
		return Owner;
	};

	const Vector4 &UIManager::GetDefaultFontColor()
	{
		return DefaultFontColor;
	};

	const Vector4 &UIManager::GetDefaultSecondaryFontColor()
	{
		return DefaultSecondaryFontColor;
	};

	uint32 UIManager::GetDefaultFontSize()
	{
		return DefaultFontSize;
	};

	FontHandle UIManager::GetDefaultFont()
	{
		return DefaultFont;
	};

	SuperSmartPointer<GenericConfig> UIManager::GetSkin()
	{
		return Skin;
	};
#endif
};

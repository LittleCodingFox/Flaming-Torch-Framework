#include "FlamingCore.hpp"
#define TAG "Framework"

#define REGISTER_KEY(key)\
	luabind::value("Key_" #key, InputKey::key)

#define REGISTER_MOUSEBUTTON(button)\
	luabind::value("Mouse_" #button, InputMouseButton::button)

#define REGISTER_JOYSTICKAXIS(axis)\
	luabind::value("JoyAxis_" #axis, InputJoystickAxis::axis)

#define REGISTER_JOYSTICKBUTTON(button)\
	luabind::value("JoyButton_" #button, button)

namespace luabind
{
	//StringID
#define REGISTER_POD_CONVERTER(x)\
	template <>\
	struct default_converter<x>\
		: native_converter_base<x>\
	{\
		static int compute_score(lua_State* L, int index)\
		{\
			return lua_type(L, index) == LUA_TNUMBER ? 0 : -1;\
		}\
		\
		x from(lua_State* L, int index)\
		{\
			return x(lua_tonumber(L, index));\
		}\
		\
		void to(lua_State* L, x const& t)\
		{\
			lua_pushnumber(L, (lua_Number)t);\
		}\
	};\
	\
	template <>\
	struct default_converter<x const&>\
		: default_converter<x>\
	{};

	REGISTER_POD_CONVERTER(wchar_t);
	REGISTER_POD_CONVERTER(FlamingTorch::int64);
	REGISTER_POD_CONVERTER(FlamingTorch::uint64);
};

namespace FlamingTorch
{
	FrameworkLib FrameworkLib::Instance;

	int32 BitwiseSet(int32 a, int32 b)
	{
		return a | b;
	};

	bool BitwiseCheck(int32 a, int32 b)
	{
		return (a & b) != 0;
	};

	int32 BitwiseRemove(int32 a, int32 b)
	{
		return a | ~b;
	};

	luabind::object GetGenericConfigSections(GenericConfig &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(GenericConfig::SectionMap::iterator it = Self.Sections.begin(); it != Self.Sections.end(); it++)
		{
			luabind::object Inner = luabind::newtable(State);

			for(GenericConfig::Section::ValueMap::iterator vit = it->second.Values.begin(); vit != it->second.Values.end(); vit++)
			{
				Inner[vit->first] = vit->second.Content;
			};

			Out[it->first] = Inner;
		};

		return Out;
	};

	luabind::object LuaDoStream(Stream *In, lua_State *State)
	{
		if(In == NULL)
			return luabind::object();

		std::string Content = In->AsString();

		static std::stringstream str;
		str.str("");

		std::string FunctionName = "LoadStreamContent_" + StringUtils::PointerString(In);

		str << "function " << FunctionName << "()\n" << Content << "\nend\n";

		if(0 != luaL_dostring(State, str.str().c_str()))
		{
			return luabind::object();
		};

		luabind::object Globals = luabind::globals(State);

		luabind::object FunctionInstance = Globals[FunctionName];

		if(!FunctionInstance)
			return luabind::object();

		try
		{
			return FunctionInstance();
		}
		catch(std::exception &)
		{
		};

		return luabind::object();
	};
	
	template<class type>
	bool LuabindSimpleCompare(type &Self, type *Other)
	{
		return &Self == Other;
	};

	luabind::object StringUtilsSplit(const std::string &Self, const std::string &Separator, lua_State *State)
	{
		if(Separator.length() == 0)
			return luabind::object();

		luabind::object Out = luabind::newtable(State);

		std::vector<std::string> Fragments = StringUtils::Split(Self, Separator[0]);

		for(uint32 i = 0; i < Fragments.size(); i++)
		{
			Out[i + 1] = Fragments[i];
		};

		return Out;
	};

	luabind::object GetComponentProperties(Component &Self)
	{
		return Self.Properties;
	};

	luabind::object GetEntityProperties(Entity &Self)
	{
		return Self.Properties;
	};

	luabind::object FileSystemUtilsGetDirectories(const std::string &Directory, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		static std::vector<std::string> Directories;

		Directories = FileSystemUtils::GetAllDirectories(Directory);

		for(uint32 i = 0; i < Directories.size(); i++)
		{
			Out[i + 1] = Directories[i];
		};

		return Out;
	};

	luabind::object FileSystemUtilsScan(const std::string &Directory, const std::string &Extension, bool Recursive, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		static std::vector<std::string> Files;

		Files = FileSystemUtils::ScanDirectory(Directory, Extension, Recursive);

		for(uint32 i = 0; i < Files.size(); i++)
		{
			Out[i + 1] = Files[i];
		};

		return Out;
	};

	SuperSmartPointer<WorldStreamerCallback> GetWorldStreamerCallbackFromGame()
	{
		return SuperSmartPointer<WorldStreamerCallback>(new ScriptedWorldStreamerCallback(GameInterface::Instance.AsDerived<ScriptedGameInterface>()->ScriptInstance));
	};

	luabind::object PackageFileSystemFindDirectories(const std::string &Directory, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		static std::vector<std::string> Directories;

		Directories = PackageFileSystemManager::Instance.FindDirectories(Directory);

		for(uint32 i = 0; i < Directories.size(); i++)
		{
			Out[i + 1] = Directories[i];
		};

		return Out;
	};

	luabind::object PackageFileSystemFindFiles(const std::string &Prefix, const std::string &Suffix,
		const std::string &Extension, const std::string &StartingDirectory, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		static std::vector<std::pair<std::string, std::string> > Files;

		Files = PackageFileSystemManager::Instance.FindFiles(Prefix, Suffix, Extension, StartingDirectory);

		for(uint32 i = 0; i < Files.size(); i++)
		{
			luabind::object Inner = luabind::newtable(State);
			Inner[1] = Files[i].first;
			Inner[2] = Files[i].second;

			Out[i + 1] = Inner;
		};

		return Out;
	};

	luabind::object GetTextureBufferData(TextureBuffer &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.Data.size(); i++)
		{
			Out[i + 1] = Self.Data[i];
		};

		return Out;
	};

	void SetTextureBufferData(TextureBuffer &Self, luabind::argument arg)
	{
		static std::vector<uint8> Bytes;
		Bytes.resize(0);

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			Bytes.push_back(ProtectedLuaCast<uint8>(*it));
		};

		if(Bytes.size() != Self.Data.size())
		{
			Log::Instance.LogWarn(TAG, "Unable to set a TextureBuffer Data: Invalid Size %d: Should be %d", Bytes.size(), Self.Data.size());

			return;
		};

		Self.Data = Bytes;
	};

	WorldStreamerCallback *GetWorldStreamerCallback(WorldStreamer &Self)
	{
		return Self.Callback;
	};

	void SetWorldStreamerCallback(WorldStreamer &Self, WorldStreamerCallback *Target)
	{
		Self.Callback = Target;
	};

	lua_Number GetMathUtilsPi()
	{
		return (lua_Number)MathUtils::Pi;
	};

	lua_Number GetMathUtilsEpsilon()
	{
		return (lua_Number)MathUtils::Epsilon;
	};

	void LogInfo(Log &Self, const std::string &Message)
	{
		Log::Instance.LogInfo(TAG, "[Script] %s", Message.c_str());
	};

	void LogErr(Log &Self, const std::string &Message)
	{
		Log::Instance.LogErr(TAG, "[Script] %s", Message.c_str());
	};

	void LogWarn(Log &Self, const std::string &Message)
	{
		Log::Instance.LogWarn(TAG, "[Script] %s", Message.c_str());
	};

	void LogDebug(Log &Self, const std::string &Message)
	{
		Log::Instance.LogDebug(TAG, "[Script] %s", Message.c_str());
	};

	luabind::object GetMatrix4x4M(Matrix4x4 &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < 4; i++)
		{
			luabind::object Line = luabind::newtable(State);

			for(uint32 j = 0; j < 4; j++)
			{
				Line[j + 1] = Self.m[i][j];
			};

			Out[i + 1] = Line;
		};

		return Out;
	};

	void SetMatrix4x4M(Matrix4x4 &Self, const luabind::argument &arg)
	{
		uint32 i = 0;

		for(luabind::iterator Column(arg), end; Column != end; Column++, i++)
		{
			uint32 j = 0;

			for(luabind::iterator Line(*Column); Line != end; Line++, j++)
			{
				Self.m[i][j] = ProtectedLuaCast<float>(*Line);
			};
		};
	};

	std::string GetConsoleVariableString(ConsoleVariable &Self)
	{
		return Self.StringValue;
	};
	
	void SetConsoleVariableString(ConsoleVariable &Self, luabind::argument arg)
	{
		try
		{
			Self.StringValue = luabind::object_cast<const char *>(arg);
		}
		catch(std::exception &)
		{
		};
	};

	void ConsoleLogConsole(Console &Self, const std::string &Message)
	{
		Self.LogConsole(Message);
	};

	void ConsoleRunConsoleCommand(Console &Self, const std::string &Command)
	{
		Self.RunConsoleCommand(Command);
	};

	luabind::object GetTiledMapInitPackageDirectoriesValue(TiledMapInitOptions &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.PackageDirectoriesValue.size(); i++)
		{
			Out[i + 1] = Self.PackageDirectoriesValue[i];
		};

		return Out;
	};

	void SetTiledMapInitPackageDirectoriesValue(TiledMapInitOptions &Self, luabind::argument arg)
	{
		Self.PackageDirectoriesValue.clear();

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			Self.PackageDirectoriesValue.push_back(ProtectedLuaCast<const char *>(*it));
		};
	};

	luabind::object GetTileLayerTiles(TiledMap::Layer &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.Tiles.size(); i++)
		{
			Out[i + 1] = Self.Tiles[i];
		};

		return Out;
	};

	void SetTileLayerTiles(TiledMap::Layer &Self, luabind::argument arg)
	{
		Self.Tiles.clear();

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			Self.Tiles.push_back(ProtectedLuaCast<TiledMap::Layer::TileInfo>(*it));
		};
	};

	luabind::object GetTilesetInfoProperties(TiledMap::UniqueTilesetInfo &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(TiledMap::UniqueTilesetInfo::PropertyMap::iterator it = Self.Properties.begin(); it != Self.Properties.end(); it++)
		{
			for(std::map<std::string, std::string>::iterator sit = it->second.begin(); sit != it->second.end(); sit++)
			{
				Out[it->first][sit->first] = sit->second;
			};
		};

		return Out;
	};

	void SetTilesetInfoProperties(TiledMap::UniqueTilesetInfo &Self, luabind::argument arg)
	{
		Self.Properties.clear();

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			for(luabind::iterator sit(*it); sit != end; sit++)
			{
				Self.Properties[ProtectedLuaCast<uint8>(it.key())][ProtectedLuaCast<const char *>(sit.key())] = ProtectedLuaCast<const char *>(*sit);
			};
		};
	};

	luabind::object GetTiledMapPolygonData(TiledMap::MapObject &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.PolygonData.size(); i++)
		{
			Out[i + 1] = Self.PolygonData[i];
		};

		return Out;
	};

	void SetTiledMapPolygonData(TiledMap::MapObject &Self, luabind::argument arg)
	{
		Self.PolygonData.clear();

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			Self.PolygonData.push_back(ProtectedLuaCast<Vector2>(*it));
		};
	};

	luabind::object GetTiledMapObjectProperties(TiledMap::MapObject &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(std::map<std::string, std::string>::iterator it = Self.Properties.begin(); it != Self.Properties.end(); it++)
		{
			Out[it->first] = it->second;
		};

		return Out;
	};

	void SetTiledMapObjectProperties(TiledMap::MapObject &Self, luabind::argument arg)
	{
		Self.Properties.clear();

		for(luabind::iterator it(arg), end; it != end; it++)
		{
			Self.Properties[ProtectedLuaCast<const char *>(it.key())] = ProtectedLuaCast<const char *>(*it);
		};
	};

	luabind::object GetTiledMapObjects(TiledMap &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.Objects.size(); i++)
		{
			Out[i + 1] = Self.Objects[i];
		};

		return Out;
	};

	luabind::object GetTiledMapLayers(TiledMap &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.Layers.size(); i++)
		{
			Out[i + 1] = Self.Layers[i];
		};

		return Out;
	};

	luabind::object GetResourceManagerScript(const std::string &ScriptFileName)
	{
		luabind::object Out;

		SuperSmartPointer<FileStream> ScriptStream(new FileStream());

		if(!ScriptStream->Open(ScriptFileName, StreamFlags::Read | StreamFlags::Text))
			return Out;

		std::string ClassName = ScriptFileName;

		int32 Index = std::string::npos;

		Index = ClassName.rfind('/');

		if(Index != std::string::npos)
		{
			ClassName = ClassName.substr(Index + 1);
		};

		Index = ClassName.rfind('\\');

		if(Index != std::string::npos)
		{
			ClassName = ClassName.substr(Index + 1);
		};

		Index = ClassName.rfind('.');

		if(Index == std::string::npos)
			return Out;

		std::stringstream ImportCode;
		std::stringstream BaseFunctionName;
		BaseFunctionName << "IMPORT_" << StringUtils::MakeIntString(MakeStringID(ScriptFileName));

		ImportCode << "function " << BaseFunctionName.str() << "()\n" << ScriptStream->AsString() << "\nend\n";

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(Owner->ScriptInstance->DoString(ImportCode.str()))
			return Out;

		luabind::object Function = luabind::globals(Owner->ScriptInstance->State)[BaseFunctionName.str()];

		if(!Function)
			return Out;

		try
		{
			Out = Function();
		}
		catch(std::exception &e)
		{
			LuaScriptManager::Instance.LogError("While loading module '" + ScriptFileName + "' ('" + ClassName + "'): " + e.what());

			return luabind::object();
		};

		return Out;
	};

	luabind::object GetResourceManagerScriptFromPackage(ResourceManager &Self, const std::string &Directory, const std::string &FileName)
	{
		luabind::object Out;

		SuperSmartPointer<Stream> ScriptStream = PackageFileSystemManager::Instance.GetFile(MakeStringID(Directory), MakeStringID(FileName));

		if(!ScriptStream)
			return Out;

		std::string ClassName = FileName;

		if(FileName.rfind('.') == std::string::npos)
			return Out;

		ClassName = FileName.substr(0, FileName.rfind('.'));

		std::stringstream ImportCode;
		std::stringstream BaseFunctionName;
		BaseFunctionName << "IMPORT_" << StringUtils::MakeIntString(MakeStringID(Directory + FileName));

		ImportCode << "function " << BaseFunctionName.str() << "()\n" << ScriptStream->AsString() << "\nend\n";

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(Owner->ScriptInstance->DoString(ImportCode.str()))
			return Out;

		luabind::object Function = luabind::globals(Owner->ScriptInstance->State)[BaseFunctionName.str()];

		if(!Function)
			return Out;

		try
		{
			Out = Function();
		}
		catch(std::exception &e)
		{
			LuaScriptManager::Instance.LogError("While loading module '" + Directory + "/" + FileName + "' ('" + ClassName + "'): " + e.what());

			return luabind::object();
		};

		return Out;
	};

	luabind::object GetWorldEntitiesWithComponent(World &Self, const std::string &ComponentName)
	{
		luabind::object Out = luabind::newtable(GameInterface::Instance.AsDerived<ScriptedGameInterface>()->ScriptInstance->State);

		std::vector<Entity *> Entities = Self.EntitiesWithComponent(ComponentName);

		for(uint32 i = 0; i < Entities.size(); i++)
		{
			Out[i + 1] = Entities[i];
		};

		return Out;
	};

#if USE_GRAPHICS
	luabind::object GetSpriteColors(SpriteDrawOptions &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		Out[1] = Self.ColorsValue[0];
		Out[2] = Self.ColorsValue[1];
		Out[3] = Self.ColorsValue[2];
		Out[4] = Self.ColorsValue[3];

		return Out;
	};
	
	void SetSpriteColors(SpriteDrawOptions &Self, luabind::argument arg)
	{
		uint32 Index = 0;

		for(luabind::iterator it(arg), end; it != end && Index < 4; it++, Index++)
		{
			Self.ColorsValue[Index] = ProtectedLuaCast<Vector4>(*it);
		};
	};

	luabind::object GetUIManagerLayouts(UIManager &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(UIManager::LayoutMap::iterator it = Self.Layouts.begin(); it != Self.Layouts.end(); it++)
		{
			Out[GetStringIDString(it->first)] = it->second;
		};

		return Out;
	};

	bool AddUIManagerLayout(UIManager &Self, SuperSmartPointer<UILayout> Layout, StringID LayoutID)
	{
		if(Self.Layouts.find(LayoutID) != Self.Layouts.end())
			return false;

		Self.Layouts[LayoutID] = Layout;

		return true;
	};

	void RemoveUIManagerLayout(UIManager &Self, StringID LayoutID)
	{
		UIManager::LayoutMap::iterator it = Self.Layouts.find(LayoutID);

		if(it != Self.Layouts.end())
			Self.Layouts.erase(it);
	};

	luabind::object GetUILayoutElements(UILayout &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(UILayout::ElementMap::iterator it = Self.Elements.begin(); it != Self.Elements.end(); it++)
		{
			Out[GetStringIDString(it->first)] = it->second;
		};

		return Out;
	};

	luabind::object GetUIPanelChildren(UIPanel &Self, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		for(uint32 i = 0; i < Self.ChildrenCount(); i++)
		{
			Out[i + 1] = Self.Child(i);
		};

		return Out;
	};

	std::string UITextGetText(UIText &Self)
	{
		return Self.Text();
	};

	void UITextSetText(UIText &Self, const std::string &Text)
	{
		Self.SetText(Text, Self.ExpandHeight);
	};

	luabind::object RenderTextFitTextOnRect(Renderer *TheRenderer, const std::string &String, TextParams Params, const Vector2 &Size, lua_State *State)
	{
		luabind::object Out = luabind::newtable(State);

		std::vector<std::string> Temp = RenderTextUtils::FitTextOnRect(TheRenderer, String, Params, Size);

		for(uint32 i = 0; i < Temp.size(); i++)
		{
			Out[i + 1] = Temp[i];
		};

		return Out;
	};

	Rect RenderTextMeasureTextSimple(Renderer *TheRenderer, const std::string &String, TextParams Params)
	{
		return RenderTextUtils::MeasureTextSimple(TheRenderer, String, Params);
	};

	int32 RenderTextFitTextAroundLength(Renderer *TheRenderer, const std::string &String, TextParams Params, f32 LengthInPixels, int32 FontSize)
	{
		RenderTextUtils::FitTextAroundLength(TheRenderer, String, Params, LengthInPixels, &FontSize);

		return FontSize;
	};

	void RenderTextRenderText(Renderer *Renderer, const std::string &String, TextParams Params)
	{
		RenderTextUtils::RenderText(Renderer, String, Params);
	};

	Rect RenderTextMeasureTextLines(Renderer *TheRenderer, luabind::object Lines, TextParams Params)
	{
		std::vector<std::string> ActualLines;

		for(luabind::iterator it(Lines), end; it != end; it++)
		{
			ActualLines.push_back(ProtectedLuaCast<const char *>(*it));
		};

		return RenderTextUtils::MeasureTextLines(TheRenderer, &ActualLines[0], ActualLines.size(), Params);
	};

	/*
	bool ShaderCompileShader(Shader &Self, uint32 Type, const std::string &Data, std::string *Log)
	{
		return Self.CompileShader(Type, (void *)Data.c_str(), Data.length(), Log);
	};

	bool ShaderLinkShader(Shader &Self, std::string *Log)
	{
		return Self.LinkShader(Log);
	};

	void ShaderUniformFloatArray(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<f32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<float>(*it));
		};

		Self.UniformFloatArray(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformVector2Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<Vector2> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<Vector2>(*it));
		};

		Self.UniformVector2Array(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformVector3Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<Vector3> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<Vector3>(*it));
		};

		Self.UniformVector3Array(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformVector4Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<Vector4> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<Vector4>(*it));
		};

		Self.UniformVector4Array(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformMat3(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<f32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			for(luabind::iterator matit(*it); matit != end; matit++)
			{
				Out.push_back(ProtectedLuaCast<float>(*matit));
			};
		};

		Self.UniformMat3(Uniform, &Out[0], Out.size());
	};

	void ShaderUniformMat4(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<Matrix4x4> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<Matrix4x4>(*it));
		};

		Self.UniformMat4(Uniform, &Out[0], Out.size());
	};

	void ShaderUniformIntArray(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<int32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			Out.push_back(ProtectedLuaCast<int32>(*it));
		};

		Self.UniformIntArray(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformInt2Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<int32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			for(luabind::iterator intit(*it); intit != end; intit++)
			{
				Out.push_back(ProtectedLuaCast<int32>(*intit));
			};
		};

		Self.UniformInt2Array(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformInt3Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<int32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			for(luabind::iterator intit(*it); intit != end; intit++)
			{
				Out.push_back(ProtectedLuaCast<int32>(*intit));
			};
		};

		Self.UniformInt3Array(Uniform, Out.size(), &Out[0]);
	};

	void ShaderUniformInt4Array(Shader &Self, uint32 Uniform, luabind::object List)
	{
		std::vector<int32> Out;

		for(luabind::iterator it(List), end; it != end; it++)
		{
			for(luabind::iterator intit(*it); intit != end; intit++)
			{
				Out.push_back(ProtectedLuaCast<int32>(*intit));
			};
		};

		Self.UniformInt4Array(Uniform, Out.size(), &Out[0]);
	};
	*/

	void AnimatedSpriteAddAnimation(AnimatedSprite &Self, const std::string &Name, luabind::object Frames)
	{
		std::vector<Vector2> ActualFrames;

		for(luabind::iterator it(Frames), end; it != end; it++)
		{
			ActualFrames.push_back(ProtectedLuaCast<Vector2>(*it));
		};

		Self.AddAnimation(Name, ActualFrames);
	};

	const InputCenter::KeyInfo &GetInputCenterKey(InputCenter &Self, uint32 Name)
	{
		return Self.Keys[Name];
	};

	const InputCenter::MouseButtonInfo &GetInputCenterMouseButton(InputCenter &Self, uint32 Name)
	{
		return Self.MouseButtons[Name];
	};

	const InputCenter::JoystickButtonInfo &GetInputCenterJoystickButton(InputCenter &Self, uint32 JoyIndex, uint32 Name)
	{
		return Self.JoystickButtons[JoyIndex][Name];
	};

	const InputCenter::JoystickAxisInfo &GetInputCenterJoystickAxis(InputCenter &Self, uint32 JoyIndex, uint32 Name)
	{
		return Self.JoystickAxis[JoyIndex][Name];
	};
#endif

	bool FrameworkLib::Register(lua_State *State)
	{
		luabind::module(State) [
			//Subsystem
			luabind::class_<SubSystem>("SubSystem")
				.def("Register", &SubSystem::Register),

			//Camera
			luabind::class_<Camera>("Camera")
				.def_readwrite("ProjectionTransform", &Camera::ProjectionTransform)
				.def_readwrite("WorldTransform", &Camera::WorldTransform)
				.def_readwrite("DrawOffset", &Camera::DrawOffset)
#if USE_GRAPHICS
				.def("BeginTransforms", &Camera::BeginTransforms)
				.def("EndTransforms", &Camera::EndTransforms)
#endif
				.def("SetOrtho", &Camera::SetOrtho)
				.def("SetPerspective", &Camera::SetPerspective),

			//Component
			luabind::class_<Component>("Component")
				.property("Name", &Component::Name)
				.property("ID", &Component::ID)
				.property("Owner", &Component::Owner)
				.property("Properties", &GetComponentProperties)
				.def("Update", &Component::Update),

			//CoreUtils
			luabind::class_<CoreUtils>("CoreUtils")
				.def("MakeVersion", &CoreUtils::MakeVersion)
				.def("MakeVersionString", &CoreUtils::MakeVersionString)
				.def("MakeIntFromBytes", (int32 (*)(uint8, uint8, uint8, uint8)) &CoreUtils::MakeIntFromBytes)
				.def("MakeIntFromBytes", (int32 (*)(uint8 *, uint32)) &CoreUtils::MakeIntFromBytes),

			//CRC32
			luabind::class_<CRC32>("CRC32")
				.def("CRC", &CRC32::CRC),

			//DijkstraField
			luabind::class_<DijkstraField>("DijkstraField")
				.property("UserData", &DijkstraField::UserData)
				.def(luabind::constructor<uint32>())
				.def("Clear", &DijkstraField::Clear)
				.def("ClearNodes", &DijkstraField::ClearNodes)
				.def("AddRegionRect", &DijkstraField::AddRegionRect)
				.def("GetNode", &DijkstraField::GetNode)
				.def("FindPath", &DijkstraField::FindPath)
				.def("GetValidNodesAroundPosition", &DijkstraField::GetValidNodesAroundPosition),

			//DijkstraNode
			luabind::class_<DijkstraNode>("DijkstraNode")
				.property("Position", &DijkstraNode::Position)
				.property("Visited", &DijkstraNode::Visited)
				.property("Cost", &DijkstraNode::Cost)
				.property("Edges", &DijkstraNode::Edges)
				.property("Parent", &DijkstraNode::Parent)
				.property("UserData", &DijkstraNode::UserData)
				.property("Owner", &DijkstraNode::Owner),

			//DijkstraEdge
			luabind::class_<DijkstraEdge>("DijkstraEdge")
				.enum_("constants") [
					luabind::value("Cross", DijkstraEdgeMode::Cross),
					luabind::value("Adjacent", DijkstraEdgeMode::Adjacent)
				]
				.property("Weight", &DijkstraEdge::Weight)
				.property("Target", &DijkstraEdge::Target),

			//Entity
			luabind::class_<Entity, SuperSmartPointer<Entity> >("Entity")
				.property("Name", &Entity::Name)
				.property("ID", &Entity::ID)
				.property("Properties", &GetEntityProperties)
				.def("Destroy", &Entity::Destroy)
				.def("Clone", &Entity::Clone)
				.def("HasComponent", &Entity::HasComponent)
				.def("HasProperty", &Entity::HasProperty)
				.def("GetComponent", &Entity::GetComponent)
				.def("AddComponent", &Entity::AddComponent),

			//World
			luabind::class_<World>("World")
				.def("LoadComponent", &World::LoadComponent)
				.def("NewEntity", (Entity *(World::*)(const std::string &)) &World::NewEntity)
				.def("NewEntity", (Entity *(World::*)(const std::string &, const std::string &)) &World::NewEntity)
				.def("IterateEntitiesWithComponents", &World::IterateEntitiesWithComponents)
				.def("EntitiesWithComponent", &GetWorldEntitiesWithComponent)
				.def("Clear", &World::Clear)
				.def("Update", &World::Update),

			//Log
			luabind::class_<Log, SubSystem>("Log")
				.def("Info", &LogInfo)
				.def("Warn", &LogWarn)
				.def("Err", &LogErr)
				.def("Debug", &LogDebug)
				.def("HideTag", &Log::HideTag)
				.property("PrintTime", &Log::PrintTime),

			//FileSystemUtils
			luabind::class_<FileSystemUtils>("FileSystemUtils")
				.scope [
					luabind::def("ScanDirectory", &FileSystemUtilsScan),
					luabind::def("GetAllDirectories", &FileSystemUtilsGetDirectories),
					luabind::def("CreateDirectory", &FileSystemUtils::CreateDirectory),
					luabind::def("CopyDirectory", &FileSystemUtils::CopyDirectory),
					luabind::def("DeleteDirectory", &FileSystemUtils::DeleteDirectory),
					luabind::def("ActiveDirectory", &FileSystemUtils::ActiveDirectory),
					luabind::def("ResourcesDirectory", &FileSystemUtils::ResourcesDirectory),
					luabind::def("PreferredStorageDirectory", &FileSystemUtils::PreferredStorageDirectory),
					luabind::def("OpenFileDialog", &FileSystemUtils::OpenFileDialog),
					luabind::def("SaveFileDialog", &FileSystemUtils::SaveFileDialog),
					luabind::def("CopyFile", &FileSystemUtils::CopyFile),
					luabind::def("RemoveFile", &FileSystemUtils::RemoveFile)
				],

			//Path
			luabind::class_<Path>("Path")
				.def(luabind::constructor<>())
				.def(luabind::constructor<const std::string &>())
				.def(luabind::constructor<const std::string &, const std::string &>())
				.property("FullPath", &Path::FullPath)
				.property("Extension", &Path::Extension)
				.def_readwrite("Directory", &Path::Directory)
				.def_readwrite("BaseName", &Path::BaseName)
				.def("ChangeExtension", &Path::ChangeExtension)
				.scope [
					luabind::def("Normalize", &Path::Normalize)
				],

			//StreamProcessor
			luabind::class_<StreamProcessor, SuperSmartPointer<StreamProcessor> >("StreamProcessor")
				.def("Encode", &StreamProcessor::Encode)
				.def("Decode", &StreamProcessor::Decode),

			//XORStreamProcessor
			luabind::class_<XORStreamProcessor, StreamProcessor>("XORStreamProcessor")
				.def(luabind::constructor<>())
				.def("Encode", &XORStreamProcessor::Encode)
				.def("Decode", &XORStreamProcessor::Decode)
				.property("Key", &XORStreamProcessor::Key),

			//Stream
			luabind::class_<Stream, SuperSmartPointer<Stream> >("Stream")
				.enum_("constants") [
					luabind::value("Write", StreamFlags::Write),
					luabind::value("Read", StreamFlags::Read),
					luabind::value("Text", StreamFlags::Text)
				]
				.property("Length", &Stream::Length)
				.property("Position", &Stream::Position)
				.def("Seek", &Stream::Seek)
				.def("Write", &Stream::Write)
				.def("Read", &Stream::Read)
				.def("AsBuffer", &Stream::AsBuffer)
				.def("AsString", &Stream::AsString)
				.def("CopyTo", &Stream::CopyTo)
				.property("Processor", &Stream::GetProcessor, &Stream::SetProcessor)
				.def("ReadFromStream", &Stream::ReadFromStream)
				.def("WriteToStream", &Stream::WriteToStream),

			//MemoryStream
			luabind::class_<MemoryStream, Stream>("MemoryStream")
				.def(luabind::constructor<>())
				.def("Clear", &MemoryStream::Clear)
				.def("Get", &MemoryStream::Get),

			//ConstantMemoryStream
			luabind::class_<ConstantMemoryStream, Stream>("ConstantMemoryStream")
				.def("Get", &ConstantMemoryStream::Get),

			//FileStream
			luabind::class_<FileStream, Stream>("FileStream")
				.def(luabind::constructor<>())
				.def("Open", &FileStream::Open)
				.def("Close", &FileStream::Close),

			//PackageFileSystemManager
			luabind::class_<PackageFileSystemManager, SubSystem>("PackageFileSystemManager")
				.def("NewPackage", &PackageFileSystemManager::NewPackage)
				.def("AddPackage", &PackageFileSystemManager::AddPackage)
				.def("RemovePackage", &PackageFileSystemManager::RemovePackage)
				.def("GetPackage", &PackageFileSystemManager::GetPackage)
				.def("GetFile", &PackageFileSystemManager::GetFile)
				.def("FindDirectories", &PackageFileSystemFindDirectories)
				.def("FindFiles", &PackageFileSystemFindFiles),

			//FPSCounter
			luabind::class_<FPSCounter, SubSystem>("FPSCounter")
				.def("FPS", &FPSCounter::FPS),

			//TextureBuffer
			luabind::class_<TextureBuffer, SuperSmartPointer<TextureBuffer> >("TextureBuffer")
				.enum_("constants") [
					luabind::value("ColorType_RGB8", ColorType::RGB8),
					luabind::value("ColorType_RGBA8", ColorType::RGBA8)
				]
				.def(luabind::constructor<>())
				.property("Data", &GetTextureBufferData, &SetTextureBufferData)
				.property("Width", &TextureBuffer::Width)
				.property("Height", &TextureBuffer::Height)
				.property("ColorType", &TextureBuffer::ColorType)
				.def("FromData", &TextureBuffer::FromData)
				.def("FromStream", &TextureBuffer::FromStream)
				.def("CreateEmpty", &TextureBuffer::CreateEmpty)
				.def("Save", &TextureBuffer::Save)
				.def("Clone", &TextureBuffer::Clone)
				.def("Multiply", &TextureBuffer::Multiply)
				.def("Blend", &TextureBuffer::Blend)
				.def("Overlay", &TextureBuffer::Overlay),

			//TextureEncoderInfo
			luabind::class_<TextureEncoderInfo>("TextureEncoderInfo")
				.enum_("constants") [
					luabind::value("Encoder_PNG", TextureEncoderType::PNG),
					luabind::value("Encoder_WEBP", TextureEncoderType::WebP),
					luabind::value("Encoder_FTI", TextureEncoderType::FTI)
				]
				.def(luabind::constructor<>())
				.property("TargetWidth", &TextureEncoderInfo::TargetWidth)
				.property("TargetHeight", &TextureEncoderInfo::TargetHeight)
				.property("Quality", &TextureEncoderInfo::Quality)
				.property("Encoder", &TextureEncoderInfo::Encoder)
				.property("Lossless", &TextureEncoderInfo::Lossless),

			//Texture
			luabind::class_<Texture, SuperSmartPointer<Texture> >("Texture")
				.enum_("constants") [
					luabind::value("Filtering_Nearest", TextureFiltering::Nearest),
					luabind::value("Filtering_Linear", TextureFiltering::Linear),
					luabind::value("Filtering_Nearest_Mipmap", TextureFiltering::Nearest_Mipmap),
					luabind::value("Filtering_Linear_Mipmap", TextureFiltering::Linear_Mipmap),
					luabind::value("Wrap_Clamp", TextureWrapMode::Clamp),
					luabind::value("Wrap_ClampToBorder", TextureWrapMode::ClampToBorder),
					luabind::value("Wrap_ClampToEdge", TextureWrapMode::ClampToEdge),
					luabind::value("Wrap_Repeat", TextureWrapMode::Repeat)
				]
				.property("TextureFilter", &Texture::FilterMode, &Texture::SetTextureFiltering)
				.property("WrapMode", &Texture::WrapMode, &Texture::SetWrapMode)
				.property("Handle", &Texture::Handle)
				.property("Width", &Texture::Width)
				.property("Height", &Texture::Height)
				.property("ColorType", &Texture::ColorType)
				.property("Size", &Texture::Size)
				.property("Data", &Texture::GetData)
				.def(luabind::constructor<>())
				.def("Destroy", &Texture::Destroy)
				.def("FromBuffer", &Texture::FromBuffer)
				.def("FromData", &Texture::FromData)
				.def("FromStream", &Texture::FromStream)
				.def("FromPackage", &Texture::FromPackage)
				.def("FromFile", &Texture::FromFile)
#if USE_GRAPHICS
				.def("FromScreen", &Texture::FromScreen)
#endif
				.def("CreateEmptyTexture", &Texture::CreateEmptyTexture)
				.def("UpdateData", &Texture::UpdateData)
				.def("Bind", &Texture::Bind)
				.def("PushTextureStates", &Texture::PushTextureStates)
				.def("PopTextureStates", &Texture::PopTextureStates)
				.def("Blur", &Texture::Blur)
				.def("GetPixel", &Texture::GetPixel)
				.def(luabind::const_self == luabind::other<const Texture &>()),

			//TexturePacker
			luabind::class_<TexturePacker>("TexturePacker")
				.scope [
					luabind::def("FromConfig", &TexturePacker::FromConfig)
				]
				.def("GetTexture", &TexturePacker::GetTexture)
				.property("IndexCount", &TexturePacker::IndexCount),

			//FrustumCuller
			luabind::class_<FrustumCuller>("FrustumCuller")
				.enum_("constants") [
					luabind::value("Visible", AABBResults::Visible),
					luabind::value("Invisible", AABBResults::Invisible),
					luabind::value("Intersecting", AABBResults::Intersecting)
				]
				.def(luabind::constructor<>())
				.def("Update", &FrustumCuller::Update)
				.def("PointTest", &FrustumCuller::PointTest)
				.def("SphereTest", &FrustumCuller::SphereTest)
				.def("AABBTest", &FrustumCuller::AABBTest),

			//TODO
			luabind::class_<Future, SubSystem>("Future")/*
				.def("Post", &FuturePost)
				.def("PostDelayed", &FuturePostDelayed)*/,

			//GameClock
			luabind::class_<GameClock, SubSystem>("GameClock")
				.def("Pause", &GameClock::Pause)
				.def("Unpause", &GameClock::Unpause)
				.def("ElapsedTime", &GameClock::GetElapsedTime)
				.def("Delta", &GameClock::Delta)
				.def("CurrentTimeAsString", &GameClock::CurrentTimeAsString)
				.def("CurrentTime", &GameClock::CurrentTime)
				.def("SetFixedStepRate", &GameClock::SetFixedStepRate)
				.def("FixedStepInterval", &GameClock::GetFixedStepInterval)
				.def("GetFixedStepRate", &GameClock::GetFixedStepRate)
				.def("MayPerformFixedStepStep", &GameClock::MayPerformFixedStepStep)
				.def("FixedStepDelta", &GameClock::FixedStepDelta)
				.def("Time", &GameClockTime)
				.def("TimeNoPause", &GameClockTimeNoPause)
				.def("Difference", &GameClockDiff)
				.def("DifferenceNoPause", &GameClockDiffNoPause),

			//LinearTimer
			luabind::class_<LinearTimer>("LinearTimer")
				.property("Value", &LinearTimer::Value)
				.property("ValueNoPause", &LinearTimer::ValueNoPause)
				.def_readwrite("Interval", &LinearTimer::Interval)
				.def("Reset", &LinearTimer::Reset)
				.def(luabind::constructor<>()),

			//GenericConfig
			luabind::class_<GenericConfig, SuperSmartPointer<GenericConfig> >("GenericConfig")
				.def(luabind::constructor<>())
				.def("Serialize", &GenericConfig::Serialize)
				.def("DeSerialize", &GenericConfig::DeSerialize)
				.def("GetValue", &GenericConfig::GetValue)
				.def("GetInt", &GenericConfig::GetInt)
				.def("GetFloat", &GenericConfig::GetFloat)
				.def("GetString", &GenericConfig::GetString)
				.def("SetValue", &GenericConfig::SetValue)
				.property("Sections", &GetGenericConfigSections)
				.scope [
					luabind::class_<GenericConfig::Value>("Value")
						.def(luabind::constructor<>())
						.property("Content", &GenericConfig::Value::Content)
						.property("Type", &GenericConfig::Value::Type),

					luabind::class_<GenericConfig::IntValue, GenericConfig::Value>("IntValue")
						.def(luabind::constructor<>())
						.def(luabind::constructor<int32>())
						.def(luabind::constructor<const char *>())
						.property("Value", &GenericConfig::IntValue::Value),

					luabind::class_<GenericConfig::FloatValue, GenericConfig::Value>("FloatValue")
						.def(luabind::constructor<>())
						.def(luabind::constructor<f32>())
						.def(luabind::constructor<const char *>())
						.property("Value", &GenericConfig::FloatValue::Value)
				],

			//GameInterface
			luabind::class_<GameInterface>("GameInterface")
				.property("GameName", &GameInterface::GameName)
				.def_readwrite("UpdateRate", &GameInterface::UpdateRateValue)
				.def_readwrite("FrameRate", &GameInterface::FrameRateValue)
				.def_readwrite("DevelopmentBuild", &GameInterface::DevelopmentBuild)
				.def_readwrite("IsGUISandbox", &GameInterface::IsGUISandbox)
#if USE_GRAPHICS
				.def("CreateRenderer", &GameInterface::CreateRenderer)
#endif
				.def("LoadPackage", &GameInterface::LoadPackage),

			//LuaScript
			luabind::class_<LuaScript>("LuaScript"),

			//LuaLib
			luabind::class_<LuaLib>("LuaLib"),

			//FrameworkLib
			luabind::class_<FrameworkLib>("FrameworkLib"),

			//LuaScriptManager
			luabind::class_<LuaScriptManager, SubSystem>("LuaScriptManager")
				.def("CreateScript", &LuaScriptManager::CreateScript),

			//LuaEventGroup
			luabind::class_<LuaEventGroup>("LuaEventGroup")
				.def("Add", &LuaEventGroup::Add)
				.def("Remove", &LuaEventGroup::Remove),

			//Matrix4x4
			luabind::class_<Matrix4x4>("Matrix4x4")
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Matrix4x4 &>())
				.property("m", &GetMatrix4x4M, &SetMatrix4x4M)
				.def(luabind::const_self * luabind::other<const Matrix4x4 &>())
				.def("Determinant", &Matrix4x4::Determinant)
				.def("Identity", &Matrix4x4::Identity)
				.def("Transpose", &Matrix4x4::Transpose)
				.def("Inverse", &Matrix4x4::Inverse)
				.scope [
					luabind::def("Scale", &Matrix4x4::Scale),
					luabind::def("Rotate", &Matrix4x4::Rotate),
					luabind::def("Translate", &Matrix4x4::Translate),
					luabind::def("Viewport", &Matrix4x4::Viewport),
					luabind::def("PerspectiveMatrixLH", &Matrix4x4::PerspectiveMatrixLH),
					luabind::def("PerspectiveMatrixRH", &Matrix4x4::PerspectiveMatrixRH),
					luabind::def("OrthoMatrixLH", &Matrix4x4::OrthoMatrixLH),
					luabind::def("OrthoMatrixRH", &Matrix4x4::OrthoMatrixRH),
					luabind::def("LookAtMatrixLH", &Matrix4x4::LookAtMatrixLH),
					luabind::def("LookAtMatrixRH", &Matrix4x4::LookAtMatrixRH)
				],

			//Vector2
			luabind::class_<Vector2>("Vector2")
				.def_readwrite("x", &Vector2::x)
				.def_readwrite("y", &Vector2::y)
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float>())
				.def(luabind::constructor<const Vector2 &>())
				.def(luabind::const_self + luabind::other<const Vector2 &>())
				.def(luabind::const_self + luabind::other<float>())
				.def(luabind::const_self - luabind::other<const Vector2 &>())
				.def(luabind::const_self - luabind::other<float>())
				.def(luabind::const_self * luabind::other<const Vector2 &>())
				.def(luabind::const_self * luabind::other<float>())
				.def(luabind::const_self / luabind::other<const Vector2 &>())
				.def(luabind::const_self / luabind::other<float>())
				.def(luabind::const_self == luabind::other<const Vector2 &>())
				.def(luabind::const_self < luabind::other<const Vector2 &>())
				.def(luabind::const_self <= luabind::other<const Vector2 &>())
				.def(-luabind::const_self)
				.def("Magnitude", &Vector2::Magnitude)
				.def("MagnitudeSquare", &Vector2::MagnitudeSquare)
				.def("Normalized", &Vector2::Normalized)
				.def("Normalize", &Vector2::Normalize)
				.def("IsNormalized", &Vector2::IsNormalized)
				.def("Distance", &Vector2::Distance)
				.def("DistanceSquare", &Vector2::DistanceSquare)
				.def("Dot", &Vector2::Dot)
				.def("Floor", &Vector2::Floor)
				.def("Ceil", &Vector2::Ceil)
				.def("Round", &Vector2::Round)

				.scope [
					luabind::def("LinearInterpolate", &Vector2::LinearInterpolate),
					luabind::def("Rotate", &Vector2::Rotate)
				],

			//Vector3
			luabind::class_<Vector3>("Vector3")
				.def_readwrite("x", &Vector3::x)
				.def_readwrite("y", &Vector3::y)
				.def_readwrite("z", &Vector3::z)
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float, float>())
				.def(luabind::constructor<const Vector2 &>())
				.def(luabind::constructor<const Vector2 &, float>())
				.def(luabind::constructor<float, const Vector2 &>())
				.def(luabind::constructor<const Vector3 &>())
				.def(luabind::const_self + luabind::other<const Vector3 &>())
				.def(luabind::const_self + luabind::other<float>())
				.def(luabind::const_self - luabind::other<const Vector3 &>())
				.def(luabind::const_self - luabind::other<float>())
				.def(luabind::const_self * luabind::other<const Vector3 &>())
				.def(luabind::const_self * luabind::other<float>())
				.def(luabind::const_self / luabind::other<const Vector3 &>())
				.def(luabind::const_self / luabind::other<float>())
				.def(luabind::const_self == luabind::other<const Vector3 &>())
				.def(luabind::const_self < luabind::other<const Vector3 &>())
				.def(luabind::const_self <= luabind::other<const Vector3 &>())
				.def(-luabind::const_self)
				.def("Magnitude", &Vector3::Magnitude)
				.def("MagnitudeSquare", &Vector3::MagnitudeSquare)
				.def("Normalized", &Vector3::Normalized)
				.def("Normalize", &Vector3::Normalize)
				.def("IsNormalized", &Vector3::IsNormalized)
				.def("Distance", &Vector3::Distance)
				.def("DistanceSquare", &Vector3::DistanceSquare)
				.def("Dot", &Vector3::Dot)
				.def("Floor", &Vector3::Floor)
				.def("Ceil", &Vector3::Ceil)
				.def("Round", &Vector3::Round)
				.def("Cross", &Vector3::Cross)
				.def("ToVector2", &Vector3::ToVector2)
				.scope [
					luabind::def("LinearInterpolate", &Vector3::LinearInterpolate),
					luabind::def("NormalTri", &Vector3::NormalTri),
					luabind::def("VectorAngles", &Vector3::VectorAngles),
					luabind::def("AnglesVector", &Vector3::AnglesVector)
				],

			//Vector4
			luabind::class_<Vector4>("Vector4")
				.def_readwrite("x", &Vector4::x)
				.def_readwrite("y", &Vector4::y)
				.def_readwrite("z", &Vector4::z)
				.def_readwrite("w", &Vector4::w)
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float, float>())
				.def(luabind::constructor<float, float, float, float>())
				.def(luabind::constructor<const Vector2 &>())
				.def(luabind::constructor<const Vector2 &, float, float>())
				.def(luabind::constructor<float, const Vector2 &>())
				.def(luabind::constructor<float, const Vector2 &, float>())
				.def(luabind::constructor<float, float, const Vector2 &>())
				.def(luabind::constructor<const Vector2 &, const Vector2 &>())
				.def(luabind::constructor<const Vector3 &>())
				.def(luabind::constructor<const Vector3 &, float>())
				.def(luabind::constructor<float, const Vector3 &>())
				.def(luabind::constructor<const Vector4 &>())
				.def(luabind::const_self + luabind::other<const Vector4 &>())
				.def(luabind::const_self + luabind::other<float>())
				.def(luabind::const_self - luabind::other<const Vector4 &>())
				.def(luabind::const_self - luabind::other<float>())
				.def(luabind::const_self * luabind::other<const Vector4 &>())
				.def(luabind::const_self * luabind::other<float>())
				.def(luabind::const_self / luabind::other<const Vector4 &>())
				.def(luabind::const_self / luabind::other<float>())
				.def(luabind::const_self == luabind::other<const Vector4 &>())
				.def(luabind::const_self < luabind::other<const Vector4 &>())
				.def(luabind::const_self <= luabind::other<const Vector4 &>())
				.def(-luabind::const_self)
				.def("Magnitude", &Vector4::Magnitude)
				.def("MagnitudeSquare", &Vector4::MagnitudeSquare)
				.def("Normalized", &Vector4::Normalized)
				.def("Normalize", &Vector4::Normalize)
				.def("IsNormalized", &Vector4::IsNormalized)
				.def("Distance", &Vector4::Distance)
				.def("DistanceSquare", &Vector4::DistanceSquare)
				.def("Dot", &Vector4::Dot)
				.def("Floor", &Vector4::Floor)
				.def("Ceil", &Vector4::Ceil)
				.def("Round", &Vector4::Round)
				.def("Cross", &Vector4::Cross)
				.def("ToVector2", &Vector4::ToVector2)
				.def("ToVector3", &Vector4::ToVector3)
				.scope [
					luabind::def("LinearInterpolate", &Vector4::LinearInterpolate)
				],

			//Plane
			luabind::class_<Plane>("Plane")
				.enum_("constants") [
					luabind::value("Point_OnPlane", PlanePointClassifications::On_Plane),
					luabind::value("Front", PlanePointClassifications::Front),
					luabind::value("Back", PlanePointClassifications::Back)
				]
				.def_readwrite("Origin", &Plane::Origin)
				.def_readwrite("Distance", &Plane::Distance)
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float, float, float>())
				.def(luabind::constructor<const Vector3 &, float>())
				.def(luabind::constructor<const Plane &>())
				.def("FromTriangle", &Plane::FromTriangle)
				.def("ClassifyPoint", &Plane::ClassifyPoint),

			//Rect
			luabind::class_<Rect>("Rect")
				.def_readwrite("Left", &Rect::Left)
				.def_readwrite("Right", &Rect::Right)
				.def_readwrite("Top", &Rect::Top)
				.def_readwrite("Bottom", &Rect::Bottom)
				.property("Size", &Rect::Size)
				.property("FullSize", &Rect::ToFullSize)
				.property("Position", &Rect::Position)
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Rect &>())
				.def(luabind::constructor<float, float>())
				.def(luabind::constructor<float, float, float, float>())
				.def(luabind::const_self + luabind::other<const Rect &>())
				.def(luabind::const_self - luabind::other<const Rect &>())
				.def("IsInside", &Rect::IsInside)
				.def("IsOutside", &Rect::IsOutside),

			//RotateableRect
			luabind::class_<RotateableRect>("RotateableRect")
				.def_readwrite("Left", &RotateableRect::Left)
				.def_readwrite("Right", &RotateableRect::Right)
				.def_readwrite("Top", &RotateableRect::Top)
				.def_readwrite("Bottom", &RotateableRect::Bottom)
				.def_readwrite("Rotation", &RotateableRect::Rotation)
				.property("Size", &RotateableRect::Size)
				.property("FullSize", &RotateableRect::ToFullSize)
				.property("Position", &RotateableRect::Position)
				.def(luabind::constructor<>())
				.def(luabind::constructor<const RotateableRect &>())
				.def(luabind::constructor<float, float, float>())
				.def(luabind::constructor<float, float, float, float, float>())
				.def(luabind::const_self + luabind::other<const RotateableRect &>())
				.def(luabind::const_self - luabind::other<const RotateableRect &>())
				.def("IsInside", &RotateableRect::IsInside)
				.def("IsOutside", &RotateableRect::IsOutside),

			//AxisAlignedBoundingBox
			luabind::class_<AxisAlignedBoundingBox>("AxisAlignedBoundingBox")
				.def_readwrite("min", &AxisAlignedBoundingBox::min)
				.def_readwrite("max", &AxisAlignedBoundingBox::max)
				.def(luabind::constructor<>())
				.def(luabind::constructor<const AxisAlignedBoundingBox &>())
				.def(luabind::constructor<const Vector3 &, const Vector3 &>())
				.def("Clear", &AxisAlignedBoundingBox::Clear)
				.def("Calculate", (void (AxisAlignedBoundingBox::*)(const Vector3 &)) &AxisAlignedBoundingBox::Calculate)
				.def("Intersects", &AxisAlignedBoundingBox::Intersects)
				.def("IsInside", &AxisAlignedBoundingBox::IsInside)
				.def("IsOutside", &AxisAlignedBoundingBox::IsOutside),

			//BoundingSphere
			luabind::class_<BoundingSphere>("BoundingSphere")
				.def_readwrite("Center", &BoundingSphere::Center)
				.def_readwrite("Radius", &BoundingSphere::Radius)
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Vector3 &, float>())
				.def("Intersects", (bool (BoundingSphere::*)(const Vector3 &) const) &BoundingSphere::Intersects)
				.def("Intersects", (bool (BoundingSphere::*)(const BoundingSphere &) const) &BoundingSphere::Intersects),

			//Ray
			luabind::class_<Ray>("Ray")
				.def_readwrite("Position", &Ray::Position)
				.def_readwrite("Direction", &Ray::Direction)
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Ray &>())
				.def(luabind::constructor<const Vector3 &, const Vector3 &>())
				.def("IntersectsTriangle", &Ray::IntersectsTriangle)
				.def("IntersectsAligned", &Ray::IntersectsAligned)
				.def("IntersectsSphere", &Ray::IntersectsSphere)
				.def("IntersectsPlane", &Ray::IntersectsPlane),

			//Quaternion
			luabind::class_<Quaternion>("Quaternion")
				.def_readwrite("x", &Quaternion::x)
				.def_readwrite("y", &Quaternion::y)
				.def_readwrite("z", &Quaternion::z)
				.def_readwrite("w", &Quaternion::w)
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float, float, float>())
				.def(-luabind::const_self)
				.def(luabind::const_self * luabind::other<float>())
				.def(luabind::const_self * luabind::other<const Quaternion &>())
				.def(luabind::const_self + luabind::other<const Quaternion &>())
				.def(luabind::const_self - luabind::other<const Quaternion &>())
				.def(luabind::const_self == luabind::other<const Quaternion &>())
				.def("Normalize", &Quaternion::Normalize)
				.def("Normalized", &Quaternion::Normalized)
				.def("IsNormalized", &Quaternion::IsNormalized)
				.def("ToMatrix4x4", &Quaternion::ToMatrix4x4)
				.def("FromCompressed", &Quaternion::FromCompressed)
				.def("Compress", &Quaternion::Compress)
				.def("FromAxis", &Quaternion::FromAxis)
				.def("BezierInterpolation", &Quaternion::BezierInterpolation),

			//MathUtils
			luabind::class_<MathUtils>("MathUtils")
				.scope [
					luabind::def("ScaleKeepingAspectRatio", &MathUtils::ScaleKeepingAspectRatio),
					luabind::def("Clamp", &MathUtils::Clamp),
					luabind::def("Round", &MathUtils::Round),
					luabind::def("Min", &MathUtils::Min),
					luabind::def("Max", &MathUtils::Max),
					luabind::def("DegToRad", &MathUtils::DegToRad),
					luabind::def("RadToDeg", &MathUtils::RadToDeg),
					luabind::def("ColorFromHTML", &MathUtils::ColorFromHTML),
					luabind::def("CalculateViewportKeepingAspectRatio", &MathUtils::CalculateViewportKeepingAspectRatio),
					luabind::def("ScaleFromViewport", &MathUtils::ScaleFromViewport)
				]
				.property("Pi", &GetMathUtilsPi)
				.property("Epsilon", &GetMathUtilsEpsilon),

			//WorldChunk
			luabind::class_<WorldChunk>("WorldChunk")
				.def_readwrite("Coordinate", &WorldChunk::Coordinate)
				.def_readwrite("GlobalCoordinate", &WorldChunk::GlobalCoordinate)
				.def_readonly("Owner", &WorldChunk::Owner)
				.def("UpdateCoordinate", &WorldChunk::UpdateCoordinate),

			//WorldStreamerCallback
			luabind::class_<WorldStreamerCallback>("WorldStreamerCallback")
				.def("OnMapChunkLoad", &WorldStreamerCallback::OnMapChunkLoad)
				.def("OnMapChunkUnload", &WorldStreamerCallback::OnMapChunkUnload)
				.def("OnMapChunkCoordinateUpdate", &WorldStreamerCallback::OnMapChunkCoordinateUpdate)
				.property("ChunkSize", &WorldStreamerCallback::ChunkSize)
				.def("ChunkNeedsLoad", &WorldStreamerCallback::ChunkNeedsLoad),

			//ScriptedWorldStreamerCallback
			luabind::class_<ScriptedWorldStreamerCallback, WorldStreamerCallback>("ScriptedWorldStreamerCallback")
				.scope [
					luabind::def("FromGame", &GetWorldStreamerCallbackFromGame)
				],

			//WorldStreamer
			luabind::class_<WorldStreamer>("WorldStreamer")
				.def(luabind::constructor<>())
				.property("Callback", &GetWorldStreamerCallback, &SetWorldStreamerCallback)
				.def("Update", &WorldStreamer::Update)
				.def("Reset", &WorldStreamer::Reset)
				.def("UnloadChunk", &WorldStreamer::UnloadChunk)
				.def("ClipUnitPosition", &WorldStreamer::ClipUnitPosition)
				.def("GetChunkIndex", &WorldStreamer::GetChunkIndex)
				.def("TranslateMainChunk", &WorldStreamer::TranslateMainChunk)
				.property("WorldRadius", &WorldStreamer::GetWorldRadius, &WorldStreamer::SetWorldRadius)
				.property("GlobalCoordinate", &WorldStreamer::GetGlobalCoordinate, &WorldStreamer::SetGlobalCoordinate),

			//FileSystemWatcher
			luabind::class_<FileSystemWatcher, SubSystem>("FileSystemWatcher")
				.enum_("constants") [
					luabind::value("Action_Added", FileSystemWatcherAction::Added),
					luabind::value("Action_Modified", FileSystemWatcherAction::Modified),
					luabind::value("Action_Deleted", FileSystemWatcherAction::Deleted)
				]
				.def("WatchDirectory", &FileSystemWatcher::WatchDirectory),

#if USE_NETWORK
			//GameClient
			luabind::class_<GameClient>("GameClient")
				.def("Destroy", &GameClient::Destroy)
				.def("Initialize", &GameClient::Initialize)
				.def("SendPacket", &GameClient::SendPacket)
				.def("SendPacketNoQueue", &GameClient::SendPacketNoQueue),

			//GameServer
			luabind::class_<GameServer>("GameServer")
				.def("Destroy", &GameClient::Destroy)
				.def("Initialize", &GameClient::Initialize)
				.def("SendPacket", &GameClient::SendPacket)
				.def("SendPacketNoQueue", &GameClient::SendPacketNoQueue),

			//GameNetwork
			luabind::class_<GameNetwork, SubSystem>("GameNetwork")
				.enum_("constants") [
					luabind::value("PacketFlag_NoAlloc", ENET_PACKET_FLAG_NO_ALLOCATE),
					luabind::value("PacketFlag_Unsequenced", ENET_PACKET_FLAG_UNSEQUENCED),
					luabind::value("PacketFlag_UnreliableFragment", ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT),
					luabind::value("PacketFlag_Reliable", ENET_PACKET_FLAG_RELIABLE)
				]
				.def("GetClient", &GameNetwork::GetClient)
				.def("GetServer", &GameNetwork::GetServer),
#endif

			//PerlinNoise
			luabind::class_<PerlinNoise>("PerlinNoise")
				.def(luabind::constructor<>())
				.def(luabind::constructor<int32>())
				.def_readwrite("Seed", &PerlinNoise::seed)
				.def("Generate", &PerlinNoise::Generate)
				.def("GenerateNormalized", &PerlinNoise::GenerateNormalized)
				.def("Noise", &PerlinNoise::Noise)
				.def("SmoothNoise", &PerlinNoise::SmoothNoise)
				.def("Interpolate", &PerlinNoise::Interpolate)
				.def("InterpolateNoise", &PerlinNoise::InterpolatedNoise),

			//Randomizer
			luabind::class_<Randomizer>("Randomizer")
				.scope [
					luabind::def("GetSeed", &Randomizer::GetSeed),
					luabind::def("SetSeed", &Randomizer::SetSeed),
					luabind::def("RandomFloat", &Randomizer::RandomFloat),
					luabind::def("RandomInt", &Randomizer::RandomInt),
					luabind::def("RandomNormalized", &Randomizer::RandomNormalized),
					luabind::def("RandomNormalizedSphere", &Randomizer::RandomNormalizedSphere)
				],

			//TiledMap
			luabind::class_<TiledMap, SuperSmartPointer<TiledMap> >("TiledMap")
				.scope [
					luabind::class_<TiledMapInitOptions>("InitOptions")
						.def_readwrite("FromPackageValue", &TiledMapInitOptions::FromPackageValue)
						.property("PackageDirectoriesValue", &GetTiledMapInitPackageDirectoriesValue, &SetTiledMapInitPackageDirectoriesValue)
						.def(luabind::constructor<>())
						.def("FromPackage", &TiledMapInitOptions::FromPackage)
						.def("AdditionalPackageDirectory", &TiledMapInitOptions::AdditionalPackageDirectory),

					luabind::class_<TiledMap::Layer>("Layer")
						.def_readwrite("Visible", &TiledMap::Layer::Visible)
						.def_readwrite("Alpha", &TiledMap::Layer::Alpha)
						.scope [
							luabind::class_<TiledMap::Layer::TileInfo>("TileInfo")
								.def_readwrite("ID", &TiledMap::Layer::TileInfo::ID)
								.def_readwrite("FlipFlag", &TiledMap::Layer::TileInfo::FlipFlag)
								.def_readwrite("Position", &TiledMap::Layer::TileInfo::Position)
						]
						.property("Tiles", &GetTileLayerTiles, &SetTileLayerTiles),

					luabind::class_<TiledMap::UniqueTilesetInfo>("UniqueTilesetInfo")
						.property("Properties", &GetTilesetInfoProperties, &SetTilesetInfoProperties),
						
					luabind::class_<TiledMap::MapObject>("MapObject")
						.def_readwrite("Position", &TiledMap::MapObject::Position)
						.def_readwrite("Size", &TiledMap::MapObject::Size)
						.def_readwrite("Rotation", &TiledMap::MapObject::Rotation)
						.def_readwrite("Name", &TiledMap::MapObject::Name)
						.def_readwrite("ObjectType", &TiledMap::MapObject::Type)
						.def_readwrite("IsPolyLine", &TiledMap::MapObject::IsPolyLine)
						.property("PolygonData", &GetTiledMapPolygonData, &SetTiledMapPolygonData)
						.property("Properties", &GetTiledMapObjectProperties, &SetTiledMapObjectProperties)
				]
#if USE_GRAPHICS
				.def("InitResources", &TiledMap::InitResources)
				.def("ResourcesInited", &TiledMap::ResourcesInited)
				.def("Draw", &TiledMap::Draw)
				.def("UpdateGeometry", &TiledMap::UpdateGeometry)
#endif
				.def(luabind::constructor<>())
				.def("DeSerialize", &TiledMap::DeSerialize)
				.def_readwrite("Scale", &TiledMap::Scale)
				.def_readwrite("Translation", &TiledMap::Translation)
				.def_readwrite("Color", &TiledMap::Translation)
				.def_readonly("MapTileSize", &TiledMap::MapTileSize)
				.def_readonly("MapPixelSize", &TiledMap::MapPixelSize)
				.def_readonly("MapTileCount", &TiledMap::MapTileCount)
				.property("Objects", &GetTiledMapObjects)
				.def_readonly("TileSet", &TiledMap::TileSet)
				.property("Layers", &GetTiledMapLayers),

			//ResourceManager
			luabind::class_<ResourceManager, SubSystem>("ResourceManager")
#if USE_GRAPHICS
				.def("GetFont", &ResourceManager::GetFont)
				.def("GetFontFromPackage", &ResourceManager::GetFontFromPackage)
#endif
				.def("GetTexture", &ResourceManager::GetTexture)
				.def("GetTextureFromPackage", &ResourceManager::GetTextureFromPackage)
				.def("PrepareResourcesReload", &ResourceManager::PrepareResourceReload)
				.def("ReloadResources", &ResourceManager::ReloadResources)
				.def("Cleanup", &ResourceManager::Cleanup)
				.def("GetScript", &GetResourceManagerScript)
				.def("GetScriptFromPackage", &GetResourceManagerScriptFromPackage)
				.scope [
					luabind::def("IsSameTexture", &ResourceManager::IsSameTexture)
				],

			//ConsoleCommand
			luabind::class_<ConsoleCommand, SuperSmartPointer<ConsoleCommand> >("ConsoleCommand")
				.def_readwrite("Name", &ConsoleCommand::Name),

			//ScriptedConsoleCommand
			luabind::class_<ScriptedConsoleCommand, ConsoleCommand>("ScriptedConsoleCommand")
				.def_readwrite("Method", &ScriptedConsoleCommand::ScriptedMethod)
				.def(luabind::constructor<>()),

			//ConsoleVariable
			luabind::class_<ConsoleVariable>("ConsoleVariable")
				.enum_("constants") [
					luabind::value("Type_UInt", ConsoleVariableType::UInt),
					luabind::value("Type_Int", ConsoleVariableType::Int),
					luabind::value("Type_Float", ConsoleVariableType::Float),
					luabind::value("Type_String", ConsoleVariableType::String)
				]
				.def(luabind::constructor<>())
				.def_readwrite("Type", &ConsoleVariable::Type)
				.def_readwrite("Name", &ConsoleVariable::Name)
				.def_readwrite("UintValue", &ConsoleVariable::UintValue)
				.def_readwrite("IntValue", &ConsoleVariable::IntValue)
				.def_readwrite("FloatValue", &ConsoleVariable::FloatValue)
				.property("StringValue", &GetConsoleVariableString, &SetConsoleVariableString),

			//Console
			luabind::class_<Console, SubSystem>("Console")
				.def("RegisterVariable", &Console::RegisterVariable)
				.def("RegisterCommand", &Console::RegisterCommand)
				.def("GetVariable", &Console::GetVariable)
				.def("LogConsole", &ConsoleLogConsole)
				.def("RunConsoleCommand", &ConsoleRunConsoleCommand),

			//StringUtils
			luabind::class_<StringUtils>("StringUtils")
				.scope [
					luabind::def("HexToFloat", &StringUtils::HexToFloat),
					luabind::def("HexToInt", &StringUtils::HexToInt),
					luabind::def("MakeByteString", &StringUtils::MakeByteString),
					luabind::def("MakeIntString", (std::string (*)(const int32 &, bool)) &StringUtils::MakeIntString),
					luabind::def("MakeIntString", (std::string (*)(const uint32 &, bool)) &StringUtils::MakeIntString),
					luabind::def("MakeFloatString", &StringUtils::MakeFloatString),
					luabind::def("Split", &StringUtilsSplit)
				],

#if USE_SOUND
			//SoundManager
			luabind::class_<SoundManager, SubSystem>("SoundManager")
				.enum_("constants") [
					luabind::value("Status_Stopped", sf::SoundSource::Stopped),
					luabind::value("Status_Paused", sf::SoundSource::Paused),
					luabind::value("Status_Playing", sf::SoundSource::Playing)
				]
				.scope [
					luabind::class_<SoundManager::Sound, SuperSmartPointer<SoundManager::Sound> >("Sound")
						.def("Play", &SoundManager::Sound::Play)
						.def("Stop", &SoundManager::Sound::Stop)
						.def("Pause", &SoundManager::Sound::Pause)
						.property("Volume", &SoundManager::Sound::GetVolume, &SoundManager::Sound::SetVolume)
						.property("Status", &SoundManager::Sound::GetStatus)
						.property("Playing", &SoundManager::Sound::IsPlaying)
						.property("Looping", &SoundManager::Sound::IsLooping, &SoundManager::Sound::SetLoop),

					luabind::class_<SoundManager::Music, SuperSmartPointer<SoundManager::Music> >("Music")
						.def("Play", &SoundManager::Music::Play)
						.def("Stop", &SoundManager::Music::Stop)
						.def("Pause", &SoundManager::Music::Pause)
						.property("Volume", &SoundManager::Music::GetVolume, &SoundManager::Music::SetVolume)
						.property("Status", &SoundManager::Music::GetStatus)
						.property("Playing", &SoundManager::Music::IsPlaying)
						.property("Looping", &SoundManager::Music::IsLooping, &SoundManager::Music::SetLoop)
						.property("Pitch", &SoundManager::Music::GetPitch, &SoundManager::Music::SetPitch)
				]
				.def("GetMusic", &SoundManager::GetMusic)
				.def("GetSound", &SoundManager::GetSound)
				.def("SoundFromStream", &SoundManager::SoundFromStream)
				.def("SoundFromFile", &SoundManager::SoundFromFile)
				.def("SoundFromPackage", &SoundManager::SoundFromPackage)
				.def("MusicFromStream", &SoundManager::MusicFromStream)
				.def("MusicFromFile", &SoundManager::MusicFromFile)
				.def("MusicFromPackage", &SoundManager::MusicFromPackage),
#endif

#if USE_GRAPHICS
			//RenderCreateOptions
			luabind::class_<RenderCreateOptions>("RenderCreateOptions")
				.enum_("constants") [
					luabind::value("Style_Default", RendererWindowStyle::Default),
					luabind::value("Style_FullScreen", RendererWindowStyle::FullScreen),
					luabind::value("Style_Popup", RendererWindowStyle::Popup)
				]
				.def(luabind::constructor<>())
				.def_readwrite("Title", &RenderCreateOptions::Title)
				.def_readwrite("Width", &RenderCreateOptions::Width)
				.def_readwrite("Height", &RenderCreateOptions::Height)
				.def_readwrite("FrameRate", &RenderCreateOptions::FrameRate)
				.def_readwrite("Style", &RenderCreateOptions::Style),

			//Sprite
			luabind::class_<Sprite>("Sprite")
				.enum_("constants") [
					luabind::value("Pin_TopLeft", PinningMode::TopLeft),
					luabind::value("Pin_TopCenter", PinningMode::TopCenter),
					luabind::value("Pin_TopRight", PinningMode::TopRight),
					luabind::value("Pin_Left", PinningMode::Left),
					luabind::value("Pin_Middle", PinningMode::Middle),
					luabind::value("Pin_Right", PinningMode::Right),
					luabind::value("Pin_BottomLeft", PinningMode::BottomLeft),
					luabind::value("Pin_BottomCenter", PinningMode::BottomCenter),
					luabind::value("Pin_BottomRight", PinningMode::BottomRight),
					luabind::value("Crop_None", CropMode::None),
					luabind::value("Crop_Normalized", CropMode::CropNormalized),
					luabind::value("Crop_Tiled", CropMode::CropTiled),
					luabind::value("Blend_None", BlendingMode::None),
					luabind::value("Blend_Alpha", BlendingMode::Alpha),
					luabind::value("Blend_Additive", BlendingMode::Additive),
					luabind::value("Blend_Subtractive", BlendingMode::Subtractive)
				]
				.scope [
					luabind::class_<SpriteDrawOptions>("DrawOptions")
						.def_readwrite("PositionValue", &SpriteDrawOptions::PositionValue)
						.def_readwrite("ScaleValue", &SpriteDrawOptions::ScaleValue)
						.def_readwrite("OffsetValue", &SpriteDrawOptions::OffsetValue)
						.def_readwrite("RotationValue", &SpriteDrawOptions::RotationValue)
						.def_readwrite("PinningModeValue", &SpriteDrawOptions::PinningModeValue)
						.def_readwrite("CropModeValue", &SpriteDrawOptions::CropModeValue)
						.def_readwrite("CropRectValue", &SpriteDrawOptions::CropRectValue)
						.def_readwrite("ColorValue", &SpriteDrawOptions::ColorValue)
						.def_readwrite("BlendingModeValue", &SpriteDrawOptions::BlendingModeValue)
						.def_readwrite("NinePatchValue", &SpriteDrawOptions::NinePatchValue)
						.def_readwrite("NinePatchRectValue", &SpriteDrawOptions::NinePatchRectValue)
						.def_readwrite("FlipXValue", &SpriteDrawOptions::FlipX)
						.def_readwrite("FlipYValue", &SpriteDrawOptions::FlipY)
						.def_readwrite("TexCoordBorderMinValue", &SpriteDrawOptions::TexCoordBorderMin)
						.def_readwrite("TexCoordBorderMaxValue", &SpriteDrawOptions::TexCoordBorderMax)
						.def_readwrite("TexCoordRotation", &SpriteDrawOptions::TexCoordRotation)
						.def_readwrite("TexCoordPosition", &SpriteDrawOptions::TexCoordPosition)
						.def_readwrite("WireframeValue", &SpriteDrawOptions::WireframeValue)
						.def_readwrite("WireframePixelSizeValue", &SpriteDrawOptions::WireframePixelSizeValue)
						.property("ColorsValue", &GetSpriteColors, &SetSpriteColors)
						.def(luabind::constructor<>())
						.def(luabind::constructor<const SpriteDrawOptions &>())
						.def("Position", &SpriteDrawOptions::Position, luabind::return_reference_to(_1))
						.def("Scale", &SpriteDrawOptions::Scale, luabind::return_reference_to(_1))
						.def("Offset", &SpriteDrawOptions::Offset, luabind::return_reference_to(_1))
						.def("Rotation", &SpriteDrawOptions::Rotation, luabind::return_reference_to(_1))
						.def("Pin", &SpriteDrawOptions::Pin, luabind::return_reference_to(_1))
						.def("Crop", &SpriteDrawOptions::Crop, luabind::return_reference_to(_1))
						.def("Color", &SpriteDrawOptions::Color, luabind::return_reference_to(_1))
						.def("BlendingMode", &SpriteDrawOptions::BlendingMode, luabind::return_reference_to(_1))
						.def("NinePatch", &SpriteDrawOptions::NinePatch, luabind::return_reference_to(_1))
						.def("Flip", &SpriteDrawOptions::Flip, luabind::return_reference_to(_1))
						.def("TextureBorders", &SpriteDrawOptions::TextureBorders, luabind::return_reference_to(_1))
						.def("TextureRotation", &SpriteDrawOptions::TextureRotation, luabind::return_reference_to(_1))
						.def("TexturePosition", &SpriteDrawOptions::TexturePosition, luabind::return_reference_to(_1))
						.def("Wireframe", &SpriteDrawOptions::Wireframe, luabind::return_reference_to(_1))
						.def("WireframePixelSize", &SpriteDrawOptions::WireframePixelSize, luabind::return_reference_to(_1))
						.def("Colors", &SpriteDrawOptions::Colors, luabind::return_reference_to(_1))
				]
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Sprite &>())
				.def_readwrite("Options", &Sprite::Options)
				.def_readwrite("Texture", &Sprite::SpriteTexture)
				.def("Draw", &Sprite::Draw),

			//AnimatedSprite
			luabind::class_<AnimatedSprite, Sprite, SuperSmartPointer<AnimatedSprite> >("AnimatedSprite")
				.def_readwrite("FrameSize", &AnimatedSprite::FrameSize)
				.def_readwrite("DefaultFrame", &AnimatedSprite::DefaultFrame)
				.def_readwrite("Scale", &AnimatedSprite::Scale)
				.def_readwrite("FrameInterval", &AnimatedSprite::FrameInterval)
				.def(luabind::constructor<const Vector2 &, const Vector2 &>())
				.def("AddAnimation", &AnimatedSpriteAddAnimation)
				.def("SetAnimation", &AnimatedSprite::SetAnimation)
				.def("StopAnimation", &AnimatedSprite::StopAnimation)
				.def("Update", &AnimatedSprite::Update),

			//RendererManager
			luabind::class_<RendererManager, SubSystem>("RendererManager")
				.enum_("constants") [
					luabind::value("Clear_Color", RendererBuffer::Color),
					luabind::value("Clear_Stencil", RendererBuffer::Stencil),
					luabind::value("Clear_Depth", RendererBuffer::Depth)
				]
				.def_readonly("Input", &RendererManager::Input)
				.def("AddRenderer", (RendererHandle (RendererManager::*)(const char *, uint32, uint32, uint32)) &RendererManager::AddRenderer)
				.def("AddRenderer", (RendererHandle (RendererManager::*)(void *)) &RendererManager::AddRenderer)
				.def("DestroyRenderer", &RendererManager::DestroyRenderer)
				.def("SetActiveRenderer", &RendererManager::SetActiveRenderer)
				.property("ActiveRenderer", &RendererManager::ActiveRenderer)
				.def("RequestFrame", &RendererManager::RequestFrame),

			//Renderer
			luabind::class_<Renderer>("Renderer")
				.def_readonly("UI", &Renderer::UI)
				.property("Handle", &Renderer::Handle)
				.def("BindTexture", (void(Renderer::*)(Texture *))&Renderer::BindTexture)
				.def("BindTexture", (void(Renderer::*)(TextureHandle))&Renderer::BindTexture)
				.def_readwrite("Camera", &Renderer::RenderCamera)
				.property("Size", &Renderer::Size)
				.def("Clear", &Renderer::Clear),
				
			//RenderTextUtils
			luabind::class_<RenderTextUtils>("RenderTextUtils")
				.scope [
					luabind::class_<TextParams>("TextParams")
						.def_readwrite("TextColorValue", &TextParams::TextColorValue)
						.def_readwrite("SecondaryTextColorValue", &TextParams::SecondaryTextColorValue)
						.def_readwrite("BorderColorValue", &TextParams::BorderColorValue)
						.def_readwrite("PositionValue", &TextParams::PositionValue)
						.def_readwrite("BorderSizeValue", &TextParams::BorderSizeValue)
						.def_readwrite("FontSizeValue", &TextParams::FontSizeValue)
						.def_readwrite("FontValue", &TextParams::FontValue)
						.def_readwrite("StyleValue", &TextParams::StyleValue)
						.def_readwrite("RotationValue", &TextParams::RotationValue)
						.def(luabind::constructor<>())
						.def(luabind::constructor<const TextParams &>())
						.def("Style", &TextParams::Style, luabind::return_reference_to(_1))
						.def("Font", &TextParams::Font, luabind::return_reference_to(_1))
						.def("FontSize", &TextParams::FontSize, luabind::return_reference_to(_1))
						.def("Color", &TextParams::Color, luabind::return_reference_to(_1))
						.def("SecondaryColor", &TextParams::SecondaryColor, luabind::return_reference_to(_1))
						.def("BorderColor", &TextParams::BorderColor, luabind::return_reference_to(_1))
						.def("BorderSize", &TextParams::BorderSize, luabind::return_reference_to(_1))
						.def("Rotate", &TextParams::Rotate, luabind::return_reference_to(_1))
						.def("Position", &TextParams::Position, luabind::return_reference_to(_1)),

					luabind::def("LoadDefaultFont", &RenderTextUtils::LoadDefaultFont),
					luabind::def("MeasureTextSimple", &RenderTextMeasureTextSimple),
					luabind::def("FitTextAroundLength", &RenderTextFitTextAroundLength),
					luabind::def("RenderText", &RenderTextRenderText),
					luabind::def("FitTextOnRect", &RenderTextFitTextOnRect),
					luabind::def("MeasureTextLines", &RenderTextMeasureTextLines)
				],

			//InputCenter
			luabind::class_<InputCenter>("InputCenter")
				.enum_("constants")
				[
					REGISTER_KEY(A),
					REGISTER_KEY(B),
					REGISTER_KEY(C),
					REGISTER_KEY(D),
					REGISTER_KEY(E),
					REGISTER_KEY(F),
					REGISTER_KEY(G),
					REGISTER_KEY(H),
					REGISTER_KEY(I),
					REGISTER_KEY(J),
					REGISTER_KEY(K),
					REGISTER_KEY(L),
					REGISTER_KEY(M),
					REGISTER_KEY(N),
					REGISTER_KEY(O),
					REGISTER_KEY(P),
					REGISTER_KEY(Q),
					REGISTER_KEY(R),
					REGISTER_KEY(S),
					REGISTER_KEY(T),
					REGISTER_KEY(U),
					REGISTER_KEY(V),
					REGISTER_KEY(W),
					REGISTER_KEY(X),
					REGISTER_KEY(Y),
					REGISTER_KEY(Z),
					REGISTER_KEY(Num0),
					REGISTER_KEY(Num1),
					REGISTER_KEY(Num2),
					REGISTER_KEY(Num3),
					REGISTER_KEY(Num4),
					REGISTER_KEY(Num5),
					REGISTER_KEY(Num6),
					REGISTER_KEY(Num7),
					REGISTER_KEY(Num8),
					REGISTER_KEY(Num9),
					REGISTER_KEY(Escape),
					REGISTER_KEY(LControl),
					REGISTER_KEY(LShift),
					REGISTER_KEY(LAlt),
					REGISTER_KEY(LSystem),
					REGISTER_KEY(RControl),
					REGISTER_KEY(RShift),
					REGISTER_KEY(RAlt),
					REGISTER_KEY(RSystem),
					REGISTER_KEY(Menu),
					REGISTER_KEY(LBracket),
					REGISTER_KEY(RBracket),
					REGISTER_KEY(SemiColon),
					REGISTER_KEY(Comma),
					REGISTER_KEY(Period),
					REGISTER_KEY(Quote),
					REGISTER_KEY(Slash),
					REGISTER_KEY(BackSlash),
					REGISTER_KEY(Tilde),
					REGISTER_KEY(Equal),
					REGISTER_KEY(Dash),
					REGISTER_KEY(Space),
					REGISTER_KEY(Return),
					REGISTER_KEY(BackSpace),
					REGISTER_KEY(Tab),
					REGISTER_KEY(PageUp),
					REGISTER_KEY(PageDown),
					REGISTER_KEY(End),
					REGISTER_KEY(Home),
					REGISTER_KEY(Insert),
					REGISTER_KEY(Delete),
					REGISTER_KEY(Add),
					REGISTER_KEY(Subtract),
					REGISTER_KEY(Multiply),
					REGISTER_KEY(Divide),
					REGISTER_KEY(Left),
					REGISTER_KEY(Right),
					REGISTER_KEY(Up),
					REGISTER_KEY(Down),
					REGISTER_KEY(Numpad0),
					REGISTER_KEY(Numpad1),
					REGISTER_KEY(Numpad2),
					REGISTER_KEY(Numpad3),
					REGISTER_KEY(Numpad4),
					REGISTER_KEY(Numpad5),
					REGISTER_KEY(Numpad6),
					REGISTER_KEY(Numpad7),
					REGISTER_KEY(Numpad8),
					REGISTER_KEY(Numpad9),
					REGISTER_KEY(F1),
					REGISTER_KEY(F2),
					REGISTER_KEY(F3),
					REGISTER_KEY(F4),
					REGISTER_KEY(F5),
					REGISTER_KEY(F6),
					REGISTER_KEY(F7),
					REGISTER_KEY(F8),
					REGISTER_KEY(F9),
					REGISTER_KEY(F10),
					REGISTER_KEY(F11),
					REGISTER_KEY(F12),
					REGISTER_KEY(F13),
					REGISTER_KEY(F14),
					REGISTER_KEY(F15),
					REGISTER_KEY(Pause),

					REGISTER_MOUSEBUTTON(Left),
					REGISTER_MOUSEBUTTON(Middle),
					REGISTER_MOUSEBUTTON(Right),
					REGISTER_MOUSEBUTTON(XButton1),
					REGISTER_MOUSEBUTTON(XButton2),

					REGISTER_JOYSTICKAXIS(X),
					REGISTER_JOYSTICKAXIS(Y),
					REGISTER_JOYSTICKAXIS(Z),
					REGISTER_JOYSTICKAXIS(R),
					REGISTER_JOYSTICKAXIS(U),
					REGISTER_JOYSTICKAXIS(V),
					REGISTER_JOYSTICKAXIS(PovX),
					REGISTER_JOYSTICKAXIS(PovY),

					REGISTER_JOYSTICKBUTTON(0),
					REGISTER_JOYSTICKBUTTON(1),
					REGISTER_JOYSTICKBUTTON(2),
					REGISTER_JOYSTICKBUTTON(3),
					REGISTER_JOYSTICKBUTTON(4),
					REGISTER_JOYSTICKBUTTON(5),
					REGISTER_JOYSTICKBUTTON(6),
					REGISTER_JOYSTICKBUTTON(7),
					REGISTER_JOYSTICKBUTTON(8),
					REGISTER_JOYSTICKBUTTON(9),
					REGISTER_JOYSTICKBUTTON(10),
					REGISTER_JOYSTICKBUTTON(11),
					REGISTER_JOYSTICKBUTTON(12),
					REGISTER_JOYSTICKBUTTON(13),
					REGISTER_JOYSTICKBUTTON(14),
					REGISTER_JOYSTICKBUTTON(15),
					REGISTER_JOYSTICKBUTTON(16),
					REGISTER_JOYSTICKBUTTON(17),
					REGISTER_JOYSTICKBUTTON(18),
					REGISTER_JOYSTICKBUTTON(19),
					REGISTER_JOYSTICKBUTTON(20),
					REGISTER_JOYSTICKBUTTON(21),
					REGISTER_JOYSTICKBUTTON(22),
					REGISTER_JOYSTICKBUTTON(23),
					REGISTER_JOYSTICKBUTTON(24),
					REGISTER_JOYSTICKBUTTON(25),
					REGISTER_JOYSTICKBUTTON(26),
					REGISTER_JOYSTICKBUTTON(27),
					REGISTER_JOYSTICKBUTTON(28),
					REGISTER_JOYSTICKBUTTON(29),
					REGISTER_JOYSTICKBUTTON(30),
					REGISTER_JOYSTICKBUTTON(31)
				]
				.scope[
					luabind::class_<InputCenter::MouseButtonInfo>("MouseButtonInfo")
						.def_readonly("Name", &InputCenter::MouseButtonInfo::Name)
						.def_readonly("Pressed", &InputCenter::MouseButtonInfo::Pressed)
						.def_readonly("JustPressed", &InputCenter::MouseButtonInfo::JustPressed)
						.def_readonly("JustReleased", &InputCenter::MouseButtonInfo::JustReleased)
						.def_readonly("FirstPress", &InputCenter::MouseButtonInfo::FirstPress)
						.def("NameAsString", &InputCenter::MouseButtonInfo::NameAsString),

					luabind::class_<InputCenter::JoystickButtonInfo>("JoystickButtonInfo")
						.def_readonly("Name", &InputCenter::JoystickButtonInfo::Name)
						.def_readonly("JoystickIndex", &InputCenter::JoystickButtonInfo::JoystickIndex)
						.def_readonly("Pressed", &InputCenter::JoystickButtonInfo::Pressed)
						.def_readonly("JustPressed", &InputCenter::JoystickButtonInfo::JustPressed)
						.def_readonly("JustReleased", &InputCenter::JoystickButtonInfo::JustReleased)
						.def_readonly("FirstPress", &InputCenter::JoystickButtonInfo::FirstPress)
						.def("NameAsString", &InputCenter::JoystickButtonInfo::NameAsString),

					luabind::class_<InputCenter::JoystickAxisInfo>("JoystickAxisInfo")
						.def_readonly("Name", &InputCenter::JoystickAxisInfo::Name)
						.def_readonly("JoystickIndex", &InputCenter::JoystickAxisInfo::JoystickIndex)
						.def_readonly("Position", &InputCenter::JoystickAxisInfo::Position)
						.def("NameAsString", &InputCenter::JoystickAxisInfo::NameAsString),

					luabind::class_<InputCenter::KeyInfo>("KeyInfo")
						.def_readonly("Name", &InputCenter::KeyInfo::Name)
						.def_readonly("Alt", &InputCenter::KeyInfo::Alt)
						.def_readonly("Control", &InputCenter::KeyInfo::Control)
						.def_readonly("Shift", &InputCenter::KeyInfo::Shift)
						.def_readonly("Pressed", &InputCenter::KeyInfo::Pressed)
						.def_readonly("JustPressed", &InputCenter::KeyInfo::JustPressed)
						.def_readonly("JustReleased", &InputCenter::KeyInfo::JustReleased)
						.def_readonly("FirstPress", &InputCenter::KeyInfo::FirstPress)
						.def("NameAsString", &InputCenter::KeyInfo::NameAsString),

					luabind::class_<InputCenter::Action>("Action")
						.property("Name", &InputCenter::Action::Name)
						.property("Type", &InputCenter::Action::Type)
						.property("Index", &InputCenter::Action::Index)
						.property("SecondaryIndex", &InputCenter::Action::SecondaryIndex)
						.property("PositiveValues", &InputCenter::Action::PositiveValues)
						.enum_("constants") [
							luabind::value("InputAction_Keyboard", InputActionType::Keyboard),
							luabind::value("InputAction_MouseButton", InputActionType::MouseButton),
							luabind::value("InputAction_MouseScroll", InputActionType::MouseScroll),
							luabind::value("InputAction_JoystickButton", InputActionType::JoystickButton),
							luabind::value("InputAction_JoystickAxis", InputActionType::JoystickAxis)
						]
						.def("Key", &InputCenter::Action::Key)
						.def("MouseButton", &InputCenter::Action::MouseButton)
						.def("JoystickButton", &InputCenter::Action::JoystickButton)
						.def("JoystickAxis", &InputCenter::Action::JoystickAxis)
						.def("AsString", &InputCenter::Action::AsString),

					luabind::class_<InputCenter::Context, SuperSmartPointer<InputCenter::Context> >("InputContext")
						.def_readwrite("Name", &InputCenter::Context::Name)
						.def("OnKey", &InputCenter::Context::OnKey)
						.def("OnMouseButton", &InputCenter::Context::OnMouseButton)
						.def("OnJoystickButton", &InputCenter::Context::OnJoystickButton)
						.def("OnJoystickAxis", &InputCenter::Context::OnJoystickAxis)
						.def("OnJoystickConnected", &InputCenter::Context::OnJoystickConnected)
						.def("OnJoystickDisconnected", &InputCenter::Context::OnJoystickDisconnected)
						.def("OnMouseMove", &InputCenter::Context::OnMouseMove)
						.def("OnCharacterEntered", &InputCenter::Context::OnCharacterEntered)
						.def("OnAction", &InputCenter::Context::OnAction)
						.def("OnGainFocus", &InputCenter::Context::OnGainFocus)
						.def("OnLoseFocus", &InputCenter::Context::OnLoseFocus),

					luabind::class_<InputCenter::ScriptedContext, InputCenter::Context, SuperSmartPointer<InputCenter::Context> >("ScriptedInputContext")
						.def(luabind::constructor<const std::string &>())
						.def_readwrite("OnKey", &InputCenter::ScriptedContext::OnKeyFunction)
						.def_readwrite("OnMouseButton", &InputCenter::ScriptedContext::OnMouseButtonFunction)
						.def_readwrite("OnJoystickButton", &InputCenter::ScriptedContext::OnJoystickButtonFunction)
						.def_readwrite("OnJoystickAxis", &InputCenter::ScriptedContext::OnJoystickAxisFunction)
						.def_readwrite("OnJoystickConnected", &InputCenter::ScriptedContext::OnJoystickConnectedFunction)
						.def_readwrite("OnJoystickDisconnected", &InputCenter::ScriptedContext::OnJoystickDisconnectedFunction)
						.def_readwrite("OnMouseMove", &InputCenter::ScriptedContext::OnMouseMoveFunction)
						.def_readwrite("OnCharacterEntered", &InputCenter::ScriptedContext::OnCharacterEnteredFunction)
						.def_readwrite("OnAction", &InputCenter::ScriptedContext::OnActionFunction)
						.def_readwrite("OnGainFocus", &InputCenter::ScriptedContext::OnGainFocusFunction)
						.def_readwrite("OnLoseFocus", &InputCenter::ScriptedContext::OnLoseFocusFunction)
				]
				.def("GetKey", &GetInputCenterKey)
				.def("GetMouseButton", &GetInputCenterMouseButton)
				.def("GetJoystickButton", &GetInputCenterJoystickButton)
				.def("GetJoystickAxis", &GetInputCenterJoystickAxis)
				.def("GetAction", &InputCenter::GetAction)
				.def_readonly("HasFocus", &InputCenter::HasFocus)
				.def_readonly("MousePosition", &InputCenter::MousePosition)
				.def("InputConsumed", &InputCenter::InputConsumed)
				.def("ConsumeInput", &InputCenter::ConsumeInput)
				.def("RegisterAction", &InputCenter::RegisterAction)
				.def("UnregisterAction", &InputCenter::UnregisterAction)
				.def("GetContext", &InputCenter::GetContext)
				.def("AddContext", &InputCenter::AddContext)
				.def("EnableContext", &InputCenter::EnableContext)
				.def("DisableContext", &InputCenter::DisableContext),

			//UIManager
			luabind::class_<UIManager>("UIManager")
				.def("AddElement", &UIManager::AddElement)
				.def("RemoveElement", &UIManager::RemoveElement)
				.def("LoadLayouts", &UIManager::LoadLayouts)
				.property("Layouts", &GetUIManagerLayouts)
				.def("AddLayout", &AddUIManagerLayout)
				.def("RemoveLayout", &RemoveUIManagerLayout)
				.def("ClearLayouts", &UIManager::ClearLayouts)
				.def("Clear", &UIManager::Clear)
				.def("ClearFocus", &UIManager::ClearFocus)
				.def("GetElement", &UIManager::GetElement)
				.property("FocusedElement", &UIManager::GetFocusedElement)
				.property("MouseOverElement", &UIManager::GetMouseOverElement)
				.property("Tooltip", &UIManager::GetTooltip)
				.property("DefaultFontColor", &UIManager::GetDefaultFontColor)
				.property("DefaultSecondaryFontColor", &UIManager::GetDefaultSecondaryFontColor)
				.property("DefaultFontSize", &UIManager::GetDefaultFontSize)
				.property("Skin", &UIManager::GetSkin, &UIManager::SetSkin),

			//UIPanel
			luabind::class_<UIPanel, SuperSmartPointer<UIPanel> >("UIPanel")
				.def_readwrite("OnMouseJustPressed", &UIPanel::OnMouseJustPressedFunction)
				.def_readwrite("OnMousePressed", &UIPanel::OnMousePressedFunction)
				.def_readwrite("OnMouseReleased", &UIPanel::OnMouseReleasedFunction)
				.def_readwrite("OnKeyJustPressed", &UIPanel::OnKeyJustPressedFunction)
				.def_readwrite("OnKeyPressed", &UIPanel::OnKeyPressedFunction)
				.def_readwrite("OnKeyReleased", &UIPanel::OnKeyReleasedFunction)
				.def_readwrite("OnMouseMoved", &UIPanel::OnMouseMovedFunction)
				.def_readwrite("OnCharacterEntered", &UIPanel::OnCharacterEnteredFunction)
				.def_readwrite("OnGainFocus", &UIPanel::OnGainFocusFunction)
				.def_readwrite("OnLoseFocus", &UIPanel::OnLoseFocusFunction)
				.def_readwrite("OnUpdate", &UIPanel::OnUpdateFunction)
				.def_readwrite("OnDraw", &UIPanel::OnDrawFunction)
				.def_readwrite("OnMouseEntered", &UIPanel::OnMouseEnteredFunction)
				.def_readwrite("OnMouseOver", &UIPanel::OnMouseOverFunction)
				.def_readwrite("OnMouseLeft", &UIPanel::OnMouseLeftFunction)
				.def_readwrite("OnClick", &UIPanel::OnClickFunction)
				.def_readwrite("OnDragBegin", &UIPanel::OnDragBeginFunction)
				.def_readwrite("OnDragEnd", &UIPanel::OnDragEndFunction)
				.def_readwrite("OnDragging", &UIPanel::OnDraggingFunction)
				.def_readwrite("OnDrop", &UIPanel::OnDropFunction)
				.def_readwrite("OnStart", &UIPanel::OnStartFunction)
				.def_readwrite("Properties", &UIPanel::Properties)
				.property("RespondsToTooltips", &UIPanel::RespondsToTooltips, &UIPanel::SetRespondsToTooltips)
				.property("TooltipsFixed", &UIPanel::TooltipsFixed, &UIPanel::SetTooltipsFixed)
				.property("TooltipsPosition", &UIPanel::TooltipsPosition, &UIPanel::SetTooltipsPosition)
				.def("PerformLayout", &UIPanel::PerformLayout)
				.property("TooltipText", &UIPanel::TooltipText, &UIPanel::SetTooltipText)
				.property("TooltipElement", &UIPanel::TooltipElement, &UIPanel::SetTooltipElement)
				.property("BlockingInput", &UIPanel::BlockingInput, &UIPanel::SetBlockingInput)
				.property("Draggable", &UIPanel::Draggable, &UIPanel::SetDraggable)
				.property("Droppable", &UIPanel::Droppable, &UIPanel::SetDroppable)
				.property("Owner", &UIPanel::Manager)
				.property("ID", &UIPanel::ID)
				.property("ParentPosition", &UIPanel::ParentPosition)
				.property("Visible", &UIPanel::Visible, &UIPanel::SetVisible)
				.property("Enabled", &UIPanel::Enabled, &UIPanel::SetEnabled)
				.property("MouseInputEnabled", &UIPanel::MouseInputEnabled, &UIPanel::SetMouseInputEnabled)
				.property("KeyboardInputEnabled", &UIPanel::KeyboardInputEnabled, &UIPanel::SetKeyboardInputEnabled)
				.def("AddChild", &UIPanel::AddChild)
				.def("RemoveChild", &UIPanel::RemoveChild)
				.def_readonly("ControlName", &UIPanel::NativeType)
				.property("Children", &GetUIPanelChildren)
				.def("ChildrenSize", &UIPanel::ChildrenSize)
				.property("Translation", &UIPanel::Translation)
				.property("Parent", &UIPanel::Parent)
				.property("Position", &UIPanel::Position, &UIPanel::SetPosition)
				.property("Offset", &UIPanel::Offset, &UIPanel::SetOffset)
				.property("Size", &UIPanel::Size, &UIPanel::SetSize)
				.property("ComposedSize", &UIPanel::ComposedSize)
				.property("Opacity", &UIPanel::Alpha, &UIPanel::SetAlpha)
				.property("Layout", &UIPanel::Layout)
				.property("ExtraSize", &UIPanel::ExtraSize)
				.property("ScaledExtraSize", &UIPanel::ScaledExtraSize)
				.property("Rotation", &UIPanel::Rotation, &UIPanel::SetRotation)
				.def("ClearAnimations", &UIPanel::ClearAnimations)
				.def("FadeIn", &UIPanel::FadeIn)
				.def("FadeOut", &UIPanel::FadeOut)
				.def("Focus", &UIPanel::Focus)
				.def("Clear", &UIPanel::Clear),

			//UIGroup
			luabind::class_<UIGroup, UIPanel>("UIGroup")
				.enum_("constants") [
					luabind::value("LayoutMode_None", UIGroupLayoutMode::None),
					luabind::value("LayoutMode_Horizontal", UIGroupLayoutMode::Horizontal),
					luabind::value("LayoutMode_Vertical", UIGroupLayoutMode::Vertical)
				]
				.def_readwrite("LayoutMode", &UIGroup::LayoutMode),
			//UILayout
			luabind::class_<UILayout, SuperSmartPointer<UILayout> >("UILayout")
				.def("Clone", &UILayout::Clone)
				.def("FindPanelById", &UILayout::FindPanelById)
				.def("FindPanelByName", &UILayout::FindPanelByName)
				.def_readwrite("Name", &UILayout::Name)
				.property("Elements", &GetUILayoutElements),

			//UISprite
			luabind::class_<UISprite, UIPanel>("UISprite")
				.def_readwrite("Sprite", &UISprite::TheSprite),

				//UIText
			luabind::class_<UIText, UIPanel>("UIText")
				.property("Text", &UITextGetText, &UITextSetText)
				.def_readwrite("TextParameters", &UIText::TextParameters),

			//UITextComposer
			luabind::class_<UITextComposer, UIPanel>("UITextComposer"),
			//UITooltip
			luabind::class_<UITooltip, UIPanel>("UITooltip"),
#endif
			
			luabind::def("MakeStringID", &MakeStringID),
			luabind::def("GetStringIDString", &GetStringIDString),
			luabind::def("BitwiseSet", &BitwiseSet),
			luabind::def("BitwiseCheck", &BitwiseCheck),
			luabind::def("BitwiseRemove", &BitwiseRemove),
			luabind::def("DoStream", &LuaDoStream)
		];

		luabind::object Globals = luabind::globals(State);

		Globals["ResourceManager"]["InvalidTexture"] = ResourceManager::InvalidTexture;

		Globals["g_CRC"] = &CRC32::Instance;
		Globals["g_World"] = &World::Instance;
		Globals["g_Clock"] = &GameClock::Instance;
		Globals["g_Log"] = &Log::Instance;
		Globals["g_FPSCounter"] = &FPSCounter::Instance;
		//TODO: Texture KeepData
		Globals["g_LuaScriptManager"] = &LuaScriptManager::Instance;
		Globals["g_GameInterface"] = GameInterface::Instance.Get();
		Globals["Game"] = GameInterface::Instance.Get();

		Globals["g_FileSystemWatcher"] = &FileSystemWatcher::Instance;

		Globals["g_PackageManager"] = &PackageFileSystemManager::Instance;

#if USE_NETWORK
		Globals["g_GameNetwork"] = &GameNetwork::Instance;
#endif

		Globals["g_ResourceManager"] = &ResourceManager::Instance;

#if USE_SOUND
		Globals["g_SoundManager"] = &SoundManager::Instance;
#endif

#if USE_GRAPHICS
		Globals["g_RendererManager"] = &RendererManager::Instance;
		Globals["g_Input"] = &RendererManager::Instance.Input;
#endif

		Globals["g_Console"] = &Console::Instance;

		return true;
	};
};

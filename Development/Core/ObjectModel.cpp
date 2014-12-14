#include "FlamingCore.hpp"
namespace FlamingTorch
{
#define TAG "ObjectModel"

#define REGISTER_TYPE(type)\
	luabind::def("Is" #type, &IsLuaType<type>)

#define REGISTER_TYPE_PTR(type)\
	luabind::def("Is" #type "Ptr", &IsLuaTypePtr<type>)

#define REGISTER_NAMESPACETYPE(nmspc, type)\
	luabind::def("Is" #type, &IsLuaType<nmspc::type>)

#define REGISTER_NAMESPACETYPE_PTR(nmspc, type)\
	luabind::def("Is" #type "Ptr", &IsLuaTypePtr<nmspc::type>)

#define REGISTER_NAMESPACETYPE_NAMED(nmspc, type, name)\
	luabind::def("Is" name, &IsLuaType<nmspc::type>)

#define REGISTER_NAMESPACETYPE_NAMED_PTR(nmspc, type, name)\
	luabind::def("Is" name "Ptr", &IsLuaTypePtr<nmspc::type>)

#define REGISTER_TYPE_ENUM(type)\
	luabind::value(#type, ObjectAttributeType::type)

#define REGISTER_TYPE_ENUM_NAME(type, name)\
	luabind::value(name, ObjectAttributeType::type)

#define REGISTER_TYPE_ENUMPTR(type)\
	luabind::value(#type "Ptr", ObjectAttributeType::type ## Ptr)

#define REGISTER_TYPE_ENUMPTR_NAME(type, name)\
	luabind::value(name "Ptr", ObjectAttributeType::type ## Ptr)

	ObjectModelManager ObjectModelManager::Instance;

	luabind::object GetObjectDefTags(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<std::string>(Self.Tags);
	};

	void SetObjectDefTags(ObjectDef &Self, luabind::object Value)
	{
		Self.Tags = GameInterface::Instance->GetScriptInstance()->VectorFromLua<std::string>(Value);
	};

	luabind::object GetObjectDefLayers(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<std::string>(Self.Layers);
	};

	void SetObjectDefLayers(ObjectDef &Self, luabind::object Value)
	{
		Self.Layers = GameInterface::Instance->GetScriptInstance()->VectorFromLua<std::string>(Value);
	};

	luabind::object GetObjectDefGroups(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<std::string>(Self.Groups);
	};

	void SetObjectDefGroups(ObjectDef &Self, luabind::object Value)
	{
		Self.Groups = GameInterface::Instance->GetScriptInstance()->VectorFromLua<std::string>(Value);
	};
	
	luabind::object GetObjectDefAttributes(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<DisposablePointer<ObjectAttribute> >(Self.Attributes);
	};

	void AddObjectDefAttribute(ObjectDef &Self, DisposablePointer<ObjectAttribute> Attribute)
	{
		for (std::vector<DisposablePointer<ObjectAttribute> >::iterator it = Self.Attributes.begin(); it != Self.Attributes.end(); it++)
		{
			if (*it == Attribute)
				return;
		};

		Self.Attributes.push_back(Attribute);
	};

	luabind::object GetObjectAttributeValue(ObjectAttribute &Self)
	{
		if (!Self.Get)
			return luabind::object();

		try
		{
			return Self.Get(Self);
		}
		catch (luabind::error &)
		{
		};

		return luabind::object();
	};

	void SetObjectAttributeValue(ObjectAttribute &Self, luabind::object Value)
	{
		if (!Self.Set)
			return;

		try
		{
			Self.Set(Self, Value);
		}
		catch (luabind::error &)
		{
		};
	};

	bool ObjectModelManager::RegisterBindings(lua_State *State)
	{
		luabind::module(State)[
			luabind::class_<ObjectModelManager>("ObjectModelManager")
				.scope [
					REGISTER_TYPE(int8),
					REGISTER_TYPE(uint8),
					REGISTER_TYPE(int16),
					REGISTER_TYPE(uint16),
					REGISTER_TYPE(int32),
					REGISTER_TYPE(uint32),
					REGISTER_TYPE(int64),
					REGISTER_TYPE(uint64),
					REGISTER_TYPE(f32),
					REGISTER_TYPE(f64),
					REGISTER_TYPE(Path),
					REGISTER_NAMESPACETYPE(std, string),
					REGISTER_TYPE(Camera),
					REGISTER_TYPE(TextureBuffer),
					REGISTER_TYPE(TextureEncoderInfo),
					REGISTER_TYPE(TexturePacker),
					REGISTER_TYPE(TextureGroup),
					REGISTER_TYPE(LinearTimer),
					REGISTER_TYPE(GenericConfig),
					REGISTER_TYPE(LuaEventGroup),
					REGISTER_TYPE(Matrix4x4),
					REGISTER_TYPE(Vector2),
					REGISTER_TYPE(Vector3),
					REGISTER_TYPE(Vector4),
					REGISTER_TYPE(Plane),
					REGISTER_TYPE(Rect),
					REGISTER_TYPE(RotateableRect),
					REGISTER_TYPE(Ray),
					REGISTER_TYPE(AxisAlignedBoundingBox),
					REGISTER_TYPE(BoundingSphere),
					REGISTER_TYPE(Quaternion),
					REGISTER_TYPE(TiledMap),
					REGISTER_TYPE(ConsoleVariable),
					REGISTER_TYPE(RendererDisplayMode),
					REGISTER_TYPE(RendererCapabilities),
					REGISTER_TYPE(RenderCreateOptions),
					REGISTER_TYPE(Sprite),
					REGISTER_TYPE(RendererFrameStats),
					REGISTER_TYPE(TextParams),
					REGISTER_NAMESPACETYPE(InputCenter, MouseButtonInfo),
					REGISTER_NAMESPACETYPE(InputCenter, KeyInfo),
					REGISTER_NAMESPACETYPE(InputCenter, JoystickAxisInfo),
					REGISTER_NAMESPACETYPE(InputCenter, JoystickButtonInfo),
					REGISTER_NAMESPACETYPE(InputCenter, Action),
					REGISTER_NAMESPACETYPE(InputCenter, TouchInfo),
					REGISTER_TYPE_PTR(int8),
					REGISTER_TYPE_PTR(uint8),
					REGISTER_TYPE_PTR(int16),
					REGISTER_TYPE_PTR(uint16),
					REGISTER_TYPE_PTR(int32),
					REGISTER_TYPE_PTR(uint32),
					REGISTER_TYPE_PTR(int64),
					REGISTER_TYPE_PTR(uint64),
					REGISTER_TYPE_PTR(f32),
					REGISTER_TYPE_PTR(f64),
					REGISTER_TYPE_PTR(Path),
					REGISTER_TYPE_PTR(Stream),
					REGISTER_NAMESPACETYPE_PTR(std, string),
					REGISTER_TYPE_PTR(Camera),
					REGISTER_TYPE_PTR(TextureBuffer),
					REGISTER_TYPE_PTR(TextureEncoderInfo),
					REGISTER_TYPE_PTR(Texture),
					REGISTER_TYPE_PTR(TexturePacker),
					REGISTER_TYPE_PTR(TextureGroup),
					REGISTER_TYPE_PTR(LinearTimer),
					REGISTER_TYPE_PTR(GenericConfig),
					REGISTER_TYPE_PTR(GameInterface),
					REGISTER_TYPE_PTR(LuaEventGroup),
					REGISTER_TYPE_PTR(Matrix4x4),
					REGISTER_TYPE_PTR(Vector2),
					REGISTER_TYPE_PTR(Vector3),
					REGISTER_TYPE_PTR(Vector4),
					REGISTER_TYPE_PTR(Plane),
					REGISTER_TYPE_PTR(Rect),
					REGISTER_TYPE_PTR(RotateableRect),
					REGISTER_TYPE_PTR(Ray),
					REGISTER_TYPE_PTR(AxisAlignedBoundingBox),
					REGISTER_TYPE_PTR(BoundingSphere),
					REGISTER_TYPE_PTR(Quaternion),
					REGISTER_TYPE_PTR(TiledMap),
					REGISTER_TYPE_PTR(ConsoleVariable),
					REGISTER_NAMESPACETYPE_PTR(SoundManager, Sound),
					REGISTER_NAMESPACETYPE_PTR(SoundManager, Music),
					REGISTER_TYPE_PTR(RendererDisplayMode),
					REGISTER_TYPE_PTR(RendererCapabilities),
					REGISTER_TYPE_PTR(RenderCreateOptions),
					REGISTER_TYPE_PTR(Sprite),
					REGISTER_TYPE_PTR(RendererFrameStats),
					REGISTER_TYPE_PTR(Renderer),
					REGISTER_TYPE_PTR(TextParams),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, MouseButtonInfo),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, KeyInfo),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, JoystickAxisInfo),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, JoystickButtonInfo),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, Action),
					REGISTER_NAMESPACETYPE_PTR(InputCenter, TouchInfo),
					REGISTER_NAMESPACETYPE_NAMED_PTR(InputCenter, Context, "InputContext"),
					REGISTER_TYPE_PTR(UIElement),
					REGISTER_TYPE_PTR(UIGroup),
					REGISTER_TYPE_PTR(UIText),
					REGISTER_TYPE_PTR(UISprite)
				]
				.enum_("constants")
				[
					REGISTER_TYPE_ENUM(Int8),
					REGISTER_TYPE_ENUM(UInt8),
					REGISTER_TYPE_ENUM(Int16),
					REGISTER_TYPE_ENUM(UInt16),
					REGISTER_TYPE_ENUM(Int32),
					REGISTER_TYPE_ENUM(UInt32),
					REGISTER_TYPE_ENUM(Int64),
					REGISTER_TYPE_ENUM(UInt64),
					REGISTER_TYPE_ENUM(F32),
					REGISTER_TYPE_ENUM(F64),
					REGISTER_TYPE_ENUM(Path),
					REGISTER_TYPE_ENUM(String),
					REGISTER_TYPE_ENUM(Camera),
					REGISTER_TYPE_ENUM(TextureBuffer),
					REGISTER_TYPE_ENUM(TextureEncoderInfo),
					REGISTER_TYPE_ENUM(TexturePacker),
					REGISTER_TYPE_ENUM(TextureGroup),
					REGISTER_TYPE_ENUM(LinearTimer),
					REGISTER_TYPE_ENUM(GenericConfig),
					REGISTER_TYPE_ENUM(LuaEventGroup),
					REGISTER_TYPE_ENUM(Matrix4x4),
					REGISTER_TYPE_ENUM(Vector2),
					REGISTER_TYPE_ENUM(Vector3),
					REGISTER_TYPE_ENUM(Vector4),
					REGISTER_TYPE_ENUM(Plane),
					REGISTER_TYPE_ENUM(Rect),
					REGISTER_TYPE_ENUM(RotateableRect),
					REGISTER_TYPE_ENUM(Ray),
					REGISTER_TYPE_ENUM(AxisAlignedBoundingBox),
					REGISTER_TYPE_ENUM(BoundingSphere),
					REGISTER_TYPE_ENUM(Quaternion),
					REGISTER_TYPE_ENUM(TiledMap),
					REGISTER_TYPE_ENUM(ConsoleVariable),
					REGISTER_TYPE_ENUM(RendererDisplayMode),
					REGISTER_TYPE_ENUM(RendererCapabilities),
					REGISTER_TYPE_ENUM(RenderCreateOptions),
					REGISTER_TYPE_ENUM(Sprite),
					REGISTER_TYPE_ENUM(RendererFrameStats),
					REGISTER_TYPE_ENUM(TextParams),
					REGISTER_TYPE_ENUM(MouseButtonInfo),
					REGISTER_TYPE_ENUM(KeyInfo),
					REGISTER_TYPE_ENUM(JoystickAxisInfo),
					REGISTER_TYPE_ENUM(JoystickButtonInfo),
					REGISTER_TYPE_ENUM(Action),
					REGISTER_TYPE_ENUM(TouchInfo),
					REGISTER_TYPE_ENUMPTR(Int8),
					REGISTER_TYPE_ENUMPTR(UInt8),
					REGISTER_TYPE_ENUMPTR(Int16),
					REGISTER_TYPE_ENUMPTR(UInt16),
					REGISTER_TYPE_ENUMPTR(Int32),
					REGISTER_TYPE_ENUMPTR(UInt32),
					REGISTER_TYPE_ENUMPTR(Int64),
					REGISTER_TYPE_ENUMPTR(UInt64),
					REGISTER_TYPE_ENUMPTR(F32),
					REGISTER_TYPE_ENUMPTR(F64),
					REGISTER_TYPE_ENUMPTR(Path),
					REGISTER_TYPE_ENUMPTR(Stream),
					REGISTER_TYPE_ENUMPTR(String),
					REGISTER_TYPE_ENUMPTR(Camera),
					REGISTER_TYPE_ENUMPTR(TextureBuffer),
					REGISTER_TYPE_ENUMPTR(TextureEncoderInfo),
					REGISTER_TYPE_ENUMPTR(Texture),
					REGISTER_TYPE_ENUMPTR(TexturePacker),
					REGISTER_TYPE_ENUMPTR(TextureGroup),
					REGISTER_TYPE_ENUMPTR(LinearTimer),
					REGISTER_TYPE_ENUMPTR(GenericConfig),
					REGISTER_TYPE_ENUMPTR(GameInterface),
					REGISTER_TYPE_ENUMPTR(LuaEventGroup),
					REGISTER_TYPE_ENUMPTR(Matrix4x4),
					REGISTER_TYPE_ENUMPTR(Vector2),
					REGISTER_TYPE_ENUMPTR(Vector3),
					REGISTER_TYPE_ENUMPTR(Vector4),
					REGISTER_TYPE_ENUMPTR(Plane),
					REGISTER_TYPE_ENUMPTR(Rect),
					REGISTER_TYPE_ENUMPTR(RotateableRect),
					REGISTER_TYPE_ENUMPTR(Ray),
					REGISTER_TYPE_ENUMPTR(AxisAlignedBoundingBox),
					REGISTER_TYPE_ENUMPTR(BoundingSphere),
					REGISTER_TYPE_ENUMPTR(Quaternion),
					REGISTER_TYPE_ENUMPTR(TiledMap),
					REGISTER_TYPE_ENUMPTR(ConsoleVariable),
					REGISTER_TYPE_ENUMPTR(Sound),
					REGISTER_TYPE_ENUMPTR(Music),
					REGISTER_TYPE_ENUMPTR(RendererDisplayMode),
					REGISTER_TYPE_ENUMPTR(RendererCapabilities),
					REGISTER_TYPE_ENUMPTR(RenderCreateOptions),
					REGISTER_TYPE_ENUMPTR(Sprite),
					REGISTER_TYPE_ENUMPTR(RendererFrameStats),
					REGISTER_TYPE_ENUMPTR(Renderer),
					REGISTER_TYPE_ENUMPTR(TextParams),
					REGISTER_TYPE_ENUMPTR(MouseButtonInfo),
					REGISTER_TYPE_ENUMPTR(KeyInfo),
					REGISTER_TYPE_ENUMPTR(JoystickAxisInfo),
					REGISTER_TYPE_ENUMPTR(JoystickButtonInfo),
					REGISTER_TYPE_ENUMPTR(Action),
					REGISTER_TYPE_ENUMPTR(TouchInfo),
					REGISTER_TYPE_ENUMPTR(InputContext),
					REGISTER_TYPE_ENUMPTR(UIElement),
					REGISTER_TYPE_ENUMPTR(UIGroup),
					REGISTER_TYPE_ENUMPTR(UIText),
					REGISTER_TYPE_ENUMPTR(UISprite)
				]
				.def("MakeObjectDef", &ObjectModelManager::MakeObjectDef)
				.def("MakeObjectAttribute", &ObjectModelManager::MakeObjectAttribute),

				luabind::class_<ObjectAttribute, DisposablePointer<ObjectAttribute> >("ObjectAttribute")
					.def_readonly("ID", &ObjectAttribute::ID)
					.def_readonly("Name", &ObjectAttribute::Name)
					.def_readonly("Type", &ObjectAttribute::Type)
					.def("IsValid", (bool (ObjectAttribute::*)() const)&ObjectAttribute::IsValid)
					.property("Value", &GetObjectAttributeValue, &SetObjectAttributeValue)
					.def_readwrite("Set", &ObjectAttribute::Set)
					.def_readwrite("Get", &ObjectAttribute::Get),

				luabind::class_<ObjectDef, DisposablePointer<ObjectDef> >("ObjectDef")
					.def_readonly("ID", &ObjectDef::ID)
					.def_readonly("Name", &ObjectDef::Name)
					.property("Tags", &GetObjectDefTags, &SetObjectDefTags)
					.property("Layers", &GetObjectDefLayers, &SetObjectDefLayers)
					.property("Groups", &GetObjectDefGroups, &SetObjectDefGroups)
					.property("Attributes", &GetObjectDefAttributes)
					.def("AddAttribute", &AddObjectDefAttribute)
		];

		luabind::object Globals = luabind::globals(State);
		Globals["g_ObjectModel"] = &ObjectModelManager::Instance;

		return true;
	};

	bool ObjectAttribute::IsValid() const
	{
		return IsValid(Value);
	};

	bool ObjectAttribute::IsValid(luabind::object Value) const
	{
		if (!ObjectModelManager::Instance.Started())
			return false;

		switch (Type)
		{
		case ObjectAttributeType::None:
			return Value == luabind::nil;

		case ObjectAttributeType::Int8:
			return IsLuaType<int8>(Value);

		case ObjectAttributeType::UInt8:
			return IsLuaType<uint8>(Value);

		case ObjectAttributeType::Int16:
			return IsLuaType<int16>(Value);

		case ObjectAttributeType::UInt16:
			return IsLuaType<uint16>(Value);

		case ObjectAttributeType::Int32:
			return IsLuaType<int32>(Value);

		case ObjectAttributeType::UInt32:
			return IsLuaType<uint32>(Value);

		case ObjectAttributeType::Int64:
			return IsLuaType<int64>(Value);

		case ObjectAttributeType::UInt64:
			return IsLuaType<uint64>(Value);

		case ObjectAttributeType::F32:
			return IsLuaType<f32>(Value);

		case ObjectAttributeType::F64:
			return IsLuaType<f64>(Value);

		case ObjectAttributeType::Path:
			return IsLuaType<Path>(Value);

		case ObjectAttributeType::String:
			return IsLuaType<std::string>(Value);

		case ObjectAttributeType::Camera:
			return IsLuaType<Camera>(Value);

		case ObjectAttributeType::TextureBuffer:
			return IsLuaType<TextureBuffer>(Value);

		case ObjectAttributeType::TextureEncoderInfo:
			return IsLuaType<TextureEncoderInfo>(Value);

		case ObjectAttributeType::TexturePacker:
			return IsLuaType<TexturePacker>(Value);

		case ObjectAttributeType::TextureGroup:
			return IsLuaType<TextureGroup>(Value);

		case ObjectAttributeType::LinearTimer:
			return IsLuaType<LinearTimer>(Value);

		case ObjectAttributeType::GenericConfig:
			return IsLuaType<GenericConfig>(Value);

		case ObjectAttributeType::LuaEventGroup:
			return IsLuaType<LuaEventGroup>(Value);

		case ObjectAttributeType::Matrix4x4:
			return IsLuaType<Matrix4x4>(Value);

		case ObjectAttributeType::Vector2:
			return IsLuaType<Vector2>(Value);

		case ObjectAttributeType::Vector3:
			return IsLuaType<Vector3>(Value);

		case ObjectAttributeType::Vector4:
			return IsLuaType<Vector4>(Value);

		case ObjectAttributeType::Plane:
			return IsLuaType<Plane>(Value);

		case ObjectAttributeType::Rect:
			return IsLuaType<Rect>(Value);

		case ObjectAttributeType::RotateableRect:
			return IsLuaType<RotateableRect>(Value);

		case ObjectAttributeType::Ray:
			return IsLuaType<Ray>(Value);

		case ObjectAttributeType::AxisAlignedBoundingBox:
			return IsLuaType<AxisAlignedBoundingBox>(Value);

		case ObjectAttributeType::BoundingSphere:
			return IsLuaType<BoundingSphere>(Value);

		case ObjectAttributeType::Quaternion:
			return IsLuaType<Quaternion>(Value);

		case ObjectAttributeType::TiledMap:
			return IsLuaType<TiledMap>(Value);

		case ObjectAttributeType::ConsoleVariable:
			return IsLuaType<ConsoleVariable>(Value);

		case ObjectAttributeType::RendererDisplayMode:
			return IsLuaType<RendererDisplayMode>(Value);

		case ObjectAttributeType::RendererCapabilities:
			return IsLuaType<RendererCapabilities>(Value);

		case ObjectAttributeType::RenderCreateOptions:
			return IsLuaType<RenderCreateOptions>(Value);

		case ObjectAttributeType::Sprite:
			return IsLuaType<Sprite>(Value);

		case ObjectAttributeType::RendererFrameStats:
			return IsLuaType<RendererFrameStats>(Value);

		case ObjectAttributeType::TextParams:
			return IsLuaType<TextParams>(Value);

		case ObjectAttributeType::MouseButtonInfo:
			return IsLuaType<InputCenter::MouseButtonInfo>(Value);

		case ObjectAttributeType::KeyInfo:
			return IsLuaType<InputCenter::KeyInfo>(Value);

		case ObjectAttributeType::JoystickAxisInfo:
			return IsLuaType<InputCenter::JoystickAxisInfo>(Value);

		case ObjectAttributeType::JoystickButtonInfo:
			return IsLuaType<InputCenter::JoystickButtonInfo>(Value);

		case ObjectAttributeType::Action:
			return IsLuaType<InputCenter::Action>(Value);

		case ObjectAttributeType::TouchInfo:
			return IsLuaType<InputCenter::TouchInfo>(Value);

		case ObjectAttributeType::Int8Ptr:
			return IsLuaTypePtr<int8>(Value);

		case ObjectAttributeType::UInt8Ptr:
			return IsLuaTypePtr<uint8>(Value);

		case ObjectAttributeType::Int16Ptr:
			return IsLuaTypePtr<int16>(Value);

		case ObjectAttributeType::UInt16Ptr:
			return IsLuaTypePtr<uint16>(Value);

		case ObjectAttributeType::Int32Ptr:
			return IsLuaTypePtr<int32>(Value);

		case ObjectAttributeType::UInt32Ptr:
			return IsLuaTypePtr<uint32>(Value);

		case ObjectAttributeType::Int64Ptr:
			return IsLuaTypePtr<int64>(Value);

		case ObjectAttributeType::UInt64Ptr:
			return IsLuaTypePtr<uint64>(Value);

		case ObjectAttributeType::F32Ptr:
			return IsLuaTypePtr<f32>(Value);

		case ObjectAttributeType::F64Ptr:
			return IsLuaTypePtr<f64>(Value);

		case ObjectAttributeType::PathPtr:
			return IsLuaTypePtr<Path>(Value);

		case ObjectAttributeType::StreamPtr:
			return IsLuaTypePtr<Stream>(Value);

		case ObjectAttributeType::StringPtr:
			return IsLuaTypePtr<std::string>(Value);

		case ObjectAttributeType::CameraPtr:
			return IsLuaTypePtr<Camera>(Value);

		case ObjectAttributeType::TextureBufferPtr:
			return IsLuaTypePtr<TextureBuffer>(Value);

		case ObjectAttributeType::TextureEncoderInfoPtr:
			return IsLuaTypePtr<TextureEncoderInfo>(Value);

		case ObjectAttributeType::TexturePtr:
			return IsLuaTypePtr<Texture>(Value);

		case ObjectAttributeType::TexturePackerPtr:
			return IsLuaTypePtr<TexturePacker>(Value);

		case ObjectAttributeType::TextureGroupPtr:
			return IsLuaTypePtr<TextureGroup>(Value);

		case ObjectAttributeType::LinearTimerPtr:
			return IsLuaTypePtr<LinearTimer>(Value);

		case ObjectAttributeType::GenericConfigPtr:
			return IsLuaTypePtr<GenericConfig>(Value);

		case ObjectAttributeType::GameInterfacePtr:
			return IsLuaTypePtr<GameInterface>(Value);

		case ObjectAttributeType::LuaEventGroupPtr:
			return IsLuaTypePtr<LuaEventGroup>(Value);

		case ObjectAttributeType::Matrix4x4Ptr:
			return IsLuaTypePtr<Matrix4x4>(Value);

		case ObjectAttributeType::Vector2Ptr:
			return IsLuaTypePtr<Vector2>(Value);

		case ObjectAttributeType::Vector3Ptr:
			return IsLuaTypePtr<Vector3>(Value);

		case ObjectAttributeType::Vector4Ptr:
			return IsLuaTypePtr<Vector4>(Value);

		case ObjectAttributeType::PlanePtr:
			return IsLuaTypePtr<Plane>(Value);

		case ObjectAttributeType::RectPtr:
			return IsLuaTypePtr<Rect>(Value);

		case ObjectAttributeType::RotateableRectPtr:
			return IsLuaTypePtr<RotateableRect>(Value);

		case ObjectAttributeType::RayPtr:
			return IsLuaTypePtr<Ray>(Value);

		case ObjectAttributeType::AxisAlignedBoundingBoxPtr:
			return IsLuaTypePtr<AxisAlignedBoundingBox>(Value);

		case ObjectAttributeType::BoundingSpherePtr:
			return IsLuaTypePtr<BoundingSphere>(Value);

		case ObjectAttributeType::QuaternionPtr:
			return IsLuaTypePtr<Quaternion>(Value);

		case ObjectAttributeType::TiledMapPtr:
			return IsLuaTypePtr<TiledMap>(Value);

		case ObjectAttributeType::ConsoleVariablePtr:
			return IsLuaTypePtr<ConsoleVariable>(Value);

		case ObjectAttributeType::SoundPtr:
			return IsLuaTypePtr<SoundManager::Sound>(Value);

		case ObjectAttributeType::MusicPtr:
			return IsLuaTypePtr<SoundManager::Music>(Value);

		case ObjectAttributeType::RendererDisplayModePtr:
			return IsLuaTypePtr<RendererDisplayMode>(Value);

		case ObjectAttributeType::RendererCapabilitiesPtr:
			return IsLuaTypePtr<RendererCapabilities>(Value);

		case ObjectAttributeType::RenderCreateOptionsPtr:
			return IsLuaTypePtr<RenderCreateOptions>(Value);

		case ObjectAttributeType::SpritePtr:
			return IsLuaTypePtr<Sprite>(Value);

		case ObjectAttributeType::RendererFrameStatsPtr:
			return IsLuaTypePtr<RendererFrameStats>(Value);

		case ObjectAttributeType::RendererPtr:
			return IsLuaTypePtr<Renderer>(Value);

		case ObjectAttributeType::TextParamsPtr:
			return IsLuaTypePtr<TextParams>(Value);

		case ObjectAttributeType::MouseButtonInfoPtr:
			return IsLuaTypePtr<InputCenter::MouseButtonInfo>(Value);

		case ObjectAttributeType::KeyInfoPtr:
			return IsLuaTypePtr<InputCenter::KeyInfo>(Value);

		case ObjectAttributeType::JoystickAxisInfoPtr:
			return IsLuaTypePtr<InputCenter::JoystickAxisInfo>(Value);

		case ObjectAttributeType::JoystickButtonInfoPtr:
			return IsLuaTypePtr<InputCenter::JoystickButtonInfo>(Value);

		case ObjectAttributeType::ActionPtr:
			return IsLuaTypePtr<InputCenter::Action>(Value);

		case ObjectAttributeType::TouchInfoPtr:
			return IsLuaTypePtr<InputCenter::TouchInfo>(Value);

		case ObjectAttributeType::InputContextPtr:
			return IsLuaTypePtr<InputCenter::Context>(Value);

		case ObjectAttributeType::UIElementPtr:
			return IsLuaTypePtr<UIElement>(Value);

		case ObjectAttributeType::UIGroupPtr:
			return IsLuaTypePtr<UIGroup>(Value);

		case ObjectAttributeType::UITextPtr:
			return IsLuaTypePtr<UIText>(Value);

		case ObjectAttributeType::UISpritePtr:
			return IsLuaTypePtr<UISprite>(Value);
		};

		return false;
	};

	DisposablePointer<ObjectDef> ObjectModelManager::MakeObjectDef(const std::string &Name)
	{
		if (!Started())
			return DisposablePointer<ObjectDef>();

		if (!GameInterface::Instance.Get() || !GameInterface::Instance->GetScriptInstance().Get())
			return DisposablePointer<ObjectDef>();

		DisposablePointer<ObjectDef> Out(new ObjectDef());
		Out->ID = ++ObjectDefCounter;
		Out->Name = Name;

		ObjectDefs[ObjectDefCounter] = Out;
		
		return Out;
	};

	DisposablePointer<ObjectAttribute> ObjectModelManager::MakeObjectAttribute(const std::string &Name)
	{
		if (!Started())
			return DisposablePointer<ObjectAttribute>();

		if (!GameInterface::Instance.Get() || !GameInterface::Instance->GetScriptInstance().Get())
			return DisposablePointer<ObjectAttribute>();

		DisposablePointer<ObjectAttribute> Out(new ObjectAttribute());
		Out->ID = ++ObjectAttributesCounter;
		Out->Name = Name;
		Out->Type = ObjectAttributeType::None;

		ObjectAttributes[ObjectAttributesCounter] = Out;

		return Out;
	};

	void ObjectModelManager::Clear()
	{
		if (!Started())
			return;

		while (ObjectDefs.size())
		{
			ObjectDefs.begin()->second.Dispose();
			ObjectDefs.erase(ObjectDefs.begin());
		};

		while (ObjectAttributes.size())
		{
			ObjectAttributes.begin()->second.Dispose();
			ObjectAttributes.erase(ObjectAttributes.begin());
		};
	};

	void ObjectModelManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK();

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing ObjectModel...");
	};

	void ObjectModelManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAG, "Terminating ObjectModel...");
	};

	void ObjectModelManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();
	};
};

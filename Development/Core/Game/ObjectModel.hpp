#pragma once

namespace ObjectAttributeType
{
	enum ObjectAttributeType
	{
		None,
		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Int64,
		UInt64,
		F32,
		F64,
		Path,
		String,
		Camera,
		TextureBuffer,
		TextureEncoderInfo,
		TexturePacker,
		TextureGroup,
		LinearTimer,
		GenericConfig,
		LuaEventGroup,
		Matrix4x4,
		Vector2,
		Vector3,
		Vector4,
		Plane,
		Rect,
		RotateableRect,
		Ray,
		AxisAlignedBoundingBox,
		BoundingSphere,
		Quaternion,
		TiledMap,
		ConsoleVariable,
		RendererDisplayMode,
		RendererCapabilities,
		RenderCreateOptions,
		Sprite,
		RendererFrameStats,
		TextParams,
		MouseButtonInfo,
		KeyInfo,
		JoystickAxisInfo,
		JoystickButtonInfo,
		Action,
		TouchInfo,
		Int8Ptr,
		UInt8Ptr,
		Int16Ptr,
		UInt16Ptr,
		Int32Ptr,
		UInt32Ptr,
		Int64Ptr,
		UInt64Ptr,
		F32Ptr,
		F64Ptr,
		PathPtr,
		StreamPtr,
		StringPtr,
		CameraPtr,
		TextureBufferPtr,
		TextureEncoderInfoPtr,
		TexturePtr,
		TexturePackerPtr,
		TextureGroupPtr,
		LinearTimerPtr,
		GenericConfigPtr,
		GameInterfacePtr,
		LuaEventGroupPtr,
		Matrix4x4Ptr,
		Vector2Ptr,
		Vector3Ptr,
		Vector4Ptr,
		PlanePtr,
		RectPtr,
		RotateableRectPtr,
		RayPtr,
		AxisAlignedBoundingBoxPtr,
		BoundingSpherePtr,
		QuaternionPtr,
		TiledMapPtr,
		ConsoleVariablePtr,
		SoundPtr,
		MusicPtr,
		RendererDisplayModePtr,
		RendererCapabilitiesPtr,
		RenderCreateOptionsPtr,
		SpritePtr,
		RendererFrameStatsPtr,
		RendererPtr,
		TextParamsPtr,
		MouseButtonInfoPtr,
		KeyInfoPtr,
		JoystickAxisInfoPtr,
		JoystickButtonInfoPtr,
		ActionPtr,
		TouchInfoPtr,
		InputContextPtr,
		UIElementPtr,
		UIGroupPtr,
		UIVerticalGroupPtr,
		UIHorizontalGroupPtr,
		UITextPtr,
		UISpritePtr
	};
};

template<typename type>
bool IsLuaType(luabind::object o)
{
	try
	{
		luabind::object_cast<type>(o);
	}
	catch (std::exception &)
	{
		return false;
	};

	return true;
};

template<typename type>
bool IsLuaTypePtr(luabind::object o)
{
	try
	{
		luabind::object_cast<type *>(o);
	}
	catch (std::exception &)
	{
		return false;
	};

	return true;
};

class ObjectAttribute
{
public:
	uint64 ID;
	std::string Name;
	uint32 Type;
	luabind::object Value, Set, Get;

	bool IsValid() const;
	bool IsValid(luabind::object Value) const;
};

class ObjectDef
{
public:
	uint64 ID;
	std::string Name;

	std::vector<std::string> Tags, Layers, Groups;
	std::vector<DisposablePointer<ObjectAttribute> > Attributes;
};

class ObjectModelManager : public SubSystem
{
private:
	typedef std::map<uint64, DisposablePointer<ObjectDef> > ObjectDefMap;
	ObjectDefMap ObjectDefs;

	typedef std::map<uint64, DisposablePointer<ObjectAttribute> > ObjectAttributeMap;
	ObjectAttributeMap ObjectAttributes;

	uint64 ObjectAttributesCounter, ObjectDefCounter, ObjectCounter;

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:
	static ObjectModelManager Instance;
	
	ObjectModelManager() : SubSystem(OBJECTMODEL_PRIORITY), ObjectAttributesCounter(0), ObjectDefCounter(0), ObjectCounter(0)
	{
	};

	static bool RegisterBindings(lua_State *State);

	DisposablePointer<ObjectDef> MakeObjectDef(const std::string &Name);
	DisposablePointer<ObjectAttribute> MakeObjectAttribute(const std::string &Name);
	void Clear();
};

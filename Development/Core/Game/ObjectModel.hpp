#pragma once

namespace FeatureMessage
{
	enum
	{
		FrameUpdate = 0,
		FixedUpdate,
		FrameDraw,
		Count
	};
}

class ObjectDef;

class ObjectFeature
{
	friend class ObjectModelManager;
private:
	bool Started;
public:
	uint64 ID;
	std::string Name;

	ObjectFeature();
	virtual ~ObjectFeature() {};

	virtual void OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments);
	virtual bool RespondsToMessage(uint32 MessageID);
	virtual void OnAttached(DisposablePointer<ObjectDef> Def);
	virtual void OnDetached(DisposablePointer<ObjectDef> Def);
	virtual void OnStart(DisposablePointer<ObjectDef> Def);
	virtual DisposablePointer<ObjectFeature> Clone() = 0;
};

class TransformFeature : public ObjectFeature
{
public:
	Vector3 Position, Rotation, Scale;

	TransformFeature();
	void OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments) override;
	bool RespondsToMessage(uint32 MessageID) override;
	DisposablePointer<ObjectFeature> Clone() override;
};

class SpriteFeature : public ObjectFeature
{
public:
	Sprite TheSprite;

	SpriteFeature();
	void OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments) override;
	bool RespondsToMessage(uint32 MessageID) override;
	DisposablePointer<ObjectFeature> Clone() override;
};

class PhysicsFeature : public ObjectFeature
{
public:
	DisposablePointer<PhysicsBody> Body;
	Vector2 Size;
	Vector2 Position;
	bool Dynamic;
	f32 Angle;
	bool FixedRotation;
	f32 Density;
	f32 Friction;

	PhysicsFeature();
	~PhysicsFeature();
	void OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments) override;
	bool RespondsToMessage(uint32 MessageID) override;
	void OnStart(DisposablePointer<ObjectDef> Def) override;
	DisposablePointer<ObjectFeature> Clone() override;
};

class ScriptedFeature : public ObjectFeature
{
public:
	enum ScriptedFeatureEvent
	{
		Message,
		RespondMessage,
		Attached,
		Detached,
		Start,
		Count
	};

	luabind::object Events[ScriptedFeatureEvent::Count];
	bool ErroredOnEvent[ScriptedFeatureEvent::Count];

	luabind::object Container;

	ScriptedFeature(const std::string &Name, luabind::object OnMessageFn, luabind::object RespondsToMessageFn,
		luabind::object OnAttachedFn, luabind::object OnDetachedFn, luabind::object OnStartFn);

	void OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments);
	bool RespondsToMessage(uint32 MessageID);
	void OnAttached(DisposablePointer<ObjectDef> Def);
	void OnDetached(DisposablePointer<ObjectDef> Def);
	void OnStart(DisposablePointer<ObjectDef> Def);
	DisposablePointer<ObjectFeature> Clone();

	template<typename R, typename... Args>
	bool RunEvent(uint32 EventID, R &Result, Args... Arguments)
	{
		if (ErroredOnEvent[EventID])
			return false;

		try
		{
			Result = ProtectedLuaCast<R>(Events[EventID](Arguments...));
		}
		catch (luabind::error &)
		{
			ErroredOnEvent[EventID] = true;

			return false;
		}

		return true;
	}

	template<typename... Args>
	bool RunEventVoid(uint32 EventID, Args... Arguments)
	{
		if (ErroredOnEvent[EventID])
			return false;

		try
		{
			Events[EventID](Arguments...);
		}
		catch (luabind::error &)
		{
			ErroredOnEvent[EventID] = true;

			return false;
		}

		return true;
	}
};

class ObjectDef
{
public:
	uint64 ID;
	std::string Name;

	std::vector<std::string> Tags;

	std::string Layer, Group;

	std::vector<DisposablePointer<ObjectFeature> > Features;

	ObjectDef();
	DisposablePointer<ObjectDef> Clone();
	DisposablePointer<ObjectFeature> GetFeature(const std::string &Name);
};

class ObjectModelManager : public SubSystem
{
private:
	typedef std::map<uint64, DisposablePointer<ObjectDef> > ObjectDefMap;
	ObjectDefMap ObjectDefs;

	typedef std::map<uint64, DisposablePointer<ObjectFeature> > ObjectFeatureMap;
	ObjectFeatureMap ObjectFeatures;

	std::vector<DisposablePointer<ObjectDef> > Objects;

	typedef std::map<uint64, std::vector<DisposablePointer<ObjectDef> > > ObjectLayerMap;
	typedef std::map<uint64, ObjectLayerMap> ObjectGroupMap;

	ObjectGroupMap ObjectGroups;

	typedef std::map<std::string, uint64> LayerPriorityMap;
	typedef std::map<std::string, LayerPriorityMap> GroupPriorityMap;

	LayerPriorityMap LayerPriorities;
	GroupPriorityMap GroupPriorities;

	typedef std::map<StringID, std::vector<DisposablePointer<ObjectDef> > > ObjectFeaturesUsageMap;
	ObjectFeaturesUsageMap ObjectFeatureUsage;

	uint64 ObjectFeatureCounter, ObjectDefCounter, ObjectCounter;

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:
	static ObjectModelManager Instance;
	
	ObjectModelManager() : SubSystem(OBJECTMODEL_PRIORITY), ObjectFeatureCounter(0), ObjectDefCounter(0), ObjectCounter(0)
	{
	}

	static bool RegisterBindings(lua_State *State);

	/*!
	*	\param Def the definition to register
	*	\note Registers the object itself, not a definition
	*/
	bool RegisterObject(DisposablePointer<ObjectDef> Def);

	/*!
	*	\param Def the definition to register
	*	\note Registers a definition archetype, not the object itself
	*/
	bool RegisterObjectDef(DisposablePointer<ObjectDef> Def);

	/*!
	*	\param Feature the feature to register
	*/
	bool RegisterObjectFeature(DisposablePointer<ObjectFeature> Feature);

	/*!
	*	Clears all data
	*/
	void Clear();

	/*!
	*	Emits a message to all objects
	*	\param MessageID one of FeatureMessage::*
	*	\param Arguments any arguments passed
	*/
	void EmitMessage(uint32 MessageID, const std::vector<void *> &Arguments);

	/*!
	*	\param Name the name of the feature to find
	*/
	std::vector<DisposablePointer<ObjectDef> > FindObjectsWithFeature(const std::string &Name);
};

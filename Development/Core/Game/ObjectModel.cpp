#include "FlamingCore.hpp"
namespace FlamingTorch
{
#define TAG "ObjectModel"

	ObjectModelManager ObjectModelManager::Instance;

	luabind::object GetObjectDefTags(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<std::string>(Self.Tags);
	}

	void AddObjectDefFeature(ObjectDef &Self, DisposablePointer<ObjectFeature> Feature)
	{
		for (uint32 i = 0; i < Self.Features.size(); i++)
		{
			if (Self.Features[i].Get() == Feature.Get())
				return;
		}

		Self.Features.push_back(Feature);
		
		Feature->OnAttached(DisposablePointer<ObjectDef>::MakeWeak(&Self));
	}

	void RemoveObjectDefFeature(ObjectDef &Self, const std::string &Name)
	{
		for (uint32 i = 0; i < Self.Features.size(); i++)
		{
			if (Self.Features[i].Get() == NULL || Self.Features[i]->Name != Name)
				continue;

			Self.Features[i]->OnDetached(DisposablePointer<ObjectDef>::MakeWeak(&Self));

			Self.Features.erase(Self.Features.begin() + i);

			return;
		}
	}

	void AddObjectDefTag(ObjectDef &Self, const std::string &Tag)
	{
		for (uint32 i = 0; i < Self.Tags.size(); i++)
		{
			if (Self.Tags[i] == Tag)
				return;
		}

		Self.Tags.push_back(Tag);
	}

	void RemoveObjectDefTag(ObjectDef &Self, const std::string &Tag)
	{
		for (uint32 i = 0; i < Self.Tags.size(); i++)
		{
			if (Self.Tags[i] == Tag)
			{
				Self.Tags.erase(Self.Tags.begin() + i);

				return;
			}
		}
	}

	luabind::object GetObjectDefFeatures(ObjectDef &Self)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<DisposablePointer<ObjectFeature> >(Self.Features);
	}

	luabind::object ObjectModelManagerFindObjectsWithFeature(ObjectModelManager &Self, const std::string &Name)
	{
		return GameInterface::Instance->GetScriptInstance()->VectorToLua<DisposablePointer<ObjectDef> >(Self.FindObjectsWithFeature(Name));
	}

	bool ObjectModelManager::RegisterBindings(lua_State *State)
	{
		luabind::module(State)[
			luabind::class_<ObjectModelManager, SubSystem>("ObjectModelManager")
				.def("RegisterObject", &ObjectModelManager::RegisterObject)
				.def("RegisterObjectDef", &ObjectModelManager::RegisterObjectDef)
				.def("RegisterObjectFeature", &ObjectModelManager::RegisterObjectFeature)
				.def("FindObjectsWithFeature", &ObjectModelManagerFindObjectsWithFeature),

			luabind::class_<ObjectDef, DisposablePointer<ObjectDef> >("ObjectDef")
				.def(luabind::constructor<>())
				.def_readonly("ID", &ObjectDef::ID)
				.def_readwrite("Name", &ObjectDef::Name)
				.property("Features", &GetObjectDefFeatures)
				.property("Tags", &GetObjectDefTags)
				.def_readwrite("Layer", &ObjectDef::Layer)
				.def_readwrite("Group", &ObjectDef::Group)
				.def("Clone", &ObjectDef::Clone)
				.def("AddFeature", &AddObjectDefFeature)
				.def("RemoveFeature", &RemoveObjectDefFeature)
				.def("AddTag", &AddObjectDefTag)
				.def("RemoveTag", &RemoveObjectDefTag)
				.def("GetFeature", &ObjectDef::GetFeature),

			luabind::class_<ObjectFeature, DisposablePointer<ObjectFeature> >("ObjectFeature")
				.def_readonly("ID", &ObjectFeature::ID)
				.def_readwrite("Name", &ObjectFeature::Name)
				.def("Clone", &ObjectDef::Clone),

			luabind::class_<TransformFeature, ObjectFeature, DisposablePointer<ObjectFeature> >("TransformFeature")
				.def(luabind::constructor<>())
				.def_readwrite("Position", &TransformFeature::Position)
				.def_readwrite("Rotation", &TransformFeature::Rotation)
				.def_readwrite("Scale", &TransformFeature::Scale),

			luabind::class_<SpriteFeature, ObjectFeature, DisposablePointer<ObjectFeature> >("SpriteFeature")
				.def(luabind::constructor<>())
				.def_readwrite("Sprite", &SpriteFeature::TheSprite),

			luabind::class_<PhysicsFeature, ObjectFeature, DisposablePointer<ObjectFeature> >("PhysicsFeature")
				.def(luabind::constructor<>())
				.def_readonly("Body", &PhysicsFeature::Body)
				.def_readwrite("Size", &PhysicsFeature::Size)
				.def_readwrite("Position", &PhysicsFeature::Position)
				.def_readwrite("Dynamic", &PhysicsFeature::Dynamic)
				.def_readwrite("Angle", &PhysicsFeature::Angle)
				.def_readwrite("FixedRotation", &PhysicsFeature::FixedRotation)
				.def_readwrite("Density", &PhysicsFeature::Density)
				.def_readwrite("Friction", &PhysicsFeature::Friction)
		];

		luabind::object Globals = luabind::globals(State);
		Globals["g_ObjectModel"] = &ObjectModelManager::Instance;

		return true;
	}

	bool ObjectModelManager::RegisterObjectDef(DisposablePointer<ObjectDef> Def)
	{
		if (!Started())
			return false;

		if (!GameInterface::Instance.Get() || !GameInterface::Instance->GetScriptInstance().Get())
			return false;

		for (ObjectDefMap::iterator it = ObjectDefs.begin(); it != ObjectDefs.end(); it++)
		{
			if (it->second.Get() == NULL)
			{
				ObjectDefs.erase(it);

				it = ObjectDefs.begin();

				if (it == ObjectDefs.end())
					return false;
			}

			if (it->second->Name == Def->Name || it->second.Get() == Def.Get())
				return false;
		}

		Def->ID = ++ObjectDefCounter;

		ObjectDefs[ObjectDefCounter] = Def;
		
		return true;
	}

	bool ObjectModelManager::RegisterObject(DisposablePointer<ObjectDef> Def)
	{
		if (!Started())
			return false;

		if (!GameInterface::Instance.Get() || !GameInterface::Instance->GetScriptInstance().Get())
			return false;

		if (Def->ID != 0) //Already registered
			return false;

		Def->ID = ++ObjectCounter;

		Objects.push_back(Def);

		std::string UpperGroup = StringUtils::ToUpperCase(Def->Group);
		std::string UpperLayer = StringUtils::ToUpperCase(Def->Layer);

		uint64 GroupPriority = 0, LayerPriority = 0;

		if (GroupPriorities.find(UpperGroup) != GroupPriorities.end())
		{
			LayerPriorityMap &LayerMap = GroupPriorities[UpperGroup];

			if (LayerMap.find(UpperLayer) == LayerMap.end())
			{
				LayerMap[UpperLayer] = LayerMap.size();
			}
		}
		else
		{
			GroupPriorities[UpperGroup][UpperLayer] = 0;
		}

		uint64 Counter = 0;

		for (GroupPriorityMap::iterator it = GroupPriorities.begin(); it != GroupPriorities.end(); it++, Counter++)
		{
			if (it->first == UpperGroup)
			{
				uint64 LayerCounter = 0;

				for (LayerPriorityMap::iterator lit = it->second.begin(); lit != it->second.end(); lit++, LayerCounter++)
				{
					if (lit->first == UpperLayer)
					{
						ObjectGroups[Counter][LayerCounter].push_back(Def);

						break;
					}
				}

				break;
			}
		}

		for (uint32 i = 0; i < Def->Features.size(); i++)
		{
			ObjectFeatureUsage[MakeStringID(Def->Features[i]->Name)].push_back(Def);
		}

		return true;
	}

	bool ObjectModelManager::RegisterObjectFeature(DisposablePointer<ObjectFeature> Feature)
	{
		if (!Started())
			return false;

		if (!GameInterface::Instance.Get() || !GameInterface::Instance->GetScriptInstance().Get())
			return false;

		//Keep duplicates out
		for (ObjectFeatureMap::iterator it = ObjectFeatures.begin(); it != ObjectFeatures.end(); it++)
		{
			if (it->second->Name == Feature->Name)
				return false;
		}

		Feature->ID = ++ObjectFeatureCounter;

		ObjectFeatures[ObjectFeatureCounter] = Feature;

		return true;
	}

	void ObjectModelManager::Clear()
	{
		if (!Started())
			return;

		while (ObjectDefs.size())
		{
			ObjectDefs.begin()->second.Dispose();
			ObjectDefs.erase(ObjectDefs.begin());
		}

		while (ObjectFeatures.size())
		{
			ObjectFeatures.begin()->second.Dispose();
			ObjectFeatures.erase(ObjectFeatures.begin());
		}
	}

	void ObjectModelManager::EmitMessage(uint32 MessageID, const std::vector<void *> &Arguments)
	{
		if (!Started())
			return;

		for (ObjectGroupMap::iterator it = ObjectGroups.begin(); it != ObjectGroups.end(); it++)
		{
			for (ObjectLayerMap::iterator lit = it->second.begin(); lit != it->second.end(); lit++)
			{
				std::vector<DisposablePointer<ObjectDef> > &Objects = lit->second;

				for (uint32 i = 0; i < Objects.size(); i++)
				{
					if (Objects[i].Get() == NULL)
						continue;

					DisposablePointer<ObjectDef> Def = Objects[i];

					for (uint32 j = 0; j < Def->Features.size(); j++)
					{
						ObjectFeature *Feature = Def->Features[j];

						if (Feature == NULL || !Feature->RespondsToMessage(MessageID))
							continue;

						Feature->OnMessage(Def, MessageID, Arguments);
					}
				}
			}
		}
	}

	void ObjectModelManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK();

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Initializing ObjectModel...");

		DisposablePointer<ObjectFeature> Transform(new TransformFeature());
		DisposablePointer<ObjectFeature> Sprite(new SpriteFeature());
		DisposablePointer<ObjectFeature> Physics(new PhysicsFeature());

		RegisterObjectFeature(Transform);
		RegisterObjectFeature(Sprite);
		RegisterObjectFeature(Physics);
	}

	void ObjectModelManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo(TAG, "Terminating ObjectModel...");
	}

	void ObjectModelManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();
	}

	std::vector<DisposablePointer<ObjectDef> > ObjectModelManager::FindObjectsWithFeature(const std::string &Name)
	{
		if (ObjectFeatureUsage.find(MakeStringID(Name)) == ObjectFeatureUsage.end())
			return std::vector<DisposablePointer<ObjectDef> >();

		return ObjectFeatureUsage[MakeStringID(Name)];
	}

	ObjectFeature::ObjectFeature() : ID(0), Name("ObjectFeature")
	{
	}

	bool ObjectFeature::RespondsToMessage(uint32 MessageID)
	{
		return false;
	}

	void ObjectFeature::OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments)
	{
	}

	void ObjectFeature::OnAttached(DisposablePointer<ObjectDef> Def)
	{
	}

	void ObjectFeature::OnDetached(DisposablePointer<ObjectDef> Def)
	{
	}

	ObjectDef::ObjectDef() : ID(0), Layer("None"), Group("None"), Name("ObjectDef")
	{
	}

	DisposablePointer<ObjectDef> ObjectDef::Clone()
	{
		DisposablePointer<ObjectDef> Out(new ObjectDef());

		Out->Name = Name;
		Out->Layer = Layer;
		Out->Tags = Tags;
		Out->Group = Group;
		Out->ID = 0;

		for (uint32 i = 0; i < Features.size(); i++)
		{
			DisposablePointer<ObjectFeature> Feature = Features[i]->Clone();

			if (!Feature)
				return DisposablePointer<ObjectDef>();

			Out->Features.push_back(Feature);

			Feature->OnAttached(Out);
		}

		return Out;
	}

	DisposablePointer<ObjectFeature> ObjectDef::GetFeature(const std::string &Name)
	{
		for (uint32 i = 0; i < Features.size(); i++)
		{
			if (Features[i].Get() == NULL)
				continue;

			if (Features[i]->Name == Name)
				return Features[i];
		}

		return DisposablePointer<ObjectFeature>();
	}

	TransformFeature::TransformFeature() : ObjectFeature(), Scale(1, 1, 1)
	{
		Name = "Transform";
	}

	void TransformFeature::OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments)
	{
	}

	bool TransformFeature::RespondsToMessage(uint32 MessageID)
	{
		return false;
	}

	DisposablePointer<ObjectFeature> TransformFeature::Clone()
	{
		DisposablePointer<TransformFeature> Out(new TransformFeature());

		Out->Position = Position;
		Out->Rotation = Rotation;
		Out->Scale = Scale;
		Out->Name = Name;
		Out->ID = ID;

		return Out;
	}

	SpriteFeature::SpriteFeature() : ObjectFeature()
	{
		Name = "Sprite";
	}

	void SpriteFeature::OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments)
	{
		switch (MessageID)
		{
		case FeatureMessage::FrameDraw:
			{
				DisposablePointer<TransformFeature> TransformFeature = Def->GetFeature("Transform");

				if (TransformFeature.Get())
				{
					TheSprite.Options.Position(TransformFeature->Position.ToVector2()).Scale(TransformFeature->Scale.ToVector2()).Rotation(TransformFeature->Rotation.z);
				}
			}

			TheSprite.Draw((Renderer *)Arguments[0]);

			break;
		}
	}

	bool SpriteFeature::RespondsToMessage(uint32 MessageID)
	{
		return MessageID == FeatureMessage::FrameDraw;
	}

	DisposablePointer<ObjectFeature> SpriteFeature::Clone()
	{
		DisposablePointer<SpriteFeature> Out(new SpriteFeature());

		Out->TheSprite = TheSprite;
		Out->Name = Name;
		Out->ID = ID;

		return Out;
	}

	PhysicsFeature::PhysicsFeature() : ObjectFeature(), Dynamic(false), FixedRotation(false), Density(0), Friction(0), Angle(0)
	{
		Name = "Physics";
	}

	void PhysicsFeature::OnMessage(DisposablePointer<ObjectDef> Def, uint32 MessageID, const std::vector<void *> &Arguments)
	{
		if (Body.Get() == NULL)
			return;

		switch (MessageID)
		{
		case FeatureMessage::FrameUpdate:
			{
				DisposablePointer<TransformFeature> Transform = Def->GetFeature("Transform");

				if (Transform.Get())
				{
					Transform->Position = Body->Position();
					Transform->Rotation.z = Body->Rotation();
				}
			}

			break;
		}
	}

	bool PhysicsFeature::RespondsToMessage(uint32 MessageID)
	{
		return MessageID == FeatureMessage::FrameUpdate;
	}

	void PhysicsFeature::OnAttached(DisposablePointer<ObjectDef> Def)
	{
		Body = PhysicsWorld::Instance.MakeBody(Dynamic, Position, Size, Angle, FixedRotation, Density, Friction);
	}

	void PhysicsFeature::OnDetached(DisposablePointer<ObjectDef> Def)
	{
		Body.Dispose();
	}

	DisposablePointer<ObjectFeature> PhysicsFeature::Clone()
	{
		DisposablePointer<PhysicsFeature> Out(new PhysicsFeature);

		Out->ID = ID;
		Out->Angle = Angle;
		Out->Density = Density;
		Out->Dynamic = Dynamic;
		Out->FixedRotation = FixedRotation;
		Out->Friction = Friction;
		Out->Position = Position;
		Out->Size = Size;

		return Out;
	}
}

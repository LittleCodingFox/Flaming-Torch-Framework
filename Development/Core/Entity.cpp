#include "FlamingCore.hpp"
#define TAG "World"

namespace FlamingTorch
{
	World World::Instance;

	Entity::~Entity()
	{
		Destroy();
	};

	void Entity::Destroy()
	{
		Properties = luabind::object();

		for(World::ComponentMap::iterator it = World::Instance.Components[ID].begin(); it != World::Instance.Components[ID].end(); it++)
		{
			World::Instance.ComponentEntityCache[it->first].erase(std::find(World::Instance.ComponentEntityCache[it->first].begin(), World::Instance.ComponentEntityCache[it->first].end(), ID));
		};

		World::Instance.Components.erase(ID);
	};

	SuperSmartPointer<Entity> Entity::Clone() const
	{
		SuperSmartPointer<Entity> Out(new Entity());

		Out->Name = Name + " (Copy)";
		Out->ID = World::MakeEntityID(Out.Get());

		World::EntityComponentMap::iterator eit = World::Instance.Components.find(ID);
		World::ComponentMap &OutMap = World::Instance.Components[Out->ID];

		for(World::ComponentMap::iterator it = eit->second.begin(); it != eit->second.end(); it++)
		{
			OutMap[it->first] = it->second;
		};

		Out->Properties = Properties;

		return Out;
	};

	bool Entity::HasComponent(const std::string &ComponentName) const
	{
		StringID ComponentID = MakeStringID(ComponentName);
		World::EntityComponentMap::iterator it = World::Instance.Components.find(ID);

		return it != World::Instance.Components.end() && it->second.find(ComponentID) != it->second.end();
	};

	bool Entity::HasProperty(const std::string &PropertyName) const
	{
		return !!Properties[PropertyName];
	};

	Component *Entity::GetComponent(const std::string &ComponentName)
	{
		StringID ComponentID = MakeStringID(ComponentName);
		World::ComponentMap::iterator it = World::Instance.Components[ID].find(ComponentID);

		return it != World::Instance.Components[ID].end() ? it->second.Get() : NULL;
	};

	void Component::Update()
	{
		if(OnUpdateFunction)
		{
			try
			{
				OnUpdateFunction(this, Owner);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr(TAG, "Error while updating component '%s' for entity '%s': Script Error '%s'", Name.c_str(), Owner->Name.c_str(), e.what());
			};
		};
	};

	bool Entity::AddComponent(const std::string &ComponentName)
	{
		if(HasComponent(ComponentName))
			return false;

		StringID ComponentID = MakeStringID(ComponentName);

		World::ComponentMap::iterator it = World::Instance.ComponentArchetypes.find(ComponentID);

		if(it == World::Instance.ComponentArchetypes.end())
		{
			Log::Instance.LogErr(TAG, "Unable to add component '%s' to entity '%s': Component not found", GetStringIDString(ComponentID).c_str(),
				Name.c_str());

			return false;
		};

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(!Owner)
		{
			Log::Instance.LogErr(TAG, "Unable to load component '%s' to entity '%s': Not using a Scripted Game Interface", it->second->Name.c_str(), Name.c_str());

			return NULL;
		};

		SuperSmartPointer<Component> Self(new Component(*it->second));
		World::Instance.Components[ID][ComponentID] = Self;
		World::Instance.ComponentEntityCache[ComponentID].push_back(ID);
		Self->Owner = this;
		Self->Properties = luabind::newtable(Owner->ScriptInstance->State);

		if(Self->OnStartFunction)
		{
			try
			{
				Self->OnStartFunction(Self, this);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr(TAG, "Unable to add component '%s' to entity '%s': Script error '%s' on OnStart", it->second->Name.c_str(),
					Name.c_str(), e.what());

				return false;
			};
		};

		return true;
	};

	StringID World::MakeEntityID(Entity *This)
	{
		return MakeStringID(This->Name + "_" + StringUtils::PointerString(This));
	};

	std::vector<Entity *> World::EntitiesWithComponent(const std::string &ComponentName)
	{
		StringID ComponentID = MakeStringID(ComponentName);
		std::vector<Entity *> Out;

		for(std::list<StringID>::iterator it = ComponentEntityCache[ComponentID].begin(); it != ComponentEntityCache[ComponentID].end(); it++)
		{
			Out.push_back(Entities[*it]);
		};

		return Out;
	};

	bool World::LoadComponent(const std::string &FileName)
	{
		std::string Directory, ActualFileName;
		int32 Index = FileName.rfind('/');

		if(Index != std::string::npos)
		{
			Directory = FileName.substr(0, Index + 1);
			ActualFileName = FileName.substr(Index + 1);
		}
		else
		{
			ActualFileName = FileName;
		};

		std::string ClassName = ActualFileName;

		Index = ClassName.rfind('.');

		if(Index != std::string::npos)
		{
			ClassName = ClassName.substr(0, Index);
		};

		StringID ClassID = MakeStringID(ClassName);

		SuperSmartPointer<Stream> In(new FileStream());

		if(!In.AsDerived<FileStream>()->Open(Directory + ActualFileName, StreamFlags::Read | StreamFlags::Text))
		{
			In = PackageFileSystemManager::Instance.GetFile(MakeStringID(Directory), MakeStringID(ActualFileName));
		};

		if(!In)
		{
			Log::Instance.LogErr(TAG, "Unable to load component '%s': File not found", FileName.c_str());

			return false;
		};

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(!Owner)
		{
			Log::Instance.LogErr(TAG, "Unable to load component '%s': Not using a Scripted Game Interface", FileName.c_str());

			return false;
		};

		if(Owner->ScriptInstance->DoString(In->AsString()))
		{
			Log::Instance.LogErr(TAG, "Unable to load component '%s': Scripting error", FileName.c_str());

			return false;
		};

		luabind::object Object = luabind::globals(Owner->ScriptInstance->State)[ClassName];

		if(!Object)
		{
			Log::Instance.LogErr(TAG, "Unable to load component '%s': Scripting error: ClassName '%s' not defined", FileName.c_str(), ClassName.c_str());

			return false;
		};

		SuperSmartPointer<Component> Out(new Component());
		Out->ID = ClassID;

		if(Object["Parent"])
		{
			Out->ParentID = MakeStringID(ProtectedLuaCast<const char *>(Object["Parent"]));
		};

		Out->Name = ClassName;
		Out->OnStartFunction = Object["OnStart"];
		Out->OnUpdateFunction = Object["OnUpdate"];

		ComponentArchetypes[ClassID] = Out;

		Log::Instance.LogDebug(TAG, "Loaded component '%s' ('%s')", FileName.c_str(), ClassName.c_str());

		return true;
	};

	Entity *World::NewEntity(const std::string &Name)
	{
		if(Entities.find(MakeStringID(Name)) != Entities.end())
		{
			Log::Instance.LogErr(TAG, "Unable to create entity '%s': Already exists", Name.c_str());

			return NULL;
		};

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(!Owner)
		{
			Log::Instance.LogErr(TAG, "Unable to create entity '%s': Not using a Scripted Game Interface", Name.c_str());

			return NULL;
		};

		SuperSmartPointer<Entity> Out(new Entity());
		Out->ID = MakeStringID(Name);
		Out->Name = Name;

		Entities[Out->ID] = Out;
		Entities[Out->ID]->Properties = luabind::newtable(Owner->ScriptInstance->State);

		return Entities[Out->ID];
	};

	Entity *World::NewEntity(const std::string &Name, const std::string &ScriptFileName)
	{
		if(Entities.find(MakeStringID(Name)) != Entities.end())
		{
			Log::Instance.LogErr(TAG, "Unable to create entity '%s': Already exists", Name.c_str());

			return NULL;
		};

		std::string Directory, ActualFileName;
		int32 Index = ScriptFileName.rfind('/');

		if(Index != std::string::npos)
		{
			Directory = ScriptFileName.substr(0, Index + 1);
			ActualFileName = ScriptFileName.substr(Index + 1);
		}
		else
		{
			ActualFileName = ScriptFileName;
		};

		std::string ClassName = ActualFileName;

		Index = ClassName.rfind('.');

		if(Index != std::string::npos)
		{
			ClassName = ClassName.substr(0, Index);
		};

		StringID ClassID = MakeStringID(ClassName);

		SuperSmartPointer<Stream> In(new FileStream());

		if(!In.AsDerived<FileStream>()->Open(Directory + ActualFileName, StreamFlags::Read | StreamFlags::Text))
		{
			In = PackageFileSystemManager::Instance.GetFile(MakeStringID(Directory), MakeStringID(ActualFileName));
		};

		if(!In)
		{
			Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': File not found", ScriptFileName.c_str());

			return NULL;
		};

		ScriptedGameInterface *Owner = GameInterface::Instance.AsDerived<ScriptedGameInterface>();

		if(!Owner)
		{
			Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': Not using a Scripted Game Interface", ScriptFileName.c_str());

			return NULL;
		};

		if(Owner->ScriptInstance->DoString(In->AsString()))
		{
			Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': Scripting error", ScriptFileName.c_str());

			return NULL;
		};

		luabind::object Object = luabind::globals(Owner->ScriptInstance->State)[ClassName];

		if(!Object)
		{
			Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': Scripting error: ClassName '%s' not defined", ScriptFileName.c_str(), ClassName.c_str());

			return NULL;
		};

		SuperSmartPointer<Entity> Out(new Entity());
		Out->ID = MakeStringID(Name);
		Out->Name = Name;

		Out->Properties = luabind::newtable(Owner->ScriptInstance->State);

		Entities[Out->ID] = Out;

		luabind::object Components = Object["Components"];

		if(Components)
		{
			for(luabind::iterator it(Components), end; it != end; it++)
			{
				std::string ComponentName = ProtectedLuaCast<const char *>(*it);

				if(!Out->AddComponent(ComponentName))
				{
					Entities.erase(Out->ID);

					Out.Dispose();

					Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': Scripting error: Component '%s' not found", ScriptFileName.c_str(), ComponentName.c_str());

					return NULL;
				};
			};
		};

		luabind::object OnStartFunction = Object["OnStart"];

		if(OnStartFunction)
		{
			try
			{
				OnStartFunction(Out);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr(TAG, "Unable to load Entity Script '%s': Scripting error: '%s'", ScriptFileName.c_str(), e.what());

				Entities.erase(Entities.find(Out->ID));

				return NULL;
			};
		};

		return Entities[Out->ID];
	};

	void World::IterateEntitiesWithComponents(luabind::object ComponentNames, luabind::object Function)
	{
		static std::vector<std::string> ComponentIDs;
		ComponentIDs.resize(0);

		for(luabind::iterator it(ComponentNames), end; it != end; it++)
		{
			ComponentIDs.push_back(ProtectedLuaCast<const char *>(*it));
		};

		if(ComponentIDs.size() == 0)
			return;

		for(EntityMap::iterator it = Entities.begin(); it != Entities.end(); it++)
		{
			bool Found = true;

			for(uint32 j = 0; j < ComponentIDs.size(); j++)
			{
				if(!it->second->HasComponent(ComponentIDs[j]))
				{
					Found = false;

					break;
				};
			};

			if(!Found)
				continue;

			try
			{
				Function(it->second);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr(TAG, "While iterating entities: Scripting Error: '%s'", e.what());
			};
		};
	};

	void World::Clear()
	{
		Entities.clear();
		Components.clear();
		ComponentArchetypes.clear();
		ComponentEntityCache.clear();
	};

	void World::Update()
	{
		for(ComponentEntityMap::iterator it = ComponentEntityCache.begin(); it != ComponentEntityCache.end(); it++)
		{
			for(std::list<StringID>::iterator eit = it->second.begin(); eit != it->second.end(); eit++)
			{
				Component *TheComponent = Components[*eit][it->first];

				if(TheComponent->OnUpdateFunction)
				{
					try
					{
						TheComponent->OnUpdateFunction(TheComponent, Entities[*eit].Get());
					}
					catch(std::exception &e)
					{
						Log::Instance.LogErr(TAG, "While processing component '%s': Scripting Error: '%s'", TheComponent->Name.c_str(), e.what());
					};
				};
			};
		};
	};
};

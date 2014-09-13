#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "StateManager"

	StateManager StateManager::Instance;

	struct StateHeader
	{
		uint32 Signature; //'F' 'T' 'G' 'S'
		uint32 Version;
		uint32 Count;
	};

	bool StateCollection::Serialize(Stream *Out)
	{
		StateHeader Header;
		Header.Signature = CoreUtils::MakeIntFromBytes('F', 'T', 'G', 'S');
		Header.Version = CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR);
		Header.Count = Variables.size();

		SFLASSERT(Out->Write2<StateHeader>(&Header));

		for(VariableMap::iterator it = Variables.begin(); it != Variables.end(); it++)
		{
			SFLASSERT(Out->Write2<StringID>(&it->first));

			switch(it->second.Type)
			{
			case StateVariableType::String:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));

				{
					uint32 Length = it->second.StringValue.length();

					SFLASSERT(Out->Write2<uint32>(&Length));
					
					if(Length)
					{
						SFLASSERT(Out->Write2<char>(it->second.StringValue.c_str(), Length));
					};
				};

				break;
			case StateVariableType::Int:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));
				SFLASSERT(Out->Write2<int32>(&it->second.IntValue));

				break;
			case StateVariableType::Float:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));
				SFLASSERT(Out->Write2<f32>(&it->second.FloatValue));

				break;
			case StateVariableType::Stream:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));

				{
					uint32 Length = (uint32)it->second.StreamValue.Length();
					SFLASSERT(Out->Write2<uint32>(&Length));
					SFLASSERT(Out->Write2<uint8>((uint8 *)it->second.StreamValue.Get(), Length));
				};

				break;
			case StateVariableType::Vector2:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));
				SFLASSERT(Out->Write2<Vector2>(&it->second.Vector2Value));

				break;
			case StateVariableType::Vector3:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));
				SFLASSERT(Out->Write2<Vector3>(&it->second.Vector3Value));

				break;
			case StateVariableType::Vector4:
				SFLASSERT(Out->Write2<uint8>(&it->second.Type));
				SFLASSERT(Out->Write2<Vector4>(&it->second.Vector4Value));

				break;
			default:
				{
					uint8 Dummy = StateVariableType::Count;
					SFLASSERT(Out->Write2<uint8>(&Dummy));
				};
				break;
			};
		};

		return true;
	};

	bool StateCollection::DeSerialize(Stream *In)
	{
		StateHeader Header;

		SFLASSERT(In->Read2<StateHeader>(&Header));

		if(Header.Signature != CoreUtils::MakeIntFromBytes('F', 'T', 'G', 'S') ||
			Header.Version != CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR))
			return false;

		for(uint32 i = 0; i < Header.Count; i++)
		{
			StringID ID;
			StateVariable Variable;

			SFLASSERT(In->Read2<StringID>(&ID));
			SFLASSERT(In->Read2<uint8>(&Variable.Type));

			switch(Variable.Type)
			{
			case StateVariableType::String:
				{
					uint32 Length = 0;

					SFLASSERT(In->Read2<uint32>(&Length));

					if(Length)
					{
						Variable.StringValue.resize(Length);
						SFLASSERT(In->Read2<char>(&Variable.StringValue[0], Length));
					};
				};

				break;
			case StateVariableType::Int:
				SFLASSERT(In->Read2<int32>(&Variable.IntValue));

				break;
			case StateVariableType::Float:
				SFLASSERT(In->Read2<f32>(&Variable.FloatValue));

				break;
			case StateVariableType::Stream:
				{
					uint32 Length = 0;
					SFLASSERT(In->Read2<uint32>(&Length));
					SFLASSERT(In->WriteToStream(&Variable.StreamValue, Length));
				};

				break;
			case StateVariableType::Vector2:
				SFLASSERT(In->Read2<Vector2>(&Variable.Vector2Value));

				break;
			case StateVariableType::Vector3:
				SFLASSERT(In->Read2<Vector3>(&Variable.Vector3Value));

				break;
			case StateVariableType::Vector4:
				SFLASSERT(In->Read2<Vector4>(&Variable.Vector4Value));

				break;
			default:
				continue;
			};

			Variables[ID] = Variable;
		};

		return true;
	};

	bool StateManager::Load(const std::string &Directory, const std::string &CollectionName)
	{
		std::string ActualName = FileSystemUtils::PreferredStorageDirectory() + "/" + Directory + "/States/" + StringUtils::MakeIntString(MakeStringID(CollectionName), true) + ".bin";

		FileStream Stream;
		
		if(!Stream.Open(ActualName, StreamFlags::Read))
			return false;

		StateCollection Collection;
		Collection.Directory = Directory;

		if(!Collection.DeSerialize(&Stream))
			return false;

		Collections[MakeStringID(CollectionName)] = Collection;

		Log::Instance.LogInfo(TAG, "Loaded collection '%s/%s' (0x%08x)!", Directory.c_str(), CollectionName.c_str(), MakeStringID(CollectionName));

		return true;
	};

	void StateManager::Save()
	{
		for(CollectionMap::iterator it = Collections.begin(); it != Collections.end(); it++)
		{
			FileStream Stream;

			FileSystemUtils::CreateDirectory(FileSystemUtils::PreferredStorageDirectory() + "/" + it->second.Directory);
			FileSystemUtils::CreateDirectory(FileSystemUtils::PreferredStorageDirectory() + "/" + it->second.Directory + "/States");

			if(!Stream.Open(FileSystemUtils::PreferredStorageDirectory() + "/" + it->second.Directory + "/States/" + StringUtils::MakeIntString(it->first, true) + ".bin", StreamFlags::Write))
				continue;

			if(!it->second.Serialize(&Stream))
			{
				Log::Instance.LogErr(TAG, "Unable to serialize a state file '%s'!", StringUtils::MakeIntString(it->first, true).c_str());

				continue;
			};

			Log::Instance.LogInfo(TAG, "Saved collection '0x%08x'!", it->first);
		};
	};
};

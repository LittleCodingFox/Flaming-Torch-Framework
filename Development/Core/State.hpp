#pragma once

namespace StateVariableType
{
	enum
	{
		String,
		Int,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Stream,
		Count
	};
};

class StateVariable
{
public:
	uint8 Type;

	std::string StringValue;
	int32 IntValue;
	f32 FloatValue;
	Vector2 Vector2Value;
	Vector3 Vector3Value;
	Vector4 Vector4Value;
	MemoryStream StreamValue;

	StateVariable() : Type(0), IntValue(0), FloatValue(0) {};
};

class StateCollection
{
public:
	typedef std::map<StringID, StateVariable> VariableMap;
	VariableMap Variables;
	std::string Directory;

	bool Serialize(Stream *Out);
	bool DeSerialize(Stream *In);

	StateVariable &GetVariable(const std::string &Name)
	{
		return Variables[MakeStringID(Name)];
	};
};

class StateManager
{
public:
	typedef std::map<StringID, StateCollection> CollectionMap;

	static StateManager Instance;

	CollectionMap Collections;

	bool Load(const std::string &Directory, const std::string &CollectionName);
	void Save();
};

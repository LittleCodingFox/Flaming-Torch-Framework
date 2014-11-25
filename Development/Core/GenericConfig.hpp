#pragma once
namespace ValueTypes
{
	enum
	{
		Unknown = 0,
		Int32,
		F32,
	};
};

/*!
*	INI-style configuration class
*/
class GenericConfig 
{
public:
	class Value 
	{
	public:
		std::string Content;
		uint32 Type;

		Value();
	};

	class IntValue : public Value
	{
	public:
		int32 Value;

		IntValue();
		IntValue(int32 Value);
		IntValue(const char *StringValue);
	};

	class FloatValue : public Value
	{
	public:
		f32 Value;

		FloatValue();
		FloatValue(f32 Value);
		FloatValue(const char *StringValue);
	};

	class Section
	{
	public:
		typedef std::map<std::string, Value> ValueMap;
		ValueMap Values;
	};

	typedef std::map<std::string, Section> SectionMap;
	SectionMap Sections;

	uint32 CRCValue;
public:

	GenericConfig();

	/*!
	*	writes the configuration to a stream
	*/
	bool Serialize(Stream *Out);
	/*!
	*	Reads the configuration from a stream
	*/
	bool DeSerialize(Stream *In);

	/*!
	*	\return the CRC content of this GenericConfig
	*/
	uint32 CRC() const;

	/*!
	*	Gets a value
	*	\param SectionName the name of the section the value is in
	*	\param ValueName the name of the value
	*	\param Default the default value on failure
	*/
	Value GetValue(const char *SectionName, const char *ValueName, const Value &Default);
	/*!
	*	Gets an int value
	*	\param SectionName the name of the section the value is in
	*	\param ValueName the name of the value
	*	\param Default the default value on failure
	*/
	int32 GetInt(const char *SectionName, const char *ValueName, int32 Default);
	/*!
	*	Gets a float value
	*	\param SectionName the name of the section the value is in
	*	\param ValueName the name of the value
	*	\param Default the default value on failure
	*/
	f32 GetFloat(const char *SectionName, const char *ValueName, f32 Default);
	/*!
	*	Gets a string value
	*	\param SectionName the name of the section the value is in
	*	\param ValueName the name of the value
	*	\param Default the default value on failure
	*/
	std::string GetString(const char *SectionName, const char *ValueName, const std::string &Default = "");
	/*!
	*	Sets a value
	*	\param SectionName the name of the section the value is in
	*	\param ValueName the name of the value
	*	\param Content the content of the value
	*/
	void SetValue(const char *SectionName, const char *ValueName, const char *Content);
};

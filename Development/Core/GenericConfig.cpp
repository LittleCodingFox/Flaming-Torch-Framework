#include "FlamingCore.hpp"
namespace FlamingTorch
{
	GenericConfig::GenericConfig() : CRCValue(0)
	{
	};

	GenericConfig::Value::Value() : Type(ValueTypes::Unknown) {};

	GenericConfig::IntValue::IntValue() : Value()
	{
		Type = ValueTypes::Int32;
		Value = 0xBAADF00D;
	};

	GenericConfig::IntValue::IntValue(int32 Value) : Value()
	{
		Type = ValueTypes::Int32;
		this->Value = Value;
		Content.resize(256);
		sprintf(&Content[0], "%d", Value);
	};

	GenericConfig::IntValue::IntValue(const char *ValueString) : Value()
	{
		Type = ValueTypes::Int32;

		if(sscanf(ValueString, "%d", &Value) <= 0)
		{
			Value = 0xBAADF00D;
			Content.resize(256);
			sprintf(&Content[0], "%d", Value);
		};
	};

	GenericConfig::FloatValue::FloatValue() : Value()
	{
		Type = ValueTypes::F32;
		Value = (f32)0xBAADF00D;
	};

	GenericConfig::FloatValue::FloatValue(f32 Value) : Value()
	{
		Type = ValueTypes::F32;
		this->Value = Value;
		Content.resize(256);
		sprintf(&Content[0], "%f", Value);
	};

	GenericConfig::FloatValue::FloatValue(const char *ValueString) : Value()
	{
		Type = ValueTypes::F32;

		if(sscanf(ValueString, "%f", &Value) <= 0)
		{
			Value = (f32)0xBAADF00D;
			Content.resize(256);
			sprintf(&Content[0], "%f", Value);
		};
	};

	GenericConfig::Value GenericConfig::GetValue(const char *SectionName, const char *ValueName, const Value &Default)
	{
		SectionMap::iterator it = Sections.find(SectionName);

		if(it == Sections.end())
			return Default;

		Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

		if(vit == it->second.Values.end())
			return Default;

		return vit->second;
	};

	int32 GenericConfig::GetInt(const char *SectionName, const char *ValueName, int32 Default)
	{
		SectionMap::iterator it = Sections.find(SectionName);

		if(it == Sections.end())
			return Default;

		Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

		if(vit == it->second.Values.end())
			return Default;

		IntValue Out(vit->second.Content.c_str());

		return Out.Value != 0xBAADF00D ? Out.Value : Default;
	};

	f32 GenericConfig::GetFloat(const char *SectionName, const char *ValueName, f32 Default)
	{
		SectionMap::iterator it = Sections.find(SectionName);

		if(it == Sections.end())
			return Default;

		Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

		if(vit == it->second.Values.end())
			return Default;

		FloatValue Out(vit->second.Content.c_str());

		return Out.Value != 0xBAADF00D ? Out.Value : Default;
	};

	void GenericConfig::SetValue(const char *SectionName, const char *ValueName, const char *Content)
	{
		Sections[SectionName].Values[ValueName].Content = Content;
	};

	bool GenericConfig::Serialize(Stream *Out)
	{
		FLASSERT(Out, "Invalid Stream!");

		if(!Out)
			return false;

		std::stringstream str;

		for(SectionMap::iterator it = Sections.begin(); it != Sections.end(); it++)
		{
			str << "[" << it->first << "]\n";

			for(Section::ValueMap::iterator vit = it->second.Values.begin(); vit != it->second.Values.end(); vit++)
			{
				str << vit->first << "=" << vit->second.Content << "\n";
			};
		};

		std::string Final = str.str();

		SFLASSERT(Out->Write2<char>(Final.c_str(), Final.length()));

		return true;
	};

	uint32 GenericConfig::CRC() const
	{
		return CRCValue;
	};

	bool GenericConfig::DeSerialize(Stream *In)
	{
		FLASSERT(In, "Invalid Stream!");

		if(!In)
			return false;

		CRCValue = In->CRC();

		Sections.clear();

		std::string str = StringUtils::Strip(In->AsString(), '\r');
		std::vector<std::string> Lines = StringUtils::Split(str, '\n');

		std::string ActiveSection;

		for(uint32 i = 0; i < Lines.size(); i++)
		{
			if(Lines[i].length() == 0 || Lines[i][0] == '#')
				continue;

			if(Lines[i][0] == '[')
			{
				if(Lines[i][Lines[i].length() - 1] != ']')
				{
					Log::Instance.LogWarn("GenericConfig", "Mismatch section header '%s'!", Lines[i].c_str());

					ActiveSection = std::string();

					continue;
				};

				ActiveSection = StringUtils::Trim(Lines[i].substr(1, Lines[i].length() - 2));
			}
			else if(ActiveSection.length())
			{
				int32 Index = Lines[i].find('=');

				if(Index == std::string::npos)
				{
					Log::Instance.LogWarn("GenericConfig", "Mismatch setting line '%s'!", Lines[i].c_str());

					continue;
				};

				Sections[ActiveSection].Values[StringUtils::Trim(Lines[i].substr(0, Index))].Content = StringUtils::Trim(Lines[i].substr(Index + 1));
			};
		};

		return true;
	};

	std::string GenericConfig::GetString(const char *SectionName, const char *ValueName, const std::string &Default)
	{
		Value DefaultValue;
		DefaultValue.Content = Default;

		return GetValue(SectionName, ValueName, DefaultValue).Content;
	};
};

#include "FlamingCore.hpp"
namespace FlamingTorch
{
	std::vector<std::string> StringUtils::Split(const std::string &String, char Separator)
	{
		static std::vector<std::string> Fragments;
		Fragments.resize(0);

		int32 Offset = 0;
		int32 Index = String.find(Separator);
	
		while(Index != std::string::npos)
		{
			Fragments.push_back(String.substr(Offset, Index - Offset));
			Offset = Index + 1;

			Index = String.find(Separator, Offset);
		};

		if(String.length() > (uint32)Offset)
			Fragments.push_back(String.substr(Offset));

		return Fragments;
	};

	std::string StringUtils::Strip(const std::string &str, char c)
	{
		static std::vector<char> Out;
		Out.resize(0);

		for(uint32 i = 0; i < str.length(); i++)
		{
			if(str[i] == c)
				continue;

			Out.push_back(str[i]);
		};

		Out.push_back(0);

		return &Out[0];
	};

	std::string StringUtils::Trim(const std::string &str)
	{
		int32 StartIndex = 0, EndIndex = str.length() - 1;

		if(EndIndex == -1)
			return std::string();

		while(StartIndex < (int32)str.length())
		{
			if(!isspace((char)str[StartIndex]))
				break;

			StartIndex++;
		};

		if(StartIndex >= (int32)str.length())
			return std::string();

		while (EndIndex >= 0)
		{
			if (!isspace((char)str[EndIndex]))
				break;

			EndIndex--;
		};

		if (EndIndex < 0)
			return std::string();

		return str.substr(StartIndex, str.length() - EndIndex - StartIndex);
	};

	std::string StringUtils::Join(const std::string &Separator, const std::vector<std::string> &Fragments)
	{
		if(Fragments.size() == 0)
			return "";

		static std::stringstream str;
		str.str(Fragments[0]);

		for(uint32 i = 1; i < Fragments.size(); i++)
		{
			str << Separator << Fragments[i];
		};

		return str.str();
	};

	uint32 StringUtils::HexToInt(const std::string &str)
	{
		uint32 Temp = 0;

		if(1 != sscanf(str.c_str(), "%x", &Temp))
			return 0;

		return Temp;
	};

	f32 StringUtils::HexToFloat(const std::string &str)
	{
		uint32 Temp = 0;

		if(1 != sscanf(str.c_str(), "%x", &Temp))
			return 0;

		return Temp / 255.f;
	};

	std::string StringUtils::MakeIntString(const int32 &i, bool Hex)
	{
		static std::stringstream str;
		str.str("");

		if(Hex)
		{
			str << std::hex;
		}
		else
		{
			str << std::dec;
		};

		str << i << std::dec;

		return str.str();
	};

	std::string StringUtils::MakeIntString(const uint32 &i, bool Hex)
	{
		static std::stringstream str;
		str.str("");

		if(Hex)
		{
			str << std::hex;
		}
		else
		{
			str << std::dec;
		};

		str << i << std::dec;

		return str.str();
	};
    
	std::string StringUtils::MakeIntString(const int64 &i, bool Hex)
	{
		static std::stringstream str;
		str.str("");
        
		if(Hex)
		{
			str << std::hex;
		}
		else
		{
			str << std::dec;
		};
        
		str << i << std::dec;
        
		return str.str();
	};
    
	std::string StringUtils::MakeIntString(const uint64 &i, bool Hex)
	{
		static std::stringstream str;
		str.str("");
        
		if(Hex)
		{
			str << std::hex;
		}
		else
		{
			str << std::dec;
		};
        
		str << i << std::dec;
        
		return str.str();
	};

	std::string StringUtils::MakeFloatString(const f32 &f)
	{
		static std::stringstream str;
		str.str("");

		str << f;

		return str.str();
	};

	std::string StringUtils::MakeByteString(const uint8 &u)
	{
		std::stringstream str;
		str << (uint32)u;

		return str.str();
	};

	std::string StringUtils::ToUpperCase(const std::string &str)
	{
		std::string Out = str;

		std::transform(str.begin(), str.end(), Out.begin(), toupper);

		return Out;
	};

	std::string StringUtils::ToLowerCase(const std::string &str)
	{
		std::string Out = str;

		std::transform(str.begin(), str.end(), Out.begin(), tolower);

		return Out;
	};

	std::string StringUtils::Replace(const std::string &str, const std::string &Find, const std::string &Replace)
	{
		if(Find.length() == 0)
			return str;

		static std::stringstream Out;
		Out.str("");

		uint32 Previous = 0;
		int32 Index = str.find(Find);

		while(Index != -1)
		{
			Out << str.substr(Previous, Index - Previous) << Replace;

			Index += Find.length();

			Previous = Index;
			Index = str.find(Find, Index);
		};

		if(Index != str.length())
		{
			Out << str.substr(Previous);
		};

		return Out.str();
	};
};

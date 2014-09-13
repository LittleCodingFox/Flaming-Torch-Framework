#include "FlamingCore.hpp"
namespace FlamingTorch
{
	class StringIDManager
	{
	public:
		std::map<StringID, std::string> StringIDMap;

		void Add(const std::string &Name, StringID Hash)
		{
			std::map<StringID, std::string>::iterator it = StringIDMap.find(Hash);

			if(it == StringIDMap.end())
			{
				StringIDMap[Hash] = Name;
			};
		};

		const std::string &Get(StringID Hash)
		{
			std::map<StringID, std::string>::iterator it = StringIDMap.find(Hash);

			if(it == StringIDMap.end())
			{
				static std::string Empty;

				return Empty;
			};
			
			return it->second;
		};

		static StringIDManager Instance;
	};

	StringIDManager StringIDManager::Instance;

	StringID MakeStringID(const std::string &Name)
	{
		StringID Hash = CRC32::Instance.CRC((uint8*)Name.c_str(), Name.length());

		StringIDManager::Instance.Add(Name, Hash);

		return Hash;
	};

	const std::string &GetStringIDString(StringID ID)
	{
		return StringIDManager::Instance.Get(ID);
	};
};

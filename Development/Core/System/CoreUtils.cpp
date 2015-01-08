#include "FlamingCore.hpp"

namespace FlamingTorch
{
	std::string CoreUtils::MakeVersionString(uint8 a, uint8 b)
	{
		std::stringstream str;
		str << (uint32)a << "." << (uint32)b;

		return str.str();
	}
}

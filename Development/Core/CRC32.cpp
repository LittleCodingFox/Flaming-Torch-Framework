#include "FlamingCore.hpp"

namespace FlamingTorch
{
	CRC32 CRC32::Instance;

	CRC32::CRC32()
	{
		uint32 Poly = 0x04C11DB7;
        uint32 CRC;

		for(uint32 i = 0; i <= 0xFF; i++)
		{
			uint32 &Value = LookupTable[i];

            CRC = i << 24;
            
            for(uint32 j = 0; j < 8; j++)
            {
                CRC = (CRC & 0x80000000) != 0 ? (CRC << 1 ^ Poly) : (CRC << 1);
            };

			Value = CRC;
		};
	};

	uint32 CRC32::Reflect(uint32 Reflect, char Char)
	{
		uint32 Value = 0;

		for(uint32 i = 1; i < (uint8)(Char + 1); i++)
		{
			if(Reflect & 1)
			{
				Value |= (1 << (Char - i));
			};

			Reflect >>= 1;
		};

		return Value;
	};

	uint32 CRC32::CRC(const uint8 *Data, uint32 DataLength)
	{
		uint32 OutCRC = 0xFFFFFFFF;

		while(DataLength--)
			OutCRC = (OutCRC >> 8) ^ LookupTable[(OutCRC & 0xFF) ^ *Data++];

		return OutCRC ^ 0xFFFFFFFF;
	};

	uint32 CRC32::IterateCRC(const uint8 *Data, uint32 DataLength, uint32 PreviousCRC)
	{
		uint32 OutCRC = PreviousCRC;

		while (DataLength--)
			OutCRC = (OutCRC >> 8) ^ LookupTable[(OutCRC & 0xFF) ^ *Data++];

		return OutCRC;
	};

	uint32 CRC32::FinishCRCIteration(uint32 PreviousCRC)
	{
		return PreviousCRC ^ 0xFFFFFFFF;
	};
};

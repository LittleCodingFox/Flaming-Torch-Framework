#pragma once
/*!
*	CRC32 Helper Class
*/
class FLAMING_API CRC32 
{
private:
	uint32 LookupTable[256];
	uint32 Reflect(uint32 Reflect, char Char);
public:
	static CRC32 Instance;

	CRC32();
	
	/*!
	*	Calculates the CRC of a piece of data
	*	\param Data the Data to calculate
	*	\param DataLength the Length in bytes of the Data
	*	\return the CRC as a uint32
	*/
	uint32 CRC(const uint8 *Data, uint32 DataLength);
};

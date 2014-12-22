#pragma once
/*!
*	CRC32 Helper Class
*/
class CRC32 
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

	/*!
	*	Calculates the CRC of a piece of data over several iterations
	*	\param Data the Data to calculate
	*	\param DataLength the Length in bytes of the Data
	*	\return the partial CRC as a uint32. Must use FinishCRCIteration to complete the CRC
	*	\param PreviousCRC the previously calculated CRC, or 0xFFFFFFFF
	*	\sa FinishCRCIteration
	*/
	uint32 IterateCRC(const uint8 *Data, uint32 DataLength, uint32 PreviousCRC = 0xFFFFFFFF);

	/*!
	*	Completes the CRC Iterations that were previously processed
	*	\param PreviousCRC the previously calculated CRC
	*/
	uint32 FinishCRCIteration(uint32 PreviousCRC);
};

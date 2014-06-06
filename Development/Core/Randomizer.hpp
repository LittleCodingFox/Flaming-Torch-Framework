#pragma once
/*!
	A Random Number Generator class
*/
class Randomizer
{
public:
	/*!
	*	Generates a random value between Min and Max f32 values
	*	\param Min the minimum value
	*	\param Max the maximum value
	*	\return a random value between Min and Max
	*/
	static f32 RandomFloat(f32 Min, f32 Max);
	/*!
	*	Generates a random value between Min and Max int32 values
	*	\param Min the minimum value
	*	\param Max the maximum value
	*	\return a random value between Min and Max
	*/
	static int32 RandomInt(int32 Min, int32 Max);
	/*!
	*	Generates a random value between 0 and 1 as a float32
	*	\return the random value
	*/
	static f32 RandomNormalized();
	/*!
	*	Generates a random point within a sphere
	*/
	static Vector3 RandomNormalizedSphere();
	/*!
	*	Sets the Random Seed
	*	\param Seed the Random Seed
	*/
	static void SetSeed(int32 Seed);
	/*!
	*	Gets the Random Seed
	*	\return the Random Seed
	*/
	static int32 GetSeed();
};

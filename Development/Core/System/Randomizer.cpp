#include "FlamingCore.hpp"
#include <float.h>
#include <time.h>
namespace FlamingTorch
{
#	define TAG "Randomizer"

	int32 RandomSeed()
	{
		int32 Seed = (int32)time(NULL);
		srand(Seed);

		return Seed;
	}

	int32 Seed = RandomSeed();

	f32 Randomizer::RandomFloat(f32 Min, f32 Max)
	{
		if (Max - Min < 0)
		{
			Log::Instance.LogWarn(TAG, "Invalid Min/Max: (%f, %f)", Min, Max);

			return 0;
		}

	    return static_cast<float>(rand()) / RAND_MAX * (Max - Min) + Min;
	}

	int32 Randomizer::RandomInt(int32 Min, int32 Max)
	{
		if (Max - Min < 0)
		{
			Log::Instance.LogWarn(TAG, "Invalid Min/Max: (%d, %d)", Min, Max);

			return 0;
		}

	    return rand() % (Max - Min + 1) + Min;
	}

	f32 Randomizer::RandomNormalized()
	{
		return RandomFloat(FLT_MIN, FLT_MAX) / (f32)FLT_MAX;
	}

	Vector3 Randomizer::RandomNormalizedSphere()
	{
		f32 Theta = 2 * (f32)MathUtils::Pi * RandomNormalized();
		f32 Phi = acosf(2 * RandomNormalized() - 1.f);
		f32 CosTheta = cosf(Theta), CosPhi = cosf(Phi), SinTheta = sinf(Theta), SinPhi = sinf(Phi);

		return Vector3(CosTheta * SinPhi, SinTheta * SinPhi, CosPhi);
	}

	void Randomizer::SetSeed(int32 _Seed)
	{
		Seed = _Seed;
		srand(_Seed);
	}

	int32 Randomizer::GetSeed()
	{
		return Seed;
	}
}

#include "FlamingCore.hpp"
namespace FlamingTorch
{
	/*
	*	Perlin Noise source code copied from http://infohost.nmt.edu/~imcgarve/perlin_noise/
	*	Since no license was found, the code was considered under public domain by me.
	*	If there is any problem at all, please do contact me.
	*/

	f32 PerlinNoise::Noise(int32 x, int32 y)
	{
		int32 n = x + y * 57 + seed;
		n = (n << 13) ^ n;
		return (f32) (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		//return value is always in range [-1.0, 1.0]
	}

	f32 PerlinNoise::SmoothNoise(int32 x, int32 y)
	{
		f32 corners = (Noise(x-1, y-1) + Noise(x+1, y-1) + Noise(x-1, y+1) + Noise(x+1, y+1)) / 16;
		f32 sides   = (Noise(x-1, y) + Noise(x+1, y) + Noise(x, y-1) + Noise(x, y+1)) / 8;
		f32 center  =  Noise(x, y ) / 4;
		return corners + sides + center;
	}

	f32 PerlinNoise::Interpolate(f32 a, f32 b, f32 x)
	{
		f32 ft = x * 3.1415927f;
		f32 f = (1 - std::cos(ft)) * .5f;

		return  a*(1-f) + b*f;
	}

	f32 PerlinNoise::InterpolatedNoise(f32 x, f32 y)
	{
		int32 wholePartX = (int32) x;
		f32 fractionPartX = x - wholePartX;

		int32 wholePartY = (int32) y;
		f32 fractionPartY = y - wholePartY;

		f32 v1 = SmoothNoise(wholePartX, wholePartY);
		f32 v2 = SmoothNoise(wholePartX + 1, wholePartY);
		f32 v3 = SmoothNoise(wholePartX, wholePartY + 1);
		f32 v4 = SmoothNoise(wholePartX + 1, wholePartY + 1);

		f32 i1 = Interpolate(v1, v2, fractionPartX);
		f32 i2 = Interpolate(v3, v4, fractionPartX);

		return Interpolate(i1, i2, fractionPartY);
	}

	void PerlinNoise::Generate(uint32 * pData, f32 persistence, int32 octaves, uint32 width, uint32 height, f32 red, f32 green, f32 blue, 
		int32 seed, f32 zoom)
	{	
		PerlinNoise::seed = seed;

		for (uint32 y = 0; y < height; ++y)
		{
			for (uint32 x = 0; x < width; ++x)
			{
				f32 total = 0;
				f32 frequency = 1;
				f32 amplitude = 1;

				for (int32 i = 0; i < octaves; ++i)
				{
					total = total + InterpolatedNoise(x * frequency / zoom, y * frequency / zoom) * amplitude;
					frequency *= 2;
					amplitude *= persistence;
				}

				pData[y * width + x] = (255 << 24) | ((uint8) (red * (total + 1) * 127.5) << 16) | ((uint8) (green * (total + 1) * 127.5) << 8) | 
					(uint8) (blue * (total + 1) * 127.5);
			}
		}
	}

	void PerlinNoise::GenerateNormalized(uint32 * pData, f32 persistence, int32 octaves, uint32 width, uint32 height, f32 red, f32 green, 
		f32 blue, int32 seed, f32 zoom)
	{	
		PerlinNoise::seed = seed;

		f32 min = 0;
		f32 max = 0;
		f32 maxColorMultiplier;
		std::vector<f32> pDataf32(width * height);

		//Generate raw f32 data
		for (uint32 y = 0; y < height; ++y)
		{
			for (uint32 x = 0; x < width; ++x)
			{
				f32 total = 0;
				f32 frequency = 1;
				f32 amplitude = 1;

				for (int32 i = 0; i < octaves; ++i)
				{
					total = total + InterpolatedNoise(x * frequency / zoom, y * frequency / zoom) * amplitude;
					frequency *= 2;
					amplitude *= persistence;
				}

				pDataf32[y * width + x] = total;

				min = total < min ? total : min;
				max = total > max ? total : max;
			}
		}

		//Normalize color multipliers
		maxColorMultiplier = red > green ? red : green;
		maxColorMultiplier = blue > maxColorMultiplier ? blue : maxColorMultiplier;
		red /= maxColorMultiplier;
		green /= maxColorMultiplier;
		blue /= maxColorMultiplier;

		//Normalize raw f32s, factor in color multipliers, and convert to bitmap color format
		for (uint32 i = 0; i < width * height; ++i)
		{
			pData[i] = (255 << 24) | ((uint8) (red * ((pDataf32[i] - min) / (max - min)) * 255) << 16) | 
				((uint8) (green * ((pDataf32[i] - min) / (max - min)) * 255) << 8) | (uint8) (blue * ((pDataf32[i] - min) / (max - min)) * 255);
		}
	}
};

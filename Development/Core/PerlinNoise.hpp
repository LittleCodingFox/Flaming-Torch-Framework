#pragma once

class PerlinNoise
{
public:
	int32 seed;

	PerlinNoise() : seed(0) {};
	PerlinNoise(int32 Seed) : seed(Seed) {};

	void Generate(uint32 *pData, f32 persistence, int32 octaves, uint32 width, uint32 height, f32 red, f32 green, f32 blue, int32 seed, 
		f32 zoom);
	void GenerateNormalized(uint32 *pData, f32 persistence, int32 octaves, uint32 width, uint32 height, f32 red, f32 green, f32 blue, 
		int32 seed, f32 zoom);

	f32 Noise(int32 x, int32 y);
	f32 SmoothNoise(int32 x, int32 y);
	f32 Interpolate(f32 a, f32 b, f32 x);
	f32 InterpolatedNoise(f32 x, f32 y);
};

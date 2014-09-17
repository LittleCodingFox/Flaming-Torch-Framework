#pragma once

class TextureCollision
{
public:
	std::vector<Vector2> Polygon;
	Vector2 Size;

	/*
		Example of how to dump the collision info into the texture for debugging purposes:

		TextureBuffer Buffer = *TheTexture->GetData();

		for(uint32 i = 0; i < Collision.Polygon.size(); i++)
		{
			uint32 Index = Buffer.Width() * Collision.Polygon[i].y * 4 + Collision.Polygon[i].x * 4;

			Buffer.Data[Index] = 255;
			Buffer.Data[Index + 1] = 255;
			Buffer.Data[Index + 2] = 255;
		};

		Buffer.Save("Collision.png");
	*/
	static TextureCollision FromTexture(Texture *TheTexture, uint8 MinTransparency = 255);

#if USE_GRAPHICS
	void Draw(Renderer *Renderer);
#endif

	TextureCollision ApplyTransform(const Matrix4x4 &Transform);
};

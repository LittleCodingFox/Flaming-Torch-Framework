#include "FlamingCore.hpp"
namespace FlamingTorch
{
	TextureCollision TextureCollision::FromTexture(Texture *TheTexture, uint8 MinTransparency)
	{
		TextureCollision Out;

		std::vector<Vector2> FoundPoints;

		bool InCollision = false;

		uint8 *Ptr = &TheTexture->GetData()->Data[0];
		uint32 RowSize = TheTexture->Width() * sizeof(uint8[4]);
		AxisAlignedBoundingBox AABB;

		Vector2 HalfTextureSize(TheTexture->Width() / 2.f, TheTexture->Height() / 2.f);

		for(uint32 y = 0; y < TheTexture->Height(); y++)
		{
			InCollision = false;

			for(uint32 x = 0; x < TheTexture->Width(); x++, Ptr+=4)
			{
				if(Ptr[3] >= MinTransparency && ((x + 1 < TheTexture->Width() && (Ptr + 4)[3] < MinTransparency) || x == 0 || x == TheTexture->Width() - 1 || (x > 0 && (Ptr - 1)[0] < MinTransparency) ||
					(y + 1 < TheTexture->Height() && (Ptr + RowSize)[3] < MinTransparency) || y == 0 || y == TheTexture->Height() - 1 || (y > 0 && (Ptr - RowSize)[3] < MinTransparency)))
				{
					AABB.Calculate(Vector2((f32)x, (f32)y));
					FoundPoints.push_back(Vector2((f32)x - HalfTextureSize.x, (f32)y - HalfTextureSize.y));
				};
			};
		};

		//TODO: Polygonize

		Out.Polygon = FoundPoints;
		Out.Size = (AABB.max - AABB.min).ToVector2();

		return Out;
	};

#if USE_GRAPHICS
	struct TextureCollisionVertex
	{
		Vector2 Position;
	};

	VertexElementDescriptor TextureCollisionVertexDescriptor[] = {
		{ 0, VertexElementType::Position, VertexElementDataType::Float2 }
	};

	void TextureCollision::Draw(Renderer *Renderer)
	{
		if(!Polygon.size())
			return;

		SpriteCache::Instance.Flush(Renderer);

		VertexBufferHandle Handle = Renderer->CreateVertexBuffer();

		if(!Handle)
			return;

		Renderer->SetVertexBufferData(Handle, VertexDetailsMode::Mixed, TextureCollisionVertexDescriptor, 1, &Polygon[0], sizeof(Vector2) * Polygon.size());

		Renderer->RenderVertices(VertexModes::Points, Handle, 0, Polygon.size());

		Renderer->DestroyVertexBuffer(Handle);
	};
#endif

	TextureCollision TextureCollision::ApplyTransform(const Matrix4x4 &Transform)
	{
		TextureCollision Out = *this;

		for(uint32 i = 0; i < Out.Polygon.size(); i++)
		{
			Out.Polygon[i] = (Vector3(Out.Polygon[i]) * Transform).ToVector2();
		};

		return Out;
	};
};

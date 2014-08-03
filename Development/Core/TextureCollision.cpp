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
	void TextureCollision::Draw(RendererManager::Renderer *Renderer)
	{
		if(!Polygon.size())
			return;

		SpriteCache::Instance.Flush(Renderer);

		Renderer->BindTexture(NULL);
		Renderer->EnableState(GL_VERTEX_ARRAY);
		Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
		Renderer->DisableState(GL_NORMAL_ARRAY);
		Renderer->DisableState(GL_COLOR_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, &Polygon[0]);

		glDrawArrays(GL_POINTS, 0, Polygon.size());
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

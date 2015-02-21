#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	struct SpriteWireframeVertex
	{
		Vector2 Position;
		Vector4 Color;
	};

	VertexElementDescriptor SpriteWireframeVertexDescriptor[] = {
		{ 0, VertexElementType::Position, VertexElementDataType::Float2 },
		{ sizeof(Vector2), VertexElementType::Color, VertexElementDataType::Float4 }
	};

	VertexBufferHandle SpriteVertexBuffer = 0;

	Sprite::Sprite()
	{
	}

	void Sprite::Draw()
	{
		if(Options.WireframeValue)
		{
			Vector2 ObjectSizeHalf = Options.ScaleValue * (SpriteTexture.Get() ? SpriteTexture->Size() : Vector2(1, 1)) / 2;

			SpriteWireframeVertex Vertices[8] = {
				{ -ObjectSizeHalf, Options.ColorValue },
				{ Vector2(ObjectSizeHalf.x, -ObjectSizeHalf.y), Options.ColorValue },
				{ Vector2(ObjectSizeHalf.x, -ObjectSizeHalf.y), Options.ColorValue },
				{ ObjectSizeHalf, Options.ColorValue },
				{ ObjectSizeHalf, Options.ColorValue },
				{ Vector2(-ObjectSizeHalf.x, ObjectSizeHalf.y), Options.ColorValue },
				{ Vector2(-ObjectSizeHalf.x, ObjectSizeHalf.y), Options.ColorValue },
				{ -ObjectSizeHalf, Options.ColorValue }
			};

			for(uint32 i = 0; i < 8; i++)
			{
				Vertices[i].Position = Vector2::Rotate(Vertices[i].Position, Options.RotationValue) + ObjectSizeHalf + Options.PositionValue;
			}

			if(!g_Renderer.IsVertexBufferHandleValid(SpriteVertexBuffer))
			{
				SpriteVertexBuffer = g_Renderer.CreateVertexBuffer();
			}

			if(!SpriteVertexBuffer)
				return;

			g_Renderer.BindTexture((TextureHandle)0);

			g_Renderer.SetVertexBufferData(SpriteVertexBuffer, VertexDetailsMode::Mixed, SpriteWireframeVertexDescriptor, sizeof(SpriteWireframeVertexDescriptor) / sizeof(SpriteWireframeVertexDescriptor[0]),
				Vertices, sizeof(Vertices));

			//TODO
			//glLineWidth(Options.WireframePixelSizeValue);

			g_Renderer.RenderVertices(VertexModes::Lines, SpriteVertexBuffer, 0, 8);

			//glLineWidth(1);

			return;
		}
		else
		{
			//Generate geometry
			if(Options.IsDirty)
			{
				static Vector2 Vertices[6],	TexCoords[6];
				static Vector4 Colors[6];

				Options.IsDirty = false;

				Vertices[0] = Vertices[5] = Vector2();
				Vertices[1] = Vector2(0, 1);
				Vertices[2] = Vertices[3] = Vector2(1, 1);
				Vertices[4] = Vector2(1, 0);

				TexCoords[0] = TexCoords[5] = Vector2(0, 0);
				TexCoords[1] = Vector2(0, 1);
				TexCoords[2] = TexCoords[3] = Vector2(1, 1);
				TexCoords[4] = Vector2(1, 0);

				Colors[0] = Colors[1] = Colors[2] = Colors[3] = Colors[4] = Colors[5] = Options.ColorValue;

				Rect ActualTextureRect(0, 0, 1, 1);
				DisposablePointer<Texture> ActualTexture;

				if (SpriteTexture.Get())
				{
					if (SpriteTexture->GetIndex().Owner.Get() != nullptr)
						const_cast<TexturePacker *>(SpriteTexture.Get()->GetIndex().Owner.Get())->Bind();

					ActualTextureRect = SpriteTexture->TextureRect();

					ActualTexture = SpriteTexture->BaseTexture();
				}

				if(SpriteTexture.Get())
				{
					//1st: Normalize
					{
						Vector2 NormalizedSize = Vector2(ActualTextureRect.Right, ActualTextureRect.Bottom) / ActualTexture->Size();
						Vector2 NormalizedPosition = Vector2(ActualTextureRect.Left, (f32)ActualTextureRect.Top) / ActualTexture->Size();

						//2nd: Multiply by size, then translate
						for(uint32 i = 0; i < 6; i++)
						{
							TexCoords[i] *= NormalizedSize;
							TexCoords[i] += NormalizedPosition;
						}
					}
				}
				
				if (SpriteTexture.Get())
				{
					for (uint32 i = 0; i < 6; i++)
					{
						Vertices[i] *= SpriteTexture->Size();
						Vertices[i] *= Options.ScaleValue;
					}
				}
				else
				{
					for (uint32 i = 0; i < 6; i++)
					{
						Vertices[i] *= Options.ScaleValue;
					}
				}

				if(Options.FlipX || Options.FlipY)
				{
					Vector2 Max;

					for(uint32 i = 0; i < 6; i++)
					{
						if(Vertices[i].x > Max.x)
							Max.x = Vertices[i].x;

						if (Vertices[i].y > Max.y)
							Max.y = Vertices[i].y;
					}

					for(uint32 i = 0; i < 6; i++)
					{
						if(Options.FlipX)
						{
							Vertices[i].x = Max.x - Vertices[i].x;
						}

						if(Options.FlipY)
						{
							Vertices[i].y = Max.y - Vertices[i].y;
						}
					}
				}

				Vector2 ObjectSize = Vector2();

				static AxisAlignedBoundingBox ObjectAABB;

				ObjectAABB.Clear();

				for(uint32 i = 0; i < 6; i++)
				{
					ObjectAABB.Calculate(Vertices[i]);
				}

				ObjectSize = (ObjectAABB.max - ObjectAABB.min).ToVector2();

				if(Options.RotationValue != 0 && !Options.WireframeValue)
				{
					//Need to invert NinePatch left/right because each element is an offset in that side, not an actual Rect
					Vector2 ExtraSize = -ObjectSize / 2;

					for(uint32 i = 0; i < 6; i++)
					{
						Vertices[i] = Vector2::Rotate(Vertices[i] + ExtraSize, Options.RotationValue) - ExtraSize;
						Vertices[i] += Options.OffsetValue + Options.PositionValue;
					}
				}
				else
				{
					for(uint32 i = 0; i < 6; i++)
					{
						Vertices[i] += Options.OffsetValue + Options.PositionValue;
					}
				}

				if (Options.CropValue)
				{
					FLASSERT(SpriteTexture.Get() != NULL, "Cropping requires a texture");

					//1st: Normalize
					{
						Vector2 NormalizedSize = Options.CropRectValue.Size() / SpriteTexture->Size();
						Vector2 NormalizedPosition = Options.CropRectValue.Position() / SpriteTexture->Size();

						//2nd: Multiply by size, then translate
						for (uint32 i = 0; i < 6; i++)
						{
							TexCoords[i] *= NormalizedSize;
							TexCoords[i] += NormalizedPosition;
						}
					}
				}

				PosTexCol *Out = &GeneratedGeometry[0];

				for(uint32 i = 0; i < 6; i++, Out++)
				{
					Out->Position = Vertices[i];
					Out->TexCoord = TexCoords[i];
					Out->Color = Colors[i];
				}
			}

			//TODO
			g_Renderer.RenderCachedVertices(VertexModes::Triangles, GeneratedGeometry, sizeof(GeneratedGeometry), PosTexColFormat, sizeof(PosTexColFormat) / sizeof(PosTexColFormat[0]), SpriteTexture, Options.BlendingModeValue);
		}
	}
#endif
}

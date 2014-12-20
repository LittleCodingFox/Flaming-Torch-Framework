#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	SpriteCache SpriteCache::Instance;

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

	//Generates a ninepatch quad
	void GenerateNinePatchGeometry(Vector2 *Vertices, Vector2 *TexCoords, const Vector2 &TextureSize, const Vector2 &Position, const Vector2 &Size, const Vector2 &Offset,
		const Vector2 &SizeOverride)
	{
		static Rect NinePatchRect;

		NinePatchRect = Rect(Position.x, Position.x + Size.x, Position.y, Position.y + Size.y);

		Vector2 ActualSize = SizeOverride.x != -1 ? SizeOverride : Size;

		Vertices[0] = Vertices[5] = Offset;
		Vertices[1] = (Offset + Vector2(0, ActualSize.y));
		Vertices[2] = Vertices[3] = (Offset + ActualSize);
		Vertices[4] = (Offset + Vector2(ActualSize.x, 0));

		TexCoords[0] = TexCoords[5] = NinePatchRect.Position() / TextureSize;
		TexCoords[1] = Vector2(NinePatchRect.Left, NinePatchRect.Bottom) / TextureSize;
		TexCoords[2] = TexCoords[3] = Vector2(NinePatchRect.Right, NinePatchRect.Bottom) / TextureSize;
		TexCoords[4] = Vector2(NinePatchRect.Right, NinePatchRect.Top) / TextureSize;
	};

	Sprite::Sprite() : VertexCount(0)
	{
	};

	void Sprite::Draw(Renderer *Renderer)
	{
		if(Options.WireframeValue)
		{
			SpriteCache::Instance.Flush(Renderer);

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
			};

			if(!Renderer->IsVertexBufferHandleValid(SpriteVertexBuffer))
			{
				SpriteVertexBuffer = Renderer->CreateVertexBuffer();
			};

			if(!SpriteVertexBuffer)
				return;

			Renderer->BindTexture((TextureHandle)0);

			Renderer->SetVertexBufferData(SpriteVertexBuffer, VertexDetailsMode::Mixed, SpriteWireframeVertexDescriptor, sizeof(SpriteWireframeVertexDescriptor) / sizeof(SpriteWireframeVertexDescriptor[0]),
				Vertices, sizeof(Vertices));

			//TODO
			//glLineWidth(Options.WireframePixelSizeValue);

			Renderer->RenderVertices(VertexModes::Lines, SpriteVertexBuffer, 0, 8);

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
				static Vector2 NinePatchVertices[54], NinePatchTexCoords[54];
				static Vector4 NinePatchColors[54];

				Options.IsDirty = false;

				Vertices[0] = Vertices[5] = Vector2();
				Vertices[1] = Vector2(0, 1);
				Vertices[2] = Vertices[3] = Vector2(1, 1);
				Vertices[4] = Vector2(1, 0);

				TexCoords[0] = TexCoords[5] = Vector2(-0.5f, -0.5f);
				TexCoords[1] = Vector2(-0.5f, 0.5f);
				TexCoords[2] = TexCoords[3] = Vector2(0.5f, 0.5f);
				TexCoords[4] = Vector2(0.5f, -0.5f);

				Colors[0] = Colors[1] = Colors[2] = Colors[3] = Colors[4] = Colors[5] = Options.ColorValue;

				if(Options.UsingColorsArray)
				{
					Colors[0] = Colors[5] = Options.ColorsValue[0];
					Colors[1] = Options.ColorsValue[1];
					Colors[2] = Colors[3] = Options.ColorsValue[2];
					Colors[4] = Options.ColorsValue[3];
				};

				NinePatchColors[0] = Options.ColorValue;
				NinePatchColors[1] = Options.ColorValue;
				NinePatchColors[2] = Options.ColorValue;
				NinePatchColors[3] = Options.ColorValue;
				NinePatchColors[4] = Options.ColorValue;
				NinePatchColors[5] = Options.ColorValue;
				NinePatchColors[6] = Options.ColorValue;
				NinePatchColors[7] = Options.ColorValue;
				NinePatchColors[8] = Options.ColorValue;
				NinePatchColors[9] = Options.ColorValue;

				//10
				NinePatchColors[10] = Options.ColorValue;
				NinePatchColors[11] = Options.ColorValue;
				NinePatchColors[12] = Options.ColorValue;
				NinePatchColors[13] = Options.ColorValue;
				NinePatchColors[14] = Options.ColorValue;
				NinePatchColors[15] = Options.ColorValue;
				NinePatchColors[16] = Options.ColorValue;
				NinePatchColors[17] = Options.ColorValue;
				NinePatchColors[18] = Options.ColorValue;
				NinePatchColors[19] = Options.ColorValue;

				//20
				NinePatchColors[20] = Options.ColorValue;
				NinePatchColors[21] = Options.ColorValue;
				NinePatchColors[22] = Options.ColorValue;
				NinePatchColors[23] = Options.ColorValue;
				NinePatchColors[24] = Options.ColorValue;
				NinePatchColors[25] = Options.ColorValue;
				NinePatchColors[26] = Options.ColorValue;
				NinePatchColors[27] = Options.ColorValue;
				NinePatchColors[28] = Options.ColorValue;
				NinePatchColors[29] = Options.ColorValue;

				//30
				NinePatchColors[30] = Options.ColorValue;
				NinePatchColors[31] = Options.ColorValue;
				NinePatchColors[32] = Options.ColorValue;
				NinePatchColors[33] = Options.ColorValue;
				NinePatchColors[34] = Options.ColorValue;
				NinePatchColors[35] = Options.ColorValue;
				NinePatchColors[36] = Options.ColorValue;
				NinePatchColors[37] = Options.ColorValue;
				NinePatchColors[38] = Options.ColorValue;
				NinePatchColors[39] = Options.ColorValue;

				//40
				NinePatchColors[40] = Options.ColorValue;
				NinePatchColors[41] = Options.ColorValue;
				NinePatchColors[42] = Options.ColorValue;
				NinePatchColors[43] = Options.ColorValue;
				NinePatchColors[44] = Options.ColorValue;
				NinePatchColors[45] = Options.ColorValue;
				NinePatchColors[46] = Options.ColorValue;
				NinePatchColors[47] = Options.ColorValue;
				NinePatchColors[48] = Options.ColorValue;
				NinePatchColors[49] = Options.ColorValue;

				//50
				NinePatchColors[50] = Options.ColorValue;
				NinePatchColors[51] = Options.ColorValue;
				NinePatchColors[52] = Options.ColorValue;
				NinePatchColors[53] = Options.ColorValue;

				if(Options.TexCoordRotation != 0)
				{
					TexCoords[0] = Vector2::Rotate(TexCoords[0], Options.TexCoordRotation);
					TexCoords[1] = Vector2::Rotate(TexCoords[1], Options.TexCoordRotation);
					TexCoords[2] = Vector2::Rotate(TexCoords[2], Options.TexCoordRotation);
					TexCoords[3] = Vector2::Rotate(TexCoords[3], Options.TexCoordRotation);
					TexCoords[4] = Vector2::Rotate(TexCoords[4], Options.TexCoordRotation);
					TexCoords[5] = Vector2::Rotate(TexCoords[5], Options.TexCoordRotation);
				};

				TexCoords[0] = (TexCoords[0] + Vector2(0.5f, 0.5f)) * Options.TexCoordBorderMin;
				TexCoords[1] = (TexCoords[1] + Vector2(0.5f, 0.5f)) * Vector2(Options.TexCoordBorderMin.x, Options.TexCoordBorderMax.y);
				TexCoords[2] = (TexCoords[2] + Vector2(0.5f, 0.5f)) * Options.TexCoordBorderMax;
				TexCoords[3] = (TexCoords[3] + Vector2(0.5f, 0.5f)) * Options.TexCoordBorderMax;
				TexCoords[4] = (TexCoords[4] + Vector2(0.5f, 0.5f)) * Vector2(Options.TexCoordBorderMax.x, Options.TexCoordBorderMin.x);
				TexCoords[5] = (TexCoords[5] + Vector2(0.5f, 0.5f)) * Options.TexCoordBorderMin;

				if(SpriteTexture.Get() && SpriteTexture->GetIndex().Index != -1 && SpriteTexture->GetIndex().Owner.Get() != nullptr &&
					SpriteTexture->GetIndex().Owner->MainTexture.Get() != nullptr)
				{
					//1st: Normalize
					{
						Vector2 NormalizedSize = Vector2(SpriteTexture->GetIndex().Width(), SpriteTexture->GetIndex().Height()) / SpriteTexture->GetIndex().Owner->MainTexture->Size();
						Vector2 NormalizedPosition = Vector2((f32)SpriteTexture->GetIndex().Owner->Indices[SpriteTexture->GetIndex().Index].x + 1,
							(f32)SpriteTexture->GetIndex().Owner->Indices[SpriteTexture->GetIndex().Index].y + 1) / SpriteTexture->GetIndex().Owner->MainTexture->Size();

						//2nd: Multiply by size, then translate
						for(uint32 i = 0; i < 6; i++)
						{
							TexCoords[i] *= NormalizedSize;
							TexCoords[i] += NormalizedPosition;
						};
					};
				};

				Vector2 *VerticesTarget = Options.NinePatchValue ? NinePatchVertices : Vertices, *TexCoordTarget = Options.NinePatchValue ? NinePatchTexCoords : TexCoords;
				Vector4 *ColorsTarget = Options.NinePatchValue ? NinePatchColors : Colors;

				VertexCount = Options.NinePatchValue ? 54 : 6;

				if(!Options.NinePatchValue)
				{
					if(SpriteTexture.Get())
					{
						for(uint32 i = 0; i < VertexCount; i++)
						{
							VerticesTarget[i] *= SpriteTexture->Size();
							VerticesTarget[i] *= Options.ScaleValue;
						};
					}
					else
					{
						for(uint32 i = 0; i < VertexCount; i++)
						{
							VerticesTarget[i] *= Options.ScaleValue;
						};
					};
				}
				else
				{
					const Rect &TextureRect = Options.NinePatchRectValue;

					Vector2 FragmentPositions[9] = {
						Vector2(),
						Vector2(SpriteTexture->Width() - TextureRect.Right, 0),
						Vector2(0, SpriteTexture->Height() - TextureRect.Bottom),
						Vector2(SpriteTexture->Width() - TextureRect.Right, SpriteTexture->Height() - TextureRect.Bottom),
						Vector2(TextureRect.Left, TextureRect.Top),
						Vector2(TextureRect.Left, 0),
						Vector2(TextureRect.Left, SpriteTexture->Height() - TextureRect.Bottom),
						Vector2(0, TextureRect.Top),
						Vector2(SpriteTexture->Width() - TextureRect.Right, TextureRect.Top),
					};

					Vector2 FragmentSizes[9] = {
						Vector2(TextureRect.Left, TextureRect.Top),
						Vector2(TextureRect.Right, TextureRect.Top),
						Vector2(TextureRect.Left, TextureRect.Bottom),
						Vector2(TextureRect.Right, TextureRect.Bottom),
						Vector2(SpriteTexture->Width() - TextureRect.Left - TextureRect.Right, SpriteTexture->Height() -
						TextureRect.Top - TextureRect.Bottom),
						Vector2(SpriteTexture->Width() - TextureRect.Left - TextureRect.Right, TextureRect.Top),
						Vector2(SpriteTexture->Width() - TextureRect.Left - TextureRect.Right, TextureRect.Bottom),
						Vector2(TextureRect.Left, SpriteTexture->Height() - TextureRect.Top - TextureRect.Bottom),
						Vector2(TextureRect.Right, SpriteTexture->Height() - TextureRect.Top - TextureRect.Bottom),
					};

					Vector2 FragmentOffsets[9] = {
						Vector2(-TextureRect.Left, -TextureRect.Top) * Options.NinePatchScaleValue,
						Vector2(MathUtils::Round(Options.ScaleValue.x), -TextureRect.Top * Options.NinePatchScaleValue),
						Vector2(-TextureRect.Left * Options.NinePatchScaleValue, MathUtils::Round(Options.ScaleValue.y)),
						Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
						Vector2(),
						Vector2(0, -TextureRect.Top * Options.NinePatchScaleValue),
						Vector2(0, MathUtils::Round(Options.ScaleValue.y)),
						Vector2(-TextureRect.Left * Options.NinePatchScaleValue, 0),
						Vector2(MathUtils::Round(Options.ScaleValue.x), 0),
					};

					Vector2 FragmentSizeOverrides[9] = {
						Vector2(TextureRect.Left, TextureRect.Top) * Options.NinePatchScaleValue,
						Vector2(TextureRect.Right, TextureRect.Top) * Options.NinePatchScaleValue,
						Vector2(TextureRect.Left, TextureRect.Bottom) * Options.NinePatchScaleValue,
						Vector2(TextureRect.Right, TextureRect.Bottom) * Options.NinePatchScaleValue,
						Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
						Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Top * Options.NinePatchScaleValue),
						Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Bottom * Options.NinePatchScaleValue),
						Vector2(TextureRect.Left * Options.NinePatchScaleValue, MathUtils::Round(Options.ScaleValue.y)),
						Vector2(TextureRect.Right * Options.NinePatchScaleValue, MathUtils::Round(Options.ScaleValue.y)),
					};

					for(uint32 i = 0, index = 0; i < 9; i++, index += 6)
					{
						GenerateNinePatchGeometry(VerticesTarget + index, TexCoordTarget + index, SpriteTexture->Size(), FragmentPositions[i], FragmentSizes[i], FragmentOffsets[i],
							FragmentSizeOverrides[i]);
					};
				};

				if(Options.FlipX || Options.FlipY)
				{
					Vector2 Max;

					for(uint32 i = 0; i < VertexCount; i++)
					{
						if(VerticesTarget[i].x > Max.x)
							Max.x = VerticesTarget[i].x;

						if(VerticesTarget[i].y > Max.y)
							Max.y = VerticesTarget[i].y;
					};

					for(uint32 i = 0; i < VertexCount; i++)
					{
						if(Options.FlipX)
						{
							VerticesTarget[i].x = Max.x - VerticesTarget[i].x;
						};

						if(Options.FlipY)
						{
							VerticesTarget[i].y = Max.y - VerticesTarget[i].y;
						};
					};
				};

				Vector2 PinningTranslation;

				Vector2 ObjectSize = Vector2();

				static AxisAlignedBoundingBox ObjectAABB;

				ObjectAABB.Clear();

				for(uint32 i = 0; i < VertexCount; i++)
				{
					ObjectAABB.Calculate(VerticesTarget[i]);
				};

				ObjectSize = (ObjectAABB.max - ObjectAABB.min).ToVector2();

				switch(Options.PinningModeValue)
				{
				case PinningMode::TopLeft:
					//Ignore

					break;
				case PinningMode::TopCenter:
					PinningTranslation = -Vector2(ObjectSize.x / 2, 0);

					break;
				case PinningMode::TopRight:
					PinningTranslation = -Vector2(ObjectSize.x, 0);

					break;
				case PinningMode::Left:
					PinningTranslation = -Vector2(0, ObjectSize.y / 2);

					break;
				case PinningMode::Middle:
					PinningTranslation = -Vector2(ObjectSize.x / 2, ObjectSize.y / 2);

					break;
				case PinningMode::Right:
					PinningTranslation = -Vector2(ObjectSize.x, ObjectSize.y / 2);

					break;
				case PinningMode::BottomLeft:
					PinningTranslation = -Vector2(0, ObjectSize.y);

					break;
				case PinningMode::BottomCenter:
					PinningTranslation = -Vector2(ObjectSize.x / 2, ObjectSize.y);

					break;
				case PinningMode::BottomRight:
					PinningTranslation = -Vector2(ObjectSize.x, ObjectSize.y);

					break;
				};

				if(Options.RotationValue != 0 && !Options.WireframeValue)
				{
					//Need to invert NinePatch left/right because each element is an offset in that side, not an actual Rect
					Vector2 ExtraSize = Options.NinePatchValue ? -ObjectSize / 2 +
						Rect(-Options.NinePatchRectValue.Left, Options.NinePatchRectValue.Right, -Options.NinePatchRectValue.Top, Options.NinePatchRectValue.Bottom).Size() / 4 * Options.NinePatchScaleValue : -ObjectSize / 2;

					for(uint32 i = 0; i < VertexCount; i++)
					{
						VerticesTarget[i] = Vector2::Rotate(VerticesTarget[i] + ExtraSize, Options.RotationValue) - ExtraSize;
						VerticesTarget[i] += PinningTranslation + Options.OffsetValue + Options.PositionValue;
					};
				}
				else
				{
					for(uint32 i = 0; i < VertexCount; i++)
					{
						VerticesTarget[i] += PinningTranslation + Options.OffsetValue + Options.PositionValue;
					};
				};

				if(!Options.NinePatchValue && Options.CropModeValue != CropMode::None)
				{
					switch(Options.CropModeValue)
					{
					case CropMode::Crop:
						FLASSERT(SpriteTexture.Get() != NULL, "Cropping requires a texture");

						//1st: Normalize
						{
							Vector2 NormalizedSize = Options.CropRectValue.Size() / SpriteTexture->Size();
							Vector2 NormalizedPosition = Options.CropRectValue.Position() / SpriteTexture->Size();

							//2nd: Multiply by size, then translate
							for(uint32 i = 0; i < VertexCount; i++)
							{
								TexCoordTarget[i] *= NormalizedSize;
								TexCoordTarget[i] += NormalizedPosition;
							};
						};

						break;
					case CropMode::CropNormalized:
						for(uint32 i = 0; i < VertexCount; i++)
						{
							TexCoordTarget[i] *= Options.CropRectValue.Size();
							TexCoordTarget[i] += Options.CropRectValue.Position();
						};

						break;
					case CropMode::CropTiled:
						FLASSERT(SpriteTexture.Get() != NULL, "Cropping requires a texture");

						{
							Vector2 OneFrame = Options.CropRectValue.Position() / SpriteTexture->Size();
							Vector2 BaseFrame = OneFrame * Vector2(Options.CropRectValue.Right, Options.CropRectValue.Bottom);

							for(uint32 i = 0; i < VertexCount; i++)
							{
								TexCoordTarget[i] *= OneFrame;
								TexCoordTarget[i] += BaseFrame;
							};
						};

						break;
					};
				};

				if(Options.TexCoordPosition != Vector2())
				{
					for(uint32 i = 0; i < VertexCount; i++)
					{
						TexCoordTarget[i] += Options.TexCoordPosition;
					};
				};

				SpriteVertex *Out = &GeneratedGeometry[0];

				for(uint32 i = 0; i < VertexCount; i++, Out++)
				{
					Out->Position = VerticesTarget[i];
					Out->TexCoord = TexCoordTarget[i];
					Out->Color = ColorsTarget[i];
				};
			};

			SpriteCache::Instance.Register(GeneratedGeometry, VertexCount, SpriteTexture, Options.BlendingModeValue, Renderer);
		};
	};

	void SpriteCache::Register(SpriteVertex *Vertices, uint32 VertexCount, Texture *TheTexture, uint32 BlendingMode, Renderer *Renderer)
	{
#if !DISABLE_SPRITE_CACHE
		if(BlendingMode != CurrentBlendingMode || (TheTexture != ActiveTexture && (TheTexture == NULL || ActiveTexture == NULL || TheTexture->GetIndex().Index == -1 ||
			TheTexture->GetIndex().Index != ActiveTexture->GetIndex().Index)))
#endif
		{
			SpriteCache::Instance.Flush(Renderer);
		};

		Renderer->ReportSkippedDrawCall();

		FLASSERT(CurrentCachePosition + VertexCount < MAX_SPRITE_CACHE_SIZE, "Exceeded vertex cache limit!");

		if(CurrentCachePosition + VertexCount > MAX_SPRITE_CACHE_SIZE)
			return;

		memcpy(&CachedGeometry[CurrentCachePosition], Vertices, sizeof(SpriteVertex) * VertexCount);

		CurrentCachePosition += VertexCount;

		ActiveTexture = TheTexture;
		CurrentBlendingMode = BlendingMode;
	};

	void SpriteCache::Flush(Renderer *Renderer)
	{
		if(CurrentCachePosition == 0)
			return;

		Renderer->BindTexture((Texture *)ActiveTexture);
		//Enable to disable textures
		//Renderer->BindTexture(TextureHandle(0));

		if(!Renderer->IsVertexBufferHandleValid(SpriteVertexBuffer))
		{
			SpriteVertexBuffer = Renderer->CreateVertexBuffer();
		};

		if(!SpriteVertexBuffer)
			return;

		Renderer->SetVertexBufferData(SpriteVertexBuffer, VertexDetailsMode::Mixed, SpriteVertexDescriptor, sizeof(SpriteVertexDescriptor) / sizeof(SpriteVertexDescriptor[0]), &CachedGeometry[0],
			CurrentCachePosition * sizeof(SpriteVertex));

		Renderer->SetBlendingMode(CurrentBlendingMode);
		Renderer->RenderVertices(VertexModes::Triangles, SpriteVertexBuffer, 0, CurrentCachePosition);

		CurrentCachePosition = 0;
		ActiveTexture = NULL;
	};

	void AnimatedSprite::AddAnimation(const std::string &Name, const std::vector<Vector2> &Frames)
	{
		StringID NameID = MakeStringID(Name);

		FLASSERT(Animations.find(NameID) == Animations.end(), "Duplicated animation found!");

		if(Animations.find(NameID) != Animations.end())
			return;

		Animations[NameID].Frames = Frames;
	};

	void AnimatedSprite::SetAnimation(const std::string &Name, bool Repeats)
	{
		StringID NameID = MakeStringID(Name);

		if(Animations.find(NameID) == Animations.end())
		{
			CurrentAnimation = NULL;
			LastFrameUpdate = 0;

			return;
		};

		CurrentAnimation = &Animations[NameID];
		CurrentAnimation->Repeating = Repeats;
		LastFrameUpdate = 0;
	};

	void AnimatedSprite::Update()
	{
		if(LastFrameUpdate == 0)
		{
			LastFrameUpdate = GameClockTime();

			if(CurrentAnimation != NULL && CurrentAnimation->Frames.size())
			{
				CurrentAnimation->CurrentFrame = 0;
			};

			Vector2 Frame = CurrentAnimation != NULL && CurrentAnimation->Frames.size() ?
				CurrentAnimation->Frames[CurrentAnimation->CurrentFrame] : DefaultFrame;

			Options = Options.Crop(CropMode::CropTiled, Rect(FrameSize.x, Frame.x, FrameSize.y, Frame.y));

			if(SpriteTexture.Get())
			{
				Options = Options.Scale(FrameSize / SpriteTexture->Size() * Scale);
			};
		};

		if(CurrentAnimation != NULL && CurrentAnimation->Frames.size() && GameClockDiff(LastFrameUpdate) > FrameInterval)
		{
			LastFrameUpdate = GameClockTime() - (GameClockTime() - LastFrameUpdate - FrameInterval);

			CurrentAnimation->CurrentFrame++;

			if(CurrentAnimation->CurrentFrame >= CurrentAnimation->Frames.size())
				CurrentAnimation->CurrentFrame = CurrentAnimation->Repeating ? 0 : CurrentAnimation->Frames.size() - 1;

			Vector2 Frame = CurrentAnimation->Frames[CurrentAnimation->CurrentFrame];

			Options = Options.Crop(CropMode::CropTiled, Rect(FrameSize.x, Frame.x, FrameSize.y, Frame.y));
		};
	};

	void AnimatedSprite::StopAnimation()
	{
		CurrentAnimation = NULL;
	};
#endif
};

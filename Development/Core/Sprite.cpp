#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	SpriteCache SpriteCache::Instance;

	//Generates a ninepatch quad
	void GenerateNinePatchGeometry(Vector2 *Vertices, Vector2 *TexCoords, const Vector2 &TextureSize, const Vector2 &Position, const Vector2 &Size, const Vector2 &Offset,
		const Vector2 &SizeOverride = Vector2(-1, -1))
	{
		static Rect NinePatchRect;

		NinePatchRect = Rect(Position.x, Position.x + Size.x, Position.y, Position.y + Size.y);

		Vector2 ActualSize = SizeOverride.x != -1 ? SizeOverride : Size;

		Vertices[0] = Vertices[5] = Offset;
		Vertices[1] = Offset + Vector2(0, ActualSize.y);
		Vertices[2] = Vertices[3] = Offset + ActualSize;
		Vertices[4] = Offset + Vector2(ActualSize.x, 0);

		TexCoords[0] = TexCoords[5] = NinePatchRect.Position() / TextureSize;
		TexCoords[1] = Vector2(NinePatchRect.Left, NinePatchRect.Bottom) / TextureSize;
		TexCoords[2] = TexCoords[3] = Vector2(NinePatchRect.Right, NinePatchRect.Bottom) / TextureSize;
		TexCoords[4] = Vector2(NinePatchRect.Right, NinePatchRect.Top) / TextureSize;
	};

	void Sprite::Draw(RendererManager::Renderer *Renderer)
	{
		static Vector2 Vertices[6],	TexCoords[6];
		static Vector4 Colors[6];
		static Vector2 NinePatchVertices[54], NinePatchTexCoords[54];
		static Vector4 NinePatchColors[54];

		Vertices[0] = Vertices[5] = Vector2();
		Vertices[1] = Vector2(0, 1);
		Vertices[2] = Vertices[3] = Vector2(1, 1);
		Vertices[4] = Vector2(1, 0);

		TexCoords[0] = TexCoords[5] = Vector2(-0.5f, -0.5f);
		TexCoords[1] = Vector2(-0.5f, 0.5f);
		TexCoords[2] = TexCoords[3] = Vector2(0.5f, 0.5f);
		TexCoords[4] = Vector2(0.5f, -0.5f);

		Colors[0] = Colors[1] = Colors[2] = Colors[3] = Colors[4] = Colors[5] = Options.ColorValue;

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

		if(SpriteTexture.Get() && SpriteTexture->GetIndex().Index != -1)
		{
			//1st: Normalize
			{
				Vector2 NormalizedSize = Vector2(SpriteTexture->GetIndex().Width() - 2, SpriteTexture->GetIndex().Height() - 2) / SpriteTexture->GetIndex().Owner->MainTexture->Size();
				Vector2 NormalizedPosition = Vector2(SpriteTexture->GetIndex().Owner->Indices[SpriteTexture->GetIndex().Index].x + 1, SpriteTexture->GetIndex().Owner->Indices[SpriteTexture->GetIndex().Index].y + 1) /
					SpriteTexture->GetIndex().Owner->MainTexture->Size();

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

		uint32 VertexCount = Options.NinePatchValue ? 54 : 6;

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
				Vector2(-TextureRect.Left, -TextureRect.Top),
				Vector2(MathUtils::Round(Options.ScaleValue.x), -TextureRect.Top),
				Vector2(-TextureRect.Left, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
				Vector2(),
				Vector2(0, -TextureRect.Top),
				Vector2(0, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(-TextureRect.Left, 0),
				Vector2(MathUtils::Round(Options.ScaleValue.x), 0),
			};

			Vector2 FragmentSizeOverrides[9] = {
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
				Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Top),
				Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Bottom),
				Vector2(TextureRect.Left, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(TextureRect.Right, MathUtils::Round(Options.ScaleValue.y)),
			};

			for(uint32 i = 0, index = 0; i < 9; i++, index += 6)
			{
				GenerateNinePatchGeometry(VerticesTarget + index, TexCoordTarget + index, SpriteTexture->Size(), FragmentPositions[i], FragmentSizes[i], FragmentOffsets[i], FragmentSizeOverrides[i]);
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

		if(Options.RotationValue != 0)
		{
			for(uint32 i = 0; i < VertexCount; i++)
			{
				VerticesTarget[i] = Vector2::Rotate(VerticesTarget[i] - ObjectSize / 2, Options.RotationValue);
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

		SpriteCache::Instance.Register(VerticesTarget, TexCoordTarget, ColorsTarget, VertexCount, SpriteTexture, Options.BlendingModeValue, Renderer);
	};

	void SpriteCache::Register(Vector2 *Vertices, Vector2 *TexCoords, Vector4 *Colors, uint32 VertexCount, SuperSmartPointer<Texture> Texture, uint32 BlendingMode, RendererManager::Renderer *Renderer)
	{
#if !DISABLE_SPRITE_CACHE
		if(BlendingMode != CurrentBlendingMode || (Texture != ActiveTexture && (Texture.Get() == NULL || ActiveTexture.Get() == NULL || Texture->GetIndex().Index == -1 ||
			Texture->GetIndex().Index != ActiveTexture->GetIndex().Index)))
#endif
		{
			SpriteCache::Instance.Flush(Renderer);
		};

		for(uint32 i = 0; i < VertexCount; i++)
		{
			CachedVertices.push_back(Vertices[i]);
			CachedTexCoords.push_back(TexCoords[i]);
			CachedColors.push_back(Colors[i]);
		};

		ActiveTexture = Texture;
		CurrentBlendingMode = BlendingMode;
	};

	void SpriteCache::Flush(RendererManager::Renderer *Renderer)
	{
		if(CachedVertices.size() == 0)
			return;

		switch(CurrentBlendingMode)
		{
		case BlendingMode::None:
			Renderer->DisableState(GL_BLEND);

			break;
		case BlendingMode::Alpha:
			Renderer->EnableState(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			break;
		case BlendingMode::Additive:
			Renderer->EnableState(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			break;
		case BlendingMode::Subtractive:
			Renderer->EnableState(GL_BLEND);
			glBlendEquation(GL_FUNC_SUBTRACT);

			break;
		};

		Renderer->BindTexture(ActiveTexture.Get());
		Renderer->EnableState(GL_VERTEX_ARRAY);
		Renderer->DisableState(GL_NORMAL_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &CachedVertices[0]);

		if(ActiveTexture.Get() == NULL)
		{
			Renderer->DisableState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			Renderer->EnableState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &CachedTexCoords[0]);
		};

		Renderer->EnableState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, &CachedColors[0]);

		glDrawArrays(GL_TRIANGLES, 0, CachedVertices.size());

		glColor4f(1, 1, 1, 1);

		CachedVertices.resize(0);
		CachedTexCoords.resize(0);
		CachedColors.resize(0);
		ActiveTexture = SuperSmartPointer<Texture>();
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

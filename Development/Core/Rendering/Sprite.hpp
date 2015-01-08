#pragma once
#if USE_GRAPHICS

#define MAX_SPRITE_CACHE_SIZE 20000

namespace PinningMode
{
	enum PinningMode
	{
		TopLeft = 0, //Default
		TopCenter,
		TopRight,
		Left,
		Middle,
		Right,
		BottomLeft,
		BottomCenter,
		BottomRight
	};
}

namespace CropMode
{
	enum CropMode
	{
		None = 0, //Default
		Crop,
		CropNormalized, //[Normalized Coordinates]
		CropTiled, //[Right/Bottom -> TileID, Left/Top -> FrameSize]
	};
}

namespace BlendingMode
{
	enum BlendingMode
	{
		None = 0,
		Alpha,
		Additive,
		Subtractive
	};
}

class SpriteDrawOptions
{
public:
	Vector2 PositionValue, ScaleValue, OffsetValue;
	f32 RotationValue;
	uint32 PinningModeValue;
	uint32 CropModeValue;
	Rect CropRectValue;
	Vector4 ColorValue;
	uint32 BlendingModeValue;
	bool NinePatchValue;
	f32 NinePatchScaleValue;
	Rect NinePatchRectValue;
	bool FlipX, FlipY;
	Vector2 TexCoordBorderMin, TexCoordBorderMax, TexCoordPosition;
	f32 TexCoordRotation;
	bool WireframeValue;
	f32 WireframePixelSizeValue;
	Vector4 ColorsValue[4];
	bool UsingColorsArray;
	bool IsDirty;

	SpriteDrawOptions() : ColorValue(1, 1, 1, 1), ScaleValue(1, 1), RotationValue(0), BlendingModeValue(BlendingMode::Alpha),
		PinningModeValue(PinningMode::TopLeft), CropModeValue(CropMode::None), NinePatchValue(false), FlipX(false), FlipY(false),
		TexCoordRotation(0), TexCoordBorderMax(1, 1), NinePatchScaleValue(1), WireframeValue(false), WireframePixelSizeValue(1),
		UsingColorsArray(false), IsDirty(true) {

		ColorsValue[0] = ColorsValue[1] = ColorsValue[2] = ColorsValue[3] = ColorValue;
	}

	SpriteDrawOptions(const SpriteDrawOptions &o) : ColorValue(o.ColorValue), BlendingModeValue(o.BlendingModeValue),
		PositionValue(o.PositionValue), ScaleValue(o.ScaleValue), RotationValue(o.RotationValue),
		PinningModeValue(o.PinningModeValue), CropModeValue(o.CropModeValue), CropRectValue(o.CropRectValue),
		NinePatchValue(o.NinePatchValue), NinePatchRectValue(o.NinePatchRectValue), FlipX(o.FlipX), FlipY(o.FlipY),
		TexCoordRotation(o.TexCoordRotation), TexCoordBorderMin(o.TexCoordBorderMin), TexCoordBorderMax(o.TexCoordBorderMax),
		OffsetValue(o.OffsetValue), TexCoordPosition(o.TexCoordPosition), NinePatchScaleValue(o.NinePatchScaleValue), WireframeValue(o.WireframeValue),
		WireframePixelSizeValue(o.WireframePixelSizeValue), UsingColorsArray(o.UsingColorsArray), IsDirty(true) {

		ColorsValue[0] = o.ColorsValue[0];
		ColorsValue[1] = o.ColorsValue[1];
		ColorsValue[2] = o.ColorsValue[2];
		ColorsValue[3] = o.ColorsValue[3];
	}

	/*!
	*	Sets the position of this sprite
	*	\param Pos the new position
	*/
	SpriteDrawOptions &Position(const Vector2 &Pos)
	{
		IsDirty = true;
		PositionValue = Pos;

		return *this;
	}

	/*!
	*	Sets the object's scale
	*	\param Scale the new Scale
	*	\note Scale is based on the texture size if available. Otherwise, it's the size in pixels
	*	Scaling specifies the object's size when NinePatching
	*/
	SpriteDrawOptions &Scale(const Vector2 &Scale)
	{
		IsDirty = true;
		ScaleValue = Scale;

		return *this;
	}
	
	/*!
	*	Sets the object's color
	*	\param Color the color of the object
	*/
	SpriteDrawOptions &Color(const Vector4 &Color)
	{
		IsDirty = true;
		UsingColorsArray = false;

		ColorValue = Color;
		
		return *this;
	}

	/*!
	*	Sets the object's blending mode
	*	\param Blending one of BlendingMode::*
	*/
	SpriteDrawOptions &BlendingMode(uint32 Blending)
	{
		IsDirty = true;
		BlendingModeValue = Blending;
		
		return *this;
	}

	/*!
	*	Sets the object's rotation
	*	\param Rotation the rotation angle, in radians
	*/
	SpriteDrawOptions &Rotation(f32 Rotation)
	{
		IsDirty = true;
		RotationValue = Rotation;
		
		return *this;
	}

	/*!
	*	Sets the origin (Pin) of the object when rendering
	*	\param PinningMode one of PinningMode::*
	*/
	SpriteDrawOptions &Pin(uint32 PinningMode)
	{
		IsDirty = true;
		PinningModeValue = PinningMode;

		return *this;
	}

	/*!
	*	Sets an additional move (Offset) in the object
	*	\param Offset the extra movement of the object
	*/
	SpriteDrawOptions &Offset(const Vector2 &Offset)
	{
		IsDirty = true;
		OffsetValue = Offset;
		return *this;
	}

	/*!
	*	Sets whether this sprite should be wireframe (lines covering the sprite)
	*	\param Value whether to make this wireframe
	*	\note Will ignore actual Textures for rendering but will generate the usual geometry
	*/
	SpriteDrawOptions &Wireframe(bool Value)
	{
		IsDirty = true;
		WireframeValue = Value;

		return *this;
	}

	/*!
	*	Sets the wireframe size for this sprite
	*	\param Value the size in pixels of the wireframe line
	*	\note Will ignore actual Textures for rendering but will generate the usual geometry
	*/
	SpriteDrawOptions &WireframePixelSize(f32 Value)
	{
		IsDirty = true;
		WireframePixelSizeValue = Value;

		return *this;
	}

	/*!
	*	Sets the object's colors for the four corners of the square rendered
	*	\param A the top left corner color
	*	\param B the top right corner color
	*	\param C the bottom left corner color
	*	\param D the bottom right corner color
	*/
	SpriteDrawOptions &Colors(const Vector4 &A, const Vector4 &B, const Vector4 &C, const Vector4 &D)
	{
		IsDirty = true;
		UsingColorsArray = true;

		ColorsValue[0] = A;
		ColorsValue[1] = B;
		ColorsValue[2] = C;
		ColorsValue[3] = D;

		return *this;
	}

	/*!
	*	Sets the texture's rotation
	*	\param Rotation rotation in Radians
	*	\note only works for non-ninepatch
	*/
	SpriteDrawOptions &TextureRotation(f32 Rotation)
	{
		IsDirty = true;
		TexCoordRotation = Rotation;
		
		return *this;
	}

	/*!
	*	Set up texture borders
	*	\param Min the min coordinate
	*	\param Max the maximum coordinate
	*	\note only works for non-Ninepatch
	*/
	SpriteDrawOptions &TextureBorders(const Vector2 &Min, const Vector2 &Max)
	{
		IsDirty = true;
		TexCoordBorderMin = Min;
		TexCoordBorderMax = Max;

		return *this;
	}

	/*!
	*	Sets the texture's position
	*	\param Position is the texture's position
	*	\note only works for non-Ninepatch
	*/
	SpriteDrawOptions &TexturePosition(const Vector2 &Position)
	{
		IsDirty = true;
		TexCoordPosition = Position;

		return *this;
	}
	
	/*!
	*	Flips a texture
	*	\param X flip the texture on the X axis
	*	\param Y flip the texture on the Y axis
	*/
	SpriteDrawOptions &Flip(bool X, bool Y)
	{
		IsDirty = true;
		FlipX = X;
		FlipY = Y;
		
		return *this;
	}

	/*!
	*	Crops a texture
	*	\param CropMode one of CropMode::*
	*	\param CropRect the rectangle to crop with
	*	\note Ignored on Nine Patches
	*/
	SpriteDrawOptions &Crop(uint32 CropMode, const Rect &CropRect)
	{
		IsDirty = true;
		CropModeValue = CropMode;
		CropRectValue = CropRect;
	
		return *this;
	}

	/*!
	*	Sets up as a Nine Patch
	*	\param NinePatch whether to use a Nine patch
	*	\param NinePatchRect the distance between each side of the square for the nine patch
	*/
	SpriteDrawOptions &NinePatch(bool NinePatch, const Rect &NinePatchRect)
	{
		IsDirty = true;
		NinePatchValue = NinePatch;
		NinePatchRectValue = NinePatchRect;

		return *this;
	}

	/*!
	*	Sets the Nine Patch Scale
	*	\param Scale the Nine Patch Scale
	*/
	SpriteDrawOptions &NinePatchScale(f32 Scale)
	{
		NinePatchScaleValue = Scale;

		return *this;
	}
};

struct SpriteVertex
{
	Vector2 Position, TexCoord;
	Vector4 Color;
};

static VertexElementDescriptor SpriteVertexDescriptor[] = {
	{ 0, VertexElementType::Position, VertexElementDataType::Float2 },
	{ sizeof(Vector2), VertexElementType::TexCoord, VertexElementDataType::Float2 },
	{ sizeof(Vector2[2]), VertexElementType::Color, VertexElementDataType::Float4 }
};

class Sprite
{
private:
	SpriteVertex GeneratedGeometry[54];
	uint32 VertexCount;
public:
	DisposablePointer<Texture> SpriteTexture;
	SpriteDrawOptions Options;

	Sprite();
	void Draw(Renderer *Renderer);
};

class SpriteCache
{
	friend class Sprite;
	friend class RendererManager;
private:
	SpriteVertex CachedGeometry[MAX_SPRITE_CACHE_SIZE];
	uint32 CurrentCachePosition;
	Texture *ActiveTexture;
	uint32 CurrentBlendingMode;
public:

	static SpriteCache Instance;

	SpriteCache() : CurrentBlendingMode(BlendingMode::None), ActiveTexture(NULL), CurrentCachePosition(0) {}
	void Register(SpriteVertex *Vertices, uint32 VertexCount, Texture *TheTexture, uint32 BlendingMode, Renderer *Renderer);
	void Flush(Renderer *Renderer);
};
#endif

#pragma once
namespace PinningMode
{
	enum
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
};

namespace CropMode
{
	enum
	{
		None = 0, //Default
		Crop,
		CropNormalized, //[Normalized Coordinates]
		CropTiled, //[Right/Bottom -> TileID, Left/Top -> FrameSize]
	};
};

namespace BlendingMode
{
	enum
	{
		None = 0,
		Alpha,
		Additive,
		Subtractive
	};
};

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

	SpriteDrawOptions() : ColorValue(1, 1, 1, 1), ScaleValue(1, 1), RotationValue(0), BlendingModeValue(BlendingMode::Alpha),
		PinningModeValue(PinningMode::TopLeft), CropModeValue(CropMode::None), NinePatchValue(false), FlipX(false), FlipY(false),
		TexCoordRotation(0), TexCoordBorderMax(1, 1), NinePatchScaleValue(1), WireframeValue(false), WireframePixelSizeValue(1),
		UsingColorsArray(false) {

		ColorsValue[0] = ColorsValue[1] = ColorsValue[2] = ColorsValue[3] = ColorValue;
	};

	SpriteDrawOptions(const SpriteDrawOptions &o) : ColorValue(o.ColorValue), BlendingModeValue(o.BlendingModeValue),
		PositionValue(o.PositionValue), ScaleValue(o.ScaleValue), RotationValue(o.RotationValue),
		PinningModeValue(o.PinningModeValue), CropModeValue(o.CropModeValue), CropRectValue(o.CropRectValue),
		NinePatchValue(o.NinePatchValue), NinePatchRectValue(o.NinePatchRectValue), FlipX(o.FlipX), FlipY(o.FlipY),
		TexCoordRotation(o.TexCoordRotation), TexCoordBorderMin(o.TexCoordBorderMin), TexCoordBorderMax(o.TexCoordBorderMax),
		OffsetValue(o.OffsetValue), TexCoordPosition(o.TexCoordPosition), NinePatchScaleValue(o.NinePatchScaleValue), WireframeValue(o.WireframeValue),
		WireframePixelSizeValue(o.WireframePixelSizeValue), UsingColorsArray(o.UsingColorsArray) {

		ColorsValue[0] = o.ColorsValue[0];
		ColorsValue[1] = o.ColorsValue[1];
		ColorsValue[2] = o.ColorsValue[2];
		ColorsValue[3] = o.ColorsValue[3];
	};

	SpriteDrawOptions &Position(const Vector2 &Pos) { PositionValue = Pos; return *this; };

	/*!
	*	Scaling specifies the object's size when NinePatching
	*/
	SpriteDrawOptions &Scale(const Vector2 &Scale) { ScaleValue = Scale; return *this; };
	
	SpriteDrawOptions &Color(const Vector4 &Color)
	{
		UsingColorsArray = false;

		ColorValue = Color;
		
		return *this;
	};

	SpriteDrawOptions &BlendingMode(uint32 Blending) { BlendingModeValue = Blending; return *this; };
	SpriteDrawOptions &Rotation(f32 Rotation) { RotationValue = Rotation; return *this; };
	SpriteDrawOptions &Pin(uint32 PinningMode) { PinningModeValue = PinningMode; return *this; };
	SpriteDrawOptions &Offset(const Vector2 &Offset) { OffsetValue = Offset; return *this; };

	/*!
	*	Sets whether this sprite should be wireframe (lines covering the sprite)
	*	\param Value whether to make this wireframe
	*	\note Will ignore actual Textures for rendering but will generate the usual geometry
	*/
	SpriteDrawOptions &Wireframe(bool Value)
	{
		WireframeValue = Value;

		return *this;
	};

	/*!
	*	Sets the wireframe size for this sprite
	*	\param Value the size in pixels of the wireframe line
	*	\note Will ignore actual Textures for rendering but will generate the usual geometry
	*/
	SpriteDrawOptions &WireframePixelSize(f32 Value)
	{
		WireframePixelSizeValue = Value;

		return *this;
	};

	SpriteDrawOptions &Colors(const Vector4 &A, const Vector4 &B, const Vector4 &C, const Vector4 &D)
	{
		UsingColorsArray = true;

		ColorsValue[0] = A;
		ColorsValue[1] = B;
		ColorsValue[2] = C;
		ColorsValue[3] = D;

		return *this;
	};

	/*!
	*	Sets the texture's rotation
	*	\param Rotation rotation in Radians
	*	\note only works for non-ninepatch
	*/
	SpriteDrawOptions &TextureRotation(f32 Rotation) { TexCoordRotation = Rotation; return *this; };

	/*!
	*	Set up texture borders
	*	\param Min the min coordinate
	*	\param Max the maximum coordinate
	*	\note only works for non-Ninepatch
	*/
	SpriteDrawOptions &TextureBorders(const Vector2 &Min, const Vector2 &Max)
	{
		TexCoordBorderMin = Min;
		TexCoordBorderMax = Max;

		return *this;
	};

	/*!
	*	Sets the texture's position
	*	\param Position is the texture's position
	*	\note only works for non-Ninepatch
	*/
	SpriteDrawOptions &TexturePosition(const Vector2 &Position)
	{
		TexCoordPosition = Position;

		return *this;
	};
	
	/*!
	*	Flips a texture
	*	\param X flip the texture on the X axis
	*	\param Y flip the texture on the Y axis
	*/
	SpriteDrawOptions &Flip(bool X, bool Y)
	{
		FlipX = X;
		FlipY = Y;
		
		return *this;
	};

	/*!
	*	Crops a texture
	*	\param CropMode one of CropMode::*
	*	\param CropRect the rectangle to crop with
	*	\note Ignored on Nine Patches
	*/
	SpriteDrawOptions &Crop(uint32 CropMode, const Rect &CropRect)
	{
		CropModeValue = CropMode;
		CropRectValue = CropRect;
	
		return *this;
	};

	/*!
	*	Sets up as a Nine Patch
	*	\param NinePatch whether to use a Nine patch
	*	\param NinePatchRect the distance between each side of the square for the nine patch
	*/
	SpriteDrawOptions &NinePatch(bool NinePatch, const Rect &NinePatchRect)
	{
		NinePatchValue = NinePatch;
		NinePatchRectValue = NinePatchRect;

		return *this;
	};

	/*!
	*	Sets the Nine Patch Scale
	*	\param Scale the Nine Patch Scale
	*/
	SpriteDrawOptions &NinePatchScale(f32 Scale)
	{
		NinePatchScaleValue = Scale;

		return *this;
	};
};

class Sprite
{
public:
	SuperSmartPointer<Texture> SpriteTexture;
	SpriteDrawOptions Options;

	void Draw(RendererManager::Renderer *Renderer);
};

class AnimatedSprite : public Sprite
{
	struct AnimationInfo
	{
		std::vector<Vector2> Frames;
		bool Repeating;
		uint32 CurrentFrame;

		AnimationInfo() : Repeating(true), CurrentFrame(0) {};
	};

	typedef std::map<StringID, AnimationInfo> FrameMap;
	FrameMap Animations;
	uint64 LastFrameUpdate;
	AnimationInfo *CurrentAnimation;
public:
	Vector2 FrameSize, DefaultFrame, Scale;

	uint64 FrameInterval;

	AnimatedSprite(const Vector2 &SpriteFrameSize, const Vector2 &SpriteDefaultFrame = Vector2()) :
		FrameSize(SpriteFrameSize), LastFrameUpdate(0), CurrentAnimation(NULL), DefaultFrame(SpriteDefaultFrame),
		FrameInterval(250), Scale(1, 1) {};

	void AddAnimation(const std::string &Name, const std::vector<Vector2> &Frames);
	//May pass any name if wishing to set no animation
	void SetAnimation(const std::string &Name, bool Repeats);
	void StopAnimation();

	void Update();
};

class SpriteCache
{
	friend class Sprite;
	friend class RendererManager;
private:
	std::vector<Vector4> CachedColors;
	std::vector<Vector2> CachedVertices, CachedTexCoords;
	SuperSmartPointer<Texture> ActiveTexture;
	uint32 CurrentBlendingMode;
public:

	static SpriteCache Instance;

	SpriteCache() : CurrentBlendingMode(BlendingMode::None) {};
	void Register(Vector2 *Vertices, Vector2 *TexCoords, Vector4 *Color, uint32 VertexCount, SuperSmartPointer<Texture> Texture, uint32 BlendingMode, RendererManager::Renderer *Renderer);
	void Flush(RendererManager::Renderer *Renderer);
};

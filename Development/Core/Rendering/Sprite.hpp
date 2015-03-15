#pragma once
#if USE_GRAPHICS

class SpriteDrawOptions
{
public:
	Vector2 PositionValue, ScaleValue, OffsetValue;
	f32 RotationValue;
	bool CropValue;
	Rect CropRectValue;
	Vector4 ColorValue;
	uint32 BlendingModeValue;
	bool FlipX, FlipY;
	bool WireframeValue;
	f32 WireframePixelSizeValue;
	bool NinePatchValue;
	f32 NinePatchScaleValue;
	Rect NinePatchRectValue;
	bool IsDirty;

	SpriteDrawOptions() : ColorValue(1, 1, 1, 1), ScaleValue(1, 1), RotationValue(0), BlendingModeValue(BlendingMode::Alpha),
		FlipX(false), FlipY(false), WireframeValue(false), WireframePixelSizeValue(1), NinePatchValue(false), NinePatchScaleValue(1),
		IsDirty(true), CropValue(false) {
	}

	SpriteDrawOptions(const SpriteDrawOptions &o) : ColorValue(o.ColorValue), BlendingModeValue(o.BlendingModeValue), PositionValue(o.PositionValue),
		ScaleValue(o.ScaleValue), RotationValue(o.RotationValue), CropRectValue(o.CropRectValue), FlipX(o.FlipX), FlipY(o.FlipY), OffsetValue(o.OffsetValue),
		WireframeValue(o.WireframeValue), WireframePixelSizeValue(o.WireframePixelSizeValue), CropValue(o.CropValue), NinePatchValue(o.NinePatchValue),
		NinePatchScaleValue(o.NinePatchScaleValue), NinePatchRectValue(o.NinePatchRectValue), IsDirty(true) {
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
	*	\param Cropping whether we're cropping
	*	\param CropRect the rectangle to crop with
	*/
	SpriteDrawOptions &Crop(bool Cropping, const Rect &CropRect)
	{
		IsDirty = true;
		CropValue = Cropping;
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

class Sprite
{
private:
	PosTexCol GeneratedGeometry[54];
	uint32 VertexCount;
public:
	DisposablePointer<Texture> SpriteTexture;
	SpriteDrawOptions Options;

	Sprite();
	void Draw();
};
#endif

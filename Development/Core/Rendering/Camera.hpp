#pragma once

#if USE_GRAPHICS
class Renderer;
#endif

/*!
*	2D/3D Camera Class
*/
class Camera
{
public:
	/*!
	*	World Transformation
	*/
	Matrix4x4 WorldTransform;

	/*!
	*	Projection Transformation
	*/
	Matrix4x4 ProjectionTransform;

	/*!
	*	Drawing Offset (Distance)
	*/
	Vector3 DrawOffset;

	/*!
	*	Set Projection as Orthographic
	*	\param Bounds is the bounds of the ortho rect
	*	\param zNear is the near plane
	*	\param zFar is the far plane
	*/
	void SetOrtho(Rect Bounds, f32 zNear, f32 zFar);

	/*!
	*	Set Projection as Perspective
	*	\param FOV the Field Of Vision (Radians)
	*	\param Aspect the aspect ratio
	*	\param zNear is the near plane
	*	\param zFar is the far plane
	*/
	void SetPerspective(f32 FOV, f32 Aspect, f32 zNear, f32 zFar);

#if USE_GRAPHICS
	/*!
	*	Begin using the Camera's Transform
	*/
	void BeginTransforms();

	/*!
	*	End using the Camera's Transform
	*/
	void EndTransforms();
#endif
};

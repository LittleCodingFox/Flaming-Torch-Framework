#include "FlamingCore.hpp"

namespace FlamingTorch
{
	void Camera::SetOrtho(Rect Bounds, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::OrthoMatrixRH(Bounds.Left, Bounds.Right, Bounds.Top, Bounds.Bottom, zNear, zFar);
	};

	void Camera::SetPerspective(f32 FOV, f32 Aspect, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::PerspectiveMatrixRH(FOV, Aspect, zNear, zFar);
	};

#if USE_GRAPHICS
	void Camera::BeginTransforms(Renderer *TheRenderer)
	{
		Matrix4x4 CameraTransform = WorldTransform;
		CameraTransform.Inverse();

		CameraTransform = CameraTransform * Matrix4x4::Translate(Vector4(DrawOffset, 1));

		TheRenderer->SetProjectionMatrix(ProjectionTransform);
		TheRenderer->SetWorldMatrix(CameraTransform);
	};

	void Camera::EndTransforms(Renderer *TheRenderer)
	{
	};
#endif
};

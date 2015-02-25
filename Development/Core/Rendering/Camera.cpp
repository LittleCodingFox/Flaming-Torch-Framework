#include "FlamingCore.hpp"

namespace FlamingTorch
{
	void Camera::SetOrtho(Rect Bounds, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::OrthoMatrixRH(Bounds.Left, Bounds.Right, Bounds.Bottom, Bounds.Top, zNear, zFar);
	}

	void Camera::SetPerspective(f32 FOV, f32 Aspect, f32 zNear, f32 zFar)
	{
		ProjectionTransform = Matrix4x4::PerspectiveMatrixRH(FOV, Aspect, zNear, zFar);
	}

#if USE_GRAPHICS
	void Camera::BeginTransforms()
	{
		Matrix4x4 CameraTransform = WorldTransform;
		CameraTransform.Inverse();

		CameraTransform = CameraTransform * Matrix4x4::Translate(Vector4(DrawOffset, 1));

		g_Renderer.PushMatrices();
		g_Renderer.SetProjectionMatrix(ProjectionTransform);
		g_Renderer.SetWorldMatrix(CameraTransform);
	}

	void Camera::EndTransforms()
	{
		g_Renderer.PopMatrices();
	}
#endif
}

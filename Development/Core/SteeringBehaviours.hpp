#pragma once

class SteeringBehaviours
{
public:
	static Vector3 Seek(const Vector3 &Position, const Vector3 &Target, f32 MaxForce)
	{
		Vector3 Velocity = (Target - Position).Normalized();

		return Position + Velocity * MaxForce;
	};

	static Vector3 Flee(const Vector3 &Position, const Vector3 &Target, f32 MaxForce)
	{
		Vector3 Velocity = (Position - Target).Normalized();

		return Position + Velocity * MaxForce;
	};
};

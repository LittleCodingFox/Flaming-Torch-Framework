#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "Physics"

	Vector2 PhysicsBody::Position() const
	{
		const b2Vec2 &PositionValue = Body->GetPosition();

		return Vector2(PositionValue.x, PositionValue.y);
	}

	f32 PhysicsBody::Rotation() const
	{
		return Body->GetAngle();
	}

	Vector2 PhysicsBody::Size() const
	{
		return SizeValue;
	}

	void PhysicsBody::SetPosition(const Vector2 &Position)
	{
		Body->SetTransform(b2Vec2(Position.x, Position.y), Body->GetAngle());
	}

	void PhysicsBody::SetRotation(f32 Rotation)
	{
		Body->SetTransform(Body->GetPosition(), Rotation);
	}
}

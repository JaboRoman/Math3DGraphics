#include "Precompiled.h"
#include "ColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include <Graphics\Inc\SimpleDraw.h>

using namespace Graphics;

AABoxColliderComponent::AABoxColliderComponent()
	: mTransformComponent(nullptr)
	, mCenter(Math::Vector3::Zero())
	, mExtend(Math::Vector3::One())
	, mColor(Math::Vector4::Green())
{
}

AABoxColliderComponent::~AABoxColliderComponent()
{
}

void AABoxColliderComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
}

void AABoxColliderComponent::Render()
{
	SimpleDraw::DrawAABB(GetAABB(), mColor);
}

bool AABoxColliderComponent::CheckCollision(AABoxColliderComponent& boxB)
{
	float x1Max = mCenter.x + mExtend.x;
	float x1Min = mCenter.x - mExtend.x;
	float x2Max = boxB.mCenter.x + boxB.mExtend.x;
	float x2Min = boxB.mCenter.x - boxB.mExtend.x;

	float y1Max = mCenter.y + mExtend.y;
	float y1Min = mCenter.y - mExtend.y;
	float y2Max = boxB.mCenter.y + boxB.mExtend.y;
	float y2Min = boxB.mCenter.y - boxB.mExtend.y;

	float z1Max = mCenter.z + mExtend.z;
	float z1Min = mCenter.z - mExtend.z;
	float z2Max = boxB.mCenter.z + boxB.mExtend.z;
	float z2Min = boxB.mCenter.z - boxB.mExtend.z;

	if (x1Max < x2Min || x1Min > x2Max) return false;
	if (y1Max < y2Min || y1Min > y2Max) return false;
	if (z1Max < z2Min || z1Min > z2Max) return false;

	mColor = boxB.mColor = Math::Vector4::Red();

	return true;
}

Math::AABB AABoxColliderComponent::GetAABB() const
{
	Math::Vector3 pos = mTransformComponent->GetPosition();
	return Math::AABB(mCenter + pos, mExtend);
}
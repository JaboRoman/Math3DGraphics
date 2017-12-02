#include "Precompiled.h"
#include "World.h"

#include "Constraints.h"
#include "Particle.h"

using namespace Physics;

World::World()
	: mTimer{ 0.0f }
{}

World::~World() {}

void World::Update(float deltaTime)
{
	mTimer += deltaTime;
	while (mTimer >= mSettings.timeStep)
	{
		mTimer -= mSettings.timeStep;
		AccumulateForces();
		Integrate();
		SatisfyConstraints();
	}
}

void World::AddParticle(Particle* p)
{
	mParticles.push_back(p);
}

void World::AddConstraint(Constraint* c)
{
	mConstraints.push_back(c);
}

void Physics::World::AddCube(Physics::World& world, Math::Vector3 position, Math::Vector3 velocity, float width, float invMass, bool fixed)
{
	float particleinvMass = invMass / 8;
	float halfWidth = width * 0.5f;

	Particle* particles[8];
	for (int i = 0; i < 8; ++i)
	{
		auto p = new Physics::Particle();
		p->SetRadius(0.3f);
		p->SetInvMass(particleinvMass);
		p->SetVelocity(velocity);
		particles[i] = p;
		world.AddParticle(p);
	}
	particles[0]->SetPosition({ position.x - halfWidth, position.y + halfWidth, position.z + halfWidth });
	particles[1]->SetPosition({ position.x + halfWidth, position.y + halfWidth, position.z + halfWidth });
	particles[2]->SetPosition({ position.x - halfWidth, position.y + halfWidth, position.z - halfWidth });
	particles[3]->SetPosition({ position.x + halfWidth, position.y + halfWidth, position.z - halfWidth });

	particles[4]->SetPosition({ position.x - halfWidth, position.y - halfWidth, position.z + halfWidth });
	particles[5]->SetPosition({ position.x + halfWidth, position.y - halfWidth, position.z + halfWidth });
	particles[6]->SetPosition({ position.x - halfWidth, position.y - halfWidth, position.z - halfWidth });
	particles[7]->SetPosition({ position.x + halfWidth, position.y - halfWidth, position.z - halfWidth });

	// top square
	for (int i = 0; i < 4; ++i)
	{
		auto c0 = new Physics::Spring(particles[i], particles[(i + 1) % 4], width);
		auto c1 = new Physics::Spring(particles[i], particles[(i + 4)], width);

		world.AddConstraint(c0);
		world.AddConstraint(c1);
	}
	// bottom square
	for (int i = 0; i < 4; ++i)
	{
		auto c = new Physics::Spring(particles[i + 4], particles[((i + 1) % 4) + 4], width);
		world.AddConstraint(c);
	}

	float squareDiagonalLength = Math::Sqrt(Math::Sqr(width) * 2.0f); // pythagoras
	float cubeDiagonalLength = Math::Sqrt(Math::Sqr(width) * Math::Sqr(squareDiagonalLength));

	// diagonals across top
	for (int i = 0; i < 4; ++i)
	{
		auto c = new Physics::Spring(particles[i], particles[(i + 2) % 4], squareDiagonalLength);
		world.AddConstraint(c);
	}

	// diagonals across bottom
	for (int i = 0; i < 4; ++i)
	{
		auto c = new Physics::Spring(particles[i + 4], particles[((i + 2) % 4) + 4], squareDiagonalLength);
		world.AddConstraint(c);
	}

	// diagonals across sides
	auto c = new Physics::Spring(particles[0], particles[5], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[1], particles[7], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[2], particles[4], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[3], particles[6], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[0], particles[6], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[1], particles[4], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[2], particles[7], squareDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[3], particles[5], squareDiagonalLength);
	world.AddConstraint(c);

	// diagonals across center
	c = new Physics::Spring(particles[0], particles[7], cubeDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[1], particles[6], cubeDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[2], particles[5], cubeDiagonalLength);
	world.AddConstraint(c);

	c = new Physics::Spring(particles[3], particles[4], cubeDiagonalLength);
	world.AddConstraint(c);

	if (fixed)
	{
		auto f = new Physics::Fixed(particles[0], particles[0]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[1], particles[1]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[2], particles[2]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[3], particles[3]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[4], particles[4]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[5], particles[5]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[6], particles[6]->mPosition);
		world.AddConstraint(f);
		f = new Physics::Fixed(particles[7], particles[7]->mPosition);
		world.AddConstraint(f);
	}

}

void World::ClearDynamic()
{
	SafeDeleteVector(mParticles);
	SafeDeleteVector(mConstraints);
}

void World::DebugDraw() const
{
	for (const auto p : mParticles)
	{
		p->DebugDraw();
	}
	for (const auto c : mConstraints)
	{
		c->DebugDraw();
	}
}

void World::AccumulateForces()
{
	for (auto p : mParticles)
	{
		p->mAcceleration = mSettings.gravity;
	}
}

void World::Integrate()
{
	const float timeStepSqr = Math::Sqr(mSettings.timeStep);
	for (auto p : mParticles)
	{
		Math::Vector3 displacement{ p->mPosition - p->mPositionOld + (p->mAcceleration * timeStepSqr) };
		p->mPositionOld = p->mPosition;
		p->mPosition = p->mPosition + displacement;
	}
}

void World::SatisfyConstraints()
{
	for (auto c : mConstraints)
	{
		c->Apply();
	}
}
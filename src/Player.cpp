#include "Player.h"
#include "utils.h"
#include <iostream>
#include <cstdlib>

Player::Player(const Point2f& pos)
	: m_Player{ 0, 360, 0, -1, 0, 1, 1, 0, 0, 0, 0, pos.x, pos.y, 1.f, 1.f, 0 }
	, m_Position{ m_Player.Grade3() }
	, m_DirectionVector{ m_Player.Grade2() }
	, m_MirrorPlane{ m_Player.Grade1() }
	, m_Energy{ m_Player[0] }
	, m_EnergyMax{ 100.f }
	, m_EnergyRegen{ 0.05f }
	, m_Health {}
	, m_Dizziness{}
	, m_DizzinessRegen{ 0.05f }
	, m_DizzinessMax{100}
	, m_MirrorUses{3}
	, m_MirrorUsesMax{3}
	, m_CurrentRotationTarget{}
	, m_RotationTargets{}
	, m_Angle{ 0.0f }
	, m_Size{ 20.0f }
	, m_Speed{ 200.0f }
	, m_MaxSpeed{ 400.0f }
	, m_MinSpeed{ 100.0f }
	, m_BoostAccel{ 1.1f }
	, m_Friction{ 0.9999f }
	, m_RotationSpeed{ 180.0f }
	, m_MaxRotationSpeed{ 180.0f }
	, m_IsBoosting{ false }
	, m_IsRotating{ false }
{
	m_Energy = 100.f;
}

Player::~Player()
{
}

void Player::Update(float elapsedSec)
{
	if (m_IsRotating)
	{
		// Rotate the player
		Rotate(elapsedSec, m_CurrentRotationTarget);
		m_Dizziness += 0.1f;
		if (m_Dizziness >= m_DizzinessMax)
		{
			m_IsRotating = false;
		}
	}
	else
	{
		// Move the player
		Move(elapsedSec);
		if (m_Dizziness > 0)
		{
			m_Dizziness -= m_DizzinessRegen;
		}
		else m_Dizziness = 0;
	}

	if (m_IsBoosting)
	{
		GenerateParticles(elapsedSec);
	}

	// Update particles
	for (auto& particle : m_Particles)
	{
		float particleSpeed = particle.velocity.VNorm() * elapsedSec;
		Motor translator = Motor::Translation(particleSpeed, particle.velocity);

		particle.position = (translator * particle.position * ~translator).Grade3();
		particle.lifetime -= elapsedSec;
	}

	// Remove expired particles
	m_Particles.erase(
		std::remove_if(m_Particles.begin(), m_Particles.end(), [](const Particle& p) { return p.lifetime <= 0; }),
		m_Particles.end()
	);
}

void Player::Draw() const
{
	// Optionally, draw the player's boundaries for debugging
	Rectf boundaries = GetBoundaries();
	utils::SetColor(Color4f(1.0f, 0.0f, 0.0f, 1.0f)); // Red color for boundaries
	utils::DrawRect(boundaries);
	utils::DrawLine(m_Position[0], m_Position[1], m_Position[0] + m_DirectionVector[0] * 20, m_Position[1] + m_DirectionVector[1] * 20);
	if (m_IsRotating)
	{
		utils::DrawLine(m_Position[0], m_Position[1], m_CurrentRotationTarget[0], m_CurrentRotationTarget[1]);
	}

	// Draw particles
	DrawParticles();
}

#pragma region Movement

void Player::Move(float elapsedSec)
{
	if (m_IsBoosting && m_Energy > 0)
	{
		if (m_Speed < m_MaxSpeed)
		{
			m_Speed *= m_BoostAccel;
		}
		else
		{
			m_Speed = m_MaxSpeed;
		}
		m_Energy -= 0.1f;
	}
	else
	{
		m_IsBoosting = false;
		if (m_Speed > m_MinSpeed)
		{
			m_Speed *= m_Friction;
		}
		else
		{
			m_Speed = m_MinSpeed;
		}

		if (m_Energy < m_EnergyMax)
		{
			m_Energy += m_EnergyRegen;
		}
	}

	float speed = m_Speed * elapsedSec;

	Motor translator = Motor::Translation(speed, m_DirectionVector);

	m_Position = (translator * m_Position * ~translator).Grade3();

	HandleCollision();
}

void Player::Rotate(float elapsedSec, const TwoBlade& rotationTargetLine)
{
	Motor rotator(-m_RotationSpeed, rotationTargetLine.VNorm(), rotationTargetLine, TwoBlade{ 0,0,0,0,0,-1 });

	m_Position = (rotator * m_Position * ~rotator).Grade3();
	CalculateTangent();
}

void Player::Boost(float elapsedSec)
{
	// Generate particles during boosting
	GenerateParticles(elapsedSec);
}

void Player::SlowDown(float elapsedSec)
{
}

void Player::ToggleBoosting(const bool keyDown)
{
	m_IsBoosting = keyDown;
}

void Player::ToggleRotating()
{
	if (m_IsRotating)
	{
		CalculateTangent();
	}
	else
	{
		// Entering rotation: Compute rotation speed and direction
		float dx = m_CurrentRotationTarget[0] - m_Position[0];
		float dy = m_CurrentRotationTarget[1] - m_Position[1];
		float distance = sqrtf(dx * dx + dy * dy);

		// Ensure the distance is valid to avoid division by zero
		if (distance > 0)
		{
			float crossProduct = m_DirectionVector[0] * dy - m_DirectionVector[1] * dx;
			m_RotationSpeed = (m_Speed / distance) * (crossProduct < 0 ? -1.0f : 1.0f);
		}
	}

	m_IsRotating = !m_IsRotating;
}

void Player::HandleCollision()
{
	OneBlade leftWall{ 0,1,0,0 };
	OneBlade rightWall{ 1280, -1,0,0 };
	OneBlade topWall{ 720,0, -1,0 };
	OneBlade bottomWall{ 0,0,1,0 };

	leftWall.Normalize();
	rightWall.Normalize();
	topWall.Normalize();
	bottomWall.Normalize();

	if ((m_Position.Normalize() & leftWall) <= 0)
	{
		m_Position = (-leftWall * m_Position * leftWall).Grade3();
		m_DirectionVector[0] *= -0.9f - static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (-1.0f + 0.9f)));
		m_DirectionVector.VNormalize();
	}
	if ((m_Position.Normalize() & rightWall) <= 0)
	{
		m_Position = (-rightWall * m_Position * rightWall).Grade3();
		m_DirectionVector[0] *= -0.9f - static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (-1.0f + 0.9f)));
		m_DirectionVector.VNormalize();
	}
	if ((m_Position.Normalize() & topWall) <= 0)
	{
		m_Position = (-topWall * m_Position * topWall).Grade3();
		m_DirectionVector[1] *= -0.9f - static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (-1.0f + 0.9f)));
		m_DirectionVector.VNormalize();
	}
	if ((m_Position.Normalize() & bottomWall) <= 0)
	{
		m_Position = (-bottomWall * m_Position * bottomWall).Grade3();
		m_DirectionVector[1] *= -0.9f - static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (-1.0f + 0.9f)));
		m_DirectionVector.VNormalize(); //Created a function to normalize the direction vector thus VNolmalize() else the direction vectors values will keep dropping closer to zero
	}
}

void Player::Mirror()
{
	if (!m_IsRotating && m_MirrorUses > 0)
	{
		// Entering rotation: Compute rotation speed and direction
		//float dx = 1280 - m_CurrentRotationTarget[0];
		//float dy = 720 - m_CurrentRotationTarget[1];
		//
		//m_MirrorPlane[0] = 0;
		//m_MirrorPlane[1] = dy;
		//
		//std::cout << dy << std::endl;
		m_Position = (m_MirrorPlane * m_Position *~ m_MirrorPlane).Grade3();

		//m_MirrorPlane[0] = dx;
		//m_MirrorPlane[1] = 0;
		//m_Position = (m_MirrorPlane * m_Position * ~m_MirrorPlane).Grade3();


		m_DirectionVector *= -1;
		--m_MirrorUses;
	}
}

#pragma endregion

#pragma region Particles

void Player::GenerateParticles(float elapsedSec)
{
	// Generate a new particle
	Particle particle;
	particle.position = m_Position;
	particle.velocity[0] = m_DirectionVector[0] * -50.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 20.0f)) - 10.0f;
	particle.velocity[1] = m_DirectionVector[1] * -50.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 20.0f)) - 10.0f;
	particle.lifetime = 1.0f; // 1 second lifetime
	particle.color = Color4f(1.0f, 0.5f, 0.0f, 1.0f); // Orange color

	m_Particles.push_back(particle);
}

void Player::DrawParticles() const
{
	for (const auto& particle : m_Particles)
	{
		utils::SetColor(particle.color);
		utils::FillEllipse(Point2f(particle.position[0],particle.position[1]), 2.0f, 2.0f); // Draw small circles as particles
	}
}

void Player::LoadRotationTargets(std::vector<Pillar>& pillars)
{
	m_RotationTargets.clear();
	m_RotationTargets = pillars;
	m_CurrentRotationTarget = m_RotationTargets[0].GetLine();
}

void Player::ChangeRotationTarget(Point2f mousePos, float radius, bool isPressed)
{
	if (isPressed)
	{
		// Find the closest rotation target
		float minDistance = std::numeric_limits<float>::max();
		for (const Pillar& target : m_RotationTargets)
		{
			float dx = target.GetLine()[0] - mousePos.x;
			float dy = target.GetLine()[1] - mousePos.y;
			float distance = sqrtf(dx * dx + dy * dy);
		
			if (distance < minDistance && distance <= radius)
			{
				minDistance = distance;
				m_CurrentRotationTarget = target.GetLine();
			}
		}
	}
}

#pragma endregion

#pragma region Drawing


#pragma endregion

#pragma region Utility

void Player::CalculateTangent()
{
	// Compute the relative position vector from the rotation target to the player
	float dx = m_Position[0] - m_CurrentRotationTarget[0];
	float dy = m_Position[1] - m_CurrentRotationTarget[1];

	// Compute the cross product to determine rotation direction
	float crossProduct = m_DirectionVector[0] * dy - m_DirectionVector[1] * dx;

	// Determine tangent direction based on rotation direction
	if (crossProduct < 0)
	{
		// Clockwise rotation: tangent is 90° clockwise from the radius
		m_DirectionVector = { dy, -dx, 0, 0, 0, 0 };
	}
	else
	{
		// Counterclockwise rotation: tangent is 90° counterclockwise from the radius
		m_DirectionVector = { -dy, dx, 0, 0, 0, 0 };
	}

	// Normalize the tangent direction
	float length = sqrtf(m_DirectionVector[0] * m_DirectionVector[0] + m_DirectionVector[1] * m_DirectionVector[1]);
	if (length > 0)
	{
		m_DirectionVector[0] /= length;
		m_DirectionVector[1] /= length;
	}
}

#pragma endregion

#pragma region Getters

Point2f Player::GetPosition() const
{
	return Point2f();
}

Rectf Player::GetBoundaries() const
{
	// Calculate the boundaries of the player
	Point2f position(m_Position[0], m_Position[1]);
	float halfSize = m_Size / 2.0f;
	return Rectf(position.x - halfSize, position.y - halfSize, m_Size, m_Size);
}

float Player::GetEnergy() const
{
	return m_Energy;
}

float Player::GetSpeed() const
{
	return m_Speed;
}

float Player::GetMaxSpeed() const
{
	return m_MaxSpeed;
}

float Player::GetHealth() const
{
	return m_Health;
}

float Player::GetDissiness() const
{
	return m_Dizziness;
}

int Player::GetMirrorUses() const
{
	return m_MirrorUses;
}

int Player::GetMirrorUsesMax() const
{
	return m_MirrorUsesMax;
}

#pragma endregion

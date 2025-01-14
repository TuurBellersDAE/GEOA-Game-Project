#include "Enemy.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Enemy::Enemy(const Point2f& pos)
    : m_Position{ pos.x, pos.y, 1.f , 1.f }
    , m_DirectionVector{ 1, 1, 0, 0, 0, 0 }
    , m_Speed{ 300.0f }
    , m_MaxSpeed{ 300.0f }
    , m_MinSpeed{ 50.0f }
    , m_Friction{ 0.99f }
    , m_Size{ 50.0f }
	, m_IsDead{ false }
{
    // Randomize initial direction
    m_DirectionVector[0] = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
    m_DirectionVector[1] = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
    m_DirectionVector.VNormalize();
}

Enemy::~Enemy()
{
}

void Enemy::Update(float elapsedSec,Player& player)
{
	if (!m_IsDead)
	{
		Move(elapsedSec);
		HandleCollision(player);
    }
}

void Enemy::Draw() const
{
	if (!m_IsDead)
	{
		Rectf boundaries = GetBoundaries();
		utils::SetColor(Color4f(0.0f, 1.0f, 0.0f, 1.0f)); // Green color for enemy
		utils::DrawRect(boundaries);
		utils::DrawLine(m_Position[0], m_Position[1], m_Position[0] + m_DirectionVector[0] * 20, m_Position[1] + m_DirectionVector[1] * 20);
	}
}

void Enemy::Move(float elapsedSec)
{
    float speed = m_Speed * elapsedSec;
    Motor translator = Motor::Translation(speed, m_DirectionVector);

    m_Position = (translator * m_Position * ~translator).Grade3();
}

void Enemy::HandleCollision(Player& player)
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
        m_DirectionVector.VNormalize();
    }

    // Check collision with player
    Rectf playerBounds = player.GetBoundaries();
    Rectf enemyBounds = GetBoundaries();

    if (enemyBounds.left < playerBounds.left + playerBounds.width &&
        enemyBounds.left + enemyBounds.width > playerBounds.left &&
        enemyBounds.bottom < playerBounds.bottom + playerBounds.height &&
        enemyBounds.bottom + enemyBounds.height > playerBounds.bottom)
    {
        if (player.IsRotating() || player.IsBoosting())
        {
            // Instead of deleting the object, we can set a flag to mark it for deletion
            m_IsDead = true;
        }
		else
		{
			// Collision detected, bounce off
			m_DirectionVector[0] *= -1.0f;
			m_DirectionVector[1] *= -1.0f;
			m_DirectionVector.VNormalize();
			player.TakeDamage();
			player.TriggerCollision();
		}
    }
}

void Enemy::Revive(const Point2f& pos)
{
	m_Position = ThreeBlade(pos.x, pos.y, 1.f);
	m_IsDead = false;

	// Randomize initial direction
	m_DirectionVector[0] = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
	m_DirectionVector[1] = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
	m_DirectionVector.VNormalize();
}

Point2f Enemy::GetPosition() const
{
    return Point2f(m_Position[0], m_Position[1]);
}

Rectf Enemy::GetBoundaries() const
{
    Point2f position(m_Position[0], m_Position[1]);
    float halfSize = m_Size / 2.0f;
    return Rectf(position.x - halfSize, position.y - halfSize, m_Size, m_Size);
}

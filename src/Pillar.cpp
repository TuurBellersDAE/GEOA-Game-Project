#include "Pillar.h"

Pillar::Pillar(Point2f position, float radius)
    : m_Position{ position.x,position.y, 1.f, 1.f}
    , m_Radius{ radius }
    , m_Color{ 0.0f, 0.0f, 1.0f, 1.0f }
{
}

void Pillar::Draw() const
{
    utils::SetColor(m_Color);
    utils::FillEllipse(Point2f(m_Position[0], m_Position[1]), m_Radius, m_Radius);
}

ThreeBlade Pillar::GetPosition() const
{
    return m_Position;
}

TwoBlade Pillar::GetLine() const
{
    return TwoBlade{ m_Position[0], m_Position[1],10,0,0,0 };
}

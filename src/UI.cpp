#include "UI.h"

UI::UI(const Player& player)
	: m_Player(player)
	, m_EnergyBarWidth(200.0f)
	, m_BarHeight(20.0f)
	, m_BarPadding(10.0f)
	, m_EnergyBarColor{ 0.0f, 1.0f, 0.0f, 0.5f } // Green
	, m_DizzinessBarColor{ 1.0f, 0.0f, 1.0f, 0.5f } // Red
	, m_MirrorBarColor{ 1.0f, 1.0f, 0.0f, 0.5f }
	, m_BackgroundColor{ 0.2f, 0.2f, 0.2f, .5f } // Dark Gray
	, m_CircleRadius(10.0f) // Initialize circle radius
{
}

UI::~UI()
{
}

void UI::Update(float elapsedSec)
{
	// Update logic if needed
}

void UI::Draw() const
{
	DrawHealthBar();
	DrawEnergyBar();
	DrawDizzinessBar();
	DrawMirrorUses();
}

void UI::DrawHealthBar() const
{
	float health = m_Player.GetHealth();
	float maxHealth = 100.0f; // Assuming max health is 100
	float healthBarWidth = m_EnergyBarWidth;
	float healthBarHeight = m_BarHeight;
	float healthPercentage = health / maxHealth;

	// Background bar
	utils::SetColor(m_BackgroundColor);
	utils::FillRect(10.0f, 10.0f, healthBarWidth, healthBarHeight);

	// Health bar
	utils::SetColor(Color4f(1.0f, 0.0f, 0.0f, 1.0f)); // Red color for health
	utils::FillRect(10.0f, 10.0f, healthBarWidth * healthPercentage, healthBarHeight);
}

void UI::DrawEnergyBar() const
{
	float energyPercentage = m_Player.GetEnergy() / 100.0f;
	float energyBarCurrentWidth = m_EnergyBarWidth * energyPercentage;

	utils::SetColor(m_BackgroundColor);
	utils::FillRect(10.0f, 40.0f, m_EnergyBarWidth, m_BarHeight);

	utils::SetColor(m_EnergyBarColor);
	utils::FillRect(10.0f, 40.0f, energyBarCurrentWidth, m_BarHeight);
}

void UI::DrawDizzinessBar() const
{
	float dizzinessPercentage = m_Player.GetDissiness() / 100.0f;
	float dizzinessBarCurrentWidth = m_EnergyBarWidth * dizzinessPercentage;

	utils::SetColor(m_BackgroundColor);
	utils::FillRect(10.0f, 70.0f, m_EnergyBarWidth, m_BarHeight);

	utils::SetColor(m_DizzinessBarColor);
	utils::FillRect(10.0f, 70.0f, dizzinessBarCurrentWidth, m_BarHeight);
}

void UI::DrawMirrorUses() const
{
	int mirrorUses = m_Player.GetMirrorUses();
	for (int i = 0; i < mirrorUses; ++i)
	{
		float x = 20.0f + i * (2 * m_CircleRadius + m_BarPadding);
		float y = 100.0f; // Position below the bars
		utils::SetColor(m_MirrorBarColor); // Use mirror bar color for circles
		utils::FillEllipse(x, y, m_CircleRadius, m_CircleRadius);
	}
}

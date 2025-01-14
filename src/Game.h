#pragma once
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "Player.h"
#include "UI.h"
#include "Pillar.h"

enum class GameState
{
	Menu,
	Playing,
	Paused,
	GameOver
};

class Game
{
public:
	explicit Game( const Window& window );
	Game( const Game& other ) = delete;
	Game& operator=( const Game& other ) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run( );

	void Update(float elapsedSec);

	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		switch (e.keysym.sym)
		{
		case SDLK_SPACE:
			m_Player.ToggleRotating();
			break;
		case SDLK_b:
			m_Player.ToggleBoosting(true);
			break;
		}
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{
		switch (e.keysym.sym)
		{
		case SDLK_b:
			m_Player.ToggleBoosting(false);
			break;
		}
	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{
		m_MousePos = Point2f(e.x, e.y);
	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			// Handle left mouse button press
			m_Player.ChangeRotationTarget(m_MousePos, m_MouseRadius, true);
		}
		else if (e.button == SDL_BUTTON_RIGHT)
		{
			// Handle right mouse button press
		}
	}
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			// Handle left mouse button press
			m_Player.ChangeRotationTarget(m_MousePos, m_MouseRadius, false);
		}
		else if (e.button == SDL_BUTTON_RIGHT)
		{
			// Handle right mouse button press
		}
	}

	const Rectf& GetViewPort() const
	{
		return m_Viewport;
	}

private:
	// DATA MEMBERS
	// The window properties
	const Window m_Window;
	const Rectf m_Viewport;
	// The window we render to
	SDL_Window* m_pWindow;
	// OpenGL context
	SDL_GLContext m_pContext;
	// Init info
	bool m_Initialized;
	// Prevent timing jumps when debugging
	const float m_MaxElapsedSeconds;
	
	// FUNCTIONS
	void InitializeGameEngine( );
	void CleanupGameEngine( );

	GameState m_GameState{ GameState::Playing };

	Player m_Player{ Point2f{200,400} };
	UI m_UI{ m_Player };
	Pillar m_CoolPillar1{ Point2f{m_Window.width / 2,m_Window.height / 2}, 10};
	Pillar m_CoolPillar2{ Point2f{m_Window.width / 3,m_Window.height / 3}, 10};
	std::vector<Pillar> m_Pillars{
		m_CoolPillar1,
		m_CoolPillar2
	};

	Point2f m_MousePos;
	float m_MouseRadius{40};
};
